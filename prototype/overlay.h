#ifndef OVERLAY_H
#define OVERLAY_H

#include <QtWidgets>


#include "frame.h"


class Overlay : public QFrame
{
	
	public:
	
		class FlowLayout : public QLayout
		{
			public:
				explicit FlowLayout(QWidget *parent = nullptr);
				~FlowLayout();
				void addItem(QLayoutItem *item) override;
				Qt::Orientations expandingDirections() const override;
				bool hasHeightForWidth() const override;
				int heightForWidth(int) const override;
				int count() const override;
				QSize sizeHint() const override;
				QLayoutItem *itemAt(int index) const override;
				QSize minimumSize() const override;
				QLayoutItem *takeAt(int index) override;
				void setGeometry(const QRect &rect) override;	
				int doLayout(const QRect &rect, bool measure) const;
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
				void setImage(QImage image);
				void setImages(CentralFrame::Stack stack);
				QLabel *grid;
				FlowLayout *layout;
				QImage image;
				void moveThis(QPoint hotspot, int w, int h);
						
			protected:
				virtual void paintEvent (QPaintEvent *e);
				
			private:	
				QLabel *icon;
				

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
				void open(Counter *counter, QPoint pos);
				void swapWidget(int fromIndex, int toIndex);
				void updateImages(Counter *counter);
				FlowLayout *layout;
				
				void moveThis(QPoint hotspot, int w, int h);
				
				void showStack();
				void hideStack(); 
						
			protected:
				virtual void mousePressEvent(QMouseEvent *event);
				virtual void dragEnterEvent(QDragEnterEvent *event);
				virtual void dropEvent(QDropEvent *event);
				
				
			private:	
				QLabel *icon;
				

		};
		
		
			
		Overlay(QWidget *parent, QScrollArea *scrollArea);
		
		QScrollArea *scrollArea;
		
		void setMasks();
		
		static Overlay *overlay;
		
		QRegion overlayMask;
		
		static int border;
		
		
		
		static Overlay::StackFrame *hooverView;
		static Overlay::StackOpen *openView;
		
		
					
	protected:
	
		virtual void paintEvent(QPaintEvent *e);
		
};

#endif // OVERLAY_H
