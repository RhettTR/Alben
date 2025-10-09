#include <string>


#include "frame.h"
#include "overlay.h"
#include "luau.h"
#include "io.h"
#include "scale.h"
#include "toolbar.h"
#include "window.h"




using namespace std;


extern IO *io;
extern Scale *scaled;

	

inline static CentralFrame::Stacks stacks;   // holds the dragged counter(s)

QScrollArea *CentralFrame::buttonParent;


		
bool CentralFrame::openStackoffset = true;	// true = open stack with offset method
bool CentralFrame::facingMatters = false;	// true = rotate counters when map rotates
CentralFrame::Area *CentralFrame::area = nullptr;
CentralFrame::GridCoordiantes CentralFrame::gridCoordinates;
bool CentralFrame::showGrid = false;
QColor CentralFrame::gridColor = QColor(255, 0, 0, 255);
int CentralFrame::gridRadius = 3;
CentralFrame::Moved CentralFrame::countersDeleted;
bool CentralFrame::allowPainting = true;	// to turn of painting while undoing

		





CentralFrame::Button::Button(const QString &text, QWidget *parent) : QPushButton(text, parent) {}
	
	
	
CentralFrame::Area::Area(CentralFrame *parent) : QLabel(parent)
{	
	this->parent = parent;		
	this->setContextMenuPolicy(Qt::CustomContextMenu);
}


CentralFrame::Area::~Area()
{
}


static Luau::PopupEntries popupentries;


void CentralFrame::Area::showRightClickMenu(Counter *counter)
{
	
	QMenu myMenu(this);
	
	
	while (this->actions().count() > 0)
		this->removeAction(this->actions().first());
	
	this->counter = counter;
	
	
	popupentries.clear();
		
	popupentries = Luau::getTraits("Global", "Area");
	

	for (auto e : popupentries)
	{
		
		// hook	
		if (!Luau::menu(this->counter->name.c_str(), e.entryname.c_str(), e.entryid.c_str()))
			continue;
		 
		const QString name = QString::fromStdString(e.entryname);
		
		QAction* action = new QAction(name, this);
				
		QVariant v;
		v.setValue(e);
		
		action->setData(v);
			
	   
		this->addAction(action);
		
		
		QObject::connect( action, &QAction::triggered, this, [=]()->void{ execute(action); } );
		
	}

	
	myMenu.addActions(this->actions());
	
	myMenu.exec(QCursor::pos());
	
}


void CentralFrame::Area::execute(QAction *action)
{
	QVariant v = action->data();
	Luau::Popupentry e = (Luau::Popupentry) v.value<Luau::Popupentry>();
	
	
	if (!e.entryid.empty())
	{
		const char *fromId = e.entryid.c_str();
		const char *toId = std::to_string(Counter::nextId()).c_str();		
		int zorder = Counter::bottomZorder();	
		
				
		Luau::copyCounter(fromId, toId, zorder, "", this->counter->state.x, this->counter->state.y);
		
		Luau::doCreate(toId, "Create");
		Luau::doEvent("end", "", "", "", 0);
		
		
		Counter::counters[atoi(toId)]->counter->lower();
		// force redraw of mask layer
		Overlay::overlay->clearMask(); 
	}
	
	if (!e.entryaction.empty())
	{
		const char *entryaction = e.entryaction.c_str();
		QByteArray bb = QString::fromStdString(this->counter->parentWindow->tag).toLocal8Bit();
		const char *window = bb.data();
		
		
		Luau::doAction(window, this->counter->name.c_str(), "Area", entryaction);
		
		Luau::doAction("", "", "", "actionEnd");
	}
	
}


