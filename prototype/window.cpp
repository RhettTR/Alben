#include "window.h"
#include "io.h"
#include "luau.h"
#include "scale.h"
#include "overlay.h"


extern IO *io;
extern Window *repositoryWindow;
extern Window *logWindow;
extern Scale *scaled;


std::string Window::rootTag; 

Window::PlainTextEdit *Window::textbox;
Window::LineEdit *Window::edit;
ToolBar *Window::bar;
		

Window *getInstance(const char *instance);
std::map<std::string, Window *> Window::instances;
	

// a bit dirty .. holds the instance of this window for class Pane .. set in root
Window *instance;




Window::PlainTextEdit::PlainTextEdit(QString css, QWidget *parent) : QPlainTextEdit(parent)
{
	this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
	this->setReadOnly(true);
	this->setStyleSheet(css);
}

Window::TextEdit::TextEdit(QWidget *parent) : QTextEdit(parent)
{
	this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

QSize Window::TextEdit::sizeHint() const
{ 
	return(((Pane *)this->parent())->size());
}

Window::LineEdit::LineEdit(QPlainTextEdit *logbox, QString css, QWidget *parent) : QLineEdit(parent)
{
	this->setStyleSheet(css);
	this->logbox = logbox;
	this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
}


void Window::LineEdit::keyPressEvent(QKeyEvent *e)
{
	std::string str = "<span style=\"color:black;\">" + this->text().toStdString() + "</span>";
	switch (e->key())
	{
		case Qt::Key_Return: 
		case Qt::Key_Enter : Luau::doLog("text", str.c_str());
							 Luau::doEvent("end", "", "", "", 0);
							 this->clear(); 
							 break;				
		default: break;
	}
	QLineEdit::keyPressEvent(e);
}




Window::Window(QWidget *parent, QString title, std::string tag, std::string type, bool anyScroll, int w, int h) : QMainWindow(parent)
{
	
	this->setWindowTitle(title);
	
 
    instances[tag] = this;
    
    this->tag = tag;
    this->font = "Sans Serif";
    this->fontSize = 12;
    this->weight = QFont::Normal;
    this->color = "#ffffff";
    this->minZoom = 0.2;
    this->maxZoom = 2.0;
    this->title = title.toStdString();
    
    
    // top level window
    if (parent == nullptr)
    {
		if (anyScroll)
		{
			this->scrollArea = new QScrollArea;
			setCentralWidget(scrollArea);
		
			this->container = new QWidget(this);
			container->setAcceptDrops(true);
				
			this->scrollArea->setWidget(container);
			this->frame = new CentralFrame(container, this, type, this->scrollArea);
		}
		else
			this->frame = new CentralFrame(parent, this, type);
		
		
		this->frame->setObjectName("centralFrame");
	}
	else
	if (tag == "Repository")
	{
		this->reset();
	
		
		this->scrollArea = nullptr;
		
		
		this->frame = new CentralFrame(parent, this, type);
		setCentralWidget(this->frame);
		
		
		QPalette palette = this->palette();
		palette.setColor(QPalette::Window, Qt::white);
		this->setPalette(palette);	
		setAutoFillBackground(true);
		
		this->move(70, 100);   
		this->resize(w, h);   
		
		
		this->setMinimumSize(QSize(300,200)); 
		
	}
	else
	if (tag == "LogChat")
	{
		
		this->scrollArea = nullptr;
		this->container = nullptr;
		
		this->frame = new CentralFrame(parent, this, type);
		setCentralWidget(this->frame);
		
		
		bar = new ToolBar(this->tag, "logbar", "Toolbar", 37, nullptr);
		bar->setIconSize(QSize(32, 37));
		this->addToolBar(Qt::TopToolBarArea, bar);
		
		
		QString fontStyle = "font: normal normal normal 15px/1.4 Arial; color: grey;";
		textbox = new PlainTextEdit(fontStyle, this);
		
		this->frame->layout()->addWidget(textbox);
		
		((QVBoxLayout *)this->frame->layout())->addStretch();
		 
		fontStyle = "font: normal normal normal 15px/1.4 Arial; color: black;";  
		edit = new LineEdit(textbox, fontStyle, this);
		this->frame->layout()->addWidget(edit);
		
		edit->setFocus();
		
		
		this->move(300, 750);   
		this->resize(w, h);
		textbox->resize(w, h - bar->height() - edit->height());   
		this->frame->backgroundColor = "white";
		

	}
	else
	{
		
		if (anyScroll)
		{
			this->scrollArea = new QScrollArea;
			this->setCentralWidget(this->scrollArea);
			
			this->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
			this->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
				
			
			this->frame = new CentralFrame(parent, this, type, this->scrollArea);
			this->scrollArea->setWidget(this->frame);
			this->frame->backgroundColor = "#F0F0F0";
			this->frame->startWidth = w;
			this->frame->startHeight = h;
			this->frame->resize(w, h);
			this->resize(w, h);
		}	
		else	
		{	
			this->scrollArea = nullptr;	
			this->frame = new CentralFrame(parent, this, type);
			this->frame->backgroundColor = "#F0F0F0";
			setCentralWidget(this->frame);
			this->frame->startWidth = w;
			this->frame->startHeight = h;
			this->resize(w, h);
			
		}
		
		
		
	}
    
    
	//setAcceptDrops(true); 		
	
}


Window::~Window()
{
	delete this->scrollArea;
}








Window *Window::getInstance(const char *instance)
{
	
	std::string name = std::string(instance);
	
	for ( auto obj = Window::instances.begin(); obj != Window::instances.end(); ++obj  )
	
		if (obj->first == name)
		{
			//Window::instance = obj->second;
			return obj->second;
		}

	name = "'" + name + "'";
	
	Luau::error(8, 1, name.c_str()); 
	std::exit(1);
	
}



void Window::showWindow()
{	
	if (this->isHidden())
		this->show();
	else
		this->hide();	
}


void Window::setOptions(QString font, int fontSize, std::string weight, std::string color)
{ 
	QFontDatabase database; 
	
	QStringList families = database.families();
	
	
	if (!families.contains(font))
	{
		Luau::error(9, 1, font.toStdString().c_str());
		std::exit(1);
	}
	else
		this->font = font;
	
	this->fontSize = fontSize;
	
	
	
	std::unordered_map<std::string, enum QFont::Weight> 
		m{{"Thin", QFont::Thin}, {"ExtraLight", QFont::ExtraLight}, 
		  {"Light", QFont::Light}, {"Normal", QFont::Normal}, 
		  {"Medium", QFont::Medium},{"DemiBold", QFont::DemiBold}, 
		  {"Bold", QFont::Bold}, {"ExtraBold", QFont::ExtraBold},
		  {"Black", QFont::Black}};
		  
	auto it = m.find(weight);
	
	if (it != m.end()) 
		this->weight = it->second;
	else 
	{ 
		Luau::error(10, 1, weight.c_str());
		std::exit(1);
	}
	
	// https://doc.qt.io/qt-6/qcolor.html#fromString
	QAnyStringView aColor = QString::fromStdString(color);
	
	if (QColor::isValidColorName(aColor))
		this->color = QColor::fromString(aColor);
	else
	{
		Luau::error(11, 1, color.c_str());
		std::exit(1);
	}	
		
		
}




void Window::setSingleRowed(int x, int y, int w, int h)
{
	
	// frame is never resized again
	this->frame->resize(w, h);
	

	
	// resize if 100% too big, 400 is height of window minus scrollbar 	
	if (this->frame->height() > 400)
	{
		this->resize(w, 400);
	}
	else
		this->resize(w, h);
			
		
	this->move(x, y);
	
}


void Window::addAWidget(std::string key,  QWidget *value)
{
	widgets[key] = value;
	setWidgets();
}




void Window::setWidgets()
{
	
	float fraction;
	
	if (widgets.size() == 0)
		return;
		
	
	
	fraction = this->frame->scaleFraction;	

	
	for (Widgets::iterator it = widgets.begin(); it != widgets.end(); it++)
	{
		PushButton *pushbutton = dynamic_cast<PushButton *>(it->second);
		
		if (pushbutton != nullptr)
		{	
			
			QImage image = io->getImage(pushbutton->resourceId);
			QSize size = io->getSize(pushbutton->resourceId);
			
			QSize scaledSize = size * fraction;
		
	
			QImage imageScaled = image.scaled( scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation); 
			
			pushbutton->setIcon(QIcon(QPixmap::fromImage(imageScaled)));
			pushbutton->resize(scaledSize);
			
			// what crap
				
			pushbutton->move((int)std::round( pushbutton->x * fraction), 
							 (int)std::round( pushbutton->y * fraction));	
							 
	
		}
		
		
		Label *label = dynamic_cast<Label *>(it->second);
		
		if (label != nullptr)
		{
			
			if (!label->backgroundImage.isNull())
			{
				QImage base =
					QImage(label->backgroundImage.width(), label->backgroundImage.height(), QImage::Format_ARGB32_Premultiplied);
				
				base.fill(Qt::transparent);
				
				QPainter *paint = new QPainter(&base);	
				paint->setRenderHint(QPainter::Antialiasing);
				paint->setRenderHint(QPainter::TextAntialiasing);
				paint->setRenderHint(QPainter::SmoothPixmapTransform);
				paint->scale((qreal)fraction, (qreal)fraction);	
				paint->drawImage(0, 0, label->backgroundImage);		
				delete paint;
				
				
				label->setPixmap(QPixmap::fromImage(base));
		
			}
			else
			{
				label->resize((int)std::round(label->w * fraction), (int)std::round(label->h * fraction));
			}
			
			
			label->move((int)std::round(label->x * fraction), (int)std::round(label->y * fraction));		
			
		}
		
		
		ChoiceBox *box = dynamic_cast<ChoiceBox *>(it->second);	
		
		if (box != nullptr)
		{
			
			int x = box->x - (int)std::round(box->width() / 2);
			int y = box->y - (int)std::round(box->height() / 2);
			
			box->move((int)std::round(x * fraction), (int)std::round(y * fraction));
			
		}
		
		
		Text *text = dynamic_cast<Text *>(it->second);	
		
		if (text != nullptr)
		{
			
			int x = text->x - (int)std::round(text->width() / 2);
			int y = text->y - (int)std::round(text->height() / 2);
			
			text->move((int)std::round(x * fraction), (int)std::round(y * fraction));
			
		}
	
	}
		
	
	
}


void Window::deleteWidgets()
{
	for (Widgets::iterator it = widgets.begin(); it != widgets.end(); it++)
		delete it->second;

	widgets.clear();
}


void Window::resizeEvent(QResizeEvent* event)
{
	
	QMainWindow::resizeEvent(event);
	
		
	if (this->frame->layout() != nullptr)
	{	
		QLayoutItem *item = this->frame->layout()->itemAt(0);
		
		if (item != nullptr)
		{	
			QWidget *widget = item->widget();	
			widget->resize(event->size());
		}
	}		
	
	if (this->tag == "LogChat")
		textbox->resize(this->width(), this->height() - this->bar->height() - edit->height());
	
	event->accept();
	
}





Window::Label::Label(Window *parent, std::string tag, int x, int y, int w, int h, std::string resourceName) : 
					 QLabel((QWidget *)parent->frame), x(x), y(y), w(w), h(h)
{
	
	if (resourceName.empty())
	{
		this->resize(w, h);			
	}
	else
	{
		this->backgroundImage = io->getImage(resourceName);
		this->setPixmap(QPixmap::fromImage(backgroundImage));
		this->resize(this->pixmap().size());	
	}	
		

	this->setVisible(true);
	
	
	this->parent = parent;
	this->tag = tag;
	
	
	parent->addAWidget(tag, this);
	

}


void Window::Label::setText(QString text)
{
	QImage image = QImage(this->w, this->h, QImage::Format_ARGB32_Premultiplied);
	
	image.fill(Qt::transparent);
	
	
	
	QPainter *paint = new QPainter(&image);
	
	paint->setRenderHint(QPainter::TextAntialiasing);
	
	paint->setFont(QFont(this->parent->font, this->parent->fontSize, this->parent->weight));
	paint->setPen(QPen(QColor(this->parent->color)));
	
	paint->drawText(0, 0, this->w, this->h, Qt::AlignCenter, text);
	
	
	delete paint;
	
	
	
	this->backgroundImage = image;	
	this->setPixmap(QPixmap::fromImage(image));
	this->resize(this->w, this->h);
	
	this->parent->setWidgets();
	
}


std::string Window::Label::get()
{
	return this->text().toStdString();
}


void Window::Label::mousePressEvent (QMouseEvent * event) 
{
	QLabel::mousePressEvent(event);
	event->accept();
}





Window::CheckBox::CheckBox(std::string id, Window *parent, int x, int y, int w, int h, QString text, std::string luaScript, 
							QString styleSheet) : QCheckBox(text, (QWidget *)parent), x(x), y(y)
{

	setStyleSheet(styleSheet);
	setFixedSize(w, h);
	setVisible(true);
	
	move(x, y);
	
	QObject::connect( this, &QAbstractButton::clicked, [=]()->void{ Luau::callbackScript(luaScript); });
	
}

bool Window::CheckBox::get()
{
	return this->isChecked();
}



Window::PushButton::PushButton(Window *window, std::string tag, std::string resourceName, QString text, int x, int y, 
								std::string luaScript) : QPushButton(window->frame), x(x), y(y)
{
	this->setStyleSheet("border: 0; background: transparent");
	
	QImage image = io->getImage(resourceName);
	this->setIcon(QIcon(QPixmap::fromImage(image)));	
	this->setIconSize(image.rect().size());
	
	QObject::connect(this, &QPushButton::clicked, [=]()->void{ Luau::callbackScript(luaScript); });
	
	this->resourceId = resourceName;
	this->window = window;
	this->tag = tag;
	
	window->addAWidget(tag, this);	

}


void Window::PushButton::mousePressEvent (QMouseEvent * event) 
{
	QPushButton::mousePressEvent(event);
	event->accept();
}




Window::ChoiceBox::ChoiceBox(QWidget *parent, Window *window, std::string tag, int x, int y) : 
			QComboBox(parent), window(window), tag(tag), x(x), y(y)
{
	
	window->addAWidget(tag, this);
	
	
	this->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	
	QObject::connect(this, &QComboBox::activated, 
					 this, [=]()->void{ active(this->currentIndex()); });
}


void Window::ChoiceBox::add(QString text, QString data)
{
	
	const QString &t = text; 
	
	this->addItem(t, QVariant(data));
	
	this->window->setWidgets();
}


void Window::ChoiceBox::active(int index)
{	
	
	QVariant variant = this->itemData(index);
	
	QString str = QVariant(variant).toString();
	
	//printf("%s\n",str.toStdString().c_str());
	
	// data is a script chunck
	
	Luau::callbackScript(str.toStdString());
	
	Window::getInstance("main")->frame->update();
}








Window::Text::Text(QWidget *parent, Window *window, std::string tag, int x, int y, int w, int h) : 
					 QLabel(parent), x(x), y(y), w(w), h(h)
{
	
	this->resize(w, h);			
	this->setVisible(true);
	
	
	this->parent = parent;
	this->window = window;
	this->tag = tag;
	
	
	this->window->addAWidget(tag, this);
	

}

void Window::Text::setOptions(QString alingment, QString border)
{
	
	QString str = alingment.simplified();
		
	QStringList entries = str.split(':');
	
	
	
	entries[0] = entries[0].trimmed();
	entries[1] = entries[1].trimmed();
	
	std::unordered_map<QString, enum Qt::AlignmentFlag> 
		verticalAlignment{{"Left", Qt::AlignLeft}, {"Right", Qt::AlignRight}, 
						  {"Center", Qt::AlignHCenter}, {"Justify", Qt::AlignJustify}};
		  
	std::unordered_map<QString, enum Qt::AlignmentFlag> 
		horisontalAlignment{{"Top", Qt::AlignTop}, {"Bottom", Qt::AlignBottom}, 
						    {"Center", Qt::AlignVCenter}, {"Base", Qt::AlignBaseline}};
		 
	
	enum Qt::AlignmentFlag vertical;
	enum Qt::AlignmentFlag horizontal;
		  
	auto it = verticalAlignment.find(entries[0]);
	
	if (it != verticalAlignment.end()) 
		vertical = it->second;
	else 
		Luau::error(37, 1, entries[0].toStdString().c_str());
	
	it = horisontalAlignment.find(entries[1]);
	
	if (it != horisontalAlignment.end()) 
		horizontal = it->second;
	else 
		Luau::error(38, 1, entries[1].toStdString().c_str());
	
	
	this->setAlignment(vertical | horizontal);
	this->window->setWidgets();

}


void Window::Text::set(QString text)
{
	
	this->setFont(QFont(this->window->font, this->window->fontSize, this->window->weight));
	this->setText(text);
	this->window->setWidgets();
	
}











Window::Pane::Pane(QWidget *parent) : QLabel(parent)
{
	
	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, Qt::white);
	this->setPalette(palette);	
	setAutoFillBackground(true);
	this->scaleFraction = 1.0;
	
	setContentsMargins(0, 0, 0, 0);
	
}
	
	
void Window::Pane::resizeEvent(QResizeEvent* event)
{
   QLabel::resizeEvent(event);
}



