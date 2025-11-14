#include <math.h>


#include <QtWidgets>


#include "counter.h"
#include "overlay.h"
#include "luau.h"
#include "window.h"

	
#include "io.h"
#include "scale.h"


using namespace std;



int Counter::_id = 0;
int Counter::_lastZorder = 0;
int Counter::_bottomZorder = 0;


map<int, Counter *> Counter::counters;
map<string, Counter *> Counter::repository;
map<string, Counter::SystemMask *> Counter::masks;




float Counter::alpha = 1.0;

bool Counter::haveOffset = true;	// true = unopened stack has offset
//QString Counter::selectionColor = QString("#bc145e");
QString Counter::selectionColor = QString("red");
bool Counter::hooverShowMap = false;
bool Counter::hooverShowPlace = false;






extern Window *repositoryWindow;
extern IO *io;
extern Scale *scaled;





static Luau::PopupEntries popupentries;




	
Counter::QtCounter *owner;



Counter::QtCounter::QtCounter(Counter *owner, QFrame *parent) : QLabel(parent)
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

 
 
 
void Counter::QtCounter::showRightClickMenu(QtCounter *counter)
{
	
	
	// check ownership and ability	
	if (counter->owner->getOwnershipField() != 0 && counter->owner->getOwnershipField() % IO::getKey() != 0)
	{
		Settings::OwnershipRights rights = counter->owner->getRights();
		
		if (rights.NoMenu)
			return;
		
	}

	
	QMenu myMenu(counter);
	QWidget widget;
	
	
	myMenu.clear();
	
	popupentries.clear();
	

	
	popupentries = Luau::getTraits(counter->owner->state.tag.c_str(), counter->owner->name.c_str());
		

	for (auto e : popupentries)
	{
	 
		const QString name = QString::fromStdString(e.entryname);
		
		
		ActionData data;
		data.entryaction = QString(e.entryaction.c_str());
		data.entrytrait = QString(e.entrytrait.c_str());
		data.entryactions = QString(e.entryactions.c_str());
		
		
		if (data.entrytrait == "SubMenu")
		{
			QMenu *menu = myMenu.addMenu(name);
			
			if (e.entrytest)
				if (!Luau::doTest(counter->owner->state.tag.c_str(), e.entrytrait.c_str(), counter->owner->name.c_str())) 
					menu->menuAction()->setEnabled(false);	
					
			
			QString str = data.entryactions.simplified();
			str = str.replace(" ","");
		
			QStringList entries = str.split(':');
			
			
					
			for (const auto& item : entries )
			{
				
				
				// entries come as menuname, menutrait, menuaction
				
				QStringList entry = item.split(',');
				
		
				QAction* action = new QAction(entry[0], counter);
			
				ActionData actionData;
				actionData.entrytrait = entry[1];
				actionData.entryaction = entry[2];
			
				QVariant v;
				
				v.setValue(actionData);
				
				action->setData(v);
				menu->addAction(action);
				
				QObject::connect( action, &QAction::triggered, counter, [=]()->void{ do_activate(action); } );
			}
			
			
		}
		else
		{
			
			QAction* action = new QAction(name, counter);
	
	
			
			
			QVariant v;
			
			v.setValue(data);
			action->setData(v);
		
			
			if (e.entrytest)
				if (!Luau::doTest(counter->owner->state.tag.c_str(), e.entrytrait.c_str(), counter->owner->name.c_str())) 
					action->setEnabled(false);
					//action->hide();
				
			
			//action->setShortcut(QKeySequence("Ctrl+D"));
			
			
		   
			widget.addAction(action);
			
			QObject::connect( action, &QAction::triggered, counter, [=]()->void{ do_activate(action); } );
		}
		
	}
	
	
	myMenu.addActions(widget.actions());
	
	myMenu.exec(QCursor::pos());
	
}


