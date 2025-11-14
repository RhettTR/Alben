#include <QtWidgets>

#include "frame.h"
#include "luau.h"
#include "io.h"
#include "scale.h"
#include "overlay.h"
#include "toolbar.h"
#include "window.h"


extern Luau l;
extern QWidget *container;
extern IO *io;
extern Scale *scaled;




ToolBar::MapSizeComboBox *ToolBar::sizeBox = nullptr;
std::map<std::string, ToolBar *> ToolBar::instances;





ToolBar::ButtonAction::ButtonAction(const char *id, std::string resourceName, QString buttonText, const QString toolTip, 
									std::function<void(void)> f, std::string luaScript) 
{
	QPixmap pixmap;
	(void)pixmap.convertFromImage(io->getImage(resourceName));
	const QIcon icon = QIcon(pixmap);


	this->setIcon(icon);
	this->setIconText(buttonText);	
	this->setToolTip(toolTip);
	this->setEnabled(false);
	
	 
	this->id = id;

	
	if (luaScript.empty())		
		QObject::connect(this, &QAction::triggered, this, [=]()->void{ f(); });	
	else
		QObject::connect(this, &QAction::triggered, this, [=](){ Luau::callbackScript(luaScript); });
	
}



ToolBar::ToolButton::ToolButton(const char *id, std::string resourceName, const QString toolTip, 
								std::function<void(void)> f, std::string luaScript) 
{
	QPixmap pixmap;
	(void)pixmap.convertFromImage(io->getImage(resourceName));
	const QIcon icon = QIcon(pixmap);


	this->setIcon(icon);
	this->setToolTip(toolTip);
	
	 
	this->id = id;

	
	if (luaScript.empty())		
		QObject::connect(this, &QAbstractButton::clicked, this, [=]()->void{ f(); });	
	else
		QObject::connect(this, &QAbstractButton::clicked, this, [=](){ Luau::callbackScript(luaScript); });
	
}