void Window::Pane::setZoom(float amount)
{
	zoomFraction(amount, true);
}

void Window::Pane::zoomFraction(float amount, bool set)
{	
	
	for (int i = 0; i < this->layout()->count(); i++) 
	{	
		
		QLayoutItem* item = this->layout()->itemAt(i);		
		QWidget *widget = item->widget();
		
		if (widget != nullptr)
		{
			Counter::QtCounter *counter = dynamic_cast<Counter::QtCounter*>(widget);
			
			if (counter != nullptr)
			{
				
				float scaleFraction = (set ? 0 : counter->owner->state.scale);
		
				if (scaleFraction + amount <= 0.2 || 
					scaleFraction + amount > 1.0)
					continue;
				
				if (set)
					counter->owner->state.scale = amount;
				else 
					counter->owner->state.scale = scaleFraction + amount;
				
				counter->owner->setImage();
				
			}
			else
			{
			
				QLabel *label = dynamic_cast<QLabel*>(widget);
				
				if (label != nullptr)
				{
					if (set)
						this->scaleFraction = amount;
					else	
						this->scaleFraction = this->scaleFraction + amount;	
					
					if (this->scaleFraction > 1.0)
						this->scaleFraction = 1.0;
					if (this->scaleFraction < 0.2)
						this->scaleFraction = 0.2;	
										
					
					if (io->isResource(this->imageID))
					{
						QImage image = io->getImage(this->imageID);
						QSize size(std::round(io->getSize(this->imageID).width() * this->scaleFraction), 
								   std::round(io->getSize(this->imageID).height() * this->scaleFraction));
						QImage imageScaled = image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
						label->resize(size);
						label->setPixmap( QPixmap::fromImage(imageScaled) );
						
						if (instance->frame->scrollArea != nullptr)
							// 32 height of tab ??
							instance->frame->resize(QSize(0,32) + size);
							
					}
					
				}
				
			}
		}
		
	}
	
			
}

