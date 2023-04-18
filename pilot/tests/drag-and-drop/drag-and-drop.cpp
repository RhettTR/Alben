// NOTE: The code is a feasibility test for drag and drop 
// with alpha of dragged object set by the module developer.


#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>


#include <assert.h>
#include "lua.h"
#include "lualib.h"
#include "luacode.h"

#include <gtk/gtk.h>



using namespace std;



static lua_State* L;



void compileScript()
{	
	char*  bytecode;
	size_t bytecodeSize = 0;
	
	
	string content;	
	ifstream ifs;
	
	ifs.open("module.luau", ifstream::in);
	content.assign( (istreambuf_iterator<char>(ifs) ),
                    (istreambuf_iterator<char>()    ) );
	ifs.close();
	
	bytecode = luau_compile(content.c_str(), content.length(), NULL, &bytecodeSize);
	assert(luau_load(L, "module", bytecode, bytecodeSize, 0) == 0);
	free(bytecode);
	
	lua_setglobal(L, "module");
	
}



void startVM()
{
	
	L = luaL_newstate();
	luaL_openlibs(L);	
	
	
	compileScript();
		
}	



extern "C" {
	
	
	
	float alpha = 1.0;
	
	static int setAlpha(lua_State *L)
	{	
		
		if (lua_isstring(L, -1))
		{	
			const char *val = lua_tostring(L, -1);	
			lua_pop(L, 1);
			alpha = atof(val);
		}
		
		
		return 0;
	}
	
	
	
	
	int offset_x;
	int offset_y;
	
	GtkWidget *dragged = NULL;
	
	
	
	
	void drag_begin (GtkWidget* self,
			 GdkDragContext* context,
			 gpointer user_data
			)
	{
		
		
		dragged = self;
		
		GdkDevice *device = gdk_drag_context_get_device (context);
		
		GdkWindow *window = gtk_widget_get_parent_window (self);
		
		
		gint abs_x, abs_y;
		
		(void)gdk_window_get_device_position ( window, device, &abs_x, &abs_y, NULL );
		
		
		
		gint start_x, start_y;

		gtk_widget_translate_coordinates(self, gtk_widget_get_toplevel(self), 0, 0, &start_x, &start_y);

		
		
		offset_x = abs_x - start_x;
	    	offset_y = abs_y - start_y;

		
		gint width = gtk_widget_get_allocated_width (self);
		gint height = gtk_widget_get_allocated_height (self);
		
		 
		cairo_surface_t *surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);                                    
        	cairo_t *cr = cairo_create (surface); 
               
		
		gtk_widget_draw (self, cr);                             
			

		gtk_drag_set_icon_surface (context, surface);
		
		gdk_drag_context_set_hotspot ( context, offset_x, offset_y);
		
		
		
		// opacity
		
		
		GdkWindow *dragicon = gdk_drag_context_get_drag_window (context);
		
		gdk_window_set_opacity ( dragicon, alpha );
		
		
		
		
		cairo_destroy (cr);
		cairo_surface_destroy (surface);
		
		
	}
	
	
	void drag_get ( GtkWidget* self,
			GdkDragContext* context,
			GtkSelectionData* data,
			guint info,
			guint time,
			gpointer user_data
		      )
	{
		
		gtk_selection_data_set ( data,
					 gdk_atom_intern_static_string ("GTK_WIDGET"),
					 32,
					 (const guchar *)self,
					 sizeof (gpointer)
				       );						   
	}
	

	
	
	void drag_received ( GtkWidget* self,
			     GdkDragContext* context,
		             gint x,
			     gint y,
			     GtkSelectionData* data,
			     guint info,
		             guint time,
			     gpointer user_data
			   )
	{		
	}



	gboolean drag_drop ( GtkWidget* self,
		             GdkDragContext* context,
			     gint x,
			     gint y,
			     guint time,
			     gpointer user_data
			   )
	{
				
		// a trick to put the dragged counter topmost 
		gtk_widget_hide (dragged);
		
		
		int dx = x - offset_x;
		int dy = y - offset_y;
		
		
		gint wx, wy;	
		gtk_widget_translate_coordinates(self, gtk_widget_get_toplevel(dragged), 0, 0, &wx, &wy);
				
		
		GtkWidget *container = gtk_widget_get_parent (dragged); 
		

		if (GTK_IS_EVENT_BOX(self))
			gtk_layout_move( GTK_LAYOUT(container), dragged, wx + dx, wy + dy);
			
					
		if (GTK_IS_WINDOW(self))
			gtk_layout_move( GTK_LAYOUT(container), GTK_WIDGET(dragged), dx, dy);
			
		
		gtk_widget_show (dragged);
		
		
		gtk_drag_finish (context, TRUE, TRUE, time);	
		
				
		return TRUE;
		
		
	}



	static GtkTargetEntry source_targets[] = {
			{ (gchar *)GTK_TYPE_WIDGET, GTK_TARGET_SAME_APP, 0 }
	};
	
	
	static GtkTargetEntry dest_targets[] = {
			{ (gchar *)GTK_TYPE_WINDOW, GTK_TARGET_SAME_APP, 0 },
			{ (gchar *)GTK_TYPE_WIDGET, GTK_TARGET_SAME_APP, 0 }
	};
	
	


    GtkWidget *makeCounter(GtkWidget *layout,
			   const char *filename, 
			   int x, int y)
	{
		
		// create the apperance of the counter
			
		GtkWidget *counter = gtk_event_box_new ();
		
		gtk_event_box_set_above_child( GTK_EVENT_BOX(counter), TRUE );

		GtkWidget *image = gtk_image_new_from_file (filename);
		gtk_container_add (GTK_CONTAINER (counter), image);
		
		
		gtk_layout_put ( GTK_LAYOUT(layout), counter, x, y );

	
		
		// add signals for drag-and-drop

		gtk_drag_source_set ( counter, GDK_BUTTON1_MASK, source_targets, 1, GDK_ACTION_MOVE );
		gtk_drag_dest_set   ( counter, GTK_DEST_DEFAULT_ALL, dest_targets, 1, GDK_ACTION_MOVE);
		
		

		g_signal_connect (counter, "drag-begin", G_CALLBACK (drag_begin), NULL);		
		g_signal_connect (counter, "drag-data-get", G_CALLBACK (drag_get), NULL);
		
		g_signal_connect (counter, "drag-data-received", G_CALLBACK (drag_received), NULL);
		g_signal_connect (counter, "drag-drop", G_CALLBACK (drag_drop), NULL);
		
		
		
		
		return counter;
		
	}
	


    
	static void activate (GtkApplication* app, gpointer user_data)
	{
		
		// set up main window and its background
		
		static GtkWidget *window = gtk_application_window_new (app);
		gtk_window_set_title (GTK_WINDOW (window), "Window");
		gtk_window_set_default_size (GTK_WINDOW (window), 600, 400);
		
		
		GtkWidget *top = gtk_overlay_new();		
		gtk_container_add ( GTK_CONTAINER(window), top );
			
		GtkWidget *image = gtk_image_new_from_file("../module/images/background.jpg");	
		gtk_overlay_add_overlay(GTK_OVERLAY (top), image);
		
		

		// set up signals to main window
		
		gtk_drag_dest_set ( window, GTK_DEST_DEFAULT_ALL, dest_targets, 1, GDK_ACTION_MOVE);
		
		g_signal_connect (window, "drag-data-received", G_CALLBACK (drag_received), NULL);
		g_signal_connect (window, "drag-drop", G_CALLBACK (drag_drop), NULL);
		
		
		
		
		// create three counters
		
		GtkWidget *counters = gtk_layout_new ( NULL, NULL );		
		gtk_overlay_add_overlay( GTK_OVERLAY (top), counters);
		
		(void)makeCounter(counters, "red.png", 31, 39);		
		(void)makeCounter(counters, "yellow.png", 41, 49);		
		(void)makeCounter(counters, "green.png", 143, 295);
		
	
				
		
		startVM();
		
		lua_pushcfunction(L, setAlpha, "setAlpha");
		lua_setglobal(L, "setAlpha");
		
		lua_getglobal(L, "module");
		lua_pcall(L, 0, 0, 0);

		lua_close(L);		
		
		
		
		//
		gtk_widget_show_all(window);
		
	}

	
}


int main() 
{  

	static GtkApplication *app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
	(void)g_application_run (G_APPLICATION (app), 0, NULL);
	g_object_unref (app);

	return 0;
	
}
