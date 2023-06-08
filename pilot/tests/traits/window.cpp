#include <gtk/gtk.h>


#include "window.h" 



		
GtkWidget *window;


GtkWidget *fixed;
	

	

Window::Window()
{	
	
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	
	gtk_window_set_title (GTK_WINDOW (window), "Counters");
	gtk_window_set_default_size (GTK_WINDOW (window), 300, 400);
	
	
	
	fixed = gtk_fixed_new ();
	
	gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (fixed));
	

	
	GtkCssProvider *css = gtk_css_provider_new ();
	const gchar *data = "*{ background-color: #ffffff; }";
	(void)gtk_css_provider_load_from_data ( css, data, -1, NULL);

	GtkStyleContext *context = gtk_widget_get_style_context (window);
	
	gtk_style_context_add_provider (context, GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_USER);
	

	gtk_widget_show_all(window);
	
	
}






void Window::add(GtkWidget *piece, int x, int y)
{
	
	gtk_fixed_put ( GTK_FIXED(fixed), piece, x, y );

	gtk_widget_show_all(window);
	
}


void Window::update(GtkWidget *piece, int x, int y)
{
	
	gtk_fixed_move ( GTK_FIXED(fixed), piece, x, y );

	gtk_widget_show_all(window);
	
	
}


void Window::remove(GtkWidget *piece)
{
	
	gtk_container_remove ( GTK_CONTAINER(fixed),  piece );
	
	
}


void Window::destroy(GtkWidget *piece)
{
	
	gtk_widget_destroy ( piece );
	
	gtk_widget_show_all(window);
	
}