void Window::Pane::wheelIn()
{
	zoomFraction(0.05, false);
}

void Window::Pane::wheelOut()
{	
	zoomFraction(-0.05, false);	
}


void Window::Pane::wheelEvent(QWheelEvent *event)
{
	
	if (this->layout() != nullptr)
	{	
		int amount = event->angleDelta().y();
					
		if (amount > 0)		
			wheelIn();		
		else		
			wheelOut();	
	}
	
	event->accept();
}


void Window::Pane::showEvent(QShowEvent *event)
{
	// resize frame based on current shown Pane content
	
	if (instance->frame->scrollArea != nullptr)
	{
		QLayoutItem* item = this->layout()->itemAt(0);		
		QWidget *widget = item->widget();
		
		if (widget != nullptr)
		{
			QLabel *label = dynamic_cast<QLabel*>(widget);
			if (label != nullptr)
				// 32 height of tab ??
				instance->frame->resize(QSize(0,32) +  label->size());
		}
	}
	
}



Window::ListBox::ListBox(QWidget *parent) : QListWidget(parent)
{
	QObject::connect(this, &QListWidget::itemClicked, 
					 this, [=]()->void{ itemClicked((ListItem *)this->selectedItems().at(0)); });
					 
	QObject::connect(this, &QListWidget::itemSelectionChanged, 
					 this, [=]()->void{ itemSelectionChanged(); });
}


