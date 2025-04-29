// NOTE: This is a feasibility test, not production code.
// The code is unfinished in many respects. 


#include <QApplication>
#include <QMainWindow>
#include <QtWidgets>

#include "luau.h"
#include "counter.h"
#include "scale.h"
#include "overlay.h"
#include "repository.h"
#include "io.h"
#include "toolbar.h"
#include "settings.h"
#include "window.h"






Luau l;


CentralFrame *mapFrame;
CentralFrame *repositoryFrame;
IO *io;
Scale *scaled;


class MainWindow;
MainWindow *window;
Repository *repositoryWindow;
Window *artilleryWindow;

void createWindow(QString, std::string, std::string);
void show();


QWidget *container;

ToolBar *mainToolBar;
QAction *beginLogAction;
QAction *endLogAction;




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

void createWindow(QString title, std::string name, std::string background)
{
	artilleryWindow = new Window((QWidget *)window, title, name, background);
}


void show()
{	
	if (artilleryWindow->isHidden())
		artilleryWindow->show();
	else
		artilleryWindow->hide();
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
    menuBar->setFixedHeight(22);
 
    
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
	mainToolBar->addImageButton("__forward", "Logfile step forward", &step);
	mainToolBar->addImageButton("__end", "Logfile go to end", &gotoEnd);
	mainToolBar->addImageButton("__abort", "Abort logfile", &abortLog);
	mainToolBar->addLabel("__record", "Logfile recording");
	mainToolBar->addSeparator();
	mainToolBar->addImageButton("__pluss", "Zoom in", [=]()->void{ mainToolBar->zoomIn(); });	
	mainToolBar->addSizeComboBox();	
	mainToolBar->addImageButton("__minus", "Zoom out", [=]()->void{ mainToolBar->zoomOut(); });	
	mainToolBar->enabled("__pluss", true);
	mainToolBar->enabled("__minus", true);
	mainToolBar->addSeparator();
	mainToolBar->addImageButton("__movedicon", "Delete all moved-makers", &clearMoved);
	mainToolBar->enabled("__movedicon", true);
	mainToolBar->addSeparator();
	
	
	
	// default rights table
	
	Settings::myOwnershipRights = {0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0};
	
	
	
	window->show();
	
	
	
	
   
	// create repository window
	
    repositoryFrame = new CentralFrame(repositoryWindow, "Repository");
    
    repositoryWindow = new Repository(window);
    repositoryWindow->setCentralWidget((QWidget *)repositoryFrame);
    
    
    
    
    
    
    l.startVM();
    
    
    
    scaled->resourceScaleRotate(CentralFrame::backgroundID);
	
    
      
    int res = app.exec();   
    
    
    
    l.closeVM();
    
    return res;
}
