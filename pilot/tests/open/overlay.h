#include <QtWidgets>


#include "frame.h"


class Overlay : public QFrame
{
	
	public:
	
		class FlowLayout : public QLayout
		{
			public:
				explicit FlowLayout(QWidget *parent);
				~FlowLayout();
				void addItem(QLayoutItem *item) override;
				int count() const override;
				QSize sizeHint() const override;
				QLayoutItem *itemAt(int index) const override;
				QLayoutItem *takeAt(int index) override;
				void setGeometry(const QRect &rect) override;
				void doLayout(const QRect &rect);
				void swapWidget(int fromIndex, int toIndex);
				void updateStackOrder();
				static int maxColumns;
				static int layoutWidth;
				static int layoutHeight; 
				
			private:
				QList<QLayoutItem *> itemList;	
				
		};
		
		
		class StackFrame : public QLabel
		{
			
			public:
			
				StackFrame(QWidget *parent);
				void setPostion(QPoint point);
				void setImage(QImage image);
				void setImages(int box, CentralFrame::Stack stack);
				QLabel *grid;
				FlowLayout *layout;
				QImage image;
				QPoint hotspot;
						
			protected:
				virtual void paintEvent (QPaintEvent *e);
				
			private:	
				QLabel *icon;
				void moveThis(int w, int h);

		};
		
		
		class StackOpen : public QFrame
		{
			
			public:
			
				class Item : public QLabel
				{
					public:			
						Item(StackOpen *parent);
						Counter *source;
				};
				
				StackOpen(QWidget *parent);
				void setImages(CentralFrame::Stack stack);
				FlowLayout *layout;
				
				void moveThis(QPoint hotspot, int w, int h);
				QPoint currentPos;
				
				void showStack();
				void hideStack(); 
						
			protected:
				virtual void mousePressEvent(QMouseEvent *event);
				virtual void dragEnterEvent(QDragEnterEvent *event);
				virtual void dropEvent(QDropEvent *event);
				
				
			private:	
				QLabel *icon;
				

		};
			
			
		Overlay();
		
		static Overlay *overlay;
		
		QRegion overlayMask;
		
		static int border;
		
		static Overlay::StackFrame *hooverView;
		static Overlay::StackOpen *openView;
		
		
					
	protected:
	
		virtual void paintEvent(QPaintEvent *e);
		
};
