#include "overlay.h"
#include "luau.h"
#include "scale.h"
#include "window.h"


extern Scale *scaled;


int Overlay::border = 9;
Overlay *Overlay::overlay = nullptr;;

// 5 is default, max columns set by set_row_length
int Overlay::FlowLayout::maxColumns = 5;


Overlay::StackFrame *Overlay::hooverView = nullptr;
Overlay::StackOpen *Overlay::openView = nullptr;







Overlay::FlowLayout::FlowLayout(QWidget *parent) : QLayout(parent)
{
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
    return itemList.count();
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
    
    int maxWidth = 0;
    int maxHeight = 0;
    
    
    
	for (QLayoutItem *item : std::as_const(itemList)) 
	{
          
        if (items == FlowLayout::maxColumns)
		{   
            items = 0;         
        }
        
        QWidget *widget = item->widget();
        
        assert(widget != nullptr);
        
       
        
        maxWidth = qMax(maxWidth, widget->width());
              	
		maxHeight = qMax(maxHeight, widget->height());
		
		
		
        items++;
           
        	
    }   	
    
    
    const QMargins margins = contentsMargins();
    
    int w = (itemList.size() < FlowLayout::maxColumns ? itemList.size() : FlowLayout::maxColumns);
    int h = std::ceil(itemList.size() / (float)FlowLayout::maxColumns);
    QSize total = QSize(maxWidth * w, maxHeight * h);
  
  
    return total + QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
    
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
    if (index >= 0 && index < itemList.count())
        return itemList.takeAt(index);
        
    return 0;
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

	int n = itemList.count();
	int incx = rect.width() / (n < FlowLayout::maxColumns? n : FlowLayout::maxColumns);	
	int incy = rect.height() / std::ceil(n / (float)FlowLayout::maxColumns);
	

    int x = rect.x();
    int y = rect.y();
     
	    
    int items = 0;
    
  
	for (QLayoutItem *item : std::as_const(itemList)) 
	{
      
         
        if (items == FlowLayout::maxColumns)
		{           
            
            x = rect.x();
            y += incy;
            
            totalHeight += height;
			height = 0;
            
            items = 0;          
        }
             		
	

		QRect current(QPoint(x, 
							 y), 
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
    
    
	const QMargins margins = contentsMargins(); 
   
    return totalHeight + margins.top() + margins.bottom(); 
   
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
	grid->layout()->setContentsMargins(0, 0, 0, 0);
	
	//
	
	image = new QLabel(this);
	image->setStyleSheet("border: 1px solid black; margin: 0px; background: rgba(0,0,0,0);");
	image->setVisible(false);
	image->resize(0,0);		 
	image->move(border, border);
	
	coordinate = new QLabel(this);
	coordinate->setStyleSheet("border: 0px; margin: 0px; font: 14pt 'Times', serif; ; qproperty-alignment: AlignCenter;");
	coordinate->setVisible(false);
	coordinate->resize(0,0);	
	
			
}
 

void Overlay::StackFrame::moveThis(QPoint hotspot, int w, int h)
{
	
	int vscroll = Overlay::overlay->scrollArea->verticalScrollBar()->value();
		
	
	if (hotspot.y() - h - vscroll > 0)
		hotspot += QPoint(0, -h);
	else
		hotspot += QPoint(0, -h + std::abs(hotspot.y() - h - vscroll));
	
	
		
	int step = Overlay::overlay->scrollArea->horizontalScrollBar()->pageStep();	
	int hscroll = Overlay::overlay->scrollArea->horizontalScrollBar()->value();
	
	if (step - (hotspot.x() - hscroll + w) < 0)
		hotspot += QPoint(-std::abs(step - (hotspot.x() - hscroll + w)), 0);
	
	 
	 
	this->move(hotspot);
	
}


QSize Overlay::StackFrame::setImage(QImage image)
{
	// +2 is for 1px border
	this->image->resize(image.width() + 2, image.height() + 2);	
	this->image->setPixmap(QPixmap::fromImage(image));
	this->image->setVisible(true);
		
	return QSize(this->image->size());
}


QSize Overlay::StackFrame::setPlace(QString place)
{	
	this->coordinate->setText(place);
	
	int width = this->coordinate->fontMetrics().boundingRect(place).width();
	int height = this->coordinate->fontMetrics().boundingRect(place).height();
	
	width = width > this->image->size().width() ? width : this->image->size().width();
	
	this->coordinate->resize(width, height);	
	this->coordinate->setVisible(true);
	
	this->coordinate->move(border, this->image->size().height() + border);
	
	return QSize(this->coordinate->size());
	
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
	
	
	bool card = stack.begin()->second->table.find("Card") != stack.begin()->second->table.end();
	
	
	
	// loop stack and create row(s) of counter images		
		
	for (auto obj = stack.begin(); obj != stack.end(); ++obj)
	{	
		
		QLabel *icon = new QLabel();
		icon->setStyleSheet("border-style: none");
		
		
		if (obj->second->baseBuffer.height() > 350)
		{
			
			QImage base = obj->second->baseBuffer.scaledToHeight( 350, Qt::SmoothTransformation);
			
			icon->resize(base.width(), base.height());			
			icon->setPixmap(QPixmap::fromImage(base));
			
		}	
		else
		{
			icon->resize(obj->second->baseBuffer.width(), obj->second->baseBuffer.height());			
			icon->setPixmap(QPixmap::fromImage(obj->second->baseBuffer));
		}
		
		if (!card)
			layout->addWidget(icon);
		else
		{
			// show only top card, it is the last in stack
			if (obj->second == prev(stack.end())->second)
				layout->addWidget(icon);
		}	
		
	}
	
	
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
			
		if (!Luau::selectable(child->source->name.c_str()))	
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
		drag->deleteLater();
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
		this->hideStack();
		


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
			
	
	
	int width = Overlay::overlay->scrollArea->horizontalScrollBar()->width();
	int scroll = Overlay::overlay->scrollArea->horizontalScrollBar()->value();
		
	if (hotspot.x() + w > scroll + width)
		hotspot -= QPoint(std::abs((hotspot.x() + w) - (scroll + width)), 0);			
	 
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
	
	// clear layout of content
	
	QLayoutItem *item;
	
	
	while ((item = layout->takeAt(0)) != nullptr) 
	{	
		delete item->widget(); 
		delete item;   
	}
	
		
	// generate stack
	
	CentralFrame::Stack stack = {{counter->state.zorder, counter}};
	
	QPoint point = QPoint(counter->state.cx, counter->state.cy);
	       
		
	for ( auto obj = Counter::counters.begin(); obj != Counter::counters.end(); ++obj  )	
	{
		QPoint p = QPoint(obj->second->state.cx, obj->second->state.cy);
		
		if (p == point)
		{
			stack[obj->second->state.zorder] = obj->second;
			obj->second->disabled = true;
		}	
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
	// force redraw of mask layer
	Overlay::overlay->clearMask();
}


void Overlay::StackOpen::hideStack()
{
	this->setVisible(false);
	
	
	// clear layout of content
	
	QLayoutItem *item;
	
	
	while ((item = layout->takeAt(0)) != nullptr) 
	{	
		((Counter *)(((Item *)item->widget())->source))->disabled = false;
		delete item->widget(); 
		delete item;   
	}
	
	
	// force redraw of mask layer
	Overlay::overlay->clearMask();
	
}






Overlay::Overlay(QWidget *parent, QScrollArea *scrollArea) : QFrame(parent)
{
  
	setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
	setAutoFillBackground(false);
	
	
	setAcceptDrops(true);
	
	this->scrollArea = scrollArea;
	
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
		CentralFrame::Stacks stacks;
		
		painter.setRenderHint(QPainter::Antialiasing);
		painter.setRenderHint(QPainter::TextAntialiasing);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		
			
		
		for (auto obj = Counter::counters.begin(); obj != Counter::counters.end(); ++obj)
			if (obj->second->state.tag == "main")
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
		
		painter.setFont(QFont("Arial", 10, QFont::Bold));
		
		
		for (auto const& [point, stack] : stacks)	
		{	
		
			int stackSize = (int)stack.size();
					
			if (stackSize > 1)
			{		
				
				auto *top = prev(stack.end())->second;
				
				
				int x = std::round(top->state.x * Window::getInstance("main")->frame->scaleFraction);
				int y = std::round(top->state.y * Window::getInstance("main")->frame->scaleFraction);
				
				
				if (Scale::rotation != 0)
					scaled->rotate(top, Scale::rotation, x, y);
					
					
				
				// margin is 9
				QRect rectangle = 
					QRect(x + top->scaledMargin + top->scaledWidth - 8, 
						  y + 1, 
						  16, 
						  16);
					
			
										
				
						
				painter.setPen(Qt::NoPen);
				painter.setBrush(QBrush(Qt::black));
				painter.drawEllipse(rectangle);				
								
				painter.setPen(QPen(QColor("#ffffff")));
				QString str = QString::fromUtf8(std::to_string(stackSize).c_str());
				painter.drawText(rectangle, Qt::AlignHCenter, str);								
				
				
				overlayMask = overlayMask.united(rectangle);
				
			}			
					
		}
		
		
	}
	
	
	if (openView->isVisible())
		overlayMask = overlayMask.united(openView->geometry());
	
	if (hooverView->isVisible())
		overlayMask = overlayMask.united(hooverView->geometry());
		
		
		
	setMask(overlayMask);
	
}
