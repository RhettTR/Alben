#include <math.h>


#include <QtWidgets>

#include "counter.h"
#include "luau.h"


using namespace std;



int Counter::_id = 0;


map<int, Counter *> Counter::objects;
map<string, string> Counter::resources;




float Counter::alpha = 1.0;

int Counter::mask_x = 0;
int Counter::mask_y = 0;





extern QWidget *centralFrame;




static Luau::PopupEntries popupentries;




	
Counter::QtCounter *owner;


Counter::QtCounter::QtCounter(Counter *owner, QWidget *parent) : QLabel(parent)
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
			popupentries = Luau::getTraits((const char *)this->owner->name.c_str());
		
			for (auto e : popupentries)
			{
				 
				const QString name = QString::fromStdString(e.entryname);
				
				QAction* action = new QAction(name, this);
				
				QVariant v = QVariant(QString(e.entryaction));
				action->setData(v);
				
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
	
			
	string filename = "./images/" + owner->state.name + ".jpg";
	
	QImage baseImage = QImage(QString::fromStdString(filename));
	owner->baseBuffer = baseImage.copy(0, 0, owner->width, owner->height);
	
	
	if (!owner->state.moved && owner->state.degrees == 0)
	{
		painter.drawImage(0, 0, owner->baseBuffer);
		return;
	}
	
	
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

		
		//
		if (owner->state.degrees == 0)
		{
			painter.drawImage(0, 0, owner->baseBuffer);
			return;
		}
		
		
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
		
		painter.drawImage(0, 0, owner->baseBuffer);
		
		
	}

}
		
		
	
		
void Counter::QtCounter::do_activate (QAction *action)
{
	QVariant v = action->data();
	QString str = (QString) v.value<QString>();
	QByteArray ba = str.toLocal8Bit();
	const char *entryaction = ba.data();			
	Luau::doAction(this->owner->name.c_str(), entryaction);			
}
	





Counter::Counter(const char *name, int x, int y)
{
	
	Counter::objects[this->id] = this;
	
	this->name = string(name);
	
	this->state.x = x;
	this->state.y = y;
	this->state.moved = false;
	this->state.degrees = 0;
	this->state.name = string(name);
	
	string filename = "./images/" + this->state.name + ".jpg";
		
	this->baseBuffer = QImage(QString::fromStdString(filename));
	
	
	
	this->width = this->baseBuffer.width();
    this->height = this->baseBuffer.height();
	
	
	this->counter = new QtCounter(this, centralFrame);
	
	
    this->counter->setPixmap(QPixmap::fromImage(this->baseBuffer));
	this->counter->move(this->state.x, this->state.y);
	
	this->counter->show();
	
	
}


Counter::~Counter() 
{
	
	this->counter->close();
	
	Counter::objects.erase(this->id);
	
}



void Counter::deleteAll()
{
	
	for (auto &&entry : objects) 
		entry.second->counter->close();
		 
	objects.clear();
	
	Counter::_id = 0;
	
}



Counter* Counter::findObj(const char *name)
{
	
	string findName = string(name);
	
	
	for ( auto obj = objects.begin(); obj != objects.end(); ++obj  )
	
		if (obj->second->name == findName)
			return obj->second;
	
			
	return NULL;
	
}


void Counter::setImage()
{
	

	this->counter->move(this->state.x, this->state.y);


	int w = this->width;
	int h = this->height;
	
	
	
	QString file = QString::fromStdString(Counter::resources["_Moved"]);
							
	QImage const mask(file);
	
	
	
	// moved
	
	if (this->state.moved)
	{	
		w = w + 2*mask.width();
		h = h + 2*mask.width();		
	}
	
	
	
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
				
		int x = this->state.x - std::round((w - this->width) / 2);
		int y = this->state.y - std::round((h - this->height) / 2); 
			
		
		this->counter->move(x, y);
			
	}
	
	
	
	
	this->counter->resize(w, h);
	this->counter->repaint();
	
	
}
