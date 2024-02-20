#include "overlay.h"
#include "luau.h"



int Overlay::border = 10;
Overlay *Overlay::overlay = nullptr;;


int Overlay::FlowLayout::maxColumns = 3;
int Overlay::FlowLayout::layoutWidth = 0;
int Overlay::FlowLayout::layoutHeight = 0; 

Overlay::StackFrame *Overlay::hooverView = nullptr;
Overlay::StackOpen *Overlay::openView = nullptr;


extern CentralFrame *mapFrame;





Overlay::FlowLayout::FlowLayout(QWidget *parent) : QLayout(parent)
{
    setContentsMargins(0, 0, 0, 0);
    setSpacing(0);
    setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}


Overlay::FlowLayout::~FlowLayout()
{
    QLayoutItem *item;
    while ((item = takeAt(0)))
    {
		delete item->widget(); 
        delete item;
	}
}


int Overlay::FlowLayout::count() const
{
    return itemList.size();
}


QSize Overlay::FlowLayout::sizeHint() const
{
    return QSize(0, 0);
}


void Overlay::FlowLayout::addItem(QLayoutItem *item)
{
    itemList.append(item);
}


QLayoutItem *Overlay::FlowLayout::itemAt(int index) const
{
    return itemList.value(index);
}


QLayoutItem *Overlay::FlowLayout::takeAt(int index)
{
    if (index >= 0 && index < itemList.size())
        return itemList.takeAt(index);
        
    return nullptr;
}


void Overlay::FlowLayout::setGeometry(const QRect &rect)
{
	
	QLayout::setGeometry(rect);
	
	doLayout(rect);
	
}


void Overlay::FlowLayout::doLayout(const QRect &rect)
{
	 
    // find line height
    
    int lineHeight = 0;
    
	for (QLayoutItem *item : qAsConst(itemList)) 
	{
		QWidget *widget = item->widget();
		
		lineHeight = qMax(lineHeight, widget->height());
	}
	
	
	// set multi-line geometry
	
	int x = rect.x();
    int y = rect.y();
    
    int items = 0;
    int maxWidth = 0;
    
	for (QLayoutItem *item : qAsConst(itemList)) 
	{
        
		items++;
         
        if (items > FlowLayout::maxColumns && lineHeight > 0)
		{     
            x = rect.x();
            y = y + lineHeight;
            items = 1;          
        }
        
        
		QWidget *widget = item->widget();
		
		// vertical center
        item->setGeometry(QRect(x, y+((lineHeight - widget->height())/2), widget->width(), widget->height()));
       	
       		 
		x = x + widget->width();
		
		maxWidth = qMax(maxWidth, x - rect.x());
        
    }
   
    
    
    
    FlowLayout::layoutWidth = maxWidth;
    FlowLayout::layoutHeight = y + lineHeight - rect.y();
	
	
}







Overlay::StackFrame::StackFrame(QWidget *parent) : QLabel(parent)
{
	this->setFrameStyle(QFrame::Panel | QFrame::Plain);
	this->setStyleSheet("border: 2px solid black; background: white;");
	this->setVisible(false);
	
	grid = new QLabel(this);
	grid->setStyleSheet("border: 0px; margin: 0px; background: rgba(0,0,0,0);");
	grid->setVisible(true); 
	
	layout = new Overlay::FlowLayout(this);	
	grid->setLayout(layout);
			
}
 

void Overlay::StackFrame::moveThis(QPoint hotspot, int w, int h)
{
	
	if (hotspot.y() - h > 0)
		hotspot += QPoint(0, -h);
	else
		hotspot += QPoint(0, -h + std::abs(hotspot.y() - h));
		
	if (hotspot.x() + w > window()->width())
		hotspot += QPoint(-w + std::abs(this->window()->width() - hotspot.x()), 0);
		
	 
	this->move(hotspot);
	
}


void Overlay::StackFrame::setImage(QImage image)
{
	this->image = image;
}


void Overlay::StackFrame::setImages(int box, CentralFrame::Stack stack)
{	
	
	// clear layout of previous content
	
	QLayoutItem *item;
	
	
	while ((item = layout->takeAt(0)) != nullptr) 
	{	
		delete item->widget(); 
		delete item;   
	}

	
	// loop stack and create row(s) of counter images
	
	
	
	for (auto obj = stack.begin(); obj != stack.end(); ++obj)
	{	
		
		QLabel *icon = new QLabel();
		icon->setStyleSheet("border-style: none");
		icon->resize(obj->second->baseBuffer.width(), obj->second->baseBuffer.height());			
		icon->setPixmap(QPixmap::fromImage(obj->second->baseBuffer));
		
		
		layout->addWidget(icon);
		
	}
	
}


