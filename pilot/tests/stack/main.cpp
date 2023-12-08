// NOTE: This is a feasibility test, not production code.
// The code is unfinished in many respects. 


#include <QApplication>
#include <QMainWindow>
#include <QtWidgets>

#include "luau.h"
#include "counter.h"
#include "stackframe.h"
#include "window.h"





Luau l;



CentralFrame *mapFrame;
CentralFrame *repositoryFrame;


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
	//window.resize(600, 400);


    mapFrame = new CentralFrame(&window, "Map");
    mapFrame->setObjectName("centralFrame");
    mapFrame->setFixedSize(600, 400);	
	mapFrame->setStyleSheet("#centralFrame {background-image: url('./images/Map.jpg');}");
	
	window.setCentralWidget(mapFrame);
	
	
	Counter::Overlay *overlay = new Counter::Overlay();
	overlay->setParent(&window);
	overlay->setFixedSize(window.width(), window.height());
	Counter::Overlay::overlay = overlay;
	
	overlay->raise();
	
	
	Counter::hooverView = new StackFrame(Counter::Overlay::overlay);

	
	window.show();
	
	
   
	// create repository window
	
	repositoryWindow = new Window(&window);
    
    repositoryFrame = new CentralFrame(repositoryWindow, "Repository");
    repositoryWindow->setCentralWidget((QWidget *)repositoryFrame);
    
   
    
    Counter::resources["_Moved"] = "./Moved.png";
    
    
    l.startVM();
      
    int res = app.exec();   
    
    
    
    l.closeVM();
    
    return res;
}