void Window::ListBox::itemClicked(ListItem *item)
{	
	for (int i = 0; i < item->listWidget()->count(); ++i)
	{
		ListItem* lt = (ListItem *)item->listWidget()->item(i);
		lt->pane->setVisible(false);
	}
	
	item->pane->setVisible(true);	
}


void Window::ListBox::itemSelectionChanged()
{	
	
	ListItem *item = (ListItem *)this->selectedItems().at(0);
	
	for (int i = 0; i < item->listWidget()->count(); ++i)
	{
		ListItem* lt = (ListItem *)item->listWidget()->item(i);
		lt->pane->setVisible(false);
	}
	
	item->pane->setVisible(true);	
}



Window::ListItem::ListItem(const QString &text, ListBox *parent, Pane *paneParent, int type) : 
										QListWidgetItem(text, (QListWidget *)parent, type)
{
	Pane *pane = new Pane(paneParent);
	
	pane->setVisible(false);
	paneParent->layout()->addWidget(pane);
	
	
	this->pane = pane;
    
    parent->addItem(this);
    
    parent->setCurrentItem(parent->item(0));
    ((ListItem *)parent->currentItem())->pane->setVisible(true);
 

}



Window::ComboBox::ComboBox(QWidget *parent) : QComboBox(parent)
{
	QObject::connect(this, &QComboBox::activated, 
					 this, [=]()->void{ activated(this->currentIndex()); });
}

