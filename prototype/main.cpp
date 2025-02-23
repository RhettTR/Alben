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
#include "toolbar.h"
#include "settings.h"






Luau l;


CentralFrame *mapFrame;
CentralFrame *repositoryFrame;
IO *io;
Scale *scaled;
ToolBar *mainToolBar;

class MainWindow;
MainWindow *window;
Window *repositoryWindow;


QWidget *container;




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

void clearMoved ()
{
	Counter::clearMoved();
}

void settings () 
{
	Settings *dialog = new Settings((QWidget *)window);
	dialog->show();
}

void cancel ()
{
	Overlay::openView->hideStack();
	Overlay::hooverView->setVisible(false);
	mapFrame->closeAllOpenStacks();
}

void load ()
{
	io->loadGame();
}

void save ()
{
	io->saveGame();
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

    window = new MainWindow();
    

    
    window->setWindowTitle("Window");
	
	
	QMenuBar *menuBar = window->menuBar();
	
    menuBar->setStyleSheet("QMenuBar {background-color: gainsboro}");
    menuBar->setFixedHeight(20);
 
    
    QMenu *fileMenu = menuBar->addMenu("&File");
         
    
    QAction *loadAction = new QAction("&Load Game...");
    QObject::connect(loadAction, &QAction::triggered, &load);
    fileMenu->addAction(loadAction);
    
    QAction *saveAction = new QAction("S&ave Game As...");
    QObject::connect(saveAction, &QAction::triggered, &save);
    fileMenu->addAction(saveAction);         
         
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
    
    
    QMenu *editMenu = menuBar->addMenu("&Edit");
    QAction *settingsAction = new QAction("Setti&ngs");
    QObject::connect(settingsAction, &QAction::triggered, &settings);
    editMenu->addAction(settingsAction);
    
    
    window->move(500, 150);
	window->resize(600, 400 + menuBar->height());
	
	
	
	// load resources
	io = new IO();
	
	// scale resources
	scaled = new Scale();
	
	
	
	container = new QWidget(window);
	container->setAcceptDrops(true);
		
	window->scrollArea->setWidget(container);
	
	
    mapFrame = new CentralFrame(container, "Map", window->scrollArea);
    mapFrame->setObjectName("centralFrame");
	
	
	
	
	Overlay *overlay = new Overlay(container, window->scrollArea);
	Overlay::overlay = overlay;
	
	overlay->raise();
	
	
	Overlay::hooverView = new Overlay::StackFrame(overlay);
	Overlay::openView = new Overlay::StackOpen(overlay);
	
	
	// tool bar
	
	mainToolBar = new ToolBar(window, window->scrollArea);
	window->addToolBar(Qt::TopToolBarArea, mainToolBar);
	mainToolBar->setIconSize(QSize(32, 24));

	
	mainToolBar->addImageButton("__undo", "Undo last move", &undo);
	mainToolBar->addImageButton("__redo", "Redo next move", &redo);
	mainToolBar->addSeparator();
	mainToolBar->addImageButton("__pluss", "Zoom in", [=]()->void{ mainToolBar->zoomIn(); });	
	mainToolBar->addSizeComboBox();	
	mainToolBar->addImageButton("__minus", "Zoom out", [=]()->void{ mainToolBar->zoomOut(); });	
	mainToolBar->enable("__pluss");
	mainToolBar->enable("__minus");
	mainToolBar->addSeparator();
	mainToolBar->addImageButton("__movedicon", "Delete all moved-makers", &clearMoved);
	mainToolBar->enable("__movedicon");
	
	
	
	// default rights table
	
	Settings::myOwnershipRights = {0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0};
	
	
	
	window->show();
	
	
	
	
   
	// create repository window
	
    repositoryFrame = new CentralFrame(repositoryWindow, "Repository");
    
    repositoryWindow = new Window(window);
    repositoryWindow->setCentralWidget((QWidget *)repositoryFrame);
    
    
	
    
    
    
    
    l.startVM();
    
    
    
    scaled->resourceScaleRotate(CentralFrame::backgroundID);
	
    
      
    int res = app.exec();   
    
    
    
    l.closeVM();
    
    return res;
}
