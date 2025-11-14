#include "window.h"
#include "io.h"
#include "luau.h"
#include "scale.h"
#include "overlay.h"
#include "toolbar.h"


extern IO *io;
extern Window *logWindow;


std::string Window::rootTag; 

Window::PlainTextEdit *Window::textbox;
Window::LineEdit *Window::edit;
		

Window *getInstance(const char *instance);
std::map<std::string, Window *> Window::instances;



Window::PlainTextEdit::PlainTextEdit(QString css, QWidget *parent) : QPlainTextEdit(parent)
{
	this->setReadOnly(true);
	this->setStyleSheet(css);
}


Window::LineEdit::LineEdit(QPlainTextEdit *logbox, QString css, QWidget *parent) : QLineEdit(parent)
{
	this->setStyleSheet(css);
	this->logbox = logbox;
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




Window::Window(QWidget *parent, QString title, std::string tag, std::string type) : QMainWindow(parent)
{
	
	this->setWindowTitle(title);
	
    
    instances[tag] = this;
    
    this->tag = tag;
    this->zooming = false;
    
    
    // top level window
    if (parent == nullptr)
    {
		this->scrollArea = new QScrollArea;
		setCentralWidget(scrollArea);
		
		this->container = new QWidget(this);
		container->setAcceptDrops(true);
			
		this->scrollArea->setWidget(container);
	
		this->frame = new CentralFrame(container, this, type, this->scrollArea);
		
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
		this->resize(480, 300);   
		this->frame->resize(480, 300);  
		
		
		this->show();
	}
	else
	if (tag == "LogChat")
	{
		
		this->scrollArea = nullptr;
		this->container = nullptr;
		
		this->frame = new CentralFrame(parent, this, type);
		setCentralWidget(this->frame);
		
		
		
		
		ToolBar *bar = new ToolBar("logbar", "Toolbar", nullptr);
		this->addToolBar(Qt::TopToolBarArea, bar);
		
		
		
		QString fontStyle = "font: normal normal normal 15px/1.4 Arial; color: grey;";
		
		textbox = new PlainTextEdit(fontStyle, this);
		this->frame->layout()->addWidget(textbox);
		
		 
		fontStyle = "font: normal normal normal 15px/1.4 Arial; color: black;";  
		edit = new LineEdit(textbox, fontStyle, this);
		this->frame->layout()->addWidget(edit);
		
		edit->setFocus();
		
		
		this->move(300, 750);   
		this->resize(800, 250);   
		this->frame->resize(800, 250);
		this->frame->backgroundColor = "white";

			 
		this->show();
	}
	else
	{
		this->scrollArea = nullptr;
		this->container = nullptr;
		
		this->frame = new CentralFrame(parent, this, type);
		setCentralWidget(this->frame);
		
	}
    
    
	setAcceptDrops(true); 		
	
}


Window::~Window()
{
	delete this->scrollArea;
}





Window *Window::instance = nullptr;


Window *Window::getInstance(const char *instance)
{
	
	std::string name = std::string(instance);
	
	for ( auto obj = Window::instances.begin(); obj != Window::instances.end(); ++obj  )
	
		if (obj->first == name)
		{
			Window::instance = obj->second;
			return obj->second;
		}
			
	return nullptr;
	
}



void Window::showWindow()
{	
	if (this->isHidden())
		this->show();
	else
		this->hide();	
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






void Window::setWidgets(int height)
{
	
	float fraction;
	
	if (widgets.size() == 0)
		return;
		
	
	if (height == 0)
		fraction = Window::getInstance(this->tag.c_str())->frame->scaleFraction;	
	else
	{	
		
		if (this->size().height() == 0)
			return;	
					
	
		float factor = (float)height / (float)this->frame->size().height();
		
		
		if (factor > 1.0)
			factor = 1.0;
			
		
		fraction = factor;	
	}
	

	
	for (Widgets::iterator it = widgets.begin(); it != widgets.end(); it++)
	{
		PushButton *pushbutton = dynamic_cast<PushButton *>(it->second);
		
		if (pushbutton != nullptr)
		{	
			QImage base =
				QImage(pushbutton->image.width(), pushbutton->image.height(), QImage::Format_ARGB32_Premultiplied);
		
			base.fill(Qt::transparent);
			
			QPainter *paint = new QPainter(&base);	
			paint->setRenderHint(QPainter::Antialiasing);
			paint->setRenderHint(QPainter::TextAntialiasing);
			paint->setRenderHint(QPainter::SmoothPixmapTransform);
			paint->scale((qreal)fraction, (qreal)fraction);	
			paint->drawImage(0, 0, pushbutton->image);		
			delete paint;
			
			pushbutton->setIcon(QIcon(QPixmap::fromImage(base)));									      
									  
			pushbutton->move((int)(pushbutton->x * fraction), (int)(pushbutton->y * fraction));
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
				label->resize((int)(label->w * fraction), (int)(label->h * fraction));
			}
			
			
			label->move((int)(label->x * fraction), (int)(label->y * fraction));		
			
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
	
	if (zooming)
		setWidgets(event->size().height());
		
	
	if (this->tag == "Repository")
	{	
		QLayoutItem *item = this->frame->layout()->itemAt(0);
		
		if (item != nullptr)
		{	
			QWidget *widget = item->widget();	
			widget->resize(event->size());
		}
	}		
		
	
	
	//this->frame->setBackground();
	
	event->accept();
	
}





Window::Label::Label(std::string tag, Window *parent, int x, int y,  int w, int h, std::string resourecName, 
						QString styleSheet) : QLabel((QWidget *)parent->frame), x(x), y(y), w(w), h(h)
{
	
	if (resourecName.empty())
	{
		this->resize(w, h);			
	}
	else
	{
		this->backgroundImage = io->getImage(resourecName);
		this->setPixmap(QPixmap::fromImage(backgroundImage));
		this->resize(this->pixmap().size());	
	}	
		

	this->setVisible(true);
	
		
		
	this->setStyleSheet(styleSheet);
	
	this->parent = parent;
	this->tag = tag;
	
	
	parent->addAWidget(tag, this);

}


void Window::Label::setText(std::string text)
{
	QImage image = QImage(this->w, this->h, QImage::Format_ARGB32_Premultiplied);
		
	image.fill(Qt::transparent);
	
	QPainter *paint = new QPainter(&image);
	paint->setRenderHint(QPainter::TextAntialiasing);
	paint->setFont(QFont("Serif", 30, QFont::Bold));
	paint->setPen(Qt::black);
	paint->drawText(0, 0, this->w, this->h, Qt::AlignCenter, QString::fromStdString(text));		
	delete paint;
	
	this->backgroundImage = image;	
	this->setPixmap(QPixmap::fromImage(image));
	
	this->parent->setWidgets();
	
}


std::string Window::Label::get()
{
	return this->text().toStdString();
}


void Window::Label::mousePressEvent (QMouseEvent * event) 
{
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



Window::PushButton::PushButton(std::string widget, QImage image, QString text, int x, int y, std::string luaScript, 
						       Window *parent) : QPushButton(QIcon(QPixmap::fromImage(image)), text, (QWidget *)parent->frame), x(x), y(y)
{
	this->setStyleSheet("QPushButton {border : 0; background: transparent}");	
	this->setIconSize(image.rect().size());
	QObject::connect(this, &QPushButton::clicked, [=]()->void{ Luau::callbackScript(luaScript); });
	
	this->image = image;
	this->parent = parent;
	
	parent->addAWidget(widget, this);
	
	

}




Window::Pane::Pane(QWidget *parent) : QLabel(parent)
{
	
	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, Qt::white);
	this->setPalette(palette);	
	setAutoFillBackground(true);
	
	setContentsMargins(0, 0, 0, 0);
	
}
	
	
void Window::Pane::resizeEvent(QResizeEvent* event)
{
   QLabel::resizeEvent(event);
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
    sizes.append(0.8 * splitter->sizeHint().width());
    sizes.append(0.2 * splitter->sizeHint().width());
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
    
	
	QImage image = io->getImage(imageID);
	
	QLabel *widget = new QLabel();
	widget->setPixmap( QPixmap::fromImage(image) );

	pane->layout()->addWidget(widget);
	
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