void Window::ComboBox::activated(int index)
{	
	
	for (int i = 0; i < this->count(); ++i)
	{
		QVariant variant = this->itemData(i);
		Pane *pane = variant.value<Pane*>();
		
		pane->setVisible(false);
	}
	
	
	
	QVariant variant = this->itemData(index);
	
	
	if ( strcmp(variant.typeName(), "Window::Pane*") == 0 )
	{
		Pane *pane = variant.value<Pane*>();		
		pane->setVisible(true); 
	}
		
}



		
Window::Pane* Window::getParent()
{

	// return current top pane
	

    Pane **pane = std::any_cast<Pane*>(&panes.top());  
  
	assert( pane != nullptr );
	
	
	
	if ((*pane)->layout() == nullptr)
	{
		(*pane)->setLayout(new Overlay::FlowLayout(*pane));
		(*pane)->layout()->setContentsMargins(0, 0, 0, 0);
	}


	return *pane;
	
}



void Window::visibility(bool value)
{
	this->setVisible(value);	
}





int lastLevel = 0;



void Window::root(int level)
{	
	
	Pane *pane = new Pane(this->frame);
	
	paneList.append(pane);
	
	
	this->frame->layout()->addWidget(pane);
	
	pane->resize(this->frame->size());
	
	lastLevel = level;
	
	
	panes.push(pane);
	
	instance = this;
	
}