ToolBar::Label::Label(std::string str, const QString toolTip, int w, int h, const QString css)
{
	if (io->isResource(str))
	{
		QImage image = io->getImage(str);	
		this->setPixmap(QPixmap::fromImage(image).scaled(w, h, Qt::KeepAspectRatio));
		this->setToolTip(toolTip);
		this->setEnabled(false);	
	}
	else 
	{
		// text label
		this->setMinimumSize(QSize(w,h));
		this->setMaximumSize(QSize(w,h));
		this->setEnabled(true);
	}	
	
	this->setStyleSheet(css);
	this->id = str;
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
		QSize total = io->getSize(Window::getInstance("main")->frame->backgroundID);
		
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




ToolBar::ToolBar(std::string tag, const QString title, QScrollArea *scrollArea) : QToolBar(title, getInstance(tag.c_str()))
{
	this->scrollArea = scrollArea;
	
	if (tag != "main")
		this->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	
	this->setIconSize(QSize(24, 24));
	this->toolbarPinned = false;	
	this->tag = tag;	
	instances[tag] = this;

}


ToolBar::~ToolBar()
{
}






ToolBar *ToolBar::getInstance(const char *instance)
{
	
	std::string name = std::string(instance);
	
	for ( auto obj = ToolBar::instances.begin(); obj != ToolBar::instances.end(); ++obj  )
	
		if (obj->first == name)
		{
			return obj->second;
		}
			
	return nullptr;
	
}



void ToolBar::reset()
{
	
	for ( auto obj = ToolBar::instances.begin(); obj != ToolBar::instances.end(); ++obj  )
	{
		if (obj->second->windowTitle() != "System toolbar")
			((Window *)this->parent())->removeToolBar(obj->second);
	}
	
}



void ToolBar::addImageButton(const char *id, std::string resourceName, QString buttonText, const QString toolTip, std::function<void(void)> func, std::string luaScript)
{
	if (this->tag == "main" || !buttonText.isEmpty())
	{
		ButtonAction *action = new ButtonAction(id, resourceName, buttonText, toolTip, func, luaScript);
		this->addAction(action);
	}
	else
	{
		ToolButton *button = new ToolButton(id, resourceName, toolTip, func, luaScript);
		this->addWidget(button);
	}
}


void ToolBar::setImageButton(std::string id, std::string resourceName, QString buttonText)
{
	foreach (QObject *child, this->children())
		if (child->inherits("QToolButton"))
			if (((ToolButton *)child)->id == id)
			{
				QImage image = io->getImage(resourceName);
				QSize size = QSize(this->width(), this->height());	
				QIcon icon(QPixmap::fromImage(image).scaled(size, Qt::KeepAspectRatio));
				((ToolButton *)child)->setIcon(icon);
				return;
			}		
}
		
		
void ToolBar::setLabelImage(std::string id, std::string resourceName, QString buttonText)
{

	foreach (QObject *child, this->children()) 
		
		if (child->inherits("QLabel"))
		{
			Label *label = (Label *)child;
			if (label->id == id)
			{	
				QImage image = io->getImage(resourceName);
				QSize size = label->pixmap().size();	
				label->setPixmap(QPixmap::fromImage(image).scaled(size, Qt::KeepAspectRatio));
				label->setText(buttonText);
			}
		}
		
		
}

void ToolBar::addLabel(std::string id, std::string resourceName, const QString toolTip, int w, int h, const QString css)
{
	Label *label = new Label(resourceName, toolTip, w, h, css);	
	label->setParent(this);
	label->resize(w, h);
	label->id = id;
	this->addWidget(label);
}

void ToolBar::setLabel(std::string id, QString text)
{	
	foreach (QObject *child, this->children()) 	
		if (child->inherits("QLabel"))
		{
			Label *label = (Label *)child;
			if (label->id == id)
				label->setText(text);
		}    
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

void ToolBar::addseparator()
{
	this->addSeparator();
}



void ToolBar::addSizeComboBox()
{
		
	ToolBar::sizeBox = new MapSizeComboBox(this);
	
	MapSizeAction *toolButtonAction = new MapSizeAction(this);
	toolButtonAction->setDefaultWidget(ToolBar::sizeBox);

	this->addAction(toolButtonAction);
}



void ToolBar::zoom(float fraction)
{
	
	Window::getInstance("main")->frame->scaleFraction = fraction;
	
	scaled->resourceScaleRotate("main", Window::getInstance("main")->frame->backgroundID);
	
	
	Counter::setGUI();
	
	
	Window::getInstance("main")->frame->repaint();
	// force redraw of mask layer
	Overlay::overlay->clearMask();
	
}



void ToolBar::zoomCoordinates(QPoint point, float newFraction)
{
	
	
		
	float oldFraction = Window::getInstance("main")->frame->scaleFraction;
		
	Window::getInstance("main")->frame->scaleFraction = newFraction;
	
	
	
	float relative = Window::getInstance("main")->frame->scaleFraction / oldFraction;
		
	
	
	int x = scrollArea->horizontalScrollBar()->value();
	int y = scrollArea->verticalScrollBar()->value();
	
	int dx = point.x() - x;
	int dy = point.y() - y;
	
	x += dx;
	y += dy;
	
	
	scaled->resourceScaleRotate("main", Window::getInstance("main")->frame->backgroundID);
	
	
	
	// scale and set

	x = std::round((float)x * relative);
	y = std::round((float)y * relative);
	

	x -= dx;
	y -= dy;
	
	
	this->scrollArea->horizontalScrollBar()->setValue(x);		
	this->scrollArea->verticalScrollBar()->setValue(y);
	
	
		
	Counter::setGUI();
	
	
	Window::getInstance("main")->frame->repaint();
	// force redraw of mask layer
	Overlay::overlay->clearMask();
	
}



void ToolBar::zoomMiddle(float newFraction)
{

	float oldFraction = Window::getInstance("main")->frame->scaleFraction;
		
	Window::getInstance("main")->frame->scaleFraction = newFraction;
	
	
	
	float relative = Window::getInstance("main")->frame->scaleFraction / oldFraction;
	
	
	
	// find middle of window
	
	QWidget *map = (QWidget *)((QMainWindow *)parent())->centralWidget();
		
	QSize size = map->size();		       
	
							
	size /= 2;
	
	
	
	int x = scrollArea->horizontalScrollBar()->value();
	int y = scrollArea->verticalScrollBar()->value();
			
	
	x += size.width();
	y += size.height();
	
	
	
	
	
	scaled->resourceScaleRotate("main", Window::getInstance("main")->frame->backgroundID);
	
	
	// scale and set

	x = std::round((float)x * relative);
	y = std::round((float)y * relative);
	
		
	x -= size.width();
	y -= size.height();
	
	
	
	this->scrollArea->horizontalScrollBar()->setValue(x);		
	this->scrollArea->verticalScrollBar()->setValue(y);
	
	
		
	Counter::setGUI();
	
	
	Window::getInstance("main")->frame->repaint();
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
			
			Window::getInstance("main")->setWidgets();

		}
	}
				
}



void ToolBar::zoomFraction(QPoint point, float amount)
{	
	float scaleFraction = Window::getInstance("main")->frame->scaleFraction;
		
	if (scaleFraction + amount <= 0.005 || 
		scaleFraction + amount > 2.0)
		return;
	
	float newFraction = scaleFraction + amount;			
	zoomCoordinates(point, newFraction);
	
	// set zoom value in combobox
	int zoom = (int)std::floor(newFraction * 100);
	ToolBar::sizeBox->setCurrentText(QString::number(zoom) + "%");
	
	Window::getInstance("main")->setWidgets();
			
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
