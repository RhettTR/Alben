#ifndef COUNTER_H
#define COUNTER_H


#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "frame.h"

#include <QtWidgets>



class CentralFrame;
class StackFrame;

class Counter
{
	
	public:
	
	
		class QtCounter : public QLabel
		{
			public:
				Counter *owner;				
				QtCounter(Counter *owner);
				~QtCounter();
				QTimer *timer;
			protected:
				virtual void mousePressEvent (QMouseEvent *e);
				virtual void mouseMoveEvent (QMouseEvent *e);
				virtual void leaveEvent (QEvent *e);
			private:
				void do_activate (QAction *action);
				QPoint popupPoint;
				void hooverAction();	
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
		
		void setImage();
		static void setGUI();
		
		int incrementDegrees(); 
		
		
		State state;
		
		static std::map<std::string, std::string> resources;
							     					      
							      
		static float alpha;
		static bool  haveOffset;
		static int   stackOffset;
		
		static int mask_x;
		static int mask_y;
			
		int zorder;				// position in a stack
		int margin;				// the area around the counter for rendering masks
		
		static StackFrame *hooverView;
		
				
		static std::map<int, Counter *> counters;
		
		static std::map<int, Counter *> repository;
		
			
	private:
	
		static int _id;			// id of latest counter	
		
		static int _lastZorder;	// the number itself is not important, 
								// only its relative value to other counters in a stack	
		
		
};
 
#endif // COUNTER_H
