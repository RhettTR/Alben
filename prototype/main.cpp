// NOTE: This is a feasibility test, not production code.
// The code is unfinished in many respects. 


#include <QApplication>
#include <QMainWindow>
#include <QtWidgets>

#include "luau.h"
#include "counter.h"
#include "scale.h"
#include "overlay.h"
#include "io.h"
#include "toolbar.h"
#include "settings.h"
#include "window.h"






Luau l;


IO *io;
Scale *scaled;


class MainWindow;
MainWindow *window;
Window *repositoryWindow;
Window *logWindow;



//QWidget *container;

ToolBar *mainToolBar;
QAction *beginLogAction;
QAction *endLogAction;



void reload ();


/*void refresh()
{		
	Luau::refresh();
}*/


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

void help ()
{
	
}

void cancel ()
{
	Overlay::openView->hideStack();
	Overlay::hooverView->setVisible(false);
	Window::getInstance("main")->frame->closeAllOpenStacks();
}

void close ()
{
	io->close();
}

void load ()
{
	io->loadGame();
}

void save ()
{
	io->saveGame("Save Game As", "Save Files (*.vsav);;All Files(*.*)", "vsav");
}





QString file = nullptr;

void beginLog ()
{
	if (IO::stepping)
		return;
	
	file = io->saveGame("Save Log As", "Save Files (*.vlog);;All Files(*.*)", "vlog");	
	if (file.isNull() || file.isEmpty())
		return; 
	beginLogAction->setEnabled(false);
	IO::recording = true;
	endLogAction->setEnabled(true);
	mainToolBar->enabled("__record", true);
	mainToolBar->enabled("__undo", false);
	mainToolBar->enabled("__redo", false);
	Luau::saveStage();
}

void endLog ()
{
	beginLogAction->setEnabled(true);
	IO::recording = false;
	endLogAction->setEnabled(false);
	mainToolBar->enabled("__record", false);
	io->saveLog(file);	
}

void step()
{
	Luau::logStep(true);
}

void gotoEnd()
{
	Luau::logStep(false);
}

void abortLog()
{
	Luau::logAbort();
}

void windows()
{
	logWindow->show();
	logWindow->activateWindow();
}




class MainWindow : public Window
{
	public:
		
