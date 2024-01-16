#include <math.h>


#include <QtWidgets>

#include "counter.h"
#include "overlay.h"
#include "luau.h"
#include "window.h"
	



using namespace std;



int Counter::_id = 0;
int Counter::_lastZorder = 0;

map<int, Counter *> Counter::repository;
map<int, Counter *> Counter::counters;
map<string, string> Counter::resources;




float Counter::alpha = 1.0;

bool Counter::haveOffset = false;
int Counter::stackOffset = 5;



int Counter::mask_x = 0;
int Counter::mask_y = 0;




extern CentralFrame *mapFrame;
extern CentralFrame *repositoryFrame;
extern Window *repositoryWindow;




static Luau::PopupEntries popupentries;




	
Counter::QtCounter *owner;



Counter::QtCounter::QtCounter(Counter *owner) : QLabel(owner->parentFrame)
{		
	this->setAttribute(Qt::WA_DeleteOnClose);		
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	this->owner = owner;
	this->timer = new QTimer(this);
	this->timer->setSingleShot(true);
	connect(timer, &QTimer::timeout, this, [=]()->void{ hooverAction(); });
	
	this->setMouseTracking(true);
}



Counter::QtCounter::~QtCounter()
{
	this->timer->stop();
	delete this->timer;
}

 

void Counter::QtCounter::mousePressEvent (QMouseEvent * e) 
{
	if (e->button() == Qt::RightButton)
	{

		
		QMenu MyMenu(this);
		
		if (this->actions().isEmpty())
		{
			popupentries.clear();
		
			QByteArray ba = QString::fromStdString(this->owner->parentFrame->name).toLocal8Bit();
			const char *window = ba.data();
			
			popupentries = Luau::getTraits(window, this->owner->name.c_str());
			
			
			
		
			for (auto e : popupentries)
			{
				 
				const QString name = QString::fromStdString(e.entryname);
				
				QAction* action = new QAction(name, this);
				
				QVariant v = QVariant(QString(e.entryaction));
				action->setData(v);
				
				if (strcmp(window, "Repository") == 0 && 
				   (e.entryname == "Delete" || e.entryname == "Moved"))
					action->setEnabled(false);
				
				//action->setShortcut(QKeySequence("Ctrl+D"));
			   
				this->addAction(action);
				
				QObject::connect( action, &QAction::triggered, this, [=]()->void{ do_activate(action); } );
				
			}
		}
		
		MyMenu.addActions(this->actions());
		
		MyMenu.exec(QCursor::pos());
	}
	
	else 
	
		e->ignore();
	
	
}



void Counter::QtCounter::hooverAction()
{	
		
	
	if (!Overlay::openView->isHidden())
		return;
		
		
	
	CentralFrame::Stack stack = {{this->owner->zorder, this->owner}};
	
	QPoint point = QPoint(this->owner->state.x, this->owner->state.y);
	
	
	for ( auto obj = counters.begin(); obj != counters.end(); ++obj  )	
	{
		QPoint p = QPoint(obj->second->state.x, obj->second->state.y);
		
		if (p == point)		
			stack[obj->second->zorder] = obj->second;	
	}
	
	
	
	
	int cellWidth = stack.begin()->second->width;
	int cellHeight = stack.begin()->second->height;

	
	string filename = "./images/Map.jpg";
	
	QImage baseImage = QImage(QString::fromStdString(filename));
	
	int centerX = this->owner->state.x + (int)(cellWidth / 2);
	int centerY = this->owner->state.y + (int)(cellHeight / 2);

	int imageSize = cellHeight + 2*Overlay::border;
	
	QImage background = baseImage.copy(centerX - (int)(imageSize / 2), centerY - (int)(imageSize / 2), 
									   imageSize, imageSize);

	Overlay::hooverView->setImage(background);
	
	

	
	int box = imageSize + 2*Overlay::border;
	
	Overlay::hooverView->grid->move(box, 2);	
	
	
	
	popupPoint += this->pos();
		
	Overlay::hooverView->setPostion(this->owner->parentFrame->mapToParent(popupPoint));
									
	
	Overlay::hooverView->setImages(box, stack);
	Overlay::hooverView->setVisible(true);
	
}



void Counter::QtCounter::mouseMoveEvent (QMouseEvent * e)
{
	if (this->owner->parentFrame->name == "Map")
	{	
		Overlay::hooverView->setVisible(false);
		popupPoint = e->position().toPoint();		
		timer->start(700);
	}
}