bool CentralFrame::Area::findCounter(QPoint point, Counter *&foundCounter)
{
	
	float scaleFraction = Window::getInstance(this->counter->state.tag.c_str())->frame->scaleFraction;
	
	int extra = (int)(15 * scaleFraction);
	
	for ( auto obj = Counter::counters.begin(); obj != Counter::counters.end(); ++obj  )
		if (obj->second->state.tag == this->parent->window->tag)	
		{
			// hook - is this a counter with a menu
			if (!Luau::menucounter(obj->second->id))
				continue;
			
			int hx = obj->second->scaledMargin + std::round(obj->second->scaledWidth/2);
			int hy = obj->second->scaledMargin + std::round(obj->second->scaledHeight/2);
						
			
			int x = std::round(obj->second->state.x * scaleFraction) + hx;
			int y = std::round(obj->second->state.y * scaleFraction) + hy;

			
			if (abs(x - point.x()) < (hx + extra) && 
				abs(y - point.y()) < (hy + extra))	
			{		
				foundCounter = obj->second;
				return true;	
			}
		}
	
	
	foundCounter = nullptr;
	return false;
	
}
	
	

	
		
	
CentralFrame::CentralFrame(QWidget *parent, Window *window, string type, QScrollArea *scrollArea) : QFrame(parent)
{
	
	this->window = window;
	this->scaleFraction = 1.0;
	
	
	if (type == "Layout")
	{
		this->setLayout(new QVBoxLayout());
		this->layout()->setContentsMargins(0, 0, 0, 0);
		this->layout()->setSpacing(0);
		this->scrollArea = nullptr;
	}
	else
	{
		this->scrollArea = scrollArea;
		buttonParent = this->scrollArea;
		setAcceptDrops(true);
		setFrameStyle(QFrame::NoFrame);
	}
	
}




				
void CentralFrame::mousePressEvent(QMouseEvent *event)
{
	
		
	if (Overlay::hooverView->isVisible())
		return;
		
		
		
	if (event->button() == Qt::RightButton)	
	{
		if (CentralFrame::area != nullptr)
		{
			Counter *counter;
			
			if (CentralFrame::area->findCounter(event->position().toPoint(), counter))
				CentralFrame::area->showRightClickMenu(counter);
		}
		
	}
	
	
	
	if (event->button() == Qt::LeftButton)
	{

		QDrag *drag = new QDrag(this);
		drag->deleteLater();		
		
		
		QPoint frameCoordinates;
		//Moved countersMoved;
			
		if (this->window->tag == "Repository")
			frameCoordinates = mapTo((QWidget *)this, event->position().toPoint());
		else
			frameCoordinates = event->position().toPoint();
				
						
		
		Counter::QtCounter *child = 
			dynamic_cast<Counter::QtCounter*>(childAt(frameCoordinates));
		
	
		
		
		if (child == nullptr)
		{
			if (this->window->tag == "main")		 
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
			{
				unselect();
				return;
			}
			
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
			int position;
	
			
			
			//update image so that ghost is scaled
			child->owner->setImage();	
				
				
			
			
			if (anySelected(child->owner, dummy , position))			
				img = selectedGhostImage(child->owner, ghostRect);
			else
			{
				child->owner->selected = true;
				(void)anySelected(child->owner, dummy , position);
				img = selectedGhostImage(child->owner, ghostRect);
				child->owner->selected = false;
			}
		
			QPixmap pix = QPixmap::fromImage(img);
		
			
		
			QByteArray itemData;
			QDataStream dataStream(&itemData, QIODevice::WriteOnly);
				
		
			QPoint o1 = mapToGlobal(QPoint(0,0));
			QPoint o2 = ((QWidget *)child->parent())->mapToGlobal(QPoint(0,0));
			
			QPoint origo = o2 - o1;
		
		
			
			
			
			QPoint counterOffset = frameCoordinates - origo - ghostRect.topLeft();		
		
		
			QPoint ghostOrigo = ghostRect.topLeft();
			
			      
																			
			dataStream << counterOffset;
			dataStream << ghostOrigo;
			
			
			
			
			int n = 0;
			for (auto const& [point, stack] : stacks)
				n = n + stack.size();

			
			dataStream << n;
			
			
			
			QPoint compensation;
			
			// get stack compensation
			// this is the position of the lowest selected counter in the stack
			// if no selection or whole stack selected then position is zero
			
			// default values
			float xoffset = 5;
			float yoffset = 5;			
			int open = 3;		// factor increase in opened stack
			int shown = 0;		// zero means unlimited shown counters in stack
			
			
			// hook
			Luau::pointoffset(child->owner->state.tag.c_str(), 
							  child->owner->state.x,
							  child->owner->state.y,
							  std::round(child->owner->width/2) + child->owner->margin, 
							  std::round(child->owner->height/2) + child->owner->margin, 
							  xoffset, 
							  yoffset,   
							  open, 
							  shown);
							  
			int openFactor = 1;
			
			Point point = Point{.x = child->owner->state.x, .y = child->owner->state.y};
			
			if (CentralFrame::openStackoffset && stackOpen.count(point))
				openFactor = open * stackOpen[point];	
			
			
			compensation.setX(-position * xoffset * openFactor);
			compensation.setY(position * yoffset * openFactor);
			
			// all values from here are supposed to be scaled
			compensation *= this->scaleFraction;
			
			dataStream << compensation;
			
			
			
			
			// this presumes that only one counter is moved from the repository at a time
			QString idStr = "";
			
			if (this->window->tag == "Repository")
				idStr = QString::fromStdString(child->owner->name);
			
			
			for (auto const& [point, stack] : stacks)
				for (auto const& [i, counter] : stack)
				{	
					int id = counter->id;
					int dx = counter->state.x - child->owner->state.x; 	
					int dy = counter->state.y - child->owner->state.y;
					dataStream << id;
					dataStream << idStr;
					dataStream << dx;
					dataStream << dy;
				}



			QMimeData *mimeData = new QMimeData;
			mimeData->setData("application/x-alben-counter", itemData);
				
				
					
			drag->setMimeData(mimeData);
			drag->setPixmap(pix);
			drag->setHotSpot(counterOffset);
			
			
		}
		
		
		auto result = drag->exec(Qt::CopyAction | Qt::MoveAction);
		
		
		if (result == Qt::CopyAction)
		{
			if (this->window->tag != "Repository")	
				for (auto d: countersDeleted)
				{	
					Luau::doDelete(std::to_string(d.id).c_str());	
					delete Counter::counters[d.id];
				}
					
			if (countersDeleted.size() > 0)
				Luau::doEvent("end", "", "", "", 0);		
		}
	
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
						child->owner->selected = Luau::selectable(child->owner->name.c_str());
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
					Counter *dummy1;
					int dummy2;
					
					if (stackOpen.count(point))
					{
						unselect();
						child->owner->selected = Luau::selectable(child->owner->name.c_str());
						child->owner->setImage();
					}
					else
						if (!anySelected(child->owner, dummy1, dummy2))
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
	
	if (event->source() == this && scrollArea != nullptr)
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

		QPoint counterOffset, ghostOrigo, compensation;
		
		int n, id, dx, dy;
		QString idStr;		
		Moved countersMoved;
		
		countersDeleted.clear();
		
		
		
		dataStream >> counterOffset; 
		dataStream >> ghostOrigo;
		dataStream >> n;
		dataStream >> compensation;
		
		
		
		for (int i = 0; i < n; i++)
		{	
			dataStream >> id;
			dataStream >> idStr;
			dataStream >> dx;
			dataStream >> dy;
			countersMoved.push_back({id, idStr, dx, dy});
		}
		
		
		
		
		int droppedX = event->position().toPoint().x() - counterOffset.x();
		int droppedY = event->position().toPoint().y() - counterOffset.y();
		
		

		
		
		// check if actual movement has taken place
		 
		int minimumMovement = (int)std::round(8 * scaleFraction);
		
		
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
			event->ignore();
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
				
					
			if (drop != nullptr)
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
			else
			{
				// apply stack position compensation
				source.setX(source.x() + compensation.x());
				source.setY(source.y() + compensation.y());
				
			}
			
			
	
				
							
					
			// move all counters
			
			QPoint delta = target - source;
			
			QSize map = scaled->getScaledSize(CentralFrame::backgroundID);
			
			float scaleFraction = this->window->frame->scaleFraction;
			
			int n = 0;
		
			
			for (auto const& [point, stack] : stacks)	
			{
			
				bool draggedStack = Counter::counters[id]->state.x == stack.begin()->second->state.x &&
									Counter::counters[id]->state.y == stack.begin()->second->state.y;
			
				
				for ( auto obj = stack.begin(); obj != stack.end(); ++obj )		
				{															
					
					int x, y;	

					
					if (drop != nullptr && draggedStack)
					{
						x = dropX;
						y = dropY;
					}
					else
					{	
											
						x = std::round((float)delta.x() / scaleFraction); 
						y = std::round((float)delta.y() / scaleFraction);
					
						if (Scale::rotation != 0)
							scaled->turn(-Scale::rotation, x, y);
						
						x += obj->second->state.x;
						y += obj->second->state.y;	
						
					}
			

									     
					if (!Luau::afterDrag(this->window->tag.c_str(),
									     obj->second->name.c_str(), 
										 (int)(std::round(obj->second->width/2) + obj->second->margin), 
									     (int)(std::round(obj->second->height/2) + obj->second->margin), 
										 x, 
										 y))
						continue;
					 
				
					
					// cant drop outside map
					// check rotated/scaled coordinates
					
					int rx = x;
					int ry = y;
					
					rx = std::round((float)rx * scaleFraction);
					ry = std::round((float)ry * scaleFraction);
			
		
					if (Scale::rotation != 0)
						scaled->rotate(obj->second, Scale::rotation, rx, ry);		
							 
					if (rx < -obj->second->scaledWidth || ry < -obj->second->scaledHeight || 
						rx > map.width() || ry > map.height())
						continue;	
					
							
					
							
					if (!(obj->second->state.x == x && obj->second->state.y == y))
					{
						Luau::updatePos(this->window->tag.c_str(), obj->second->name.c_str(), x, y);
						Luau::doEvent("movetrigger", obj->second->name.c_str(), "", "", 0);
					}					
					
					
					obj->second->state.x = x;
					obj->second->state.y = y;
					obj->second->state.tag = this->window->tag;					
					
					
					Luau::updatePos(this->window->tag.c_str(), obj->second->name.c_str(), obj->second->state.x, obj->second->state.y);
								
					
					
					Luau::doEvent("move", obj->second->name.c_str(), "Counter", "x", obj->second->state.x);
					Luau::doEvent("move", obj->second->name.c_str(), "Counter", "y", obj->second->state.y);
					
					// increment number of moved
					n++;
		
					// hook
					Luau::moved(this->window->tag.c_str(), obj->second->name.c_str(), obj->second->state.x, obj->second->state.y);
						
					
					
				}	// for
				
			}	// for
			
			
			
			
			// never add 'end' or new zorder unless at least one moved
			if (n > 0)
			{
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
			
			
			event->setDropAction(Qt::MoveAction);
			event->accept();
			
			
		}
		else
		{
			
			// drag from another window
			
			
			
			for (auto m : countersMoved)
				// only drag a single stack (for the time being)
				if (m.dx == 0 && m.dy == 0)
				{
					
					int dropX = droppedX;
					int dropY = droppedY;			
			
	
					Counter *counter;
					
					if (m.idStr.isEmpty())	
						counter = Counter::counters[m.id];	
					else
						counter = Counter::repository[m.idStr.toStdString()];
						
						
						
					
					
					float scaleFraction = this->window->frame->scaleFraction;
										
					
					if (Scale::rotation != 0)
						scaled->unrotate(counter, -Scale::rotation, dropX, dropY);
						
														
					if (counter->state.degrees != 0)
					{
						// sets the counter coordinate for rotated counter to the standard non-rotated
						// upper left corner coordinate of the counter
						
						dropX += std::round(((counter->scaledBuffer.width() - counter->scaledWidth) / 2) - counter->scaledMargin);
						dropY += std::round(((counter->scaledBuffer.height() - counter->scaledHeight) / 2) - counter->scaledMargin);
								
						
					}			
					
					
					//dropX += m.dx;
					//dropY += m.dy;
					
					
					dropX = std::round((float)dropX / scaleFraction);
					dropY = std::round((float)dropY / scaleFraction);
					
					
					
				
				
					// hook
					if (!Luau::afterDrag(this->window->tag.c_str(),
										 counter->name.c_str(), 
										 std::round(counter->width/2) + counter->margin, 
										 std::round(counter->height/2) + counter->margin, 
										 dropX, 
										 dropY))
					{
						event->acceptProposedAction();
						return;
					}
				
					
					// find if dropped on a counter
														
					Counter::QtCounter *drop = 
						dynamic_cast<Counter::QtCounter*>(childAt(event->position().toPoint()));
						
					if (drop)
					{
						dropX = drop->owner->state.x;
						dropY = drop->owner->state.y;
					}	
						
					
			
					
					const char *fromId = counter->image.c_str();
					const char *toId = std::to_string(Counter::nextId()).c_str();			
					int zorder = Counter::topZorder();
					const char *tag = this->window->tag.c_str();
					
					
					// create Luau representation and C++ representation of counter
					Luau::copyCounter(fromId, toId, zorder, tag, dropX, dropY);
					
					// add counter to undo stack
					Luau::doCreate(toId, "Create");
					
					
					// save what counter to delete in window moved from
					countersDeleted.push_back({m.id, m.idStr, m.dx, m.dy});
					
					
					// hook
					Luau::dropped(this->window->tag.c_str(), fromId, toId, dropX, dropY);
					
				
				}
			
			
			
			event->setDropAction(Qt::CopyAction);
			event->accept();
			
			
			//this->activateWindow();
			
		}
		
		
		//event->acceptProposedAction();

		
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
			obj->second->selected = Luau::selectable(obj->second->name.c_str());
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

		counter->selected = Luau::selectable(counter->name.c_str());
		counter->setImage();
	}
		
}