void Counter::QtCounter::mousePressEvent (QMouseEvent * e) 
{
	
	if (this->owner->disabled)
	{
		e->ignore();
		return;
	}
	

		
	if (e->button() == Qt::RightButton)
	{
		
		if (QApplication::keyboardModifiers() == Qt::ControlModifier)
		{
			if (CentralFrame::openStackoffset)
				((CentralFrame *)this->parent())->toggleOpenStack(this->owner, -1);
		}
		else
		{
		
			Counter *selected;
			int dummy;
			
			Window *window = Window::getInstance(this->owner->state.tag.c_str());
			
			if (window->frame->anySelected(this->owner, selected, dummy))
				selected->counter->showRightClickMenu(selected->counter);
			else
				showRightClickMenu(this);
				
		}
		e->accept();
	}
	else 	
		e->ignore();
		
}


void Counter::QtCounter::mouseDoubleClickEvent(QMouseEvent *e)
{
	if (CentralFrame::openStackoffset)						
		((CentralFrame *)this->parent())->toggleOpenStack(this->owner, 1);
	else
	{
		((CentralFrame *)this->parent())->selectCounter(this->owner);
		QPoint pos = e->position().toPoint() + this->pos();		
		Overlay::openView->open(this->owner, pos);
	}
}






void Counter::QtCounter::hooverAction()
{	
		
	
	if (Overlay::openView->isVisible())
		return;
		
	if (QApplication::keyboardModifiers() == Qt::ControlModifier)
		return;
		
	if (QApplication::keyboardModifiers() == Qt::ShiftModifier)
		return;
		

	
	CentralFrame::Stack stack = {{this->owner->state.zorder, this->owner}};
	
	QPoint point = QPoint(this->owner->state.x, this->owner->state.y);
	
	
	for ( auto obj = counters.begin(); obj != counters.end(); ++obj  )	
	{
		QPoint p = QPoint(obj->second->state.x, obj->second->state.y);
		
		if (p == point)		
			stack[obj->second->state.zorder] = obj->second;	
	}
	

	Counter *counter = stack.begin()->second;
	
	QSize s = QSize(0,0);
	
	
	if (Counter::hooverShowMap)
	{
		
		
		QImage baseImage = scaled->getScaledImage(Window::getInstance(counter->state.tag.c_str())->frame->backgroundID);
		
		
		
		QRect source(0, 
					 0, 
					 counter->scaledBuffer.width(), 
					 counter->scaledBuffer.height());
						
		QRect dest(counter->state.x,
				   counter->state.y,
				   counter->scaledWidth + 2*counter->scaledMargin,					   
				   counter->scaledHeight + 2*counter->scaledMargin);						   			
					  
		source.moveCenter( dest.center() );
		

		CentralFrame::Point trans = 
			scaled->getScaleRotateCoordinate(counter, source.topLeft().x(), source.topLeft().y());
		
		
		QImage background = baseImage.copy(trans.x, trans.y, source.width(), source.height());

	
		s = Overlay::hooverView->setImage(background);
		
	}
	
	
	QSize t = QSize(0,0);
	
	if (Counter::hooverShowPlace)
		t = Overlay::hooverView->setPlace(Luau::mapPlace(counter->state.tag.c_str(),
														 std::round(counter->width/2) + counter->margin, 
														 std::round(counter->height/2) + counter->margin,
														 counter->state.x, 
														 counter->state.y));
	
		
	
	
	int width = s.width() > t.width() ?  s.width() : t.width();
	int height = s.height() + t.height();
	
	width = width + 2*Overlay::border;
	height = height + 2*Overlay::border; 
	
	
	
	
	// hooverview 2px border
	Overlay::hooverView->grid->move(width, 2);
									
	
	Overlay::hooverView->setImages(stack);	
	
	
    
    QSize size = Overlay::hooverView->layout->minimumSize();
    
   
   
	Overlay::hooverView->grid->resize(size.width(), size.height());
	

	
	// 4 is double the hoover view border height
	int heightBox = height > size.height() + 4 ? height : size.height() + 4;
	
	Overlay::hooverView->resize(width + size.width(), heightBox);
	
	
	
	
	QPoint pnt = this->popupPoint + this->pos();
	
	Overlay::hooverView->moveThis(pnt, 
								  Overlay::hooverView->width(), 
								  Overlay::hooverView->height());	
	
	
	Overlay::hooverView->setVisible(true);
	// force redraw of mask layer
	Overlay::overlay->clearMask();	
	
	
}



