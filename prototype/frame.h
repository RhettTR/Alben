#ifndef FRAME_H
#define FRAME_H


#include <QtWidgets>


#include "counter.h"




class Counter;
class Window;




class CentralFrame : public QFrame
{
	
	public:
	
		class Button : public QPushButton
		{
			public:
				Button(const QString &text, QWidget *parent);
		};
		
		class Area : public QLabel
		{
			public:	
				Area(QFrame *parent);
				~Area();
				Counter *counter;
				void showRightClickMenu(Counter *counter);
				void execute(QAction *action);
				bool findCounter(QPoint point, Counter *&counter);
		};
			
		
		
		
		
		CentralFrame(QWidget *parent, std::string name, Window *window, QScrollArea *scrollArea = nullptr);
		
		std::string name;
		
		QScrollArea *scrollArea;
		Window *window;
		
		bool anySelected(Counter *counter, Counter *&selected);
		void toggleOpenStack(Counter *counter, int sign);
		void closeAllOpenStacks();
		void selectCounter(Counter *counter);
		
		
		
		static std::string backgroundID;
		static bool openStackoffset;
		static bool facingMatters;
		static Area *area;
		
		
		static QScrollArea *buttonParent;
		static void createButton(const char *id, const char *text, const char *handler, int x, int y, int w, int h);
		static void deleteButton(const char *id);
		
		
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
			struct pt operator *(const float &f) const
			{
				struct pt point;
				point.x = std::round((float)x * f);
				point.y = std::round((float)y * f);
				return point;
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
		
		
	

		
			
	protected:
	
		virtual void mousePressEvent(QMouseEvent *event);
		virtual void dragEnterEvent(QDragEnterEvent *event);
		virtual void dropEvent(QDropEvent *event);
		virtual void dragMoveEvent(QDragMoveEvent *event);
		virtual void resizeEvent(QResizeEvent* event);
		virtual void wheelEvent(QWheelEvent *event);	
		virtual void paintEvent (QPaintEvent *e);
		
		
		
	private:
		
		std::map<Point, int> stackOpen;		// int is sign for offset
		void selectStack(Counter *counter);			
		void unselect();
		QImage stackGhostImage(Counter *counter, QRect &totalRect);
		QImage selectedGhostImage(QRect &totalRect);
		
};

#endif // FRAME_H
