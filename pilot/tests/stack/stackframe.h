#ifndef STACKFRAME_H
#define STACKFRAME_H


#include <QtWidgets>

#include "counter.h"


class CentralFrame;


class StackFrame : public QLabel
{
	
	public:
	
		StackFrame(QWidget *parent);
		void setPostion(QPoint point);
		void setImage(QImage image);
		void setImages(int box, CentralFrame::Stack stack);
		QLabel *grid;
		QVBoxLayout *rows;
		static int border;
		static int maxColumns;
		QImage image;
		QPoint hotspot;
				
	protected:
		virtual void paintEvent (QPaintEvent *e);
		
	private:	
		QLabel *icon;
		void moveThis(int w, int h);

};

#endif // STACKFRAME_H