void Overlay::StackFrame::paintEvent(QPaintEvent *e)
{
	
	QPainter painter(this);
	
	
	painter.drawRect(border-1, border-1, this->image.width() + 1, this->image.height() + 1);
		
	QPoint point = QPoint(border, border);	
	painter.drawImage(point, this->image);
	
}




Overlay::StackOpen::Item::Item(StackOpen *parent) : QLabel(parent)
{
	this->setAttribute(Qt::WA_DeleteOnClose);
}




Overlay::StackOpen::StackOpen(QWidget *parent) : QFrame(parent)
{
	this->setFrameStyle(QFrame::Panel | QFrame::Plain);
	this->setStyleSheet("border: 2px solid black; background: white;");
	this->setVisible(false);
	
	
	layout = new Overlay::FlowLayout(this);
	this->setLayout(layout);
	
	currentPos = QPoint(-1,-1);
	
	
	//setAttribute(Qt::WA_TransparentForMouseEvents, true);

	
	
	setAcceptDrops(true);
	
		
}
 
 
 
inline static Overlay::StackOpen::Item *dragged = NULL;

 
 
void Overlay::StackOpen::mousePressEvent(QMouseEvent *event)
{
	
	
	if (event->button() == Qt::LeftButton)
	{
		
		Item *child = 
			static_cast<Item*>(childAt(event->position().toPoint()));
		
		
			
		if (!child)
			return;
			

		child->setVisible(false);

		dragged = child;
		
		int index = this->layout->indexOf(child);
		
		if (index == -1)
			return;
		
			
		QByteArray itemData;
		QDataStream dataStream(&itemData, QIODevice::WriteOnly);
		dataStream << index;

		QMimeData *mimeData = new QMimeData;
		mimeData->setData("application/x-alben-open", itemData);
			
		
		QImage image = child->pixmap().toImage();
		
		
			
		QPixmap img(image.size());
		
		img.fill(Qt::transparent);		
		QPainter painter;
		painter.begin(&img);
		painter.drawImage(0, 0, image); 				
		painter.end();
		
		
		
		
		
		
		
		QDrag *drag = new QDrag(this);
		drag->setMimeData(mimeData);
		drag->setPixmap(img);
		drag->setHotSpot(event->position().toPoint() - child->pos());
		
		
		
	
		auto result = drag->exec(Qt::MoveAction);
		
		if (result == Qt::IgnoreAction) 
		{
			child->setVisible(true);
			
			// toggle
			child->source->selected = !child->source->selected;
			// regen
			child->source->setImage();
			updateImages(child->source);
		}
		
	}
	
	
	
}


void Overlay::StackOpen::dragEnterEvent(QDragEnterEvent *event)
{		
	if (event->mimeData()->hasFormat("application/x-alben-open")) 
		event->acceptProposedAction();
	else 
		event->ignore();
}


void Overlay::StackOpen::dropEvent(QDropEvent *event)
{
	
	if (event->mimeData()->hasFormat("application/x-alben-open")) 
	{
		
		QByteArray itemData = event->mimeData()->data("application/x-alben-open");
		QDataStream dataStream(&itemData, QIODevice::ReadOnly);

		
		
		int fromIndex;
		dataStream >> fromIndex;
		
		
		Item *child = 
			static_cast<Item*>(childAt(event->position().toPoint()));
			
		if (!child)
			return;
		
			
		int toIndex = this->layout->indexOf(child);
		
		if (toIndex == -1)
			return;
		
		
		dragged->setVisible(true);
		
		// swap counters on board
		swapWidget(fromIndex, toIndex);
		
	
		event->acceptProposedAction();
			
	} 
	
	else 
	
		event->ignore();	
	
					
}






void Overlay::StackOpen::moveThis(QPoint hotspot, int w, int h)
{
	
	if (hotspot.y() - h > 0)
		hotspot += QPoint(0, -h);
	else
		hotspot += QPoint(0, -h + std::abs(hotspot.y() - h));
		
	if (hotspot.x() + w > this->window()->width())
		hotspot += QPoint(-w + std::abs(this->window()->width() - hotspot.x()), 0);
		
	 
	this->move(hotspot);
	
}



void Overlay::StackOpen::swapWidget(int fromIndex, int toIndex)
{
	
	Item *fromItem = (Item *)((QWidgetItem *)layout->itemAt(fromIndex))->widget();
	int fromZorder = fromItem->source->zorder;
	
	Item *toItem = (Item *)((QWidgetItem *)layout->itemAt(toIndex))->widget();
	int toZorder = toItem->source->zorder;
	
	
	
	fromItem->source->zorder = toZorder;
	toItem->source->zorder = fromZorder;
	
	Luau::doEvent("move", fromItem->source->name.c_str(), "Counter", "zorder", toZorder);
	Luau::doEvent("move", toItem->source->name.c_str(), "Counter", "zorder", fromZorder);
	Luau::doEvent("end", "", "", "", 0);				
	
	
	updateImages(fromItem->source);
	
	
}



