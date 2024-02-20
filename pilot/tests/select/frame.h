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
	
		
		struct pt 
		{
			int x; 
			int y;
			
			bool operator <(const pt &p) const
			{
				return (x < p.x) || (!(p.x < x) && y < p.y);
			}
			bool operator ==(const pt &p) const
			{
				return (x == p.x) && (y == p.y);
			}
			friend QDataStream& operator <<(QDataStream& stream, const pt &p) 
			{
				return stream << p.x << p.y;
			}
			friend QDataStream& operator >>(QDataStream& stream, pt &p) 
			{
				return stream >> p.x >> p.y;
			}

		}; 
		typedef struct pt Point;
		
		
		typedef struct std::map<int, Counter *> Stack;
		typedef struct std::map<Point, Stack> Stacks;
		
		
			
		virtual void mousePressEvent(QMouseEvent *event);
		virtual void dragEnterEvent(QDragEnterEvent *event);
		virtual void dropEvent(QDropEvent *event);
				
	protected:
		
		virtual void paintEvent (QPaintEvent *e);
		
	private:
		
		void stackSelect(Counter *counter);
		bool anySelected(Counter *counter);
		void unselect();
		QImage stackGhostImage(Counter *counter, QRect &totalRect);
		QImage selectedGhostImage(QRect &totalRect);
		
};

#endif // FRAME_H
