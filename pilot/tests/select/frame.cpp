#include <string>


#include "frame.h"
#include "overlay.h"
#include "luau.h"
#include "window.h"


using namespace std;


	

inline static CentralFrame::Stacks stacks;   // holds the dragged counter(s)

	
	
CentralFrame::CentralFrame(QWidget *parent, std::string name) : QFrame(parent)
{
	
	this->name = name;
	
	if (name == "Map")			
		setAcceptDrops(true);
	
}
	
	
				
void CentralFrame::mousePressEvent(QMouseEvent *event)
{
	
	
	if (Overlay::hooverView->isVisible())
		return;
	
	
	if (event->button() == Qt::LeftButton)
	{
	
		
		
		Counter::QtCounter *child = 
			static_cast<Counter::QtCounter*>(childAt(event->position().toPoint()));
			
		if (!child)
		{
			unselect();
			return;
		}
			
		
	
		QImage img;
		QRect  ghostRect = QRect();
		stacks.clear();
		
		
		if (anySelected(child->owner))	
			img = selectedGhostImage(ghostRect);
			
		else
			img = stackGhostImage(child->owner, ghostRect);
	
		QPixmap pix = QPixmap::fromImage(img);
		
		
	
		QByteArray itemData;
		QDataStream dataStream(&itemData, QIODevice::WriteOnly);
			
		QPoint counterOffset = event->position().toPoint() - ghostRect.topLeft();
		QPoint ghostOrigo = ghostRect.topLeft();			
																
		dataStream << counterOffset;
		dataStream << ghostOrigo;
		


		QMimeData *mimeData = new QMimeData;
		mimeData->setData("application/x-alben-counter", itemData);
			
			
				
		QDrag *drag = new QDrag(this);
		drag->setMimeData(mimeData);
		drag->setPixmap(pix);
		drag->setHotSpot(counterOffset);
		
		

		auto result = drag->exec(Qt::MoveAction);
		
		
		if (result == Qt::IgnoreAction) 
		{
			
			// select counter(s)
			
			// add to selection if SHIFT button (right or left) is pressed
			if (QApplication::keyboardModifiers() == Qt::ShiftModifier)
				stackSelect(child->owner);
			else
			{
				if (!anySelected(child->owner))
				{
					unselect();
					stackSelect(child->owner);
				}
			}	
		}
		
		
	}
	
}



void CentralFrame::dragEnterEvent(QDragEnterEvent *event)
{		
	if (event->mimeData()->hasFormat("application/x-alben-counter")) 		
		event->acceptProposedAction();
	else 
		event->ignore();
}



