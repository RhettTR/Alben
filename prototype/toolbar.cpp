#include <QtWidgets>

#include "frame.h"
#include "luau.h"
#include "io.h"
#include "scale.h"
#include "overlay.h"
#include "toolbar.h"


extern Luau l;
extern QWidget *container;
extern CentralFrame *mapFrame;
extern IO *io;
extern Scale *scaled;




ToolBar::MapSizeComboBox *ToolBar::sizeBox = nullptr;





ToolBar::ButtonAction::ButtonAction(std::string resourceName, const QString toolTip, std::function<void(void)> f, std::string luaScript)
{
	QPixmap pixmap;
	(void)pixmap.convertFromImage(io->getImage(resourceName));
	const QIcon icon = QIcon(pixmap);
	
	this->setIcon(icon);
	this->setToolTip(toolTip);
	this->setEnabled(false);
	this->id = resourceName;

	
	if (luaScript.empty())		
		QObject::connect(this, &QAction::triggered, this, [=]()->void{ f(); });	
	else
		QObject::connect(this, &QAction::triggered, this, [=](){ Luau::callbackScript(luaScript); });
	
}


ToolBar::Label::Label(std::string resourceName, const QString toolTip)
{
	QPixmap pix(32, 24);
	(void)pix.convertFromImage(io->getImage(resourceName));
	this->setPixmap(pix);
	
	this->setToolTip(toolTip);
	this->setEnabled(false);
	this->id = resourceName;
	
}



ToolBar::MapSizeComboBox::MapSizeComboBox(ToolBar *parent) : QComboBox((QWidget *)parent)
{
	
	this->parent = parent;
	
	this->setStyleSheet("QListView::item {height:20px;}");
	
	this->setEditable(true);
	
	
	this->insertSeparator(this->count());
	this->lineEdit()->setToolTip("Cr add - Del delete"); 
	
	this->addItem("Width", "na");
	this->addItem("Height", "na");
	
	
	// allow custom insert
	this->setInsertPolicy(QComboBox::NoInsert);
	
		
	
		
	
	QObject::connect( this, &QComboBox::textActivated, [=]()->void{ textActivated(this->currentText()); });
	QObject::connect( this->lineEdit(), &QLineEdit::editingFinished, [=]()->void{ editingFinished(); });
	
	
	this->installEventFilter(this);
	
}


void ToolBar::MapSizeComboBox::focusInEvent(QFocusEvent *e)
{
	this->clearEditText();
}


void ToolBar::MapSizeComboBox::focusOutEvent(QFocusEvent *e)
{
	this->clearEditText();	
	this->setCurrentIndex(this->currentIndex());
}


bool ToolBar::MapSizeComboBox::eventFilter(QObject *obj, QEvent *e)
{
	if (e->type() == QEvent::KeyPress)
	{
		QKeyEvent *keyEvent = static_cast<QKeyEvent*>(e);
		
		if (keyEvent->key() == Qt::Key_Delete)
		{
			this->removeItem(this->currentIndex());
			return true;
		}
	}

	return QObject::eventFilter(obj, e);
}


int ToolBar::MapSizeComboBox::findPlace(int forNewIndex)
{                               
	for (int i = 0; i < this->count(); i++)
		if (forNewIndex > this->itemData(i).value<int>())
			return i;
			
	return count();		
}


void ToolBar::MapSizeComboBox::editingFinished()
{
	bool ok;
	auto string = this->currentText();
	int value = string.toInt(&ok); 
	
	if (!ok)
		return;
		
	
	this->insertItem(findPlace(value), string + "%", value);
	
	int index = this->findData(value);
	if (index != -1) 
		this->setCurrentIndex(index);	
}





void ToolBar::MapSizeComboBox::textActivated(QString text)
{
	
	int index = findText(text);
	
	if (index == -1)
		return;
	
	QVariant variant = ToolBar::sizeBox->itemData(index);
	
	
	if (variant.userType() == QMetaType::Int)
	{
		const int percent = variant.value<int>();	
		float fraction = (float)percent / 100.0;
		parent->zoomMiddle(fraction);
		return;		
	}
	
	
			
	if (variant.userType() == QMetaType::QString)
	{
		QSize total = io->getSize(CentralFrame::backgroundID);
		
		QWidget *map = (QWidget *)((QMainWindow *)parent->parent())->centralWidget();
		
		QSize size = map->size();		
		
		
		
		if (text == "Width")
		{	
	
			float fraction = (float)size.width() / (float)total.width();
			
			
			if (fraction * total.height() > size.height())
			{			    				
				size -= QSize(parent->scrollArea->verticalScrollBar()->width(), 0);			
				fraction = (float)size.width() / (float)total.width();
			}
			
			parent->zoom(fraction);	
			
		}
			
		if (text == "Height")
		{	
	
			float fraction = (float)size.height() / (float)total.height();
			
			
			if (fraction * total.height() > size.height())	
			{					
				size -= QSize(0, parent->scrollArea->horizontalScrollBar()->width());
				fraction = (float)size.height() / (float)total.height();
			}
			
			parent->zoom(fraction);	
			
		}
	}
	
}


void ToolBar::MapSizeComboBox::insert(int zoom)
{	
	ToolBar::sizeBox->insertItem(ToolBar::sizeBox->findPlace(zoom), QString::number(zoom) + "%", zoom);
}


void ToolBar::MapSizeComboBox::setDefault(int zoom)
{
	int index = ToolBar::sizeBox->findData(zoom);	
	if (index != -1)
		ToolBar::sizeBox->setCurrentIndex(index);
}






ToolBar::MapSizeAction::MapSizeAction(QObject *parent) : QWidgetAction(parent) {}



