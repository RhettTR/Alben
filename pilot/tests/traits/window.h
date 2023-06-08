#include <gtk/gtk.h>



		
		
class Window
{

	public:

		
		static void add(GtkWidget *piece, int x, int y);
		
		static void update(GtkWidget *piece, int x, int y);
		
		static void remove(GtkWidget *piece);
		
		static void destroy(GtkWidget *piece);


		Window();
		
	
		
};
