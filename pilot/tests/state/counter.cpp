#include <math.h>

#include "counter.h"
#include "luau.h"


using namespace std;



int Counter::_id = 0;


map<int, Counter *> Counter::objects;
map<string, string> Counter::resources;




GtkWidget* Counter::fixedLayout = NULL;


float Counter::alpha = 1.0;

int Counter::mask_x = 0;
int Counter::mask_y = 0;



extern GtkWidget *window;



Counter::Counter(const char *name, int x, int y)
{
	
	Counter::objects[this->id] = this;
	
	this->name = string(name);
	
	this->state.x = x;
	this->state.y = y;
	this->state.moved = false;
	this->state.degrees = 0;
	this->state.name = string(name);
	
	string filename = "./images/" + this->state.name + ".jpg";
			
	
	
	
	GError *err = NULL;
		
	this->baseBuffer = gdk_pixbuf_new_from_file(filename.c_str(), &err);
	
	if (err != NULL)
	{
		fprintf (stderr, "Unable to read file: %s\n", err->message);
		g_error_free (err);
	}
	
	this->image = gtk_image_new_from_pixbuf (this->baseBuffer);

	
	this->width = gdk_pixbuf_get_width(this->baseBuffer);
    this->height = gdk_pixbuf_get_height(this->baseBuffer);
	
	
	this->counter = makePiece();
	
	
	gtk_fixed_put ( GTK_FIXED(fixedLayout), this->counter, this->state.x, this->state.y );

	
	gtk_widget_show_all(fixedLayout);

}


Counter::~Counter() 
{
	
	gtk_widget_destroy(this->counter);
	
	g_object_unref(this->baseBuffer);
	
	
	
	
	Counter::objects.erase(this->id);
	
}



void Counter::deleteAll()
{
	
	for (auto &&entry : objects) 
		gtk_container_remove ( GTK_CONTAINER(fixedLayout), entry.second->counter );
		 
	objects.clear();
	
	Counter::_id = 0;
	
}





	
	
int offset_x;
int offset_y;

Counter *dragged = NULL;
	



GtkTargetEntry Counter::source_targets[] = {
		{ (gchar *)GTK_TYPE_WIDGET, GTK_TARGET_SAME_APP, 0 }
};


GtkTargetEntry Counter::dest_targets[] = {			
		{ (gchar *)GTK_TYPE_WINDOW, GTK_TARGET_SAME_APP, 0 },
		{ (gchar *)GTK_TYPE_WIDGET, GTK_TARGET_SAME_APP, 1 }
};

	
	
	
static void drag_begin (GtkWidget* self,
						GdkDragContext* context,
						gpointer user_data
					   )
{
	
	
	dragged = (Counter *)user_data;
	
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
	
	
	 
	cairo_surface_t *surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
														   width, height);                                    
	cairo_t *cr = cairo_create (surface); 
		   
	
	gtk_widget_draw (self, cr);                             
		

	gtk_drag_set_icon_surface (context, surface);
	
	gdk_drag_context_set_hotspot ( context, offset_x, offset_y);
	
	
	
	// opacity
	
	
	GdkWindow *dragicon = gdk_drag_context_get_drag_window (context);
	
	gdk_window_set_opacity ( dragicon, Counter::alpha );
	

	
	
	
	
	cairo_destroy (cr);
	cairo_surface_destroy (surface);
	
	
}


