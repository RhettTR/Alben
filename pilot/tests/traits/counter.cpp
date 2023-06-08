#include <math.h>
#include "window.h"
#include "counter.h"
#include "luau.h"


using namespace std;




int Counter::_id = 0;

std::map<int, Counter *> Counter::objects;


int Counter::incDegrees = 60;



Counter::Counter(const char *name)
{
	
	this->name = string(name);
	
	string filename = "./images/" + this->name + ".jpg";		
	
	
	
	GError *err = NULL;
		
	this->baseBuffer = gdk_pixbuf_new_from_file(filename.c_str(), &err);
	
	if (err != NULL)
	{
		fprintf (stderr, "Unable to read file: %s\n", err->message);
		g_error_free (err);
	}
	
	this->image = gtk_image_new_from_pixbuf (this->baseBuffer);
	
	
	
	
	this->state.width = gdk_pixbuf_get_width(this->baseBuffer);
    this->state.height = gdk_pixbuf_get_height(this->baseBuffer);
	
	this->diagonal = (int)(sqrt(pow(this->state.width, 2) + pow(this->state.height, 2)));
	
	
	
	this->counter = makePiece();
	
	
	this->id = Counter::_id++;
	
	Counter::objects[this->id] = this;
		
	
	this->state.x = 40 + ((this->diagonal+10)*this->id);
	this->state.y = 40;
	this->state.degrees = 0;
	this->state.flip = 1;		// table index

	
	Window::add(this->counter, this->state.x, this->state.y);
	
	
}


Counter::~Counter() 
{
	
	Window::remove( this->counter );
	
	Counter::objects.erase(this->id);
	
}



void Counter::deleteAll()
{
	
	for (auto &&entry : objects) 
		Window::remove( entry.second->counter );
		 
	objects.clear();
	
	_id = 0;
	
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
		
		
	}
	
	return TRUE;
		
}
	


GtkWidget* Counter::makePiece()
{
	
	// create the apperance of the counter
		
	GtkWidget *event_box = gtk_event_box_new();	
	g_signal_connect (event_box, "button-press-event", G_CALLBACK (do_popup), (gpointer)this->name.c_str());
	
	
	gtk_container_add (GTK_CONTAINER (event_box), GTK_WIDGET(this->image));
	gtk_event_box_set_above_child( GTK_EVENT_BOX(event_box), TRUE );

	
	
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




void Counter::setImage(const char *name)
{
	
	printf("setImage %s\n", name);
	
	string file = string(name);
	
	string filename = "./images/" + file + ".jpg";
	
	
	this->baseBuffer = gdk_pixbuf_new_from_file(filename.c_str(), NULL);
	
	gtk_image_set_from_file ( GTK_IMAGE(this->image), filename.c_str() );
	
	if (this->state.degrees != 0)
		rotateImage();
	else
		Window::update(this->counter, this->state.x, this->state.y);	
	
}

 
 
int Counter::incrementDegrees()
{
	
	int newDegrees = this->state.degrees + Counter::incDegrees;
	
	newDegrees = newDegrees % 360;
	
	if (newDegrees > 180)
		newDegrees = -(360 - newDegrees);
	
	if (newDegrees < -180)
		newDegrees = 360 + newDegrees;
	
	return newDegrees;
	
} 



void Counter::rotateImage()
{
	
	
	int w = this->diagonal;
	int h = this->diagonal;
	
	
	cairo_surface_t *surface = 
		cairo_image_surface_create (CAIRO_FORMAT_ARGB32, w, h);
	
	cairo_t *cr = cairo_create (surface);
	
	
	cairo_translate(cr, w/2.0, h/2.0);
	cairo_rotate(cr, this->state.degrees * (M_PI / 180));
	cairo_translate(cr, - this->state.width/2.0, -this->state.height/2.0);
	
    
    // always use original image
    
    gdk_cairo_set_source_pixbuf ( cr, this->baseBuffer, 0, 0 );
    
    
    cairo_paint (cr);



	// repaint
	
	gtk_container_remove ( GTK_CONTAINER(this->counter), this->image );
	
	this->image = gtk_image_new_from_surface ( surface );
		
	gtk_container_add ( GTK_CONTAINER(this->counter), this->image );
	
	
	// ajust counter position so that rotation is around center
	
	int deltax = (diagonal - this->state.width) / 2;  
	int deltay = (diagonal - this->state.height) / 2;
	
	Window::update(this->counter, this->state.x - deltax, this->state.y - deltay);
	
	
	
	cairo_destroy (cr);
	cairo_surface_destroy (surface);
	
	
}


