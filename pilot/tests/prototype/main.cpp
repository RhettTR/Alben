// NOTE: This is a feasibility test, not production code.
// The code is unfinished in many respects. 


#include <QApplication>
#include <QMainWindow>
#include <QtWidgets>

#include "luau.h"
#include "counter.h"
#include "scale.h"
#include "overlay.h"
#include "window.h"
#include "io.h"





Luau l;



CentralFrame *mapFrame;
CentralFrame *repositoryFrame;
IO *io;
Scale *scaled;


Window *repositoryWindow;







void reload ()
{
	printf("loading ...\n");
	
	l.closeVM();
	
	Counter::deleteAll();
	
	repositoryWindow->reset();
	
	
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


void cancel ()
{
	Overlay::openView->setVisible(false);
	Overlay::hooverView->setVisible(false);
}



class MainWindow : public QMainWindow
{
	public:
		QScrollArea *scrollArea;
		
		MainWindow() : scrollArea(new QScrollArea)
		{
			setCentralWidget(scrollArea);
		}
		
	protected:
		void resizeEvent(QResizeEvent* event)
		{
		   QMainWindow::resizeEvent(event);	   
		   Overlay::overlay->setFixedSize(event->size());
		}
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
			else
				switch (e->key())
				{
					case Qt::Key_Escape: cancel (); break;				
					default: break;
				}
			
		}
		
};





int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    
    
    
    window.setWindowTitle("Window");
	
	
	QMenuBar *menuBar = window.menuBar();
	
    menuBar->setStyleSheet("QMenuBar {background-color: gainsboro}");
    menuBar->setFixedHeight(20);
 
    
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
    
    
    window.move(500, 150);
	window.resize(600, 400 + menuBar->height());
	
	
	// load resources
	io = new IO();
	
	// scale resources
	scaled = new Scale();
	


    mapFrame = new CentralFrame(&window, "Map", window.scrollArea);
    mapFrame->setObjectName("centralFrame");	
	
	
	window.scrollArea->setWidget(mapFrame);
	
	
	
	Overlay *overlay = new Overlay();
	overlay->setParent(&window);
	overlay->setFixedSize(window.width(), window.height());
	Overlay::overlay = overlay;
	
	overlay->raise();
	
	
	Overlay::hooverView = new Overlay::StackFrame(Overlay::overlay);
	Overlay::openView = new Overlay::StackOpen(Overlay::overlay);

	
	window.show();
	
	
   
	// create repository window
	
	repositoryWindow = new Window(&window);
    
    repositoryFrame = new CentralFrame(repositoryWindow, "Repository");
    repositoryWindow->setCentralWidget((QWidget *)repositoryFrame);
    
   
    
    
    
    l.startVM();
    
    
    scaled->resourceScaleRotate(CentralFrame::backgroundID);
    mapFrame->setFixedSize(scaled->getScaledSize(CentralFrame::backgroundID));
    scaled->coordinatesScaleRotate();	
    
      
    int res = app.exec();   
    
    
    
    l.closeVM();
    
    return res;
}
