#include <iostream>
#include <vector>
#include <cmath>


#include <gtk/gtk.h>

#include "grid.h"



struct Coordinate 
{ 
	int x; 
	int y; 
};


typedef typename std::vector<Coordinate> Coordiantes;

struct Circle 
{ 
	int radius; 
	GdkRGBA *color; 
};



static Coordiantes hexCoordinates;
static GdkPixbuf *image = NULL;
static GtkWidget *gridWindow = NULL;
// default GdkRGBA color
static GdkRGBA color = { 1.0, 0.0, 0.0, 0.5 };


bool showGrid = false;
Circle gridCircle;




gboolean on_draw (GtkWidget *widget, cairo_t *cr, gpointer data)
{
	
	if (image != NULL)
	{
		gdk_cairo_set_source_pixbuf(cr, image, 0, 0);
		cairo_paint(cr);
	}
	
	
	gdk_cairo_set_source_rgba(cr, (const GdkRGBA *)gridCircle.color);
	
	if (showGrid)
	{		
		for (auto i : hexCoordinates)
		{
			cairo_save(cr);
			cairo_arc(cr, i.x, i.y, gridCircle.radius, 0.0, 2.0 * M_PI); 
			cairo_fill(cr);
		}
	}
	
	
	return FALSE;
	
}




Grid::Grid()
{
	
	image = gdk_pixbuf_new_from_file("../module/images/background.jpg", NULL);

	// defaults
	gridCircle.radius = 5;	
	gridCircle.color = &color;
	
}


void Grid::setCirle(int radius, GdkRGBA *color)
{
	
	gridCircle.radius = radius;
	gridCircle.color = color;
	
}


void Grid::setGridWindow(GtkWidget *window)
{
	
	gridWindow = window;
	
	g_signal_connect(gridWindow, "draw", G_CALLBACK(on_draw), NULL);
	
}


GtkWidget *Grid::getGridWindow()
{
	
	return gridWindow;
	
}


void Grid::setShowGrid(bool show)
{
	
	showGrid = show;

}


void Grid::hexagonalGrid (float a, float b, float xoff, float yoff, int width, int height)
{
	
	int x = 0;
	int y = 0;
	
	hexCoordinates.clear();
	
	// 60 degrees is 1.0471975512 radians
	float c = b / ( 2*tan(1.0471975512) ); 
	
	
	for (int i = 0;;i++)
	{
		 
		float fx = xoff + i*(a + c);
		
		x = static_cast<int>(fx);

		
		if (x >= width)
			return;
		
			
		for (int j = 0;;j++)
		{
			float fy = yoff + j*b + (i%2 == 0 ? 0 : -yoff);
			
			y = static_cast<int>(fy);
			
			if (y >= height)
				break;
			
			hexCoordinates.push_back({x, y});
		}
		
	}
	
  
}


void Grid::snaptoGrid (int x, int y, Coordinate *coordinate)
{
	
	if (hexCoordinates.empty())
		return;
	
	int minimum_distance = std::numeric_limits<int>::max();
	
	Coordinate minimum;
		
	for (auto i : hexCoordinates)
	{
		// simplification of sqr(dx² + dy²)
		int sum = abs(x - i.x) + abs(y -i.y);
		
		if (sum < minimum_distance)
		{
			minimum_distance = sum;
			minimum.x = i.x;
			minimum.y = i.y;
			
		}
	}
	
	
	if (minimum_distance < std::numeric_limits<int>::max())
	{
		coordinate->x = minimum.x;
		coordinate->y = minimum.y;
	}
	
}