void Counter::QtCounter::leaveEvent (QEvent *e)
{	
	if (this->owner->parentFrame->name == "Map")
	{	
		Overlay::hooverView->setVisible(false);	
		timer->stop();
	}
}

	

void Counter::QtCounter::mouseDoubleClickEvent (QMouseEvent * e)
{
    
    
    if (Overlay::openView->isVisible())
    {
		Overlay::openView->hideStack();
		return;
	}
	
    
    
    if (e->button() == Qt::LeftButton)
    {
       
        QPoint point = QPoint(this->owner->state.x, this->owner->state.y);
        
        
        if (!Overlay::openView->isHidden())
        {
			Overlay::openView->hideStack();
			
			if (Overlay::openView->currentPos == point)
				return;
		}
			
		Overlay::openView->currentPos = point;
		
			
        
        CentralFrame::Stack stack = {{this->owner->zorder, this->owner}};
        
		
		for ( auto obj = counters.begin(); obj != counters.end(); ++obj  )	
		{
			QPoint p = QPoint(obj->second->state.x, obj->second->state.y);
			
			if (p == point)		
				stack[obj->second->zorder] = obj->second;	
		}
		
		
		QPoint popup = e->position().toPoint() + this->pos();
		popup = this->owner->parentFrame->mapToParent(popup);
		
		
        Overlay::openView->setImages(stack);
        Overlay::openView->showStack();
        Overlay::openView->resize(Overlay::FlowLayout::layoutWidth + 4, Overlay::FlowLayout::layoutHeight + 4);	
		Overlay::openView->moveThis(popup, Overlay::FlowLayout::layoutWidth + 4, Overlay::FlowLayout::layoutHeight + 4);
        
        
    }
    
    else 
	
		e->ignore();
    
}



				
void Counter::QtCounter::do_activate (QAction *action)
{
	QVariant v = action->data();
	QString str = (QString) v.value<QString>();
	QByteArray ba = str.toLocal8Bit();
	const char *entryaction = ba.data();
	QByteArray bb = QString::fromStdString(this->owner->parentFrame->name).toLocal8Bit();
	const char *window = bb.data();			
	Luau::doAction(window, this->owner->name.c_str(), entryaction);			
}
	
	
	

	


// constructor for the repository

Counter::Counter(const char *name)		
{
	
	this->id = Counter::nextId();
	
	Counter::repository[this->id] = this;
	
	
	this->name = string(name);
	
	this->state.x = repositoryWindow->allocateX();
	this->state.y = repositoryWindow->allocateY();
	this->state.moved = false;
	this->state.degrees = 0;
	this->state.image = this->name;
	
	string filename = "./images/" + this->state.image + ".jpg";
		
	this->baseBuffer = QImage(QString::fromStdString(filename));
	
	
	
	this->width = this->baseBuffer.width();
    this->height = this->baseBuffer.height();
    
	repositoryWindow->setPieceWidth(this->width);
	repositoryWindow->setPieceHeight(this->height);
	
	
	this->parentFrame = repositoryFrame;
	
	this->counter = new QtCounter(this);
		
    this->setImage();
	this->counter->move(this->state.x, this->state.y);
	
	this->margin = 0;
	
	
	this->counter->show();
	
	
}


// constructor for map

Counter::Counter(int id, const char *image, int degrees, int x, int y)  
{
	
	this->id = id;
	
	Counter::counters[this->id] = this;
	


	
	this->name = std::to_string(id);
	
	this->state.x = x;
	this->state.y = y;
	this->state.moved = false;
	this->state.degrees = degrees;
	this->state.image = string(image);
	
	string filename = "./images/" + this->state.image + ".jpg";
		
	this->baseBuffer = QImage(QString::fromStdString(filename));
	
	
	
	this->width = this->baseBuffer.width();
    this->height = this->baseBuffer.height();
    
    this->zorder = Counter::topZorder();
	this->margin = 0;
	
	
	this->parentFrame = mapFrame;
	
	
	repositoryWindow->setPieceWidth(this->width);
	repositoryWindow->setPieceHeight(this->height);
	
	
	this->counter = new QtCounter(this);
    this->setImage();
    this->counter->move(this->state.x, this->state.y);
    this->counter->raise();				// NB!!
    
    
    this->counter->show();
    this->parentFrame->repaint();
    
	
}


Counter::~Counter() 
{
		
	this->counter->close();
	
	Overlay::hooverView->setVisible(false);
	
	Counter::counters.erase(this->id);
	
}



