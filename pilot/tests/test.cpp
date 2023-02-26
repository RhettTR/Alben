#include <iostream>


#include <assert.h>
#include "/home/me/luau/VM/include/lua.h"
#include "/home/me/luau/VM/include/lualib.h"
#include "/home/me/luau/Compiler/include/luacode.h"

#include <gtk/gtk.h>

using namespace std;


extern "C" {
	
	
	static void activate (GtkApplication* app, gpointer user_data)
	{
		GtkWidget *window;

		window = gtk_application_window_new (app);
		gtk_window_set_title (GTK_WINDOW (window), "Window");
		gtk_window_set_default_size (GTK_WINDOW (window), 400, 400);
		gtk_widget_show_all (window);
	}
	
	
	static int window(lua_State *L)
	{
		GtkApplication *app;
		int status;

		app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
		g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
		status = g_application_run (G_APPLICATION (app), 0, NULL);
		g_object_unref (app);

		lua_pushnumber(L, status);
		return 1;
	}
	
}

int main() 
{  

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	
	
	
	lua_pushcfunction(L, window, "window");
    	lua_setglobal(L, "window");
	
	std::string source = R"(
        f = function()
		print('return value from call: '.. window())
        end
        print("starting ...")        
    	)";
	size_t bytecodeSize = 0;
	char* bytecode = luau_compile(source.c_str(), source.length(), NULL, &bytecodeSize);
	assert(luau_load(L, "foo", bytecode, bytecodeSize, 0) == 0);
	free(bytecode);
	
	
	if (lua_pcall(L, 0, 0, 0) != LUA_OK) 
	{
		auto err = lua_tostring(L, -1);
		std::cout << err << std::endl;
	}
	
	
	
	int v = lua_getglobal(L, "f");
	
	std::string str = lua_typename(L, v);
	
	std::cout << str << std::endl;
		
	
	if (lua_isfunction(L, -1))
		lua_pcall(L, 0, 1, 0);
	

	
	lua_close(L);
	
	
	
	return 0;
	
}