void Window::tabs(int level)
{
	
	QTabWidget *w;
	
	
	
	Pane **parent = (Pane **)std::any_cast<Pane*>(&panes.top());
	
	assert( parent != nullptr );
	
	
	
	(*parent)->setLayout(new QVBoxLayout((*parent)));	
	(*parent)->layout()->setContentsMargins(QMargins());
	(*parent)->layout()->setSpacing(0);	
	
	
	
	
	w = new QTabWidget(*parent);
	
	
	(*parent)->layout()->addWidget(w);	
	w->resize((*parent)->size());
	
	
	lastLevel = level;
	
    panes.push(w);
 
}


void Window::tab(int level, std::string text)
{
	
	if (level == lastLevel)
		panes.pop();
	
		
	if (level < lastLevel) 			
		for (int i = 0; i <= lastLevel - level; i++)
			panes.pop();
	
	
	Pane **parent1 = std::any_cast<Pane*>(&panes.top());		
	QTabWidget **parent2 = std::any_cast<QTabWidget*>(&panes.top());
	
	assert( parent1 != nullptr || parent2 != nullptr );
	
	
	
	
	if (parent1 != nullptr)
	{
		
	}
	
	else
	
	{
		
		Pane *pane = new Pane(*parent2); 
		
		paneList.append(pane); 
    
		// IMPORTANT: you can not presume anything about what follows a tab;
		// therefore you can not set its layout here
		

		(void)(*parent2)->addTab(pane, QString::fromStdString(text));
		
	   
		pane->resize( ((QWidget*)(*parent2)->parent())->size() );
		
		panes.push(pane);
		
	}
	
	
		
	lastLevel = level;
    

}