ToolBar::ToolBar(QWidget *parent, QScrollArea *scrollArea) : QToolBar(parent)
{
	this->scrollArea = scrollArea;
}


ToolBar::~ToolBar()
{
}




void ToolBar::addImageButton(std::string resourceName, const QString toolTip, std::function<void(void)> func, std::string luaScript)
{
	ButtonAction *action = new ButtonAction(resourceName, toolTip, func, luaScript);
	this->addAction(action);
}


void ToolBar::addLabel(std::string resourceName, const QString toolTip)
{
	Label *label = new Label(resourceName, toolTip);
	label->setParent(this);	
	this->addWidget(label);
}


void ToolBar::addSizeComboBox()
{
		
	ToolBar::sizeBox = new MapSizeComboBox(this);
	
	MapSizeAction *toolButtonAction = new MapSizeAction(this);
	toolButtonAction->setDefaultWidget(ToolBar::sizeBox);

	this->addAction(toolButtonAction);
}


void ToolBar::enabled(std::string id, bool value)
{
	
	foreach (QAction *action, this->actions())
		if (typeid(*action) == typeid(ButtonAction))
			if (((ButtonAction *)action)->id == id)
			{
				action->setEnabled(value);
				return;
			}
		
	
	QObjectList children = this->children();
	
    for (int i = 0; i < children.length(); i++)
    {	
		Label *label = dynamic_cast<Label *>(children[i]);
		if (label != nullptr)
			if (label->id == id)
				label->setEnabled(value);
	}
	
}



void ToolBar::zoom(float fraction)
{
	
	Scale::scaleFraction = fraction;
	
	scaled->resourceScaleRotate(CentralFrame::backgroundID);
	
	
	Counter::setGUI();
	
	
	mapFrame->repaint();
	// force redraw of mask layer
	Overlay::overlay->clearMask();
	
}



void ToolBar::zoomCoordinates(QPoint point, float newFraction)
{
	
	
		
	float oldFraction = Scale::scaleFraction;
		
	Scale::scaleFraction = newFraction;
	
	
	
	float relative = Scale::scaleFraction / oldFraction;
		
	
	
	int x = scrollArea->horizontalScrollBar()->value();
	int y = scrollArea->verticalScrollBar()->value();
	
	int dx = point.x() - x;
	int dy = point.y() - y;
	
	x += dx;
	y += dy;
	
	
	scaled->resourceScaleRotate(CentralFrame::backgroundID);
	
	
	
	// scale and set

	x = std::round((float)x * relative);
	y = std::round((float)y * relative);
	

	x -= dx;
	y -= dy;
	
	
	this->scrollArea->horizontalScrollBar()->setValue(x);		
	this->scrollArea->verticalScrollBar()->setValue(y);
	
	
		
	Counter::setGUI();
	
	
	mapFrame->repaint();
	// force redraw of mask layer
	Overlay::overlay->clearMask();
	
}



void ToolBar::zoomMiddle(float newFraction)
{

	float oldFraction = Scale::scaleFraction;
		
	Scale::scaleFraction = newFraction;
	
	
	
	float relative = Scale::scaleFraction / oldFraction;
	
	
	
	// find middle of window
	
	QWidget *map = (QWidget *)((QMainWindow *)parent())->centralWidget();
		
	QSize size = map->size();		       
	
							
	size /= 2;
	
	
	
	int x = scrollArea->horizontalScrollBar()->value();
	int y = scrollArea->verticalScrollBar()->value();
			
	
	x += size.width();
	y += size.height();
	
	
	
	
	
	scaled->resourceScaleRotate(CentralFrame::backgroundID);
	
	
	// scale and set

	x = std::round((float)x * relative);
	y = std::round((float)y * relative);
	
		
	x -= size.width();
	y -= size.height();
	
	
	
	this->scrollArea->horizontalScrollBar()->setValue(x);		
	this->scrollArea->verticalScrollBar()->setValue(y);
	
	
		
	Counter::setGUI();
	
	
	mapFrame->repaint();
	// force redraw of mask layer
	Overlay::overlay->clearMask();
			
}



void ToolBar::zoomIndex(int index)
{
			
	if (index >= 0 && index <= ToolBar::sizeBox->count()-1)
	{	
		QVariant data = ToolBar::sizeBox->itemData(index);
		
		if (data.userType() == QMetaType::Int)
		{
			ToolBar::sizeBox->setCurrentIndex(index);			
			const int percent = (ToolBar::sizeBox->itemData(index)).value<int>();	
			float fraction = (float)percent / 100.0;
			zoomMiddle(fraction);
		}
	}
				
}



void ToolBar::zoomFraction(QPoint point, float amount)
{		
	if (Scale::scaleFraction + amount <= 0.005 || 
		Scale::scaleFraction + amount > 2.0)
		return;
	
	float newFraction = Scale::scaleFraction + amount;			
	zoomCoordinates(point, newFraction);
	
	// set zoom value in combobox
	int zoom = (int)std::floor(newFraction * 100);
	ToolBar::sizeBox->setCurrentText(QString::number(zoom) + "%");			
}



void ToolBar::zoomIn()
{
	if (ToolBar::sizeBox != nullptr)
		zoomIndex(ToolBar::sizeBox->currentIndex() - 1);
}


void ToolBar::zoomOut()
{	
	if (ToolBar::sizeBox != nullptr)
		zoomIndex(ToolBar::sizeBox->currentIndex() + 1);	
}


void ToolBar::wheelIn(QPoint point)
{
	zoomFraction(point, 0.1);
}


void ToolBar::wheelOut(QPoint point)
{	
	zoomFraction(point, -0.1);	
}