void CentralFrame::setBackground(std::string background)
{
	
	if (background[0] == '#')
	{
		// background is a color
		//this->setStyleSheet("CentralFrame { background-color:" + QString::fromStdString(background) + "; }");
		this->backgroundColor = QColor(QString::fromStdString(background));
		this->backgroundID = "";
	}
	else
	{
		//background is an image
		if (io->isResource(background))
		{	
			this->backgroundID = background;
			
			scaled->resourceScaleRotate(this->window->tag, this->backgroundID);
			
			QSize size = scaled->getScaledSize(this->backgroundID);
			
			if (this->scrollArea != nullptr)
				this->scrollArea->resize(size); 
			else
			 { 
				this->window->resize(size);
				this->resize(size);
			}
			
			
		}
	}
	
	
}





bool CentralFrame::anySelected(Counter *counter, Counter *&selected, int &selectedPos)
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
	
	
	selectedPos = 0;
	
	for (auto obj = stack.begin(); obj != stack.end(); ++obj)
	{		
		if (obj->second->selected == true)
		{
			selected = obj->second;
			return true;
		}
	
		selectedPos++;
	
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


QImage CentralFrame::selectedGhostImage(Counter *counter, QRect &totalRect)
{
	
	
	if (counter->state.tag == "Repository")
	{
		Point point = {counter->state.x, counter->state.y};
		Stack stack = {{counter->state.zorder, counter}};
		stacks[point] = stack;
	}
	
	else
	
		// generate stacks of selected

		for (auto obj = Counter::counters.begin(); obj != Counter::counters.end(); ++obj)
			if (obj->second->state.tag == this->window->tag)
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



void CentralFrame::setGrid(Counter::Table *grid)
{
	
	gridCoordinates.clear();
	
	
	for (auto obj = grid->begin(); obj != grid->end(); ++obj)
	{
				
		Counter::Table table = std::get<Counter::Table>(obj->second);
		
		int x = (int)std::get<double>((table)["x"]);
		int y = (int)std::get<double>((table)["y"]);
		
		
		int hx = 0, hy = 0;
		QString text("");
		
		if ((table).find("p") != (table).end())
			text = QString::fromStdString(std::get<std::string>((table)["p"]));
			
		if ((table).find("hx") != (table).end())
			hx = (int)std::get<double>((table)["hx"]);
			
		if ((table).find("hy") != (table).end())
			hy = (int)std::get<double>((table)["hy"]);			
		 
		
		gridCoordinates.push_back({x, y, text, hx, hy});
		
	}
	
	
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



void CentralFrame::resizeEvent(QResizeEvent* event)
{
	
	QFrame::resizeEvent(event);
	
	window->setWidgets();

}



void CentralFrame::wheelEvent(QWheelEvent *event)
{
	if (this->window->tag == "main")	
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



void CentralFrame::paintEvent(QPaintEvent *event)
{
	
	
	if (this->window->tag == "Repository")
		return;		// layout does rendering
		
	if (!CentralFrame::allowPainting)
	{
		event->accept();
		return;		// no painting while undoing
	}
		
		
	QPainter painter(this);
		

				
	std::map<Point, Stack> stacks;
	
	

	if (!backgroundID.empty())
	{	
		QImage image = scaled->getScaledImage(backgroundID);
		painter.drawImage(0,0,image);
	}
	else		
		painter.fillRect(QRect(0,0,this->size().width(), this->size().height()), QBrush(backgroundColor));
		
	
	
	
	
	// generate stacks
	
	for (auto obj = Counter::counters.begin(); obj != Counter::counters.end(); ++obj)
		if (obj->second->state.tag == this->window->tag)
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
	
	
	// draw any coordinates
	if (this->window->tag == "main")
		if (!CentralFrame::gridCoordinates.empty())
		{
			for (auto c : CentralFrame::gridCoordinates)
				if (!c.text.isEmpty())
				{		
					int fontSize = 9 * scaleFraction < 1.0 ? 1 : (int)(9 * scaleFraction);
					QFont font("Arial", fontSize, QFont::Normal);
					painter.setFont(font);
					
					QFontMetrics fm(font);
					QRect b(fm.boundingRect(c.text));
					
					
					QPen pen(QColor(131, 52, 90));
					painter.setPen(pen);
					
					painter.setRenderHint(QPainter::TextAntialiasing);
					painter.setRenderHint(QPainter::SmoothPixmapTransform);
					
					
					QRect rect((int)(c.hx * scaleFraction) - (int)(b.width() / 2), 
							   (int)(c.hy * scaleFraction), 
							   b.width(), 
							   b.height());
					
							   
					painter.drawText(rect, Qt::AlignHCenter, c.text);      			     
				}
		}
	
	// draw any grid
	if (CentralFrame::showGrid)
	
		// don't show dot for small scales
		if (scaleFraction > Scale::minScaleGrid)
			if (!CentralFrame::gridCoordinates.empty())
			{
				QPen pen;
				
				pen.setWidth(1);
				pen.setColor(CentralFrame::gridColor);
				painter.setPen(pen);
				painter.setBrush(CentralFrame::gridColor);
				painter.setRenderHint(QPainter::Antialiasing);
				painter.setRenderHint(QPainter::SmoothPixmapTransform);
			
				int size = CentralFrame::gridRadius  * scaleFraction;						
						
				for (auto c : CentralFrame::gridCoordinates)
					painter.drawEllipse(c.x * scaleFraction, c.y * scaleFraction, size, size);
			}	
		
	// draw any Area of Effect	
	for (auto const& [point, stack] : stacks)
	{	
		
		for ( auto obj = stack.begin(); obj != stack.end(); ++obj  )
		{
			
			if ((obj->second->table).find("AreaOfEffect") != (obj->second->table).end())
			{
				
				Counter::Table trait = std::get<Counter::Table>(obj->second->table["AreaOfEffect"]);
				
				if (std::get<bool>(trait["apply"]))	
				{
				
					string str = std::get<std::string>(trait["color"]);
					QColor color(QString::fromStdString(str));	
					painter.setBrush(color);

					float opacity = (float)std::get<double>(trait["opacity"]);	
					painter.setOpacity(opacity);
					
					//QPen pen;
					//pen.setWidth(3);
					//painter.setPen(pen);
					
					
					Counter::Table traitPoints = std::get<Counter::Table>(trait["area"]);
					int areaOfEffectNumber = (int)traitPoints.size();
					
					QPoint points[areaOfEffectNumber];
					
					
					int n = 0;
					
					for (auto obj = traitPoints.begin(); obj != traitPoints.end(); ++obj)
					{	
						Counter::Table table = std::get<Counter::Table>(obj->second);
					
						int x = (int)std::get<double>((table)["x"]);
						int y = (int)std::get<double>((table)["y"]);
						
						points[n].setX(x);
						points[n].setY(y);
						
						n++;
					}
			
					
					if (std::get<std::string>(trait["type"]) == "hex")
						if (n > 0)
						{
					
							// set area of effect points to right position and scale
							
							int x = obj->second->state.x + obj->second->margin + std::round(obj->second->width / 2); 
							int y = obj->second->state.y + obj->second->margin + std::round(obj->second->height / 2);
							
							for (int i = 0; i < areaOfEffectNumber; i++)
							{
								int px = std::round((x + points[i].x()) * scaleFraction);
								int py = std::round((y + points[i].y()) * scaleFraction);
								 
								points[i].setX(px);
								points[i].setY(py);
							}
					
							painter.drawConvexPolygon(points, areaOfEffectNumber);
						}
							

					// reset
					painter.setOpacity(1.0);
					
					// draw only once per stack
					break;
				}
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
	

		
			
		int top = stack.size();
		int n = 1;
		
		float xoffset = 5;
		float yoffset = 5;
		int openFactor = 1;
		int open = 3;
		int shown = 0;		//  zero means unlimited shown counters in stack
		
		
		// hook
		Luau::pointoffset(first->state.tag.c_str(), 
						  fx, 
						  fy,
						  std::round(first->width/2) + first->margin, 
						  std::round(first->height/2) + first->margin, 
						  xoffset, 
						  yoffset,   
						  open, 
						  shown);
		
		
		
		
		xoffset = xoffset * scaleFraction;
		yoffset = yoffset * scaleFraction;
		
		
		if (CentralFrame::openStackoffset && stackOpen.count(point))
			openFactor = open * stackOpen[point];
			
			
			
			
		for ( auto obj = stack.begin(); obj != stack.end(); ++obj  )
		{		
				
			int x = fx;
			int y = fy;
	
			
			x = std::round((float)x * scaleFraction);
			y = std::round((float)y * scaleFraction);
			
			

			if (obj->second->state.degrees != 0)
			{
				
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
				
			}	
				
			
		
			if (Scale::rotation != 0)
				scaled->rotate(obj->second, Scale::rotation, x, y);				
			
			
			
			
			// offset is 0, 1, 2 ... the postion in the stack with 0 bottom
			if (Counter::haveOffset)
			{
				if (obj->second->table.find("Marker") != obj->second->table.end())
				{
					Counter::Table table = 
						std::get<Counter::Table>(std::get<Counter::Table>((obj->second->table)["Marker"])["renderOffset"]);		
					x += (int)(std::get<double>((table)["dx"]) * scaleFraction);
					y += (int)(std::get<double>((table)["dy"]) * scaleFraction);
				}	
				else				
				{
					
					x = x + (int)(xoffset * offset * openFactor);
					y = y - (int)(yoffset * offset * openFactor);
					
					// 'shown' is the number of shown counters in a stack
					if (shown == 0 || (n > top - shown))
						offset++;
						
					n++;
				}
			}
	
			
			
					
			obj->second->setPos(x, y);
			
				
		}
			
	}
	
	
}
