#include <iostream>
#include <vector>
#include <string>
#include <map>

#include <gtk/gtk.h>




class Counter
{
	
	public:
	
		struct State
		{
			int x;
			int y;
			bool moved;				// only true if trait		
			int degrees;			// only not zero if trait
			std::string name;		// name of current (flipped) image	
		};
		
		
		Counter(const char *name, int x, int y);
		
		~Counter();
		
		static void deleteAll();
		
		static Counter* findObj(const char *name);
		
		void setImage();
		
		void setMovedMask(const char *mask, bool moved);
		
		int incrementDegrees(); 
		
		void rotateImage();
		
		State state;
		
		static std::map<std::string, std::string> resources;
		
		static GtkWidget* fixedLayout;
	
		static GtkTargetEntry source_targets[];

		static GtkTargetEntry dest_targets[];	
		
		static gboolean drag_drop ( GtkWidget* self,
								    GdkDragContext* context,
								    gint x,
								    gint y,
								    guint time,
								    gpointer user_data
							      );
		static float alpha;
		
		static int mask_x;
		static int mask_y;

		
		
	private:
	
		
		const int id = _id++;	// id of this counter		
		
		static int _id;			// id of latest counter
		
		std::string name;		// string id of this counter 
				
		GtkWidget *counter;
		
		GdkPixbuf *baseBuffer;
		
		GtkWidget *image;
		
		int width;		
		int height;	
		
		static std::map<int, Counter *> objects;
		
		GtkWidget *makePiece();
		
};
 