void CentralFrame::dropEvent(QDropEvent *event)
{
	
	if (event->mimeData()->hasFormat("application/x-alben-counter")) 
	{
		
		
		QByteArray itemData = event->mimeData()->data("application/x-alben-counter");
		QDataStream dataStream(&itemData, QIODevice::ReadOnly);

		QPoint counterOffset, ghostOrigo;
		
		
		dataStream >> counterOffset; 
		dataStream >> ghostOrigo;
		
	
		
		int droppedX = event->position().toPoint().x() - counterOffset.x();
		int droppedY = event->position().toPoint().y() - counterOffset.y();
		
		
		
		
		// check if actual movement has taken place
		 
		int minimumMovement = 8;
		
		
		QPoint source(ghostOrigo);
	
		QPoint target(droppedX, droppedY);
		
		
		if (abs(target.x() == source.x()) &&
			abs(target.y() == source.y()))
		{
			event->ignore();
			return;
		}
		
		
		if (abs(target.x() - source.x()) < minimumMovement &&
			abs(target.y() - source.y()) < minimumMovement)
		{
			event->acceptProposedAction();
			return;
		}
		
			
		
		
		if (event->source() == this)
		{
					
			QPoint delta = target - source;
			
					
			// move all counters
			
			for (auto const& [point, stack] : stacks)	
			{
			
				for ( auto obj = stack.begin(); obj != stack.end(); ++obj )		
				{															
					obj->second->state.x += delta.x();
					obj->second->state.y += delta.y();	
				}
		
			}
			
			
			// align all counters if possible and add them to undo stack
			
			for (auto const& [point, stack] : stacks)	
			{
			
				for ( auto obj = stack.begin(); obj != stack.end(); ++obj )		
				{	
										
					Counter::snaptoDefaultGrid (obj->second, obj->second->state.x, obj->second->state.y);
					
					obj->second->zorder = Counter::topZorder();
					
											
					Luau::doEvent("movetrigger", obj->second->name.c_str(), "", "", 0);
					Luau::doEvent("move", obj->second->name.c_str(), "Counter", "x", obj->second->state.x);
					Luau::doEvent("move", obj->second->name.c_str(), "Counter", "y", obj->second->state.y);
					Luau::doEvent("move", obj->second->name.c_str(), "Counter", "zorder", obj->second->zorder);
					
					obj->second->counter->raise();		
				}
				
			}
			
			Luau::doEvent("end", "", "", "", 0);
			
		}
		else
		{
			
			// holds per definition only one stack with one counter 
							
			
			Counter *counter = (stacks.begin()->second).begin()->second;
		
						
			if (counter->state.degrees != 0)
			{
				// sets the counter coordinate for rotated counter to the standard non-rotated
				// upper left corner coordinate of a counter
				droppedX += std::round(((counter->counter->width() - counter->width) / 2) - counter->margin);
				droppedY += std::round(((counter->counter->height() - counter->height) / 2) - counter->margin);
			}
			
	
			
			Counter::snaptoDefaultGrid (counter, droppedX, droppedY);
			
		
			const char *fromId = counter->name.c_str();
			const char *toId = std::to_string(Counter::nextId()).c_str();
			int zorder = Counter::topZorder();
				
			Luau::copyCounter(fromId, toId, counter->state.degrees, zorder, droppedX, droppedY);
			
			Luau::doCreate(fromId, "Create", toId, counter->state.degrees, zorder, droppedX, droppedY);		
			Luau::doEvent("end", "", "", "", 0);
			
			
			
			this->activateWindow();
			
			
		}
		
		
		event->acceptProposedAction();
		
		Overlay::overlay->repaint();
		
		
	} 
	
	else 
	
		event->ignore();
		
		
				
}



void CentralFrame::stackSelect(Counter *counter)
{
	
	QPoint point = QPoint(counter->state.x, counter->state.y);
	
	
	for (auto obj = Counter::counters.begin(); obj !=Counter::counters.end(); ++obj)
	{
		QPoint p = QPoint(obj->second->state.x, obj->second->state.y);
			
		if (p == point)	
		{	
			obj->second->selected = true;
			obj->second->setImage();
		}
	}
	
}


bool CentralFrame::anySelected(Counter *counter)
{
	
	// decide if any selected in this stack
	
	QPoint point = QPoint(counter->state.x, counter->state.y);
	
	
	for (auto obj = Counter::counters.begin(); obj !=Counter::counters.end(); ++obj)
	{
		QPoint p = QPoint(obj->second->state.x, obj->second->state.y);
			
		if (p == point)	
		{	
			if (obj->second->selected == true)
				return true;
		}
	}
	
	return false;
	
}


void CentralFrame::unselect()
{
	
	for (auto obj = Counter::counters.begin(); obj !=Counter::counters.end(); ++obj)
	{
		obj->second->selected = false;
		obj->second->setImage();	
	}
	
}





QImage CentralFrame::stackGhostImage(Counter *counter, QRect &totalRect)
{
	
	
	Point point = {counter->state.x, counter->state.y};
	
	Stack stack = {{counter->zorder, counter}};
	
	
	// find stack of this counter
	
	for (auto obj = Counter::counters.begin(); obj != Counter::counters.end(); ++obj)
	{
		
		Point p = (Point){.x = obj->second->state.x, .y = obj->second->state.y};
			
		if (p == point && obj->second != counter)	
			stack[obj->second->zorder] = obj->second;		
			
	}
	
	
		
	// find size of ghost
	
	
	for ( auto obj = stack.begin(); obj != stack.end(); ++obj )
	{		
			
		int x = obj->second->counter->pos().x();
		int y = obj->second->counter->pos().y();
		
		
		QPoint p = QPoint(x, y);
		
		QRect thisRect = QRect(p, obj->second->counter->size());
		
		totalRect = totalRect.united(thisRect);
		
	}
	

	
	// paint ghost
	
	QImage base = QImage (totalRect.size(), QImage::Format_ARGB32_Premultiplied);
		
	base.fill(Qt::transparent);


	
	QPainter *paint = new QPainter(&base);
		
	for ( auto obj = stack.begin(); obj != stack.end(); ++obj )
	{			
		
		// coordinates relative to paint surface
		int x = obj->second->counter->pos().x() - totalRect.x();
		int y = obj->second->counter->pos().y() - totalRect.y();
		
			
		paint->drawImage(x, y, obj->second->baseBuffer);
		
	}
	
	delete paint;
	
	
		
	// make transparent
	QImage transparent = QImage (totalRect.size(), QImage::Format_ARGB32_Premultiplied);
	transparent.fill(Qt::transparent);
	paint = new QPainter(&transparent);
	paint->setOpacity(0.5);
	paint->drawImage(0, 0, base);
	delete paint;
	
	
	
	stacks[point] = stack;
	
	
	return transparent;
	
}


