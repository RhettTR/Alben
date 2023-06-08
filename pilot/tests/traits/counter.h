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
			int width;
			int height;
			int degrees;
			int flip;
		};
		
		
		Counter(const char *name);
		
		~Counter();
		
		static void deleteAll();
		
		static Counter* findObj(const char *name);
		
		void setImage(const char *name);
		
		static int incDegrees;
		
		int incrementDegrees(); 
		
		void rotateImage();
		
		State state;
		
		
		
	private:
	
		std::string name;
		
		GtkWidget *counter;
		
		GdkPixbuf *baseBuffer;
		
		GtkWidget *image;
		
		int id;
		
		static int _id;
		
		static std::map<int, Counter *> objects;
		
		int diagonal;
		
		GtkWidget *makePiece();
		
};
 