void Overlay::StackOpen::updateImages(Counter *counter)
{	
	
	// clear layout of previous content
	
	QLayoutItem *item;
	
	
	while ((item = layout->takeAt(0)) != nullptr) 
	{	
		delete item->widget(); 
		delete item;   
	}
	
		
	// generate stack
	
	CentralFrame::Stack stack = {{counter->zorder, counter}};
	       
		
	for ( auto obj = Counter::counters.begin(); obj != Counter::counters.end(); ++obj  )	
	{
		QPoint p = QPoint(obj->second->state.x, obj->second->state.y);
		
		if (p == currentPos)		
			stack[obj->second->zorder] = obj->second;	
	}	
	
		
	// loop stack and add counter images to layout	
	
	for (auto obj = stack.begin(); obj != stack.end(); ++obj)
	{		
		Item *item = new Item(this);
		item->setStyleSheet("border-style: none");
		item->resize(obj->second->baseBuffer.width(), obj->second->baseBuffer.height());		
		item->setPixmap(QPixmap::fromImage(obj->second->baseBuffer));
		item->source = obj->second;
		
		QWidgetItem *listItem = new QWidgetItem(item);
		
		layout->addItem((QLayoutItem *)listItem);
		item->setVisible(true);	
	}
	
	
}




void Overlay::StackOpen::showStack()
{
	if (hooverView->isVisible())
		return;
	
	this->setVisible(true);
	Overlay::overlay->repaint();
}


void Overlay::StackOpen::hideStack()
{
	this->setVisible(false);
	Overlay::overlay->repaint();
}





Overlay::Overlay() : QFrame() 
{
  
	setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
	setAutoFillBackground(false);
	
	
	setAcceptDrops(true);
	
	//setAttribute(Qt::WA_TransparentForMouseEvents, true);
	overlayMask = QRegion(0, 0, 1, 1);
	setMask(overlayMask);
	
}


	
void Overlay::paintEvent(QPaintEvent *e)
{
	
	QPainter painter(this);
	
	
		
	
	clearMask();
	overlayMask = QRegion(0, 0, 1, 1);
	
	
	if (!Counter::haveOffset)
	{
		std::map<CentralFrame::Point, CentralFrame::Stack> stacks;
		
		
		for (auto obj = Counter::counters.begin(); obj != Counter::counters.end(); ++obj)
		{
			CentralFrame::Point p = (CentralFrame::Point){.x = obj->second->state.x, .y = obj->second->state.y};
			
			if (stacks.find( p ) == stacks.end()) 
			{
				// not found
				CentralFrame::Stack stack = {{obj->second->zorder, obj->second}};
				stacks[p] = stack;
			} 
			else 
			{
				// found			
				CentralFrame::Stack stack = stacks.at( p );
				stack[obj->second->zorder] = obj->second;
				stacks[p] = stack;
			}	
		}
		
		
		
		// render stack size numbers
		
		for (auto const& [point, stack] : stacks)	
		{	

			
			int stackSize = (int)stack.size();
					
			if (stackSize > 1)
			{
					
				painter.setRenderHint(QPainter::Antialiasing);
				painter.setRenderHint(QPainter::TextAntialiasing);
				painter.setRenderHint(QPainter::SmoothPixmapTransform);
					
				
				auto *top = prev(stack.end())->second;
				
				QRect rectangle = QRect(top->state.x + top->margin + top->width - 8, 
										top->state.y + top->margin + 8, 
										16, 
										16);
										
				overlayMask = overlayMask.united(rectangle);
				
					
				painter.setPen(QPen(QColor("#000000")));
				painter.setBrush(QBrush(Qt::black));
				painter.drawEllipse(rectangle);				
				
				painter.setFont(QFont("Arial", 10, QFont::Bold));
				painter.setPen(QPen(QColor("#ffffff")));
				QString str = QString::fromUtf8(std::to_string(stackSize).c_str());
				painter.drawText(rectangle, Qt::AlignHCenter, str);	

			}
				
					
		}
	}
	
	
	
	if (openView->isVisible())
		overlayMask = overlayMask.united(openView->geometry());
	
	if (hooverView->isVisible())
		overlayMask = overlayMask.united(hooverView->geometry());
		
		
		
	setMask(overlayMask);
	
}