QImage CentralFrame::selectedGhostImage(QRect &totalRect)
{
	
	
	// generate stacks of selected

	for (auto obj = Counter::counters.begin(); obj != Counter::counters.end(); ++obj)
	{
		
		if (obj->second->selected == true)
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
			
	}
	
	
	
	
	// find size of ghost
	
	
	for (auto const& [point, stack] : stacks)	
	{	
		
		for ( auto obj = stack.begin(); obj != stack.end(); ++obj )
		{		
				
			int x = obj->second->counter->pos().x();
			int y = obj->second->counter->pos().y();
			
			
			QPoint p = QPoint(x, y);
			
			QRect thisRect = QRect(p, obj->second->counter->size());
			
			totalRect = totalRect.united(thisRect);
			
		}
		
	}
	
	
	
	// paint ghost
	
	QImage base = QImage (totalRect.size(), QImage::Format_ARGB32_Premultiplied);
		
	base.fill(Qt::transparent);
	
	QPainter *paint = new QPainter(&base);
	
		
	for (auto const& [point, stack] : stacks)	
	{
	
		for ( auto obj = stack.begin(); obj != stack.end(); ++obj )
		{			
			
			// coordinates relative to paint surface
			int x = obj->second->counter->pos().x() - totalRect.x();
			int y = obj->second->counter->pos().y() - totalRect.y();
			
				
			paint->drawImage(x, y, obj->second->baseBuffer);
			
		}
	
	}
	
	delete paint;

	
	
	// make transparent
	QImage transparent = QImage (totalRect.size(), QImage::Format_ARGB32_Premultiplied);
	transparent.fill(Qt::transparent);
	paint = new QPainter(&transparent);
	paint->setOpacity(0.5);
	paint->drawImage(0, 0, base);
	delete paint;
	
	
	
	
	return transparent;

}




void CentralFrame::paintEvent(QPaintEvent *e)
{
	
			
	std::map<Point, Stack> stacks;
	
	
	std::map<int, Counter *> *input;
	
	
	
	if (this->name == "Map")
		input = &Counter::counters;
	else
		input = &Counter::repository;
	
	
	for (auto obj = input->begin(); obj != input->end(); ++obj)
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
	
	

	QPainter painter(this);
	
	
	// render all "stacks" with 1 or more counters
	
	for (auto const& [point, stack] : stacks)	
	{	

	
		int offset = 0;
			
			
		for ( auto obj = stack.begin(); obj != stack.end(); ++obj  )
		{		
			
			int x = obj->second->state.x;
			int y = obj->second->state.y;
			
			
			// offset is 0, 1, 2 ... the postion in the stack with 0 bottom
			if (Counter::haveOffset)
			{
				x = x + (Counter::stackOffset * offset);
				y = y - (Counter::stackOffset * offset);
			}
	
			
			offset++;

			
			QRect source(0, 
						 0, 
						 obj->second->baseBuffer.width(), 
						 obj->second->baseBuffer.height());
							
			QRect dest(x,
					   y,
					   obj->second->width + 2*obj->second->margin,					   
					   obj->second->height + 2*obj->second->margin);
					   
					   
						
						  
			source.moveCenter( dest.center() );
			
			obj->second->counter->move(source.topLeft());
			
			
			obj->second->counter->raise();	
					
		}
		
				
	}
	
	
}
