#ifndef FRAME_H
#define FRAME_H


#include <QtWidgets>


#include "counter.h"


class Counter;


class CentralFrame : public QFrame
{
	
	public:
		
		CentralFrame(QWidget *parent, std::string name);
		
		std::string name;
		
		
		typedef struct std::map<int, Counter *> Stack;
		
		struct pt 
		{
			int x; 
			int y;
			
			bool operator <(const pt &p) const
			{
				return (x < p.x) || (!(p.x < x) && y < p.y);
			}
		}; 
		typedef struct pt Point;
			
		virtual void mousePressEvent(QMouseEvent *event);
		virtual void dragEnterEvent(QDragEnterEvent *event);
		virtual void dropEvent(QDropEvent *event);
				
	protected:
		
		virtual void paintEvent (QPaintEvent *e);
		
		
};

#endif // FRAME_H
