#include "overlay.h"
#include "luau.h"



int Overlay::border = 9;
Overlay *Overlay::overlay = nullptr;;


int Overlay::FlowLayout::maxColumns = 4;


Overlay::StackFrame *Overlay::hooverView = nullptr;
Overlay::StackOpen *Overlay::openView = nullptr;


extern CentralFrame *mapFrame;





Overlay::FlowLayout::FlowLayout(QWidget *parent) : QLayout(parent)
{
    setContentsMargins(0, 0, 0, 0);
    setSpacing(0);
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
    return minimumSize();
}


QSize Overlay::FlowLayout::minimumSize() const
{
	if (itemList.count() == 0)
		return QSize(0,0);
	
	
  
    
    int items = 0;
    
    int width = 0;
    int height = 0;
    
    int lineHeight = 0;
    
    QRect total;
    
    
	for (QLayoutItem *item : std::as_const(itemList)) 
	{
          
        if (items == FlowLayout::maxColumns)
		{ 
			lineHeight += height;
			
			height = 0;
            width = 0;   
            items = 0;         
        }
        
        QWidget *widget = item->widget();
        
        assert(widget != nullptr);
        
        
        QRect rect(width, lineHeight, widget->width(), widget->height());
             
			 
        width += widget->width();
              	
		height = qMax(height, widget->height());
		
		total = total.united(rect);
		
		
        items++;
           
        	
    }   	
    
    
    const QMargins margins = contentsMargins();
    
    return total.size() + QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
    
}


void Overlay::FlowLayout::addItem(QLayoutItem *item)
{
    itemList.append(item);
}


Qt::Orientations Overlay::FlowLayout::expandingDirections() const
{
    return { };
}


bool Overlay::FlowLayout::hasHeightForWidth() const
{
    return true;
}


int Overlay::FlowLayout::heightForWidth(int width) const
{
    int height = doLayout(QRect(0, 0, width, 0), true);
    return height;
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
	
	doLayout(rect, false);
	
}


int Overlay::FlowLayout::doLayout(const QRect &rect, bool measure) const
{
	 
    if (itemList.count() == 0)
		return 0;
	
		
	int height = 0;
    int totalHeight = 0;
	
	// set multi-line layout
	 
	int w = rect.width();
	int h = rect.height();
	int n = itemList.count();
	int incx = w / (n < FlowLayout::maxColumns? n : FlowLayout::maxColumns);	
	int incy = h / std::ceil(n / (float)FlowLayout::maxColumns);
	
	
	const QMargins margins = contentsMargins();
	

	
	int x = margins.left() + incx/2;
    int y = margins.top() + incy/2;
    
    
    int items = 0;
    
  
	for (QLayoutItem *item : std::as_const(itemList)) 
	{
      
         
        if (items == FlowLayout::maxColumns)
		{     
            totalHeight += height;
			height = 0;
            
            x = margins.left() + incx/2;
            y += incy;
            items = 0;          
        }
             
	
		
		QRect current(QPoint(x - (item->sizeHint().width()/2), 
							 y - (item->sizeHint().height()/2)),
					  item->sizeHint());
		   	   
		      	   
		if (!measure)
		{			   
			item->setGeometry(current);
		}
		
			
        x += incx;
        
        
        height = qMax(height, item->sizeHint().height());
        totalHeight = qMax(totalHeight, height);
        
        items++;
        	
    }
    
   
    return totalHeight +  margins.top() + margins.bottom(); 
   
}







Overlay::StackFrame::StackFrame(QWidget *parent) : QLabel(parent)
{
	this->setFrameStyle(QFrame::Panel | QFrame::Plain);
	this->setStyleSheet("border: 2px solid black; background: white;");
	this->setVisible(false);
	
	grid = new QLabel(this);
	grid->setStyleSheet("border: 0px; margin: 0px; background: rgba(0,0,0,0);");
	grid->setVisible(true); 
	layout = new Overlay::FlowLayout(grid);	
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


void Overlay::StackFrame::setImages(CentralFrame::Stack stack)
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
	this->setLineWidth(2);
	this->setStyleSheet("background: white;");
	this->setVisible(false);
	
	this->layout = new Overlay::FlowLayout(this);
	this->layout->setContentsMargins(2, 2, 2, 2);	
	this->setLayout(layout);
	
	
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



void Overlay::StackOpen::open(Counter *counter, QPoint pos)
{
     
    if (Overlay::hooverView->isVisible())
		return;
    
    if (this->isVisible())
    {
		this->hideStack();
		return;	
	}
	
	
	pos = counter->parentFrame->mapToParent(pos);
	
	this->updateImages(counter);
	
	
	QSize size = Overlay::openView->layout->minimumSize();
	
	this->resize(size);
	
		
	this->moveThis(pos, size.width(), size.height());
        
    this->showStack();
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
	int fromZorder = fromItem->source->state.zorder;
	
	Item *toItem = (Item *)((QWidgetItem *)layout->itemAt(toIndex))->widget();
	int toZorder = toItem->source->state.zorder;
	
	
	
	fromItem->source->state.zorder = toZorder;
	toItem->source->state.zorder = fromZorder;
	
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
	
	CentralFrame::Stack stack = {{counter->state.zorder, counter}};
	
	QPoint point = QPoint(counter->state.x, counter->state.y);
	       
		
	for ( auto obj = Counter::counters.begin(); obj != Counter::counters.end(); ++obj  )	
	{
		QPoint p = QPoint(obj->second->state.x, obj->second->state.y);
		
		if (p == point)
			stack[obj->second->state.zorder] = obj->second;	
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
		
		obj->second->counter->raise();		// this does the reorder in Qt	
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
