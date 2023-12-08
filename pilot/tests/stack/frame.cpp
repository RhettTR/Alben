#include <string>


#include "frame.h"
#include "luau.h"
#include "window.h"


using namespace std;


	

inline static Counter::QtCounter *dragged = NULL;

	
	
CentralFrame::CentralFrame(QWidget *parent, std::string name) : QFrame(parent)
{
	
	this->name = name;
	
	if (name == "Map")			
		setAcceptDrops(true);
	
}

				
void CentralFrame::mousePressEvent(QMouseEvent *event)
{
	
	if (event->button() == Qt::LeftButton)
	{
	
		Counter::QtCounter *child = 
			static_cast<Counter::QtCounter*>(childAt(event->position().toPoint()));
			
		if (!child)
			return;
			



		dragged = child;
		
		
			
		QByteArray itemData;
		QDataStream dataStream(&itemData, QIODevice::WriteOnly);
		dataStream << QPoint(event->position().toPoint() - child->pos());

		QMimeData *mimeData = new QMimeData;
		mimeData->setData("application/x-alben-counter", itemData);
			
		
		QPixmap pixmap = QPixmap::fromImage(child->owner->baseBuffer);
			
		QPixmap pix(pixmap.size());
		pix.fill(Qt::transparent);		
		QPainter painter;
		painter.begin(&pix);
		painter.setOpacity(0.5);
		painter.drawPixmap(0, 0, pixmap); 				
		painter.end();
		
		
		QDrag *drag = new QDrag(this);
		drag->setMimeData(mimeData);
		drag->setPixmap(pix);
		drag->setHotSpot(event->position().toPoint() - child->pos());

		

		if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction) 
		{
			//child->close();
		} 
		else 
		{
			child->show();
			//child->setPixmap(pixmap);
		}
	}
	
}


void CentralFrame::dragEnterEvent(QDragEnterEvent *event)
{		
	if (event->mimeData()->hasFormat("application/x-alben-counter")) 
	{
		if (event->source() == this) 
		{
			event->setDropAction(Qt::MoveAction);
			event->accept();					
		} 
		else 
		{
			event->acceptProposedAction();
		}
	} 
	else 
		event->ignore();
}


void CentralFrame::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasFormat("application/x-alben-counter")) 
	{
		
		QByteArray itemData = event->mimeData()->data("application/x-alben-counter");
		QDataStream dataStream(&itemData, QIODevice::ReadOnly);

		QPixmap pixmap;
		QPoint offset;
		dataStream >> offset;
	
		
		int dx = std::round(event->position().x() - offset.x());
		int dy = std::round(event->position().y() - offset.y());
		
		dx += std::round((dragged->width() - dragged->owner->width) / 2);
		dy += std::round((dragged->height() - dragged->owner->height) / 2);
		
		
	
		Counter::snaptoDefaultGrid (dragged->owner, &dx, &dy);
		
		
		if (event->source() == this)
		{
			
			int minimumMovement = 4;

			
			if (abs(dragged->owner->state.x - dx) > minimumMovement ||
			    abs(dragged->owner->state.y - dy) > minimumMovement)
			{
				dragged->owner->state.x = dx;
				dragged->owner->state.y = dy;
				
				Luau::doEvent("movetrigger", dragged->owner->name.c_str(), "", "", 0);
				Luau::doEvent("move", dragged->owner->name.c_str(), "Counter", "x", dx);
				Luau::doEvent("move", dragged->owner->name.c_str(), "Counter", "y", dy);
				Luau::doEvent("end", "", "", "", 0);
			}
			
			dragged->owner->zorder = Counter::topZorder();
			dragged->owner->parentFrame->repaint();
			dragged->raise();				// NB!!
			
		}
		else
		{
			const char *fromId = dragged->owner->name.c_str();
			const char *toId = std::to_string(Counter::nextId()).c_str();
			
			
			Luau::copyCounter(fromId, toId, dragged->owner->state.degrees, dx, dy);
			
			Luau::doCreate(fromId, "Create", toId, dragged->owner->state.degrees, dx, dy);		
			Luau::doEvent("end", "", "", "", 0);
			
			this->activateWindow();
			
		}
		
		
	} 
	else 
		event->ignore();			
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
					   obj->second->width,					   
					   obj->second->height);
					   
					   
						
						  
			source.moveCenter( dest.center() );
			
			obj->second->counter->move(source.topLeft());
			
		}
		
				
	}
	
	
}
