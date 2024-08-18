#include <string>


#include "frame.h"
#include "overlay.h"
#include "luau.h"
#include "window.h"
#include "io.h"
#include "scale.h"


using namespace std;


extern CentralFrame *repositoryFrame;
extern Scale *scaled;

	

inline static CentralFrame::Stacks stacks;   // holds the dragged counter(s)

string CentralFrame::backgroundID;

	
	
CentralFrame::CentralFrame(QWidget *parent, std::string name, QScrollArea *scrollArea) : QFrame(parent)
{
	
	this->name = name;
	
	if (name == "Map")
	{
		this->scrollArea = scrollArea;
		setAcceptDrops(true);
	}
	
	if (name == "Repository")
	{
		this->setLayout(new QVBoxLayout());
		this->layout()->setContentsMargins(0, 0, 0, 0);
		this->layout()->setSpacing(0);
	}
	
}
	
	
				
void CentralFrame::mousePressEvent(QMouseEvent *event)
{
	
	
	if (Overlay::hooverView->isVisible())
		return;
	
	
	if (event->button() == Qt::LeftButton)
	{
	
		QDrag *drag = new QDrag(this);
		drag->deleteLater();
		
		
		
		QPoint frameCoordinates;
				
		if (this->name == "Repository")
			frameCoordinates = mapTo((QWidget *)repositoryFrame, event->position().toPoint());
		else
			frameCoordinates = event->position().toPoint();
				
				
		
		Counter::QtCounter *child = 
			dynamic_cast<Counter::QtCounter*>(childAt(frameCoordinates));
			
			
		
		
		if (child == nullptr)
		{
			if (this->name == "Map")		 
			{
				
				this->setCursor(Qt::ClosedHandCursor);
				
				QByteArray itemData;
				QDataStream dataStream(&itemData, QIODevice::WriteOnly);																		
				dataStream << event->position().toPoint();
				dataStream << scrollArea->horizontalScrollBar()->value();
				dataStream << scrollArea->verticalScrollBar()->value();
			
				QMimeData *mimeData = new QMimeData;
				mimeData->deleteLater();
				mimeData->setData("application/x-alben-drag", itemData);
								
				drag->setMimeData(mimeData);
						
			}
			else
				return;
			
		}
		else
		{	
		
			if (!Luau::beforeDrag(child->owner->name.c_str()))
				return;
			
		
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
				
		
			QPoint o1 = mapToGlobal(QPoint(0,0));
			QPoint o2 = ((QWidget *)child->parent())->mapToGlobal(QPoint(0,0));
			
			QPoint origo = o2 - o1;
		
		
			
			QPoint counterOffset = frameCoordinates - origo - ghostRect.topLeft();
					
			if (this->name == "Repository")	
				counterOffset *= Scale::ratio;
				
			
			QPoint ghostOrigo = ghostRect.topLeft();
			
			
				
																	
			dataStream << counterOffset;
			dataStream << ghostOrigo;
			


			QMimeData *mimeData = new QMimeData;
			mimeData->setData("application/x-alben-counter", itemData);
				
				
					
			drag->setMimeData(mimeData);
			drag->setPixmap(pix);
			drag->setHotSpot(counterOffset);
		
		}
		
		

		auto result = drag->exec(Qt::MoveAction);
		
		
		if (result == Qt::IgnoreAction) 
		{
			
			if (!child)
			{
				unselect();
				Overlay::openView->setVisible(false);
				Overlay::hooverView->setVisible(false);
			}
			else
			{	
			
				// add to selection if SHIFT button (right or left) is pressed
				if (QApplication::keyboardModifiers() == Qt::ShiftModifier)
					stackSelect(child->owner);
				else
				if (QApplication::keyboardModifiers() == Qt::ControlModifier)
					Overlay::openView->open(child->owner, event->position().toPoint());
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
	
}



void CentralFrame::dragEnterEvent(QDragEnterEvent *event)
{		
	this->activateWindow();
	
	if (event->mimeData()->hasFormat("application/x-alben-counter") ||
		event->mimeData()->hasFormat("application/x-alben-drag")) 		
		event->acceptProposedAction();		
	else 
		event->ignore();
	
}



void CentralFrame::dragMoveEvent(QDragMoveEvent *event)
{		
	
	if (event->source() == this)
	{	
		int horizontalPos = scrollArea->horizontalScrollBar()->value();
		int verticalPos = scrollArea->verticalScrollBar()->value();
		
			
		if (event->mimeData()->hasFormat("application/x-alben-counter"))
		{	
			int magicNumber = 64;		// distance to edge to begin scrolling
			int increment = 8;			// the scroll amount
			
			QPoint relativeToWindow = mapToParent(event->position().toPoint());
			
			int width = ((QMainWindow *)this->parent())->width();
			int height = ((QMainWindow *)this->parent())->height();
			
												
			if (width - relativeToWindow.x() < magicNumber)
			{
				scrollArea->horizontalScrollBar()->setValue(horizontalPos + increment);
			}
			else
			if (relativeToWindow.x() < magicNumber)
			{
				scrollArea->horizontalScrollBar()->setValue(horizontalPos - increment);
			}
			else
			if (height - relativeToWindow.y() < magicNumber)
			{
				scrollArea->verticalScrollBar()->setValue(verticalPos + increment);
			}
			else
			if (relativeToWindow.y() < magicNumber)
			{
				scrollArea->verticalScrollBar()->setValue(verticalPos - increment);
			}
			
		}
		
		else
		
		if (event->mimeData()->hasFormat("application/x-alben-drag"))
		{
			
			QByteArray itemData = event->mimeData()->data("application/x-alben-drag");
			QDataStream dataStream(&itemData, QIODevice::ReadOnly);

			QPoint lastPoint;
			
			dataStream >> lastPoint; 

			scrollArea->horizontalScrollBar()->setValue(
				horizontalPos + (lastPoint.x() - event->position().toPoint().x()));
			scrollArea->verticalScrollBar()->setValue(
				verticalPos + (lastPoint.y() - event->position().toPoint().y()));
		}
		
	}
	
		
	if (event->mimeData()->hasFormat("application/x-alben-counter") ||
		event->mimeData()->hasFormat("application/x-alben-drag")) 		
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
					
					obj->second->state.zorder = Counter::topZorder();
					
											
					Luau::doEvent("movetrigger", obj->second->name.c_str(), "", "", 0);
					Luau::doEvent("move", obj->second->name.c_str(), "Counter", "x", obj->second->state.x);
					Luau::doEvent("move", obj->second->name.c_str(), "Counter", "y", obj->second->state.y);
					Luau::doEvent("move", obj->second->name.c_str(), "Counter", "zorder", obj->second->state.zorder);
					
					obj->second->counter->raise();		
				}
				
			}
			
			Luau::doEvent("end", "", "", "", 0);
			
			repaint();
			
		}
		else
		{
			
			// holds per definition only one stack with one counter 
							
			
			Counter *counter = (stacks.begin()->second).begin()->second;
		
						
			if (counter->state.degrees != 0)
			{
				// sets the counter coordinate for rotated counter to the standard non-rotated
				// upper left corner coordinate of a counter
				
				droppedX += std::round(((counter->scaledBuffer.width() - counter->scaledWidth) / 2) - counter->scaledMargin);
				droppedY += std::round(((counter->scaledBuffer.height() - counter->scaledHeight) / 2) - counter->scaledMargin);
			}
			
	
			
			Counter::snaptoDefaultGrid (counter, droppedX, droppedY);
			
		
			const char *fromId = counter->name.c_str();
			const char *toId = std::to_string(Counter::nextId()).c_str();
			int zorder = Counter::topZorder();
				
			// create Luau representation and C++ representation of counter
			Luau::copyCounter(fromId, toId, zorder, droppedX, droppedY);
			
			// add counter to undo stack
			//Luau::doCreate(fromId, "Create", toId, counter->state.degrees, zorder, false, droppedX, droppedY);		
			Luau::doCreate(toId, "Create");
			Luau::doEvent("end", "", "", "", 0);
			
			
			
			this->activateWindow();
			
			
		}
		
		
		event->acceptProposedAction();
		
		Overlay::overlay->repaint();
		
		
	}
	
	else 
	
	if (event->mimeData()->hasFormat("application/x-alben-drag")) 
	{
		
		this->setCursor(Qt::ArrowCursor);
				
		QByteArray itemData = event->mimeData()->data("application/x-alben-drag");
		QDataStream dataStream(&itemData, QIODevice::ReadOnly);

		QPoint lastPoint;

		int scrollHorizontal, scrollVertical;
			
		dataStream >> lastPoint; 
		dataStream >> scrollHorizontal;
		dataStream >> scrollVertical;	 
		
		
		if (scrollArea->horizontalScrollBar()->value() == scrollHorizontal &&
			scrollArea->verticalScrollBar()->value() == scrollVertical)
			event->ignore();
		else	
			event->acceptProposedAction();
		
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
	
	Stack stack = {{counter->state.zorder, counter}};
	
	
	// find stack of this counter
	
	for (auto obj = Counter::counters.begin(); obj != Counter::counters.end(); ++obj)
	{
		
		Point p = (Point){.x = obj->second->state.x, .y = obj->second->state.y};
			
		if (p == point && obj->second != counter)	
			stack[obj->second->state.zorder] = obj->second;		
			
	}
	
	
		
	// find size of ghost
	
	
	for ( auto obj = stack.begin(); obj != stack.end(); ++obj )
	{		
			
		int x = obj->second->counter->pos().x();
		int y = obj->second->counter->pos().y();
		
		
		QPoint p = QPoint(x, y);
		
		QRect thisRect = QRect(p, obj->second->scaledBuffer.size());
		
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
		
			
		paint->drawImage(x, y, obj->second->scaledBuffer);
		
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
				Stack stack = {{obj->second->state.zorder, obj->second}};
				stacks[p] = stack;
			} 
			else 
			{
				// found			
				Stack stack = stacks.at( p );
				stack[obj->second->state.zorder] = obj->second;
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
			
			QRect thisRect = QRect(p, obj->second->scaledBuffer.size());
			
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
			
				
			paint->drawImage(x, y, obj->second->scaledBuffer);
			
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
	
	
	
	if (this->name == "Repository")
		return;		// layout does rendering
	
	
	
	QPainter painter(this);
		
	
				
	std::map<Point, Stack> stacks;

		
	QImage image = scaled->getScaledImage(backgroundID);
	painter.drawImage(0,0,image);
	
	int offsetAmount = Counter::stackOffset * Scale::ratio;

	
	
	
	for (auto obj = Counter::counters.begin(); obj != Counter::counters.end(); ++obj)
	{
		Point p = (Point){.x = obj->second->state.x, .y = obj->second->state.y};
		
		if (stacks.find( p ) == stacks.end()) 
		{
			// not found
			Stack stack = {{obj->second->state.zorder, obj->second}};
			stacks[p] = stack;
		} 
		else 
		{
			// found			
			Stack stack = stacks.at( p );
			stack[obj->second->state.zorder] = obj->second;
			stacks[p] = stack;
		}	
	}
	
	


	
	
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
				x = x + (offsetAmount * offset);
				y = y - (offsetAmount * offset);
			}
	
			
			offset++;

			
			QRect source(0, 
						 0, 
						 obj->second->scaledBuffer.width(), 
						 obj->second->scaledBuffer.height());
							
			QRect dest(x,
					   y,
					   obj->second->scaledWidth + 2*obj->second->scaledMargin,					   
					   obj->second->scaledHeight + 2*obj->second->scaledMargin);
					   					   
						
						  
			source.moveCenter( dest.center() );
			
			obj->second->counter->move(source.topLeft());
				
					
		}
		
				
	}
	
	
}
