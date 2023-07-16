// NOTE: This is a feasibility test, not production code.
// The code is unfinished in many respects. 


#include <iostream>
#include <cstring>

#include "luau.h"
#include "counter.h"


#include <gtk/gtk.h>


using namespace std;



Luau l;




static void reloadCallback (GSimpleAction *action, GVariant *var, gpointer pt)
{
	printf("loading ...\n");
	
	l.closeVM();
	
	Counter::deleteAll();
	
	l.startVM();
	
	gtk_widget_show_all(GTK_WIDGET(pt));
			
	printf("done\n");
}


static void undoCallback (GSimpleAction *action, GVariant *var, gpointer pt)
{
	Luau::undo();
}


static void redoCallback (GSimpleAction *action, GVariant *var, gpointer pt)
{
	Luau::redo();
}


gboolean on_draw (GtkWidget *widget, cairo_t *cr, gpointer data)
{
	
	GdkPixbuf *image = gdk_pixbuf_new_from_file("./images/Map.jpg", NULL);
	
	if (image != NULL)
	{
		gdk_cairo_set_source_pixbuf(cr, image, 0, 0);
		cairo_paint(cr);
	}
	
	
	return FALSE;
	
}
	
	
	
GtkWidget *window;

	
static void activate (GtkApplication* app, gpointer user_data)
{
	
	// set up main window
		
	window = gtk_application_window_new (app);
	
	gtk_window_set_title (GTK_WINDOW (window), "Window");
	gtk_window_set_default_size (GTK_WINDOW (window), 600, 426);
	
	
	gtk_window_set_position ( GTK_WINDOW (window), GTK_WIN_POS_CENTER );
	
	
	Counter::fixedLayout = gtk_fixed_new ();	
	gtk_container_add ( GTK_CONTAINER(window), Counter::fixedLayout );
	
	
	g_signal_connect(Counter::fixedLayout, "draw", G_CALLBACK(on_draw), NULL);
	
	gtk_application_add_window (app, GTK_WINDOW(window));
	
	
	
	// set up drag-drop signals to main window
	
	gtk_drag_dest_set ( window, GTK_DEST_DEFAULT_ALL, Counter::dest_targets, 2, GDK_ACTION_MOVE);
	
	g_signal_connect (window, "drag-drop", G_CALLBACK (Counter::drag_drop), NULL);
	
	
	
	
	
	
	// menu bar
	
	
	GMenu *menubar = g_menu_new();


	GMenu *submenu = g_menu_new ();
	
	
	GSimpleAction *reload_action = g_simple_action_new ( "reload", NULL);
	g_signal_connect( reload_action, "activate", G_CALLBACK ( reloadCallback ),  window );
	
	GSimpleAction *undo_action = g_simple_action_new ( "undo", NULL);
	g_signal_connect( undo_action, "activate", G_CALLBACK ( undoCallback ),  window );
	 
	GSimpleAction *redo_action = g_simple_action_new ( "redo", NULL);
	g_signal_connect( redo_action, "activate", G_CALLBACK ( redoCallback ),  window );  
	
	
	GActionMap *action_map = G_ACTION_MAP ( app );
	g_action_map_add_action ( action_map, G_ACTION ( reload_action ) );
	g_action_map_add_action ( action_map, G_ACTION ( undo_action ) );
	g_action_map_add_action ( action_map, G_ACTION ( redo_action ) );

	
	g_menu_append (submenu, "Reload script" , "app.reload" );
	g_menu_append (submenu, "Undo" , "app.undo" ); 
	g_menu_append (submenu, "Redo" , "app.redo" );  
	
	
	
	GMenuItem *filemenu = g_menu_item_new ("_File", NULL);
	g_menu_item_set_submenu (filemenu, G_MENU_MODEL (submenu));
	
			
	g_menu_append_item (menubar, filemenu);
	
	
	const gchar *const accreload[] = { "<Ctrl>r", NULL };
	gtk_application_set_accels_for_action ( GTK_APPLICATION ( app ), "app.reload", accreload );
	const gchar *const accundo[] = { "<Ctrl>z", NULL };
	gtk_application_set_accels_for_action ( GTK_APPLICATION ( app ), "app.undo", accundo );
	const gchar *const accredo[] = { "<Ctrl>y", NULL };
	gtk_application_set_accels_for_action ( GTK_APPLICATION ( app ), "app.redo", accredo );
	 	
	gtk_application_set_menubar (app, (GMenuModel *)menubar);
	gtk_application_window_set_show_menubar(GTK_APPLICATION_WINDOW(window), TRUE);
	gtk_window_present(GTK_WINDOW(window));
	
	
	
	g_object_unref ( submenu );
	g_object_unref ( menubar ); 
	
	
	Counter::resources["_Moved"] = "./Moved.png";
	
	
	
	
	
	l.startVM();

	//
	
	
	
	gtk_widget_show_all(window);
	
	
	
}



int main() 
{  

	static GtkApplication *app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
	(void)g_application_run (G_APPLICATION (app), 0, NULL);
	l.closeVM();	
	g_object_unref (app);

	return 0;
	
}
