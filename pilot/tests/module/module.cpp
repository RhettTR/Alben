// NOTE: this code lacks much input validation, error handling and 
// checks for memory leaks. The code is a feasibility test, not 
// production code.


#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <cstring>

#include <archive.h>
#include <archive_entry.h>


#include <assert.h>
#include "lua.h"
#include "lualib.h"
#include "luacode.h"

#include <gtk/gtk.h>


using namespace std;



static lua_State* L;

struct aScript
{
	std::string chunkName;
	std::string chunk;
};

typedef struct std::vector<aScript> Scripts;



struct anImage
{
	std::string filename;
	GtkWidget *image;
};

typedef struct std::vector<anImage> Images;



int read_script(struct archive *a, std::string *content)
{
	int r;
	const void *buff;
	size_t size;
#if ARCHIVE_VERSION_NUMBER >= 3000000
	int64_t offset;
#else
	off_t offset;
#endif

	for (;;) {
		r = archive_read_data_block(a, &buff, &size, &offset);
		if (r == ARCHIVE_EOF)
			return (ARCHIVE_OK);
		if (r != ARCHIVE_OK)
			return (r);
		
		content->append((char *)buff, size);
	}
}



int read_image(struct archive *a, int64_t imageSize, GtkWidget **image)
{
	int r;	
	const void *buff;
	size_t size;
#if ARCHIVE_VERSION_NUMBER >= 3000000
	int64_t offset;
#else
	off_t offset;
#endif

	unsigned char *buffer;
	size_t length = 0;
	

	buffer = (unsigned char *)malloc(imageSize);
	
	for (;;) 
	{
		r = archive_read_data_block(a, &buff, &size, &offset);
		if (r == ARCHIVE_EOF)
			break;		
		if (r != ARCHIVE_OK)
		{
			free(buffer);
			return (r);
		}
		
		
		memcpy(buffer + offset, (char *)buff, size);
		
		length += size;

	}
	

	
	GdkPixbufLoader *loader = gdk_pixbuf_loader_new_with_type("jpeg", NULL);
	gdk_pixbuf_loader_write (loader, buffer, length, NULL);	
	gdk_pixbuf_loader_close(loader, NULL);
	
	GdkPixbuf *pixbuf = gdk_pixbuf_loader_get_pixbuf(loader);
	
	
	// unfortunately the image is stored uncompressed, i.e. the pixbuf 
	// length is width*height*3 bytes, this will take up much more RAM;
	// has to be done differently
	 
	cout << gdk_pixbuf_get_byte_length(pixbuf) << endl;
	
	
	*image = gtk_image_new_from_pixbuf(pixbuf);
	
	
	free(buffer);
	
	
	
	
	return (ARCHIVE_OK);
	
}


Scripts scripts;
Images images;



int readArchive()
{
	struct archive *a;
    struct archive_entry *entry;
    int r;
    std::string chunkName, extension;
    
    // list of allowed resource file types
    vector<std::string> suffix = { ".jpg", ".png", ".gif" };
    
    a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);
    r = archive_read_open_filename(a, "module.zip", 10240);
    
    if (r != ARCHIVE_OK)
		return 1;
		
	while (archive_read_next_header(a, &entry) == ARCHIVE_OK) 
	{
		
		std::filesystem::path path = archive_entry_pathname(entry);
		
		extension = path.extension();
		chunkName = path.filename().replace_extension();

		if (extension == ".luau")
		{
			std::string content;
			r = read_script(a, &content);
			
			if (r != ARCHIVE_OK)
				return 1;
			
			aScript script = aScript{ chunkName, content };	
			scripts.push_back(script);		
		}
		
		else
		if (std::find(suffix.begin(), suffix.end(), extension) != suffix.end())
		{
			GtkWidget *image = NULL;
			
			int64_t s = archive_entry_size(entry);
			
			r = read_image(a, s, &image);
			
			if (r != ARCHIVE_OK)
				return 1;
			
			// note that the case of subdirectories in the 'images' directory
			// is not dealt with here
			anImage animage = anImage{ path.filename(), image };	
			images.push_back(animage);			
		}
		else
			archive_read_data_skip(a);
	}
	
	r = archive_read_free(a);
	 
    if (r != ARCHIVE_OK)
		return 1;
		
	return 0;
}


void compileArchiveScript()
{	
	char*  bytecode;
	size_t bytecodeSize = 0;
	
	for (auto &element : scripts) 
	{

		cout << element.chunkName << endl;

		bytecode = luau_compile(element.chunk.c_str(), element.chunk.length(), NULL, &bytecodeSize);
		assert(luau_load(L, element.chunkName.c_str(), bytecode, bytecodeSize, 0) == 0);
		free(bytecode);	
		
		// set global to make other script files use this script file
		lua_setglobal(L, element.chunkName.c_str());
	}
}


void startVM()
{
	
	readArchive();
	
	L = luaL_newstate();
	luaL_openlibs(L);	
	
	compileArchiveScript();
	
	// list found resource files
	for (auto &element : images) 
	{
		std::cout << element.filename << std::endl;		
	}
		
}	


extern "C" {
	
	static GtkApplication *app;
	static GtkWidget *window;
	
	
	static int setbackground(lua_State *L)
	{	
		GtkWidget *layout = gtk_layout_new(NULL, NULL);
		gtk_container_add(GTK_CONTAINER (window), layout);
				
		
		std::string fileName = lua_tostring(L, 1);
		lua_pop(L, 1);
		 
		
		
		anImage *found;
		
		// this can be done a lot better with a hash table
		 
		for (anImage &animage : images)
			if (animage.filename == fileName)
					found = &animage;
					
		
		gtk_layout_put(GTK_LAYOUT(layout), found->image, 0, 0);
		gtk_widget_show(layout);
		
		
		return 0;
	}
	
	static int setwindow(lua_State *L)
	{
		
		window = gtk_application_window_new (app);
		gtk_window_set_title (GTK_WINDOW (window), "Window");
		gtk_window_set_default_size (GTK_WINDOW (window), 600, 400);
		
		gtk_widget_show_all (window);
		
		return 0;		
	}
	
	static void activate (GtkApplication* app, gpointer user_data)
	{
		
		//
		
		
		startVM();
		
		
		
		lua_pushcfunction(L, setbackground, "setbackground");
		lua_setglobal(L, "setbackground");
		
		lua_pushcfunction(L, setwindow, "setwindow");
		lua_setglobal(L, "setwindow");
		
		
		lua_getglobal(L, "module");
		lua_pcall(L, 0, 0, 0);
		lua_getglobal(L, "f");
		lua_pcall(L, 0, 0, 0);
		

		lua_close(L);		
		
		
		//
		gtk_widget_show_all (window);
		
	}

	
}


int main() 
{  

	app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
	(void)g_application_run (G_APPLICATION (app), 0, NULL);
	g_object_unref (app);

	return 0;
	
}