void Counter::deleteAll()
{
	
	for (auto &&entry : repository) 
		entry.second->counter->close();
		 
	repository.clear();
	
	for (auto &&entry : counters) 
		entry.second->counter->close();
		 
	counters.clear();
	
	
	Counter::_id = 0;
	
}


int Counter::nextId()
{
	
	return Counter::_id++;
	
}


int Counter::topZorder()
{
	
	return Counter::_lastZorder++;
	
}


Counter* Counter::findObj(const char *name)
{
	
	string findName = string(name);
	
	
	for ( auto obj = counters.begin(); obj != counters.end(); ++obj  )
	
		if (obj->second->name == findName)
			return obj->second;
	
	
	for ( auto obj = repository.begin(); obj != repository.end(); ++obj  )
	
		if (obj->second->name == findName)
			return obj->second;
	
			
	return NULL;
	
}


void Counter::snaptoDefaultGrid (Counter *counter, int *x, int *y)
{
	
	// find the first (if any) counter close enough to snap to 
	
	int maxDistance = 15;
		
	for ( auto obj = counters.begin(); obj != counters.end(); ++obj  )
		if (counter != obj->second)
		{
			if (abs(*x - obj->second->state.x) < maxDistance && 
				abs(*y - obj->second->state.y) < maxDistance)
			{	
				*x = obj->second->state.x;
				*y = obj->second->state.y;
				return;
			}
		}
	
}


void Counter::setImage()
{
	

	this->margin = 0;
	
	int w = this->width;
	int h = this->height;


	
	QString file = QString::fromStdString(Counter::resources["_Moved"]);
								
	QImage const mask(file);
		
	w = w + 2*mask.width();
	h = h + 2*mask.width();
	this->margin = mask.width();		
	

	
	// rotated
	
	if (this->state.degrees != 0)
	{
	
		// compute size of rotated image
		
		
		float rad = (abs(this->state.degrees) % 90) * (M_PI/180);		
		float sine = sin(rad);
		float cosine = cos(rad);
		
		
		int nw = std::round(cosine * w + sine * h);
		int nh = std::round(cosine * h + sine * w);


		w = nw;
		h = nh;
					
	}


	
	this->counter->resize(w, h);
	
	
	
	//
	
	
	this->counter->clearMask();
		
		
	string filename = "./images/" + state.image + ".jpg";
	
	QImage baseImage = QImage(QString::fromStdString(filename));
	
	
	
	QImage image = QImage (this->counter->width(), this->counter->height(), QImage::Format_ARGB32_Premultiplied);
		
	image.fill(Qt::transparent);
	
	QPainter *paint = new QPainter(&image);			
	paint->drawImage(margin, margin, baseImage);						
	delete paint;
		
	
	
	
	// moved
	
	if (state.moved)
	{
		
		QString file = QString::fromStdString(Counter::resources["_Moved"]);
					
		QImage const mask(file);
		
		
		int dx = margin + Counter::mask_x;
		int dy = margin + Counter::mask_y;
				

		
		QPainter *paint = new QPainter(&image);
		paint->drawImage(dx, dy, mask);			
		delete paint;
		
	}
	
	
	// rotated

	if (state.degrees != 0)
	{
				
		qreal degrees = (qreal)state.degrees;
		
		
		QImage base = QImage (this->counter->width(), this->counter->height(), QImage::Format_ARGB32_Premultiplied);
		
		base.fill(Qt::transparent);
		
		
		QPainter *paint = new QPainter(&base);
		
		paint->setRenderHint(QPainter::Antialiasing);
		paint->setRenderHint(QPainter::TextAntialiasing);
		paint->setRenderHint(QPainter::SmoothPixmapTransform);
		
		QTransform transform;
		transform.translate(image.width()/2, image.height()/2);
		transform.rotate(degrees);
		transform.translate(-(margin+width/2), -(margin+height/2));
		
		
		paint->setTransform(transform);	

		
		paint->drawImage(0, 0, image);
		
		delete paint;
		
	
		
		image = base.copy(0, 0, base.width(), base.height());
		
		
	}
	
	
	
	
	baseBuffer = image.copy(0, 0, image.width(), image.height());
	
	this->counter->setPixmap(QPixmap::fromImage(image));
	
	this->counter->setMask(QBitmap::fromImage(image.createAlphaMask()));

	
	
}



void Counter::setGUI()
{
	
	for (auto obj = counters.begin(); obj != counters.end(); ++obj)
		obj->second->setImage();
	
}


