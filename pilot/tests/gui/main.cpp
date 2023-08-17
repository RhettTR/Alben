// NOTE: This is a feasibility test, not production code.
// The code is unfinished in many respects. 


#include <QApplication>
#include <QMainWindow>
#include <QtWidgets>

#include "luau.h"
#include "counter.h"




Luau l;




void reload ()
{
	printf("loading ...\n");
	
	l.closeVM();
	
	Counter::deleteAll();
	
	l.startVM();
			
	printf("done\n");
}


void undo ()
{
	Luau::undo();
}


void redo ()
{
	Luau::redo();
}



class MainWindow : public QMainWindow
{
	protected:
		void keyPressEvent(QKeyEvent *e)
		{
			if (e->modifiers() == Qt::ControlModifier)
			{
				switch (e->key())
				{
					case Qt::Key_R: reload (); break;
					case Qt::Key_Z: undo (); break;
					case Qt::Key_Y: redo (); break;
					default: break;
				}
			}
		}
		
};



class CentralFrame : public QFrame
{	
	public:
	
		CentralFrame(QWidget *parent) : QFrame(parent)
		{
			setAcceptDrops(true);
		}

		
		Counter::QtCounter *dragged = NULL;
		

		
		void dragEnterEvent(QDragEnterEvent *event)
		{		
			if (event->mimeData()->hasFormat("application/x-alben-counter")) 
			{
				if (event->source() == this) 
				{
					event->setDropAction(Qt::MoveAction);
					event->accept();
				} 
				else 
				{
					event->acceptProposedAction();
				}
			} 
			else 
				event->ignore();
		}
		
		
		void dragMoveEvent(QDragMoveEvent *event)
		{
			if (event->mimeData()->hasFormat("application/x-alben-counter")) 
			{
				if (event->source() == this) 
				{
					event->setDropAction(Qt::MoveAction);
					event->accept();
				} 
				else 
				{
					event->acceptProposedAction();
				}
			} 
			else 
				event->ignore();			
		}
		
		
		void dropEvent(QDropEvent *event)
		{
			if (event->mimeData()->hasFormat("application/x-alben-counter")) 
			{
				QByteArray itemData = event->mimeData()->data("application/x-alben-counter");
				QDataStream dataStream(&itemData, QIODevice::ReadOnly);

				QPixmap pixmap;
				QPoint offset;
				dataStream >> offset;

				/*
				QLabel *newIcon = new QLabel(this);
				newIcon->setPixmap(pixmap);
				newIcon->move(event->position().toPoint() - offset);
				newIcon->show();
				newIcon->setAttribute(Qt::WA_DeleteOnClose);
				*/
				
				int dx = std::round(event->position().x() - offset.x());
				int dy = std::round(event->position().y() - offset.y());
				
				dx += std::round((dragged->width() - dragged->owner->width) / 2);
				dy += std::round((dragged->height() - dragged->owner->height) / 2);
				
				if (dragged->owner->state.moved && dragged->owner->state.degrees == 0)
				{	
					QString file = QString::fromStdString(Counter::resources["_Moved"]);	
					QImage const mask(file);
					dx -= mask.width();
					dy -= mask.width();		
				}
				
				dragged->owner->state.x = dx;
				dragged->owner->state.y = dy;
				dragged->owner->setImage();
				dragged->raise();				// NB!!
				
				Luau::doEvent("movetrigger", dragged->owner->name.c_str(), "", "", 0);
				Luau::doEvent("move", dragged->owner->name.c_str(), "Counter", "x", dx);
				Luau::doEvent("move", dragged->owner->name.c_str(), "Counter", "y", dy);
				Luau::doEvent("end", "", "", "", 0);
				
				
				if (event->source() == this) 
				{
					event->setDropAction(Qt::MoveAction);
					event->accept();
				} 
				else 
				{
					event->acceptProposedAction();
				}
			} 
			else 
				event->ignore();			
		}
		
		
		void mousePressEvent(QMouseEvent *event)
		{
			Counter::QtCounter *child = static_cast<Counter::QtCounter*>(childAt(event->position().toPoint()));
			if (!child)
				return;

			dragged = child;
			
			
				
			QByteArray itemData;
			QDataStream dataStream(&itemData, QIODevice::WriteOnly);
			dataStream << QPoint(event->position().toPoint() - child->pos());
		
			QMimeData *mimeData = new QMimeData;
			mimeData->setData("application/x-alben-counter", itemData);
				
			
			QPixmap pixmap = QPixmap::fromImage(child->owner->baseBuffer);
				
			QPixmap pix(pixmap.size());
			pix.fill(Qt::transparent);		
			QPainter painter;
			painter.begin(&pix);
			painter.setOpacity(0.5);
			painter.drawPixmap(0, 0, pixmap); 				
			painter.end();
			
			
			QDrag *drag = new QDrag(this);
			drag->setMimeData(mimeData);
			drag->setPixmap(pix);
			drag->setHotSpot(event->position().toPoint() - child->pos());
		
			

			if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction) 
			{
				//child->close();
			} 
			else 
			{
				child->show();
				//child->setPixmap(pixmap);
			}
		}
	
};


CentralFrame *centralFrame;


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    
    window.setWindowTitle("Window");
	
	
	QMenuBar *menuBar = window.menuBar();
	centralFrame = new CentralFrame(&window);
    
    
    menuBar->setStyleSheet("QMenuBar {background-color: gainsboro}");
    menuBar->setFixedHeight(20);
    
    
    centralFrame->setObjectName("centralFrame");	
	centralFrame->setStyleSheet("#centralFrame {background-image: url('./images/Map.jpg');}");
	window.setCentralWidget(centralFrame);
	 
    
    QMenu *fileMenu = menuBar->addMenu("&File");
       
    QAction *reloadAction = new QAction("Reload");
    reloadAction->setShortcut(QKeySequence("Ctrl+R"));
    QObject::connect(reloadAction, &QAction::triggered, &reload);
    fileMenu->addAction(reloadAction);
    
    reloadAction = new QAction("Undo");
    reloadAction->setShortcut(QKeySequence("Ctrl+Z"));
    QObject::connect(reloadAction, &QAction::triggered, &undo);
    fileMenu->addAction(reloadAction);
    
    reloadAction = new QAction("Redo");
    reloadAction->setShortcut(QKeySequence("Ctrl+Y"));
    QObject::connect(reloadAction, &QAction::triggered, &redo);
    fileMenu->addAction(reloadAction);
    
    
	window.resize(600, 400 + menuBar->height());
   
    
    
    window.show();
   
   
    
    Counter::resources["_Moved"] = "./Moved.png";
    
    
    l.startVM();
      
    int res = app.exec();   
    
    
    
    l.closeVM();
    
    return res;
}