void Counter::QtCounter::mouseMoveEvent (QMouseEvent * e)
{
	Window *window = Window::getInstance(this->owner->state.tag.c_str());

	if (window->tag == "main")
	{	
		popupPoint = e->position().toPoint();		
		timer->start(1000);
	}
}



void Counter::QtCounter::leaveEvent (QEvent *e)
{	
	Window *window = Window::getInstance(this->owner->state.tag.c_str());
	
	if (window->tag == "main")
	{	
		if (Overlay::hooverView->isVisible())
		{
			Overlay::hooverView->setVisible(false);
			//Overlay::overlay->setMasks();
		}	
		timer->stop();
	}
}

	


				
void Counter::QtCounter::do_activate (QAction *action)
{
	QVariant v = action->data();
	
	ActionData retrieved = qvariant_cast<ActionData>(v);
	
	const char *entryaction = retrieved.entryaction.toStdString().c_str();
	const char *entrytrait = retrieved.entrytrait.toStdString().c_str();
	
	const char *window = this->owner->state.tag.c_str();
	
	
	
	CentralFrame::Stacks stacks;

	
	if (this->owner->selected && (strcmp(entryaction, "actionSelect") != 0))	// select is always individual
	{
		// generate stacks of selected
		
		for (auto obj = counters.begin(); obj != counters.end(); ++obj)
		{
			if (obj->second->selected == true)
			{
				CentralFrame::Point p = (CentralFrame::Point){.x = obj->second->state.x, .y = obj->second->state.y};
				
				if (stacks.find( p ) == stacks.end()) 
				{
					// not found
					CentralFrame::Stack stack = {{obj->second->state.zorder, obj->second}};
					stacks[p] = stack;
				} 
				else 
				{
					// found			
					CentralFrame::Stack stack = stacks.at( p );
					stack[obj->second->state.zorder] = obj->second;
					stacks[p] = stack;
				}
			}	
		}
	
	
		// do action on all selected
		
		for (auto const& [point, stack] : stacks)	
			for ( auto obj = stack.begin(); obj != stack.end(); ++obj )
				Luau::doAction(window, obj->second->name.c_str(), entrytrait, entryaction);
				
		Luau::doAction("", "", "", "actionEnd");
		
	}
	else	
	{	
		// else do action on unselected this
				
		Luau::doAction(window, this->owner->name.c_str(), entrytrait, entryaction);
		
		if (strcmp(window, "main") == 0)  // oh oh
			Luau::doAction("", "", "", "actionEnd");
	}
					
}


void Counter::QtCounter::paintEvent (QPaintEvent *e)
{
	
	if (!CentralFrame::allowPainting)
	{
		e->accept();
		return;		// no painting while undoing
	}
	
	QLabel::paintEvent(e);
}





	
Counter::SystemMask::SystemMask(const char *name, const char *mask, const char *valign, const char *halign)
{
	
	this->name = string(name);
	this->mask = string(mask);
	this->vAlign = string(valign);
	this->hAlign = string(halign); 
	
	Counter::masks[this->name] = this;
	
}	



	


// constructor for the repository

