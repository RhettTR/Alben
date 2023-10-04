#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "frame.h"

#include <QtWidgets>





class Counter
{
	
	public:
	
		class QtCounter : public QLabel
		{
			public:
				Counter *owner;				
				QtCounter(Counter *owner);
			protected:
				virtual void mousePressEvent (QMouseEvent * e);
				virtual void paintEvent(QPaintEvent *e);
			private:
				void do_activate (QAction *action);
		};
		
		class Overlay : public QFrame
		{
			public:
				Overlay();
				static Overlay *overlay;
				
			protected:
				virtual void paintEvent(QPaintEvent *e);	
					
		};
			
		
	
		int id;						// id of this counter		
		std::string name;			// unique string id of this counter
		
		
		typedef struct std::map<int, Counter *> Stack;
		
		
		struct State
		{
			int x;
			int y;
			bool moved;				// only true if trait		
			int degrees;			// only not zero if trait
			std::string image;		// name of current (flipped) image	
		};
		
		
		Counter(const char *name);
		Counter(int id, const char *image, int degrees, int x, int y);
		
		~Counter();
		
		
		CentralFrame *parentFrame;
		
		Counter::QtCounter *counter;
		
		QImage baseBuffer;
	
		
		int width;		
		int height;	
		
		static void deleteAll();
		
		static int nextId();
		static int topZorder();
		static Counter* findObj(const char *name);
		static void snaptoDefaultGrid (Counter *counter, int *x, int *y);
		
		static void setGUI();
		static void setStack(Stack stack);
		void setImage(int offsetFactor, int stackSize);
		
		void setMovedMask(const char *mask, bool moved);
		
		int incrementDegrees(); 
		
		void rotateImage();
		
		State state;
		
		static std::map<std::string, std::string> resources;
							     					      
							      
		static float alpha;
		static bool  haveOffset;
		static int   stackOffset;
		
		static int mask_x;
		static int mask_y;
			
		int zorder;				// position in a stack
		bool marker;			// true if a stack size marker is to be drawn
		int stackSize;			// the size of the counter's stack
		int margin;				// the area around the counter for rendering masks	
			
			
			
	private:
	
		static int _id;			// id of latest counter	
		
		static int _lastZorder;	// the number itself is not important, 
								// only its relative value to other counters in a stack
			
								
		static std::map<int, Counter *> repository;
		static std::map<int, Counter *> counters;
		
};
 