void Window::listBox(int level)
{
	
    
	Pane **parent = (Pane **)std::any_cast<Pane*>(&panes.top());
		
	assert( parent != nullptr );
	
	
	
	(*parent)->setLayout(new QVBoxLayout((*parent)));	
	(*parent)->layout()->setContentsMargins(QMargins());
	(*parent)->layout()->setSpacing(0);		
	
   

	
	
	QSplitter *splitter = new QSplitter(Qt::Horizontal, *parent);
	
	splitter->setStyleSheet("QSplitter::handle{ background:gainsboro }");
	splitter->setHandleWidth(3);
	
		
	(*parent)->layout()->addWidget(splitter);
	
	
	
	
     
	ListBox *listWidget = new ListBox(splitter);
	
	
	
	listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	
	
	Pane *pane = new Pane(splitter);
	
	paneList.append(pane);
	
	pane->setLayout(new QVBoxLayout());
	pane->layout()->setContentsMargins(QMargins());
	
    
	splitter->addWidget(pane);
	splitter->addWidget(listWidget);

	QList<int> sizes;
    sizes.append(0.75 * splitter->sizeHint().width());
    sizes.append(0.25 * splitter->sizeHint().width());
    splitter->setSizes(sizes);
    
    
    lastLevel = level;
    
	panes.push(splitter);

	
}


void Window::listItem(int level, std::string text)
{

	if (level == lastLevel)
		panes.pop();
	
		
	if (level < lastLevel) 			
		for (int i = 0; i <= lastLevel - level; i++)
			panes.pop();
	
	
	Pane **parent1 = std::any_cast<Pane*>(&panes.top());		
	QSplitter **parent2 = std::any_cast<QSplitter*>(&panes.top());
	
	assert( parent1 != nullptr || parent2 != nullptr );
	
	
	
	if (parent1 != nullptr)
	{
		
	}
	
	else
	
	{
			
		ListItem *item = new ListItem(QString::fromStdString(text), 
									 (ListBox *)(*parent2)->widget(1), 
									 (Pane *)(*parent2)->widget(0));
    
		panes.push(item->pane);
	}


    lastLevel = level;
    
    
}


	
void Window::comboBox(int level)
{
	
	
	Pane **parent = (Pane **)std::any_cast<Pane*>(&panes.top());
	
	
	assert( parent != nullptr );
	
	
	(*parent)->setLayout(new QVBoxLayout((*parent)));	
	(*parent)->layout()->setContentsMargins(QMargins());
	(*parent)->layout()->setSpacing(0);	
			
   
	
	
	
	Pane *pane = new Pane(*parent);
	
	paneList.append(pane);
	
	pane->setLayout(new QVBoxLayout(pane));
	pane->layout()->setContentsMargins(QMargins());
	pane->layout()->setSpacing(0);
	
	pane->resize( (*parent)->size() );
	
	
	(*parent)->layout()->addWidget(pane);

	
	
	ComboBox *c = new ComboBox(pane);

	
	pane->layout()->addWidget(c);	
	
	
	
	lastLevel = level;
	
    
	panes.push(c);
	
}