Counter::Counter(Table *state)		
{
	
	this->id = Counter::nextId();
	
	

	this->name = std::get<std::string>(std::get<Table>(std::get<Table>((*state)["Image"])["images"])[1]);
	this->image = this->name;	
	
	Counter::repository[this->name] = this;
	
	this->state.x = 0;
	this->state.y = 0;
	this->state.moved = false;
	this->state.degrees = 0;
	this->state.image = this->name;
	this->state.overlays = nullptr;
	this->state.opacity = 1.0;
	this->state.tag = std::get<std::string>((*state)["window"]);
	
	
	this->width = io->getSize(this->state.image).width();
    this->height = io->getSize(this->state.image).height();
    this->margin = 9;
	
	
	this->parentWindow = repositoryWindow;
	
	
	
	Window::Pane *pane = repositoryWindow->getParent();
	
	
	
	this->counter = new QtCounter(this, pane);
		
	// make sure images in repository have height less than 200	
    this->setImage(200);
    
    
      
    pane->layout()->addWidget(this->counter);
    //
    pane->resize(pane->layout()->minimumSize());

    
    
	
	this->selected = false;
	this->doesNotStack = ((*state).find("DoesNotStack") != (*state).end());
	this->disabled = false;
	
	this->_ownershipField = 0;	
	
	
	this->counter->show();
	
	
}


// constructor for map

Counter::Counter(int id, Table *state) 
  
{
	
	this->id = id;
	
	Counter::counters[this->id] = this;
	

	
	this->name = std::to_string(id);
	
	
	this->state.x = (int)std::get<double>((*state)["x"]);	 	
	this->state.y = (int)std::get<double>((*state)["y"]);
	this->state.tag = std::get<std::string>((*state)["window"]);
	
	Window *window = Window::getInstance(this->state.tag.c_str());
	
	this->parentWindow = window;
	
	

	Table images = std::get<Table>(std::get<Table>((*state)["Image"])["images"]);
	int index = (int)std::get<double>(std::get<Table>((*state)["Image"])["imageIndex"]);
	this->image = std::get<std::string>(images[1]);	
 	this->state.image = std::get<std::string>(images[index]);
 	
 	
 	// optional fields
 	
 	if ((*state).find("MarkMoved") != (*state).end())	
		this->state.moved = std::get<bool>(std::get<Table>((*state)["MarkMoved"])["moved"]);
	else
		this->state.moved = false;
		
	if ((*state).find("Rotate") != (*state).end())		
		this->state.degrees = (int)std::get<double>(std::get<Table>((*state)["Rotate"])["degrees"]);
	else
		this->state.degrees = 0;
		
	if ((*state).find("zorder") != (*state).end())		
		this->state.zorder = (int)std::get<double>((*state)["zorder"]);
	else
		this->state.zorder = topZorder();
		
	if ((*state).find("Overlays") != (*state).end())		
	{	
		Table *overlays = new Table();	
		(*overlays) = std::get<Table>((*state)["Overlays"]);	
		this->state.overlays = overlays;
	}
	else
		this->state.overlays = nullptr;
	
	if ((*state).find("Visibility") != (*state).end())		
		this->state.opacity = (float)std::get<double>(std::get<Table>((*state)["Visibility"])["opacity"]);
	else
		this->state.opacity = 1.0;	

	
	
	
	this->table = *state;
	
	
	this->width = io->getSize(this->state.image).width();
    this->height = io->getSize(this->state.image).height();
	this->margin = 9;
	
   
	this->selected = false;
	this->doesNotStack = ((*state).find("DoesNotStack") != (*state).end());
	this->disabled = false;
	
	
	this->_ownershipField = 0;
	
	if ((*state).find("Side") != (*state).end())
	{
		if (std::get<std::string>((*state)["Side"]) == Settings::playerSide)
		{
			// NOTE!! the factor 0xef06eea1 should not be a constant but generated and then discarded
			this->_ownershipField = IO::getKey() * 0xef06eea1;
			this->_ownershipRights = Settings::myOwnershipRights;
		}
		else
		{
			// default rights same as own
			this->_ownershipRights = Settings::myOwnershipRights;
		}
	}
	
	
	

	//if (CentralFrame::allowPainting)
	{
	this->counter = new QtCounter(this, window->frame);
    this->setImage();
    

    
    this->counter->show();
	}
    
    //window->frame->repaint();
    window->frame->update();
	
}