static void drag_get ( GtkWidget* self,
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
	
	
	

gboolean Counter::drag_drop ( GtkWidget* self,
							  GdkDragContext* context,
							  gint x,
							  gint y,
							  guint time,
							  gpointer user_data
						    )
{
			
	int dx = x - offset_x;
	int dy = y - offset_y;
	
	// hack, x and y are now relative to the 426 window
		dy -= 26;
		
	
	// find where we drag from
	gint wx, wy;	
	gtk_widget_translate_coordinates(self, gtk_widget_get_toplevel(dragged->counter), 0, 0, &wx, &wy);
			
	
	GtkWidget *container = gtk_widget_get_parent (dragged->counter); 
	

	if (GTK_IS_EVENT_BOX(self))
	{
		gtk_fixed_move ( GTK_FIXED(container), dragged->counter, wx + dx, wy + dy );
		dragged->state.x = wx + dx;
		dragged->state.y = wy + dy;
		Luau::doEvent("movetrigger", dragged->name.c_str(), "", "", 0);
		Luau::doEvent("move", dragged->name.c_str(), "Counter", "x", wx + dx);
		Luau::doEvent("move", dragged->name.c_str(), "Counter", "y", wy + dy);
		Luau::doEvent("end", "", "", "", 0);
	}
		
				
	if (GTK_IS_WINDOW(self))
	{
		gtk_fixed_move ( GTK_FIXED(container), dragged->counter, dx, dy );	
		dragged->state.x = dx;
		dragged->state.y = dy;
		Luau::doEvent("movetrigger", dragged->name.c_str(), "", "", 0);
		Luau::doEvent("move", dragged->name.c_str(), "Counter", "x", dx);
		Luau::doEvent("move", dragged->name.c_str(), "Counter", "y", dy);
		Luau::doEvent("end", "", "", "", 0);
	}
	
	
	gtk_drag_finish (context, TRUE, TRUE, time);	
	
			
	return TRUE;
	
	
}



static void do_activate ( GtkMenuItem* self,
			              gpointer user_data
					    )
{
	
	Luau::doAction((const char *)user_data);
		
}



static Luau::PopupEntries list;



static gboolean do_popup ( GtkWidget *self,
						   GdkEventButton *event,
						   gpointer user_data
						 )
{
	
	if (event->button == 3)
	{
		
		GtkWidget *menu = gtk_menu_new ();
	
		// populate menu with traits
		
		list.clear();
		list = Luau::getTraits((const char *)user_data);
		
		
		for (auto e : list)
		{
			 
		    GtkWidget *menuitem = gtk_menu_item_new_with_label (e.entryname.c_str());
		    
		    
		    //GtkWidget *child = gtk_bin_get_child (GTK_BIN (menuitem));		    
		    //gtk_accel_label_set_accel (GTK_ACCEL_LABEL (child), GDK_KEY_D, GDK_CONTROL_MASK);
		   
		    
		    g_signal_connect (menuitem, 
						      "activate", 
						      G_CALLBACK (do_activate), 
							  (gpointer)e.entryaction);
		  
			gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);
	  
		}
		 
		
		gtk_widget_show_all(menu);		
		gtk_menu_popup_at_pointer ( GTK_MENU(menu), (GdkEvent *)event );
		
		return TRUE;
		
	}
	
	return FALSE;
		
}
	


GtkWidget* Counter::makePiece()
{
	
	// create the apperance of the counter
		
	GtkWidget *event_box = gtk_event_box_new();	
	g_signal_connect (event_box, "button-press-event", G_CALLBACK (do_popup), (gpointer)this->name.c_str());
	
	
	gtk_container_add (GTK_CONTAINER (event_box), GTK_WIDGET(this->image));
	gtk_event_box_set_above_child( GTK_EVENT_BOX(event_box), TRUE );
	
	
	
	// add signals for drag-and-drop

	gtk_drag_source_set ( event_box, GDK_BUTTON1_MASK, Counter::source_targets, 1, GDK_ACTION_MOVE );
	gtk_drag_dest_set   ( event_box, GTK_DEST_DEFAULT_ALL, Counter::dest_targets, 2, GDK_ACTION_MOVE);
	
	

	g_signal_connect (event_box, "drag-begin", G_CALLBACK (drag_begin), (gpointer)this);		
	g_signal_connect (event_box, "drag-data-get", G_CALLBACK (drag_get), NULL);
	
	g_signal_connect (event_box, "drag-drop", G_CALLBACK (drag_drop), NULL);

	
	//
	gtk_drag_dest_set (window, GTK_DEST_DEFAULT_ALL, Counter::dest_targets, 2, GDK_ACTION_MOVE);
	
	g_signal_connect (window, "drag-drop", G_CALLBACK (Counter::drag_drop), NULL);
	
	
	
	return event_box;
	
}