void Window::comboItem(int level, std::string text)
{
	
	if (level == lastLevel)
		panes.pop();
	
		
	if (level < lastLevel) 			
		for (int i = 0; i <= lastLevel - level; i++)
			panes.pop();
	
	
	Pane **parent1 = std::any_cast<Pane*>(&panes.top());		
	ComboBox **parent2 = std::any_cast<ComboBox*>(&panes.top());
	
	
	assert( parent1 != nullptr || parent2 != nullptr );
	
	
	
	
	if (parent1 != nullptr)
	{
		
	}
	
	else
	{
	
		(*parent2)->addItem(QString::fromStdString(text)); 
	
		Pane *pane = new Pane((Pane *)(*parent2)->parent());
		
		paneList.append(pane);
		
		pane->setVisible(false);
		
		(*parent2)->setItemData((*parent2)->count() - 1, QVariant::fromValue(pane), Qt::UserRole);
	
		((Pane *)(*parent2)->parent())->layout()->addWidget(pane);
		
		(*parent2)->activated(0);
		
		
		
		panes.push(pane);
			
	}


	lastLevel = level;
	

}
	
	
void Window::imageItem(std::string imageID)
{

	Pane *pane = this->getParent();
	
	pane->imageID = imageID;
    
	
	QImage image = io->getImage(imageID);
	
	QLabel *widget = new QLabel();
	widget->setPixmap( QPixmap::fromImage(image) );

	pane->layout()->addWidget(widget);
	
}


void Window::htmlItem(std::string filename)
{

	Pane *pane = this->getParent();
	
	
	TextEdit *text = new TextEdit();
	
	text->setReadOnly(true);
	
	pane->resize(this->size());
	text->resize(pane->size());
	
		
	QString source = io->loadHtml(filename);
	
	text->show();
	text->resize(pane->size());
	
	
	
	text->setHtml(source);


	pane->layout()->addWidget(text);
	
}


void Window::scaleFrame(float amount)
{
	this->frame->zoomFraction(amount, true);
}	

	
void Window::scalePane(float amount)
{
	Window::Pane *pane = this->getParent();
	pane->setZoom(amount);
}	

	
	

void Window::reset()
{
	
	//  the code here is not finished !!!
	
	
	QMutableListIterator<Pane*> p(paneList);
	
	if (p.hasNext())
		delete p.next();
	
	
	paneList.clear();
	paneList.squeeze();

    
}


QString Window::textInput(QString title, QString caption, QString start)
{
	
	bool ok{};
	Window *parent = Window::getInstance("main");
	
    QString text = QInputDialog::getText(parent, title,
                                         caption, QLineEdit::Normal,
                                         start, &ok);
    if (ok)
        return text;
	
	return start;
	
}



// from cppreference, seed set in main.cpp 

// NOTE: this uses a simplified generator and should be changed, see more here: 
// https://en.cppreference.com/w/cpp/numeric/random/rand.html

unsigned bounded_rand(unsigned range)
{
    for (unsigned x, r;;)
        if (x = rand(), r = x % range, x - r <= -range)
            return r;
}

void Window::rollDie()
{
	 
	unsigned roll = 1 + bounded_rand(6);
	
	// print roll in log window
	std::string text = "<span style=\"color:gray;\">dr: </span><span style=\"color:red;\">" + std::to_string(roll) + "</span>";
	Luau::doLog("roll", text.c_str());
	Luau::doEvent("end", "", "", "", 0);
	
}
