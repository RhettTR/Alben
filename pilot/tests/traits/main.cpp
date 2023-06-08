// NOTE: This is a feasibility test, not production code.
// The code is unfinished in many respects. 


#include <iostream>
#include <cstring>

#include <gtk/gtk.h>


#include "luau.h"
#include "counter.h"
#include "window.h"



using namespace std;



Luau l;



static void reloadCallback (GSimpleAction *action, GVariant *var, gpointer pt)
{
	printf("loading ...\n");
	
	l.closeVM();
	
	Counter::deleteAll();
	
	l.startVM();
			
	printf("done\n");
}



static GdkPixbuf *image = NULL;

gboolean on_draw (GtkWidget *widget, cairo_t *cr, gpointer data)
{
	
	if (image != NULL)
	{
		gdk_cairo_set_source_pixbuf(cr, image, 0, 0);
		cairo_paint(cr);
	}
	
	
	return FALSE;
	
}
	
	
static void activate (GtkApplication* app, gpointer user_data)
{
	
	// set up main window
	
	
	static GtkWidget *window = gtk_application_window_new (app);
	
	gtk_window_set_title (GTK_WINDOW (window), "Window");
	gtk_window_set_default_size (GTK_WINDOW (window), 600, 426);
	
	
	gtk_window_set_position ( GTK_WINDOW (window), GTK_WIN_POS_CENTER );
	
	
	
	
	GtkWidget *top = gtk_layout_new (NULL, NULL);		
	gtk_container_add ( GTK_CONTAINER(window), top );
	
	
	image = gdk_pixbuf_new_from_file("./images/Map.jpg", NULL);
	g_signal_connect(top, "draw", G_CALLBACK(on_draw), NULL);

	
	gtk_application_add_window (app, GTK_WINDOW(window));
	
	
	//create Counter window
	
	Window w;
	
	
	
	// menu bar
	
	
	GMenu *menubar = g_menu_new();


	GMenu *reload = g_menu_new ();	
	
	GSimpleAction *reload_action = g_simple_action_new ( "reload", NULL);
	g_signal_connect( reload_action, "activate", G_CALLBACK ( reloadCallback ),  NULL ); 
	
	
	GActionMap *action_map = G_ACTION_MAP ( app );
	g_action_map_add_action ( action_map, G_ACTION ( reload_action ) );

	
	g_menu_append (reload, "Reload script" , "app.reload" ); 
	
	
	
	GMenuItem *filemenu = g_menu_item_new ("_File", NULL);
	g_menu_item_set_submenu (filemenu, G_MENU_MODEL (reload));		
	g_menu_append_item (menubar, filemenu);
	
	
	const gchar *const accelerators[] = { "<Ctrl>r", NULL };
	gtk_application_set_accels_for_action ( GTK_APPLICATION ( app ), "app.reload", accelerators ); 	
	gtk_application_set_menubar (app, (GMenuModel *)menubar);
	gtk_application_window_set_show_menubar(GTK_APPLICATION_WINDOW(window), TRUE);
	gtk_window_present(GTK_WINDOW(window));
	
	
	
	g_object_unref ( reload );
	g_object_unref ( menubar ); 
	
	
	
	
	
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