Counter* Counter::findObj(const char *name)
{
	
	string findName = string(name);
	
	
	for ( auto obj = objects.begin(); obj != objects.end(); ++obj  )
	
		if (obj->second->name == findName)
			return obj->second;
	
			
	return NULL;
	
}




void Counter::setImage()
{
	
	
	string file = this->state.name;
	
	string filename = "./images/" + file + ".jpg";
	
	
	this->baseBuffer = gdk_pixbuf_new_from_file(filename.c_str(), NULL);
	
	
	gtk_image_set_from_pixbuf ( GTK_IMAGE(this->image), this->baseBuffer );
	
	gtk_fixed_move ( GTK_FIXED(fixedLayout), this->counter, this->state.x, this->state.y );
	
	
	
	
	int w = this->width;
	int h = this->height;
	
	
	// moved
	
	if (this->state.moved)
	{
		
		string file = Counter::resources["_Moved"];
	
		
		const GdkPixbuf *mask = gdk_pixbuf_new_from_file(file.c_str(), NULL);
		
		w = w + gdk_pixbuf_get_width (mask);
		
		cairo_surface_t *surface = 
			cairo_image_surface_create (CAIRO_FORMAT_ARGB32, w, h);
		
		cairo_t *cr = cairo_create (surface);
		
		
		int dx = Counter::mask_x;
		int dy = Counter::mask_y;
		
		
		gdk_cairo_set_source_pixbuf(cr, this->baseBuffer, 0, 0);
		cairo_paint(cr);
		gdk_cairo_set_source_pixbuf(cr, mask, dx, dy);
		cairo_paint(cr);
		
		
		this->baseBuffer = gdk_pixbuf_get_from_surface ( surface, 0, 0, w, h );
		gtk_image_set_from_pixbuf ( GTK_IMAGE(this->image), this->baseBuffer );
		
		
		
		cairo_destroy (cr);
		cairo_surface_destroy (surface);
	
	}
	
	
	
	// rotated
	
	if (this->state.degrees != 0)
	{
	
		// compute size of rotated image
		
		float radians = this->state.degrees * (M_PI/180);
		float rad = (abs(this->state.degrees) % 90) * (M_PI/180);
		
		float sine = sin(rad);
		float cosine = cos(rad);
		
		
		
		int ew = this->width;
		int eh = this->height;
		
		w = std::round(cosine * w + sine * h);
		h = std::round(cosine * h + sine * w);
		
		
		// toggle w and h if tured sideways
		
		if (abs(this->state.degrees) == 90)
		{
			int helper = w;
			w = h;
			h = helper;
		}
		
		
		int deltax = std::round((w - ew) / 2);  
		int deltay = std::round((h - eh) / 2);
		
		
		
		
		cairo_surface_t *surface = 
			cairo_image_surface_create (CAIRO_FORMAT_ARGB32, w, h);
		
		cairo_t *cr = cairo_create (surface);
		
			
		
		cairo_translate(cr, w/2, h/2);
		cairo_rotate(cr, radians);
		cairo_translate(cr, - w/2, - h/2);
		
		gdk_cairo_set_source_pixbuf ( cr, this->baseBuffer, deltax, deltay );
		
		cairo_paint (cr);
		
		
		
		g_object_unref(this->baseBuffer);
		
		
		
			
		this->baseBuffer = gdk_pixbuf_get_from_surface ( surface, 0, 0, w, h );

		gtk_image_set_from_pixbuf ( GTK_IMAGE(this->image), this->baseBuffer );
		
		
		
			
		// ajust counter position 
		
		
		int x = this->state.x + std::round((this->width - w) / 2);
		int y = this->state.y + std::round((this->height - h) / 2); 	
		
		gtk_fixed_move ( GTK_FIXED(fixedLayout), this->counter, x, y );
		
		
		
		cairo_destroy (cr);
		cairo_surface_destroy (surface);
		
	}
	
	
}


