#include <string>


#include "frame.h"
#include "overlay.h"
#include "luau.h"
#include "window.h"
#include "io.h"
#include "scale.h"
#include "toolbar.h"


using namespace std;


extern IO *io;
extern CentralFrame *repositoryFrame;
extern Scale *scaled;

	

inline static CentralFrame::Stacks stacks;   // holds the dragged counter(s)

QScrollArea *CentralFrame::buttonParent;
string CentralFrame::backgroundID;


bool CentralFrame::openStackoffset = true;	// true = open stack with offset method
bool CentralFrame::facingMatters = false;	// true = rotate counters when map rotates





CentralFrame::Button::Button(const QString &text, QWidget *parent) : QPushButton(text, parent) {}
	
	
	
	
CentralFrame::CentralFrame(QWidget *parent, std::string name, QScrollArea *scrollArea) : QFrame(parent)
{
	
	this->name = name;
	
	if (name == "Map")
	{
		this->scrollArea = scrollArea;
		buttonParent = this->scrollArea;
		setAcceptDrops(true);
		setFrameStyle(QFrame::NoFrame);
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
			
			if (child->owner->disabled)
				return;
				
			if (IO::stepping)
				return;
		
			if (!Luau::beforeDrag(child->owner->name.c_str()))
				return;
			
		
			QImage img;
			QRect  ghostRect = QRect();			
			stacks.clear();
			Counter *dummy;
	
			
			if (this->name == "Repository")
				//update image so that ghost is scaled
				child->owner->setImage();	
			
			
			if (anySelected(child->owner, dummy ))		
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
				counterOffset *= Scale::scaleFraction;
				
			
			QPoint ghostOrigo = ghostRect.topLeft();
			
			int id = child->owner->id;
			      
																			
			dataStream << counterOffset;
			dataStream << ghostOrigo;
			dataStream << id;


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
				if (!Overlay::openView->isVisible())
					unselect();
				Overlay::openView->hideStack();
				Overlay::hooverView->setVisible(false);
			}
			else
			{	
				Point point = (Point){.x = child->owner->state.x, .y = child->owner->state.y};		
				
				// add to selection if SHIFT button (right or left) is pressed
				if (QApplication::keyboardModifiers() == Qt::ShiftModifier)
				{		
					if (stackOpen.count(point))
					{
						child->owner->selected = true;
						child->owner->setImage();
					}
					else
						selectStack(child->owner);
				}
				else
				// CTRL button toggle open stack
				if (QApplication::keyboardModifiers() == Qt::ControlModifier)
				{
					if (CentralFrame::openStackoffset)
						toggleOpenStack(child->owner, 1);
					else
						Overlay::openView->open(child->owner, event->position().toPoint());
										
				}
				else
				{			
					Counter *dummy;
					
					if (stackOpen.count(point))
					{
						unselect();
						child->owner->selected = true;
						child->owner->setImage();
					}
					else
						if (!anySelected(child->owner, dummy))
						{
							unselect();
							selectStack(child->owner);						
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
					
			// only scroll map if dragged for a minimum distance
			
			QByteArray itemData = event->mimeData()->data("application/x-alben-counter");
			QDataStream dataStream(&itemData, QIODevice::ReadOnly);
			
			QPoint counterOffset, ghostOrigo;
			
			dataStream >> counterOffset; 
			dataStream >> ghostOrigo;
			
			int x = event->position().toPoint().x() - counterOffset.x();
			int y = event->position().toPoint().y() - counterOffset.y();
		
			QPoint source(ghostOrigo);
			QPoint target(x, y);
			
			int minimumMovement = 8;
			
			
			if (abs(target.x() - source.x()) < minimumMovement &&
				abs(target.y() - source.y()) < minimumMovement)
			{
				event->acceptProposedAction();
				return;
			}
			else   	// scroll map
			{	
				
				int magicNumber = 64;		// distance to edge to begin scrolling
				int increment = 8;			// the scroll amount
				
				QPoint relativeToWindow = 
					((QWidget *)this->parent())->mapToParent(event->position().toPoint());
				
				int width = ((QMainWindow *)this->parent()->parent())->width();
				int height = ((QMainWindow *)this->parent()->parent())->height();
				
												
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
			
			
		}
		
		else
		
		if (event->mimeData()->hasFormat("application/x-alben-drag"))
		{
			
			QByteArray itemData = event->mimeData()->data("application/x-alben-drag");
			QDataStream dataStream(&itemData, QIODevice::ReadOnly);

			QPoint lastPoint;
			
			dataStream >> lastPoint;
			
			int dx = horizontalPos + (lastPoint.x() - event->position().toPoint().x());
			int dy = verticalPos + (lastPoint.y() - event->position().toPoint().y()); 

			scrollArea->horizontalScrollBar()->setValue(dx);
			scrollArea->verticalScrollBar()->setValue(dy);
				
					
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
		
		int id;		// id of dragged counter
		
		
		dataStream >> counterOffset; 
		dataStream >> ghostOrigo;
		dataStream >> id;
		
	
		
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
			
			int dropX = 0;
			int dropY = 0; 
			int dropZorder = 0;
			Stack dropTo;
			
		
			// find if dropped on a counter
												
			Counter::QtCounter *drop = 
				dynamic_cast<Counter::QtCounter*>(childAt(event->position().toPoint()));
				
				
			if (drop)
			{	
				
				Stack dropFrom;
				
				dropX = drop->owner->state.x;
				dropY = drop->owner->state.y;				
				dropZorder = drop->owner->state.zorder;
				
				// find the stack dropped from
				
				for (auto const& [point, stack] : stacks)						
					if (Counter::counters[id]->state.x == stack.begin()->second->state.x &&
						Counter::counters[id]->state.y == stack.begin()->second->state.y)
					{
						dropFrom = stack;
						break;
					}					
			
				
				// find the stack dropped to
				
				QPoint point = QPoint(dropX, dropY);
					   				   			
				for ( auto obj = Counter::counters.begin(); obj != Counter::counters.end(); ++obj  )	
				{
					QPoint p = QPoint(obj->second->state.x, obj->second->state.y);
						
					if (p == point)
						dropTo[obj->second->state.zorder] = obj->second;
				}
				
				
				// if dropTo and dropFrom is the same stack delete all dropFrom in dropTo
				
				for ( auto obj = dropFrom.begin(); obj != dropFrom.end(); ++obj  )
					dropTo.erase(obj->second->state.zorder);
				
				
			}
			
			
	
			
							
					
			// move all counters
			
			QPoint delta = target - source;
			
			QSize map = scaled->getScaledSize(CentralFrame::backgroundID);
		
			
			for (auto const& [point, stack] : stacks)	
			{
			
				bool draggedStack = Counter::counters[id]->state.x == stack.begin()->second->state.x &&
									Counter::counters[id]->state.y == stack.begin()->second->state.y;
				
				
				for ( auto obj = stack.begin(); obj != stack.end(); ++obj )		
				{															
					
					int x, y;	

					
					if (drop && draggedStack)
					{
						x = dropX;
						y = dropY;
					}
					else
					{	
											
						x = std::round((float)delta.x() / Scale::scaleFraction); 
						y = std::round((float)delta.y() / Scale::scaleFraction);
					
						if (Scale::rotation != 0)
							scaled->turn(-Scale::rotation, x, y);
						
						x += obj->second->state.x;
						y += obj->second->state.y;	
						
					}
				
					
					if (!Luau::afterDrag(obj->second->name.c_str(), x, y))
						continue;
					
					
					
					// cant drop outside map
					// check rotated/scaled coordinates
					
					int rx = x;
					int ry = y;
					
					rx = std::round((float)rx * Scale::scaleFraction);
					ry = std::round((float)ry * Scale::scaleFraction);
			
		
					if (Scale::rotation != 0)
						scaled->rotate(obj->second, Scale::rotation, rx, ry);		
							 
					if (rx < -obj->second->scaledWidth || ry < -obj->second->scaledHeight || 
						rx > map.width() || ry > map.height())
						continue;	
					
							
					
					
							
					if (!(obj->second->state.x == x && obj->second->state.y == y))
					{						
						//stackOpen.erase((Point){.x = obj->second->state.x, .y = obj->second->state.y});
						Luau::doEvent("movetrigger", obj->second->name.c_str(), "", "", 0);
					}
					
					
				
						
					obj->second->state.x = x;
					obj->second->state.y = y;					
					
					
					Luau::updatePos(obj->second->name.c_str(), obj->second->state.x, obj->second->state.y);			
					
					Luau::doEvent("move", obj->second->name.c_str(), "Counter", "x", obj->second->state.x);
					Luau::doEvent("move", obj->second->name.c_str(), "Counter", "y", obj->second->state.y);
		
					
					
				}
				
			}
			
			
			
			
			// assign new zorder to all dropped counters
			
			for (auto const& [point, stack] : stacks)	
				for (auto obj = stack.begin(); obj != stack.end(); ++obj)	
				{
					obj->second->state.zorder = Counter::topZorder();						
					Luau::doEvent("move", obj->second->name.c_str(), "Counter", "zorder", obj->second->state.zorder);
					obj->second->counter->raise();
				}
				
				
			// update drop stack zorders for counters higher than dropped
					
			for (auto obj = dropTo.begin(); obj != dropTo.end(); ++obj)	
				if (obj->second->state.zorder > dropZorder)
				{
					obj->second->state.zorder = Counter::topZorder();
					Luau::doEvent("move", obj->second->name.c_str(), "Counter", "zorder", obj->second->state.zorder);						
					obj->second->counter->raise();
				}
			


			
			Luau::doEvent("end", "", "", "", 0);
			
			
		}
		else
		{
			
			// holds per definition only one stack with one counter 
							
			
			Counter *counter = (stacks.begin()->second).begin()->second;			
			
			if (Scale::rotation != 0)
				scaled->unrotate(counter, -Scale::rotation, droppedX, droppedY);
				
												
			if (counter->state.degrees != 0)
			{
				// sets the counter coordinate for rotated counter to the standard non-rotated
				// upper left corner coordinate of the counter
				
				droppedX += std::round(((counter->scaledBuffer.width() - counter->scaledWidth) / 2) - counter->scaledMargin);
				droppedY += std::round(((counter->scaledBuffer.height() - counter->scaledHeight) / 2) - counter->scaledMargin);
						
				
			}			
			
					
			
					
			if (!Luau::afterDrag(counter->name.c_str(), droppedX, droppedY))
			{
				event->acceptProposedAction();
				return;
			}
			
			droppedX = std::round((float)droppedX / Scale::scaleFraction);
			droppedY = std::round((float)droppedY / Scale::scaleFraction);
			
		
			
			
			// find if dropped on a counter
												
			Counter::QtCounter *drop = 
				dynamic_cast<Counter::QtCounter*>(childAt(event->position().toPoint()));
				
			if (drop)
			{
				droppedX = drop->owner->state.x;
				droppedY = drop->owner->state.y;
			}	
				
			
			
			const char *fromId = counter->name.c_str();
			const char *toId = std::to_string(Counter::nextId()).c_str();			
			int zorder = Counter::topZorder();
				
			// create Luau representation and C++ representation of counter
			Luau::copyCounter(fromId, toId, zorder, droppedX, droppedY);
			
			// add counter to undo stack
			Luau::doCreate(toId, "Create");
			Luau::doEvent("end", "", "", "", 0);
			
			
			
			this->activateWindow();
			
			
		}
		
		
		event->acceptProposedAction();

		
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
		
		
		
		
	repaint();
	// force redraw of mask layer
	Overlay::overlay->clearMask();
	
	
				
}



void CentralFrame::selectStack(Counter *counter)
{
	
	Point point = {counter->state.x, counter->state.y};
	
	
	for (auto obj = Counter::counters.begin(); obj != Counter::counters.end(); ++obj)
	{
		Point p = {obj->second->state.x, obj->second->state.y};			
			
		if (p == point)
		{	
			obj->second->selected = true;
			obj->second->setImage();
		}	
	}	
	
}


void CentralFrame::selectCounter(Counter *counter)
{
	
	Point point = {counter->state.x, counter->state.y};
	
	if (!stackOpen.count(point))
	{
		for (auto obj = Counter::counters.begin(); obj != Counter::counters.end(); ++obj)
		{
			Point p = {obj->second->state.x, obj->second->state.y};			
				
			if (p == point)
			{	
				obj->second->selected = false;
				obj->second->setImage();
			}	
		}

		counter->selected = true;
		counter->setImage();
	}
		
}


bool CentralFrame::anySelected(Counter *counter, Counter *&selected)
{
	
	// decide if any selected in this stack, returns first selected
	
	
	QPoint point = QPoint(counter->state.x, counter->state.y);
	
	Stack stack;
	
	
	// find stack of this counter
	
	for (auto obj = Counter::counters.begin(); obj != Counter::counters.end(); ++obj)
	{
		
		QPoint p = QPoint(obj->second->state.x, obj->second->state.y);
			
		if (p == point)	
			stack[obj->second->state.zorder] = obj->second;		
			
	}
	
	
	for (auto obj = stack.begin(); obj != stack.end(); ++obj)
	{		
		if (obj->second->selected == true)
		{
			selected = obj->second;
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



std::map<const char*,CentralFrame::Button*> buttons;


void CentralFrame::createButton(const char *id, const char *text, const char *handler, int x, int y, int w, int h)
{
	CentralFrame::Button *btn;
	
	btn = new CentralFrame::Button(text, buttonParent);
	
	QObject::connect(btn, &QPushButton::clicked, [=]() { Luau::handlers(id, handler); });
	
	
	btn->setVisible(true);
	
	btn->move(x, y);
	btn->resize(w, h);
	
	buttons[id] = btn;
	
}	


void CentralFrame::deleteButton(const char *id)
{
	if (buttons.find(id) == buttons.end()) 
		return;
		
	delete buttons[id];
	buttons.erase(id);
	
}


void CentralFrame::toggleOpenStack(Counter *counter, int sign)
{
	Point point = (Point){.x = counter->state.x, .y = counter->state.y};
	
	if (stackOpen.count(point))		
		stackOpen.erase(point);
	else
	{
		this->selectCounter(counter);
		stackOpen[point] = sign;
	}
	
	repaint();
	
}


void CentralFrame::closeAllOpenStacks()
{
	stackOpen.clear();
	repaint();	
}





void CentralFrame::wheelEvent(QWheelEvent *event)
{
	if (ToolBar::sizeBox != nullptr)
	{
		int amount = event->angleDelta().y();
		
		if (amount > 0)		
			ToolBar::sizeBox->parent->wheelIn(event->position().toPoint());		
		else		
			ToolBar::sizeBox->parent->wheelOut(event->position().toPoint());
			
	}
	event->accept();
}



void CentralFrame::paintEvent(QPaintEvent *e)
{
	
	
	
	if (this->name == "Repository")
		return;		// layout does rendering
	
		
	QPainter painter(this);
		

				
	std::map<Point, Stack> stacks;

		
	QImage image = scaled->getScaledImage(backgroundID);
	painter.drawImage(0,0,image);
	
	int offsetAmount = Counter::stackOffset * Scale::scaleFraction;
	
	
	
	// generate stacks
	
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
	
	
	
	
	// draw below counters
	
	for (auto const& [point, stack] : stacks)		
		for ( auto obj = stack.begin(); obj != stack.end(); ++obj  )
		{
			
			if ((obj->second->table).find("AreaOfEffect") != (obj->second->table).end())
			{
				
				Counter::Table trait = std::get<Counter::Table>(obj->second->table["AreaOfEffect"]);
				
				if (std::get<bool>(trait["apply"]))	
				{
					
					int x = obj->second->state.x + obj->second->margin;
					int y = obj->second->state.y + obj->second->margin;
				
					float radius = (float)std::get<double>(trait["radius"]);
					
					const QPoint points[4] = {
					QPoint(x - (int)(radius*obj->second->width), y - (int)(radius*obj->second->height)),
					QPoint(x - (int)(radius*obj->second->width), y + (int)((1 + radius)*obj->second->height)),
					QPoint(x + (int)((1 + radius)*obj->second->width), y + (int)((1 + radius)*obj->second->height)),
					QPoint(x + (int)((1 + radius)*obj->second->width), y - (int)(radius*obj->second->height))
					};

					
					string str = std::get<std::string>(trait["color"]);
					QColor color(QString::fromStdString(str));	
					painter.setBrush(color);

					float opacity = (float)std::get<double>(trait["opacity"]);	
					painter.setOpacity(opacity);
					
					painter.drawConvexPolygon(points, 4);

					// reset
					painter.setOpacity(1.0);
					
					// draw only once per stack
					break;
				}
			}
		}
	
	
	
	
	
		
	
	// render all "stacks" with 1 or more counters
	
	
	for (auto const& [point, stack] : stacks)	
	{	

	
		int offset = 0;
		
		
		Counter *first = stack.begin()->second; 
		
		int fx = first->state.x;
		int fy = first->state.y;
		int openFactor = 1;		
		
		if (CentralFrame::openStackoffset && stackOpen.count(point))
			openFactor = 3 * stackOpen[point]; 
		
			
			
		for ( auto obj = stack.begin(); obj != stack.end(); ++obj  )
		{		
				
			int x = fx;
			int y = fy;
	
			
			x = std::round((float)x * Scale::scaleFraction);
			y = std::round((float)y * Scale::scaleFraction);
			
			

			
			QRect source(0, 
						 0, 
						 obj->second->scaledBuffer.width(), 
						 obj->second->scaledBuffer.height());
							
			QRect dest(x,
					   y,
					   obj->second->scaledWidth + 2*obj->second->scaledMargin,					   
					   obj->second->scaledHeight + 2*obj->second->scaledMargin);
					   					   
						
						  
			source.moveCenter( dest.center() );
			
	
			
			x = source.topLeft().x();
			y = source.topLeft().y();
				
				
			
		
			if (Scale::rotation != 0)
				scaled->rotate(obj->second, Scale::rotation, x, y);				
			
			
			
			
			// offset is 0, 1, 2 ... the postion in the stack with 0 bottom
			if (Counter::haveOffset)
			{
				x = x + (offsetAmount * offset * openFactor);
				y = y - (offsetAmount * offset * openFactor);
			}
	
			
			offset++;
			
			
					
			obj->second->setPos(x, y);
			
				
		}
			
	}

	
}
