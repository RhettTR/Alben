// NOTE: this is a feasibility test, not production code. There 
// is no input validation and little testing has been done.


#include <iostream>
#include <cstring>

#include <gtk/gtk.h>


#include "luau.h"



using namespace std;


Grid g;
Luau l(&g);



GtkWidget *top;
	
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

	gtk_widget_translate_coordinates(self, gtk_widget_get_parent(self), 0, 0, &start_x, &start_y);

	
	offset_x = abs_x - start_x;
	offset_y = abs_y - start_y;


	gint width = gtk_widget_get_allocated_width (self);
	gint height = gtk_widget_get_allocated_height (self);
	
	 
	cairo_surface_t *surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
														   width, height);                                    
	cairo_t *cr = cairo_create (surface); 
		   
	
	
	gtk_widget_draw (self, cr);                             
		

	gtk_drag_set_icon_surface (context, surface);
	
	gdk_drag_context_set_hotspot ( context, offset_x, offset_y);
	
	
	
	// opacity
	
	
	GdkWindow *dragicon = gdk_drag_context_get_drag_window (context);
	
	gdk_window_set_opacity ( dragicon, l.getAlphadnd() );
	
	
	
	
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
	
	
	// find where we drag from
	gint wx, wy;	
	gtk_widget_translate_coordinates(self, gtk_widget_get_parent(dragged), 0, 0, &wx, &wy);
	

	
	GtkWidget *container = gtk_widget_get_parent (dragged);
	
	gint width = gtk_widget_get_allocated_width (dragged);
	gint height = gtk_widget_get_allocated_height (dragged);
	 
	 
	if (GTK_IS_EVENT_BOX(self))
	{
		
		dx = dx + (width / 2);
		dy = dy + (height / 2);
		
		Grid::Coordinate c;
		
		g.snaptoGrid(wx + dx, wy + dy, &c);
		
		c.x -= width / 2;
		c.y -= height / 2;

		gtk_layout_move( GTK_LAYOUT(container), dragged, c.x, c.y);
	
	}
	
		
	if (GTK_IS_WINDOW(self))
	{
		// hack, x and y are now relative to the 426 window
		dy -= 26;
		
		dx = dx + (width / 2);
		dy = dy + (height / 2);
		
		Grid::Coordinate c;
		
		g.snaptoGrid(dx, dy, &c);
		
		c.x -= width / 2;
		c.y -= height / 2;
		
		
		gtk_layout_move( GTK_LAYOUT(container), dragged, c.x, c.y);
		
	}
		

	
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
	gtk_drag_dest_set   ( counter, GTK_DEST_DEFAULT_ALL, dest_targets, 2, GDK_ACTION_MOVE);
	

	g_signal_connect (counter, "drag-begin", G_CALLBACK (drag_begin), NULL);		
	g_signal_connect (counter, "drag-data-get", G_CALLBACK (drag_get), NULL);
	
	g_signal_connect (counter, "drag-data-received", G_CALLBACK (drag_received), NULL);
	g_signal_connect (counter, "drag-drop", G_CALLBACK (drag_drop), NULL);
	
	
	
	
	return counter;
	
}




static void reloadCallback (GSimpleAction *action, GVariant *var, gpointer pt)
{
	printf("loading ...\n");
	
	l.closeVM();
	
	l.startVM();
			
	printf("done\n");
}



	
static void activate (GtkApplication* app, gpointer user_data)
{
	
	// set up main window
	
	
	static GtkWidget *window = gtk_application_window_new (app);
	
	gtk_window_set_title (GTK_WINDOW (window), "Window");
	gtk_window_set_default_size (GTK_WINDOW (window), 600, 426);
	
	top = gtk_layout_new (NULL, NULL);		
	gtk_container_add ( GTK_CONTAINER(window), top );
	
	
	

	// set up drop signals for main window
	
	gtk_drag_dest_set ( window, GTK_DEST_DEFAULT_ALL, dest_targets, 2, GDK_ACTION_MOVE);
	
	g_signal_connect (window, "drag-data-received", G_CALLBACK (drag_received), NULL);
	g_signal_connect (window, "drag-drop", G_CALLBACK (drag_drop), NULL);
	
	gtk_application_add_window (app, GTK_WINDOW(window));
	
	
	
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
	
	
	
	// create three counters
	
	(void)makeCounter(top, "../draganddrop/red.png", 31, 39);		
	(void)makeCounter(top, "../draganddrop/yellow.png", 41, 49);		
	(void)makeCounter(top, "../draganddrop/green.png", 143, 295);
	
	
	
	g.setGridWindow(top);
	
	
	
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
