#include <gtk/gtk.h>


class Grid
{

	public:
	
		struct Coordinate 
		{ 
			int x; 
			int y; 
		};

		
		Grid();
		
		void setCirle(int radius, GdkRGBA *color);
		void setGridWindow(GtkWidget *window);
		GtkWidget *getGridWindow();
		void setShowGrid(bool show);
		void hexagonalGrid (float a, float b, float xoff, float yoff, int width, int height);
		void drawGrid (GtkWidget* window, double radius, GdkRGBA *color);
		void snaptoGrid (int x, int y, Coordinate *coordinate);
		
};
