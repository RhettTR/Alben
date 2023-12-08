#include "stackframe.h"




int StackFrame::border = 10;
int StackFrame::maxColumns = 3;




StackFrame::StackFrame(QWidget *parent) : QLabel(parent)
{
	this->setFrameStyle(QFrame::Panel | QFrame::Plain);
	this->setStyleSheet("border: 2px solid black; background: white;");
	this->setVisible(false);
	
	grid = new QLabel(this);
	grid->setStyleSheet("border: 0px; margin: 0px"); 
	
	rows = new QVBoxLayout();
	rows->setSpacing(0);
	rows->setContentsMargins(0, 0, 0, 0);
	grid->setLayout(rows);
	
	
		
}
 

void StackFrame::setPostion(QPoint point)
{	
	this->hotspot = point;
}


void StackFrame::moveThis(int w, int h)
{
	
	if (this->hotspot.y() - h > 0)
		this->hotspot += QPoint(0, -h);
	else
		this->hotspot += QPoint(0, -h + std::abs(this->hotspot.y() - h));
		
	if (this->hotspot.x() + w > this->window()->width())
		this->hotspot += QPoint(-w + std::abs(this->window()->width() - this->hotspot.x()), 0);
		
	 
	this->move(this->hotspot);
}


void StackFrame::setImage(QImage image)
{
	this->image = image;
}


void StackFrame::setImages(int box, CentralFrame::Stack stack)
{	
	
	// clear layout of previous content
	
	QLayoutItem *column, *item;
	
	while ((column = rows->takeAt(0)) != nullptr) 
	{
		while ((item = ((QVBoxLayout *)column)->takeAt(0)) != nullptr) 
		{	
			delete item->widget(); 
			delete item;   
		}
		
		delete column;
	}

	
	// loop stack and create row(s) of counter images
	
	int width, maxWidth = 0;
	
	for (auto obj = stack.begin(); obj != stack.end(); ++obj)
	{	
		
		QHBoxLayout *layout;
		
		
		if (rows->count() == 0 || 
		   ((layout = (QHBoxLayout *)rows->itemAt(rows->count() - 1)) && 
		    (layout->count() == StackFrame::maxColumns)))
		{
		
			layout = new QHBoxLayout();
			
			layout->setSpacing(0);
			layout->setContentsMargins(0, 0, 0, 0);
			layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			
			rows->addLayout(layout);
			
			width = 0;
	
		}
		
		
		
		QLabel *icon = new QLabel();		
		icon->setPixmap(QPixmap::fromImage(obj->second->baseBuffer));
		
		layout->addWidget(icon);
		
		width += obj->second->baseBuffer.width();
		
		if (width > maxWidth)
			maxWidth = width;
		
	}
	
	
	moveThis(box + StackFrame::border + maxWidth, box*rows->count());
	this->resize(box + StackFrame::border + maxWidth, box*rows->count());	
	this->grid->resize(maxWidth, box*rows->count() - 4);	// -4 for both borders
	
	grid->setVisible(true);
	
}


void StackFrame::paintEvent(QPaintEvent *e)
{
	
	QPainter painter(this);
	
	
	painter.drawRect(border-1, border-1, this->image.width() + 1, this->image.height() + 1);
		
	QPoint point = QPoint(border, border);	
	painter.drawImage(point, this->image);
	
}

