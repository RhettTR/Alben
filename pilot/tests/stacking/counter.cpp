#include <math.h>


#include <QtWidgets>

#include "counter.h"
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
Counter::Overlay *Counter::Overlay::overlay = NULL;


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


void Counter::QtCounter::paintEvent(QPaintEvent *e)
{
	QPainter painter(this);
	
			
	string filename = "./images/" + owner->state.image + ".jpg";
	
	QImage baseImage = QImage(QString::fromStdString(filename));
	owner->baseBuffer = baseImage.copy(0, 0, owner->width, owner->height);
	
	
	
	// moved
	
	if (owner->state.moved)
	{
		
		QString file = QString::fromStdString(Counter::resources["_Moved"]);
					
		QImage const mask(file);
		
				
		
		int dx = Counter::mask_x;
		int dy = Counter::mask_y;
		
		QImage image = QImage (this->width(), this->height(), QImage::Format_ARGB32_Premultiplied);
		
		image.fill(Qt::transparent);
		
		
		QPainter *paint = new QPainter(&image);
		
		paint->drawImage(QRect(0, 0, owner->width, owner->height), owner->baseBuffer);	
		paint->drawImage(QRect(dx, dy, mask.width(), mask.height()), mask);	
				
		delete paint;
		
		
		owner->baseBuffer = image.copy(0, 0, this->width(), this->height());
		
	}
	
	
	//  stack marker
	
	if (!Counter::haveOffset && owner->marker)
	{
		
		QImage image = QImage (this->width(), this->height(), QImage::Format_ARGB32_Premultiplied);
		
		image.fill(Qt::transparent);
		
		
		QPainter *paint = new QPainter(&image);
		
		paint->setRenderHint(QPainter::Antialiasing);
		paint->setRenderHint(QPainter::TextAntialiasing);
		paint->setRenderHint(QPainter::SmoothPixmapTransform);
		
		
		paint->drawImage(0, 0, owner->baseBuffer);	
		
		
		QRect rectangle(owner->width - 8, owner->height - 8, 16, 16);
		paint->setPen(QPen(QColor("#000000")));
		paint->setBrush(QBrush(Qt::black));
		paint->drawEllipse(rectangle);				
		
		paint->setFont(QFont("Arial", 10, QFont::Bold));
		paint->setPen(QPen(QColor("#ffffff")));
		QString str = QString::fromUtf8(std::to_string(owner->stackSize).c_str());
		paint->drawText(rectangle, Qt::AlignHCenter, str);
		
		delete paint;
		
		
		owner->baseBuffer = image.copy(0, 0, this->width(), this->height());
		
	}
	
	
	// rotated

	if (owner->state.degrees != 0)
	{
				
		qreal degrees = (qreal)owner->state.degrees;
		
		
		
		QImage image = QImage (this->width(), this->height(), QImage::Format_ARGB32_Premultiplied);
		
		image.fill(Qt::transparent);
		
		
		QPainter *paint = new QPainter(&image);
		
		paint->setRenderHint(QPainter::Antialiasing);
		paint->setRenderHint(QPainter::TextAntialiasing);
		paint->setRenderHint(QPainter::SmoothPixmapTransform);
		
		QTransform transform;
		transform.translate(this->width()/2, this->height()/2);
		transform.rotate(degrees);
		transform.translate(-owner->width/2, -owner->height/2);
		
		
		paint->setTransform(transform);
		paint->drawImage(0, 0, owner->baseBuffer);
		
		delete paint;
		
		
		owner->baseBuffer = image.copy(0, 0, this->width(), this->height());
		
		
	}
	
		
	painter.drawImage(0, 0, owner->baseBuffer);	
	

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
	
	
	
	


Counter::Overlay::Overlay() : QFrame() 
{
  
	setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
	setAutoFillBackground(false);
	
	setAttribute(Qt::WA_TransparentForMouseEvents, true);
	
	setAcceptDrops(true);
  
}
	
		
void Counter::Overlay::paintEvent(QPaintEvent *e)
{
	
	QPainter painter(this);

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
		
    this->counter->setPixmap(QPixmap::fromImage(this->baseBuffer));
	this->counter->move(this->state.x, this->state.y);
	
	this->stackSize = 1;
	this->marker = false;
	
	
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
    this->stackSize = 1;
	this->marker = false;
	this->margin = 0;
	
	
	this->parentFrame = mapFrame;
	
	this->counter = new QtCounter(this);
	repositoryWindow->setPieceWidth(this->width);
	repositoryWindow->setPieceHeight(this->height);
	
	this->stackSize = 1;
	this->marker = false;
	
	
    this->counter->setPixmap(QPixmap::fromImage(this->baseBuffer));
    this->counter->move(this->state.x, this->state.y);
    this->counter->raise();				// NB!!
    
    
    this->counter->show();
    this->setGUI();
    
	
}


Counter::~Counter() 
{
	
	this->counter->close();
	
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


void Counter::setGUI()
{
	
	struct point 
	{
		int x; 
		int y;
		
		bool operator <(const point &p) const
		{
			return (x < p.x) || (!(p.x < x) && y < p.y);
		}
	}; 
	typedef struct point Point;
	std::map<Point, Stack> stacks;


	
	for ( auto obj = counters.begin(); obj != counters.end(); ++obj  )
	{
		Point p = (Point){.x = obj->second->state.x, .y = obj->second->state.y};
		
		if (stacks.find( p ) == stacks.end()) 
		{
			// not found
			Stack stack = {{obj->second->zorder, obj->second}};
			stacks[p] = stack;
		} 
		else 
		{
			// found			
			Stack stack = stacks.at( p );
			stack[obj->second->zorder] = obj->second;
			stacks[p] = stack;
		}
		
	}
	
	
	// render all "stacks" with 1 or more counters
	
	for (auto const& [point, stack] : stacks)	
		setStack(stack);
	
		
}


void Counter::setStack(Stack stack)
{
	
	int offset = 0;
	int size = (int)stack.size();
		
	for ( auto obj = stack.begin(); obj != stack.end(); ++obj  )
	{		
		obj->second->setImage(offset, size);
		offset++;
	}
		
}


void Counter::setImage(int offsetFactor, int stackSize)
{
	
	
	int x = this->state.x;
	int y = this->state.y;
	this->marker = false;
	this->margin = 0;
	
	
	
	// offsetFactor is 0, 1, 2 ... the postion in the stack with 0 bottom
	if (Counter::haveOffset)
	{
		x = x + (Counter::stackOffset * offsetFactor);
		y = y - (Counter::stackOffset * offsetFactor);
	}
	else
		// only render stack size marker on top counter
		if (stackSize > 1 && offsetFactor == stackSize - 1)
			this->marker = true;
	
	

	this->counter->move(x, y);


	int w = this->width;
	int h = this->height;

	
	QString file = QString::fromStdString(Counter::resources["_Moved"]);
							
	QImage const mask(file);
	
	
	
	// moved
	
	if (this->state.moved)
	{	
		w = w + 2*mask.width();
		h = h + 2*mask.width();
		this->margin = mask.width();		
	}
	
	
	
	// stack marker
	
	this->stackSize = stackSize;
	
		
	if (this->marker)	
	{		
		if (this->margin == 0)
		{
			w = w + 16;
			h = h + 16;
			this->margin = 8;
		}
	}
		
	//x = x - this->margin;
	//y = y - this->margin;
	
	//this->counter->move(x, y); 
	
	
	
	
	// rotated
	
	if (this->state.degrees != 0)
	{
	
		// compute size of rotated image
		
		
		float rad = (abs(this->state.degrees) % 90) * (M_PI/180);		
		float sine = sin(rad);
		float cosine = cos(rad);
		
		
		int nw = std::round(cosine * w + sine * h);
		int nh = std::round(cosine * h + sine * w);

		
		// 
		
		w = nw;
		h = nh;
		
		
		
		
		// compute image position		
				
		x = x - std::round((w - this->width) / 2);
		y = y - std::round((h - this->height) / 2); 
			
		
		this->counter->move(x, y);
			
	}
	
	
	
	
	this->counter->resize(w, h);
	this->counter->repaint();
	
	
}