Counter::~Counter() 
{
		
	// hook
	Luau::deleted(this->name.c_str());
	
	if (this->state.overlays != nullptr)
		delete this->state.overlays;
	
	this->counter->close();
	
	Overlay::hooverView->setVisible(false);
	
	Counter::counters.erase(this->id);
	

	
	//Window::getInstance(this->state.tag.c_str())->frame->repaint();
	
}



void Counter::deleteAll()
{
	
	for (auto &&entry : repository) 
		delete entry.second->counter;
		 
	repository.clear();
	
	for (auto &&entry : counters) 
		delete entry.second->counter;
		 
	counters.clear();
	
	
	Counter::_id = 0;
	
}


int Counter::nextId()
{
	
	return ++Counter::_id;
	
}


int Counter::topZorder()
{
	
	return ++Counter::_lastZorder;
	
}


int Counter::bottomZorder()
{
	
	return --Counter::_bottomZorder;
	
}


void Counter::resetId()
{
	int max = 0;
	
	for ( auto obj = counters.begin(); obj != counters.end(); ++obj  )
		if (obj->second->id > max) 
			max = obj->second->id;
		
	Counter::_id = max;
}


void Counter::resetZorder()
{
	// find highest zorder in loaded game
	// set _lastZorder to greater than highest
	
	int max = 0;
	
	for ( auto obj = counters.begin(); obj != counters.end(); ++obj  )
		if (obj->second->state.zorder > max) 
			max = obj->second->state.zorder;
		
	Counter::_lastZorder = max;	
}


Counter* Counter::findObj(const char *id)
{
	
	string findName = string(id);
	
	
	for ( auto obj = counters.begin(); obj != counters.end(); ++obj  )
	
		if (obj->second->name == findName)
			return obj->second;
	
	
	for ( auto obj = repository.begin(); obj != repository.end(); ++obj  )
	
		if (obj->second->name == findName)
			return obj->second;
	
			
	return nullptr;
	
}


QSize Counter::getSize(Counter *counter)
{
	return QSize(std::round(counter->width/2) + counter->margin, 
				 std::round(counter->height/2) + counter->margin); 
}


void Counter::toggleSelect(const char *id)
{
	
	Counter *counter = findObj(id);
	
	if (counter != nullptr)
	{
		counter->selected = !counter->selected;
		counter->setImage();
	}
		
}



void Counter::clearMoved()
{
	
	bool atLeastOne = false;
	
	for (auto obj = counters.begin(); obj != counters.end(); ++obj)
		if (obj->second->state.moved)
		{	
			Luau::updateMoved(obj->second->name.c_str(), false);
			atLeastOne = true;
		}
	
	if (atLeastOne)
	{
		Luau::reportText("move", "All move-markers removed");
		Luau::doEvent("end", "", "", "", 0);
	}
	
}



void Counter::setDisabled(bool disable)
{
	for (auto obj = counters.begin(); obj != counters.end(); ++obj)
		obj->second->disabled = disable;
}



int Counter::findOffset(int size, std::string type)
{	
	if (type == "left")
		return this->width;
	if (type == "hcenter")
		return std::floor((this->height - size) / 2);
		
	return 0;
}





