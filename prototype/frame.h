#ifndef FRAME_H
#define FRAME_H


#include <QtWidgets>


#include "counter.h"





class Counter;
class Window;
class Repository;




class CentralFrame : public QFrame
{
	
	public:
	
		class Button : public QPushButton
		{
			public:
				Button(const QString &text, QWidget *parent);
		};
		
			
		
		
		
		CentralFrame(QWidget *parent, Window *window, std::string type, QScrollArea *scrollArea = nullptr);
		

		
		QScrollArea *scrollArea;
		Window *window;
		//
		float scaleFraction;
		int startWidth;
		int startHeight;
		std::string backgroundID;
		QColor backgroundColor;
		
		bool anySelected(Counter *counter, Counter *&selected, int &selectedPos);
		void toggleOpenStack(Counter *counter, int sign);
		void closeAllOpenStacks();
		void zoomFraction(float amount, bool set);
		void selectCounter(Counter *counter);
		void setBackground(std::string background);
		void setGrid(std::string zone, Counter::Table *grid);
		void setBorders(std::string zone, Counter::Table *borders);
		
		
		static bool openStackoffset;
		static bool facingMatters;
		static bool showGrid;
		static QColor gridColor;
		static int gridRadius;
		static bool allowPainting;
		
		
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
		
		typedef struct
		{
			int x;
			int y;
			const QString text;
			int hx;
			int hy;
		} GridPoint;
		
		
		
		
		typedef struct
		{
			int id;				// id int of counter moved 
			std::string name;	// id string of counter moved 
			std::string tag;	// window tag of counter
			int x;				// position of counter moved
			int y;
			int cx;
			int cy; 
			int dx;				// multi-stack differernce from source stack dragged
			int dy;
		} Move;
		
		typedef struct std::vector<Move> Moved;
	

		
			
	protected:
	
		virtual void mousePressEvent(QMouseEvent *event);
		virtual void dragEnterEvent(QDragEnterEvent *event);
		virtual void dropEvent(QDropEvent *event);
		virtual void dragMoveEvent(QDragMoveEvent *event);
		virtual void wheelEvent(QWheelEvent *event);	
		virtual void paintEvent(QPaintEvent *event);
		
		
		
		
	private:
		
		std::map<Point, int> stackOpen;		// int is sign for offset
		void selectStack(Counter *counter);			
		void unselect();
		QImage selectedGhostImage(Counter *counter, QRect &totalRect);
		std::map<std::string, std::vector<GridPoint>> gridCoordinates;
		std::map<std::string, std::vector<Point>> borders;
		
		
		
};

#endif // FRAME_H