		MainWindow() : Window(nullptr, "Window", "main", "notype")
		{
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


void reload ()
{
	printf("loading ...\n");
	
	l.closeVM();
	
	Counter::deleteAll();
	
	repositoryWindow->reset();
	mainToolBar->reset();
	io->reset();
	window->deleteWidgets();
	
	
	l.startVM();
			
	printf("done\n");
}





int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    
    qRegisterMetaType<ActionData>("ActionData");
    
    std::srand(std::time({})); // seed for random generator
    

    window = new MainWindow();
	
	
	QMenuBar *menuBar = window->menuBar();
	
    menuBar->setStyleSheet("QMenuBar {background-color: gainsboro}");
    menuBar->setFixedHeight(22);
 
    
    // File
    
    QMenu *fileMenu = menuBar->addMenu("&File");
         
    
    QAction *loadAction = new QAction("&Load Game or Log...");
    QObject::connect(loadAction, &QAction::triggered, &load);
    fileMenu->addAction(loadAction);
    
    QAction *saveAction = new QAction("S&ave Game As...");
    QObject::connect(saveAction, &QAction::triggered, &save);
    fileMenu->addAction(saveAction);
    
    QAction *closeAction = new QAction("&Close Game");
    QObject::connect(closeAction, &QAction::triggered, &close);
    fileMenu->addAction(closeAction);
    
    fileMenu->addSeparator();
    
    beginLogAction = new QAction("&Begin logfile...");
    QObject::connect(beginLogAction, &QAction::triggered, &beginLog);
    fileMenu->addAction(beginLogAction);
    
    endLogAction = new QAction("&End logfile");
    QObject::connect(endLogAction, &QAction::triggered, &endLog);
    endLogAction->setEnabled(false);
    fileMenu->addAction(endLogAction);
    
    fileMenu->addSeparator();         
         
    QAction *reloadAction = new QAction("Reload");
    reloadAction->setShortcut(QKeySequence("Ctrl+R"));
    QObject::connect(reloadAction, &QAction::triggered, &reload);
    fileMenu->addAction(reloadAction);
    
    /*QAction *refreshAction = new QAction("Refresh");
    refreshAction->setShortcut(QKeySequence("Ctrl+E"));
    QObject::connect(refreshAction, &QAction::triggered, &refresh);
    fileMenu->addAction(refreshAction);*/
    
    reloadAction = new QAction("Undo");
    reloadAction->setShortcut(QKeySequence("Ctrl+Z"));
    QObject::connect(reloadAction, &QAction::triggered, &undo);
    fileMenu->addAction(reloadAction);
    
    reloadAction = new QAction("Redo");
    reloadAction->setShortcut(QKeySequence("Ctrl+Y"));
    QObject::connect(reloadAction, &QAction::triggered, &redo);
    fileMenu->addAction(reloadAction);
    
    
    // Edit
    
    QMenu *editMenu = menuBar->addMenu("&Edit");
    QAction *settingsAction = new QAction("Setti&ngs");
    QObject::connect(settingsAction, &QAction::triggered, &settings);
    editMenu->addAction(settingsAction);
    
    // Windows
    
    QMenu *windowsMenu = menuBar->addMenu("&Windows");
    QAction *chatAction = new QAction("Show &LogChat window");
    QObject::connect(chatAction, &QAction::triggered, []()
					 { 
						logWindow->show();
						logWindow->activateWindow();
					 });
    windowsMenu->addAction(chatAction);
    
    QAction *repositoryAction = new QAction("Show &Repository window");
    QObject::connect(repositoryAction, &QAction::triggered, []()
					 { 
						repositoryWindow->show();
						repositoryWindow->activateWindow();
					 });
    windowsMenu->addAction(repositoryAction);
    
    
    // Help
    
    QMenu *helpMenu = menuBar->addMenu("&Help");
   
    
    
    
    window->move(250, 150);
	window->resize(1120, 500 + menuBar->height());
	
	
	
	// load resources
	io = new IO();
	
	// scale resources
	scaled = new Scale();
	
	
	
	
	Overlay *overlay = new Overlay(window->container, window->scrollArea);
	Overlay::overlay = overlay;
	
	overlay->raise();
	
	
	Overlay::hooverView = new Overlay::StackFrame(overlay);
	Overlay::openView = new Overlay::StackOpen(overlay);
	
	
	// tool bar
	
	mainToolBar = new ToolBar("main", "System toolbar", window->scrollArea);
	mainToolBar->setIconSize(QSize(32, 24));
	window->addToolBar(Qt::TopToolBarArea, mainToolBar);
	
	
	mainToolBar->addImageButton("__undo", "__undo", "", "Undo last move", &undo);
	mainToolBar->addImageButton("__redo", "__redo", "", "Redo next move", &redo);
	mainToolBar->addSeparator();
	mainToolBar->addImageButton("__forward", "__forward", "", "Logfile step forward", &step);
	mainToolBar->addImageButton("__end", "__end", "", "Logfile go to end", &gotoEnd);
	mainToolBar->addImageButton("__abort", "__abort", "", "Abort logfile", &abortLog);
	mainToolBar->addLabel("__record", "__record", "Logfile recording", 24, 24, "");
	mainToolBar->addSeparator();
	mainToolBar->addImageButton("__pluss", "__pluss", "", "Zoom in", [=]()->void{ mainToolBar->zoomIn(); });	
	mainToolBar->addSizeComboBox();	
	mainToolBar->addImageButton("__minus", "__minus", "", "Zoom out", [=]()->void{ mainToolBar->zoomOut(); });	
	mainToolBar->enabled("__pluss", true);
	mainToolBar->enabled("__minus", true);
	mainToolBar->addSeparator();
	mainToolBar->addImageButton("__movedicon", "__movedicon", "", "Delete all moved-makers", &clearMoved);
	mainToolBar->enabled("__movedicon", true);
	

	
	
	// default rights table
	
	Settings::myOwnershipRights = {0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0};

	
	
	
   
	// create repository window
	
    repositoryWindow = new Window(window, "Counters", "Repository", "Layout");
    
    
    // create log / chat window
	
    logWindow = new Window(window, "Log / Chat", "LogChat", "Layout");
    


    
    
    l.startVM();
    
    
    
    scaled->resourceScaleRotate("main", window->frame->backgroundID);
	
    window->show();
    if (repositoryWindow->isVisible())
		repositoryWindow->show();
    logWindow->show();
    
      
    int res = app.exec();   
    
    
    
    l.closeVM();
    
    return res;
}