void Counter::setImage(int maxHeight)
{
	
	
	int w = this->width;
	int h = this->height;


		
	w = w + 2*this->margin;
	h = h + 2*this->margin;


	
	// rotated
	
	
	int degrees = this->state.degrees;
	
	
	if (degrees != 0 || (Scale::rotation != 0 && CentralFrame::facingMatters))
	{
	
		// compute size of rotated image	
		
		if (Scale::rotation != 0 && CentralFrame::facingMatters)
			degrees = degrees + Scale::rotation;
		
		
		float rad = (abs(degrees) % 90) * (M_PI/180);		
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
	
	
	//
		
		
	QImage baseImage = io->getImage(this->state.image);
	
	
	
	QImage image = QImage (this->counter->width(), this->counter->height(), QImage::Format_ARGB32_Premultiplied);
		
	image.fill(Qt::transparent);
	
	QPainter *paint = new QPainter(&image);			
	paint->drawImage(margin, margin, baseImage);						
	delete paint;
	
	
		
	// selected
	
	if (this->selected == true && this->parentWindow->tag != "Repository")
	{
		
		QPainter *paint = new QPainter(&image);
		QPen pen = QPen(QColor::fromString(Counter::selectionColor));
		pen.setWidth(2);
		//pen.setWidth(3);		
		pen.setCapStyle(Qt::SquareCap);
		paint->setPen(pen);
		// 1.5 is middle of line width 3; width +3 pen size 
		//QRectF rect = QRectF(margin-1.5, margin-1.5, this->width+3.0, this->height+3.0);
		QRectF rect = QRectF(margin-1.0, margin-1.0, this->width+2.0, this->height+2.0);
		paint->drawRect(rect);			
		delete paint;
		
	}
	
	
	// moved
	
	if (state.moved)
	{
		
		QImage const mask = io->getImage(Counter::masks["MovedMarker"]->mask);
		
		int x = findOffset(mask.width(), Counter::masks["MovedMarker"]->vAlign);
		int y = findOffset(mask.height(), Counter::masks["MovedMarker"]->hAlign);		
		
		int dx = margin + x;
		int dy = margin + y;
		
		QPainter *paint = new QPainter(&image);
		paint->drawImage(dx, dy, mask);			
		delete paint;
		
	}
	

	// masks & labels
	
	if (state.overlays != nullptr)
	{
			
		for (auto const &table : (*state.overlays))
		{
			Table trait = std::get<Table>(table.second);
		
			auto itr = trait.find("mask");
			
			if (itr != trait.end()) 
			{
				// mask	
				
				if (std::get<bool>(trait["apply"]))
				{
					
					int x = (int)std::get<double>(trait["x"]) + margin;
					int y = (int)std::get<double>(trait["y"]) + margin;
					
					QImage const mask = io->getImage(std::get<std::string>(trait["mask"]));
					
					
					QPainter *paint = new QPainter(&image);
					paint->drawImage(x, y, mask);			
					delete paint;
					
				} 
					
			}
			else
			{
				// label
				
				if (std::get<bool>(trait["apply"]))
				{
					int x = (int)std::get<double>(trait["x"]) + margin;
					int y = (int)std::get<double>(trait["y"]) + margin;
					
					QFont font;
					font.setFamily(QString::fromStdString(std::get<std::string>(trait["font"])));
					font.setPixelSize((int)std::get<double>(trait["size"]));
					
					std::string color = std::get<std::string>(trait["color"]);
	

					const QString text = QString::fromStdString(std::get<std::string>(trait["text"]));
					
					QPainter *paint = new QPainter(&image);
					paint->setFont(font);
					paint->setPen(QString::fromStdString(color));
					paint->drawText(x, y, text);
					delete paint;
				}
				 
			}
			
			
		}
	
	}
	

	// rotated

	if (degrees != 0)
	{
				
		
		QImage base = QImage (this->counter->width(), this->counter->height(), QImage::Format_ARGB32_Premultiplied);
		
		base.fill(Qt::transparent);
		
		
		QPainter *paint = new QPainter(&base);
		
		paint->setRenderHint(QPainter::Antialiasing);
		paint->setRenderHint(QPainter::TextAntialiasing);
		paint->setRenderHint(QPainter::SmoothPixmapTransform);
		
		QTransform transform;
		transform.translate(image.width()/2, image.height()/2);
		transform.rotate((qreal)degrees);
		transform.translate(-(margin+width/2), -(margin+height/2));
		
		
		paint->setTransform(transform);	

		
		paint->drawImage(0, 0, image);
		
		delete paint;
		
	
		
		image = base.copy(0, 0, base.width(), base.height());
		
		
	}
	
	
		
	
	
	baseBuffer = image.copy(0, 0, image.width(), image.height());
	
	
	
	
	float scaleFraction;
	 
	//if (this->state.tag == "Repository")
		scaleFraction = Window::getInstance("main")->frame->scaleFraction;
	//else
		//scaleFraction = Window::getInstance(this->state.tag.c_str())->frame->scaleFraction;
	
	
	
	this->scaledWidth = this->width * scaleFraction;
	this->scaledHeight = this->height * scaleFraction;
	this->scaledMargin = this->margin * scaleFraction;
	
	
	QSize scaledSize(std::round(image.width() * scaleFraction), 
					 std::round(image.height() * scaleFraction));
	
	this->scaledBuffer = image.scaled( scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		
	
	if (this->parentWindow->tag == "main")	
	{	
		this->counter->resize(scaledSize);
		this->counter->setMask(QBitmap::fromImage(this->scaledBuffer.createAlphaMask()));
	}
	else	
	{	this->counter->setMinimumSize(image.width(), image.height());	
		this->counter->setMask(QBitmap::fromImage(this->baseBuffer.createAlphaMask()));
	}
	

	
		
	// visibility 
	
	if (state.opacity < 1.0)
	{
		auto itr = this->table.find("Visibility");
		
		if (itr != this->table.end()) 
		
			if (std::get<bool>(std::get<Table>((this->table)["Visibility"])["apply"]))
			{
				
				QImage temp1(baseBuffer.size(), QImage::Format_ARGB32_Premultiplied);
				temp1.fill(Qt::transparent);
				
				QPainter *paint = new QPainter(&temp1);
				paint->setOpacity(this->state.opacity);
				paint->drawImage(0, 0, baseBuffer);
				delete paint;
				
				baseBuffer = temp1;
				
				
				QImage temp2(scaledBuffer.size(), QImage::Format_ARGB32_Premultiplied);
				temp2.fill(Qt::transparent);
				
				paint = new QPainter(&temp2);
				paint->setOpacity(this->state.opacity);
				paint->drawImage(0, 0, scaledBuffer);
				delete paint;
				
				scaledBuffer = temp2;
			}
	}



	
	if (this->parentWindow->tag == "main")	
		this->counter->setPixmap(QPixmap::fromImage(this->scaledBuffer));		
		
	else
		this->counter->setPixmap(QPixmap::fromImage(this->baseBuffer));		
		
	
	
	
}



void Counter::setPos(int x, int y)
{
	if (!(x == this->counter->pos().x() && y == this->counter->pos().y()))
		this->counter->move(x, y);
}


unsigned long long Counter::getOwnershipField()
{
	return this->_ownershipField;
}

void Counter::setOwnershipField(unsigned long long field)
{
	this->_ownershipField = field;
}

Settings::OwnershipRights Counter::getRights()
{
	return this->_ownershipRights;
}

void Counter::setRights(Settings::OwnershipRights rights)
{
	this->_ownershipRights = rights;
}



void Counter::setGUI()
{
	
	CentralFrame::Stacks stacks;
	
	
	for (auto obj = counters.begin(); obj != counters.end(); ++obj)
	{
		CentralFrame::Point p = (CentralFrame::Point){.x = obj->second->state.x, .y = obj->second->state.y};
		
		if (stacks.find( p ) == stacks.end()) 
		{
			// not found
			CentralFrame::Stack stack = {{obj->second->state.zorder, obj->second}};
			stacks[p] = stack;
		} 
		else 
		{
			// found			
			CentralFrame::Stack stack = stacks.at( p );
			stack[obj->second->state.zorder] = obj->second;
			stacks[p] = stack;
		}	
	}
	


	
	// order stacks globally from low to high zorder
	
	std::map<int, CentralFrame::Stack> zordered;
	
	for (auto const& [point, stack] : stacks)
		zordered[std::prev(stack.end())->first] = stack;
	
	
	
	
	
	for (auto const& [zorder, stack] : zordered)	
		for ( auto obj = stack.begin(); obj != stack.end(); ++obj  )
		{
			obj->second->setImage();
			obj->second->counter->raise();
		}
	
}




