#include "luau.h"
#include "counter.h"
#include "window.h"



	

inline static Counter::QtCounter *dragged = NULL;

	
	
CentralFrame::CentralFrame(QWidget *parent, std::string name) : QFrame(parent)
{
	
	this->name = name;
	
	if (name == "Map")			
		setAcceptDrops(true);
	
}


				
void CentralFrame::mousePressEvent(QMouseEvent *event)
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
		
		if (dragged->owner->state.moved && 
			dragged->owner->state.degrees == 0)
		{	
			QString file = QString::fromStdString(Counter::resources["_Moved"]);	
			QImage const mask(file);
			dx -= mask.width();
			dy -= mask.width();		
		}
		
		
	
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
			dragged->owner->setGUI();
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
