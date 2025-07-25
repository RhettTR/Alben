#include <stack>

#include "window.h"
#include "io.h"
#include "luau.h"
#include "scale.h"


extern IO *io;


Window *getInstance(const char *instance);
std::map<std::string, Window *> Window::instances;





Window::Window(QWidget *parent, QString title, std::string tag) : QMainWindow(parent)
{
	
	this->setWindowTitle(title);
    
    
    instances[tag] = this;
    this->tag = tag;
    this->factor = 1.0;
    
    
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



void Window::createToken(int id, Counter::Table *state)
{
	(void)new Token(this, id, state);
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
	

	((Frame *)this->frame)->height = h;
	

	
	// resize if 100% too big, 400 is height of window minus scrollbar 	
	if (h > 400)
	{
		((Frame *)this->frame)->height = 400;
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


void Window::addAToken(int key,  Token *value)
{
	tokens[key] = value;
	setWidgets();
}



inline std::stack<Window::Token *> setAsDeleted;


void Window::removeAToken(int key)
{
	
		
	// a very ugly hack; need to return to this
	
	Token *token = tokens[key];
	
	setAsDeleted.push(token);
	
	token->setVisible(false);
			

	
	tokens.erase(key);
	
}






void Window::setWidgets(int height)
{
	
	float fraction;
	
	if (widgets.size() == 0 && tokens.size() == 0)
		return;
		
	
	if (this->tag == "main")
		fraction = Scale::scaleFraction;
	else
	if (height == 0)
		fraction = this->factor;	
	else
	{	
		
		if (this->size().height() == 0)
			return;	
					
	
		this->factor = (float)height / (float)this->frame->size().height();
		
		
		if (this->factor > 1.0)
			this->factor = 1.0;
			
		
		fraction = this->factor;	
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
			
			label->move((int)(label->x * fraction), (int)(label->y * fraction));		
			
		}	
	
	}
	
	
	// "delayed" deleting - ugly hack 

	while (!setAsDeleted.empty()) 
	{
        delete setAsDeleted.top();
        setAsDeleted.pop();
    }
		
		
		
	for (Tokens::iterator it = tokens.begin(); it != tokens.end(); it++)
	{
		
		QImage baseImage = io->getImage(it->second->state.image);
		
		
		
		
		
		int w = (int)(it->second->baseWidth * fraction);
		int h = (int)(it->second->baseHeight * fraction);
	
		
		it->second->resize(w,h);
		
			
		
		QImage image = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
		
		
		image.fill(Qt::transparent);
		
		
		QPainter *paint = new QPainter(&image);
		paint->setRenderHint(QPainter::Antialiasing);
		paint->setRenderHint(QPainter::TextAntialiasing);
		paint->setRenderHint(QPainter::SmoothPixmapTransform);
		paint->scale((qreal)fraction, (qreal)fraction);			
		paint->drawImage(it->second->margin, it->second->margin, baseImage);
								
		delete paint;
		
		
		

		
		if (it->second->overlays != nullptr)		
		{	
			
			for (auto const &label : (*it->second->overlays))
			{
				
				Counter::Table trait = std::get<Counter::Table>(label.second);
			
				
				try
				{
					if (std::get<bool>(trait["apply"]))
					{
						int x = (int)std::get<double>(trait["x"]);
						int y = (int)std::get<double>(trait["y"]);
						
						QFont font;
						font.setFamily(QString::fromStdString(std::get<std::string>(trait["font"])));
						font.setPixelSize((int)std::get<double>(trait["size"]));
						font.setWeight(QFont::Bold);
						
						std::string color = std::get<std::string>(trait["color"]);


						const QString text = QString::fromStdString(std::get<std::string>(trait["text"]));
						
						QPainter *paint = new QPainter(&image);
						paint->setFont(font);
						paint->setPen(QString::fromStdString(color));
						paint->drawText(x, y, width(), it->second->margin, Qt::AlignVCenter | Qt::AlignHCenter, text);
						delete paint;
					}
					
				}
				catch (std::bad_variant_access const& e)
				{
					std::cout << e.what() << std::endl;
				}	
				
			}
					
		}
		
	
		

		it->second->setPixmap(QPixmap::fromImage(image));
		
		it->second->move((int)(it->second->state.x * fraction), (int)(it->second->state.y  * fraction));
			
		
	}	
	
}


void Window::resizeEvent(QResizeEvent* event)
{
	
	QMainWindow::resizeEvent(event);
	

	setWidgets(event->size().height());
	
	event->accept();
	
}






Window::Token::Token(Window *window, int id, Counter::Table *table) : QLabel(window->frame)
{	

	this->setAutoFillBackground(true);
	this->setScaledContents(true);
	this->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
	this->setStyleSheet("background: transparent");
	
	this->setStyleSheet("border-style: none");    
	this->counterId	= id;
	this->name = std::to_string(id);
	

	this->margin = 9;  
	this->window = window;
		
	
	try
    {
		Counter::Table images = std::get<Counter::Table>(std::get<Counter::Table>((*table)["Image"])["images"]);	
		this->state.image = std::get<std::string>(images[1]);
		this->state.id = id;
		
		this->baseWidth = io->getSize(this->state.image).width() + 2 * this->margin;
		this->baseHeight = io->getSize(this->state.image).height() + 2 * this->margin;	
		this->resize(baseWidth, baseHeight);
		this->setVisible(true);
		
		
		
		
		this->state.x = (int)std::get<double>((*table)["x"]);	 	
		this->state.y = (int)std::get<double>((*table)["y"]);	   
	    
		if ((*table).find("Overlays") != (*table).end())		
		{	
			Counter::Table *overlays = new Counter::Table();
				
			(*overlays) = std::get<Counter::Table>((*table)["Overlays"]);
			this->overlays = overlays;
		}
		else
			this->overlays = nullptr;
			
			
		window->addAToken(this->state.id, this);
		
		
		//window->removeAToken(this->state.id);
		
			
			
	}
	catch (std::bad_variant_access const& e)
    {
        std::cout << e.what() << std::endl;
    }
    
 
	
}


Window::Token::~Token() 
{	
	
	std::map<int, Token *>::iterator iter = this->window->tokens.find(this->state.id);
	
	if (iter != this->window->tokens.end())
		window->tokens.erase(iter);
	
}



static Luau::PopupEntries popupentries;


void Window::Token::showRightClickMenu()
{
	
	QMenu myMenu(this);
		
	if (this->actions().isEmpty())
	{
		popupentries.clear();
		
		const char *tag = this->window->tag.c_str();
		const char *token = std::to_string(this->state.id).c_str();
	
		popupentries = Luau::getTraits(tag, token);
		
	

		for (auto e : popupentries)
		{
			 
			const QString name = QString::fromStdString(e.entryname);
			
			QAction* action = new QAction(name, this);
			
			QVariant v = QVariant(QString(e.entryaction.c_str()));
			action->setData(v);

		   
			this->addAction(action);
			
			QObject::connect( action, &QAction::triggered, this, [=]()->void{ do_activate(action); } );
			
		}
	}
	
	myMenu.addActions(this->actions());
	
	myMenu.exec(QCursor::pos());
}


void Window::Token::do_activate (QAction *action)
{
	QVariant v = action->data();
	QString str = (QString) v.value<QString>();
	QByteArray ba = str.toLocal8Bit();
	const char *entryaction = ba.data();
	const char *window = this->window->tag.c_str();
	std::string id = std::to_string(this->state.id);

			
	Luau::doAction(window, id.c_str(), entryaction);
					
}


void Window::Token::mousePressEvent (QMouseEvent * event) 
{
			
	if (event->button() == Qt::RightButton)
	{
		showRightClickMenu();
		event->accept();
	}
	else 	
		event->ignore();
		
}


void Window::Token::set(std::string text)
{
	this->setText(QString::fromStdString(text));
}


void Window::Token::setImage(std::string image)
{
		
	this->state.image = image;
	
	this->window->setWidgets();	
	
}






Window::Frame::Frame(Window *parent, std::string background) : QFrame(parent)
{
	
	this->window = parent;
	
	if (background[0] == '#')
	{
		// background is a color
		this->setStyleSheet("QFrame { background-color:" + QString::fromStdString(background) + "; }");
	}
	else
	{
		//background is an image
		if (io->isResource(background))
		{
			backgroundImage = io->getImage(background);
			parent->scrollArea->resize(backgroundImage.size()); 
		}
	}
	
	setAcceptDrops(true);
	
}

 
Window::Frame::~Frame()
{
}



 
inline Window::Token *dragged = nullptr;

 
 
void Window::Frame::mousePressEvent(QMouseEvent *event)
{
	
	event->accept();
	
	if (event->button() == Qt::LeftButton)
	{
		
		Token *child = 
			static_cast<Token*>(childAt(event->position().toPoint()));
	
			
		if (child == nullptr)
			return;			
				

		dragged = child;
		
	
		QImage image;
	
		if (event->modifiers() == Qt::ControlModifier)
			image = io->getImage(child->state.image);
		else
			image = child->pixmap().toImage();
		
			
			
		QPixmap img(image.size());
		
		
		
		img.fill(Qt::transparent);		
		QPainter painter;
		painter.begin(&img);
		painter.setRenderHint(QPainter::Antialiasing);
		painter.setRenderHint(QPainter::TextAntialiasing);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		if (event->modifiers() == Qt::ControlModifier)
			painter.scale((qreal)Scale::scaleFraction, (qreal)Scale::scaleFraction);	
		painter.drawImage(child->margin, child->margin, image);			
		painter.end();
		
		
		QByteArray itemData;
		QDataStream dataStream(&itemData, QIODevice::WriteOnly);
		
		
			
		
		QPoint counterOffset = event->position().toPoint() - child->pos(); 
		
		if (event->modifiers() == Qt::ControlModifier)
			// scales up to normal size
			counterOffset /= window->factor;
		
			
	
		dataStream << counterOffset;																
		dataStream << QString::fromStdString(child->state.image);
		dataStream << child->state.id;
				
	
		QMimeData *mimeData = new QMimeData;
		mimeData->setData("application/x-alben-window", itemData);
						
		
			
		
		QDrag *drag = new QDrag(this);
		drag->deleteLater();
		drag->setMimeData(mimeData);
		drag->setPixmap(img);
		if (event->modifiers() == Qt::ControlModifier)
			drag->setHotSpot(counterOffset * Scale::scaleFraction);
		else
			drag->setHotSpot(counterOffset + QPoint(child->margin,child->margin));
			
	
		
		auto result = drag->exec(Qt::CopyAction | Qt::MoveAction);
		
		if (result == Qt::MoveAction) 
		{
			delete child;
		}
		
		
	}
	
}
	

void Window::Frame::dragEnterEvent(QDragEnterEvent *event)
{	
	if (event->mimeData()->hasFormat("application/x-alben-window")) 	
		event->acceptProposedAction();
}


void Window::Frame::dropEvent(QDropEvent *event)
{
	
	if (event->mimeData()->hasFormat("application/x-alben-window"))
	{
		
		QPoint counterOffset;
		
		QByteArray itemData = event->mimeData()->data("application/x-alben-window");
		QDataStream dataStream(&itemData, QIODevice::ReadOnly);
			
		dataStream >> counterOffset;
		
	
		 
		int x = event->position().toPoint().x() - counterOffset.x();
		int y = event->position().toPoint().y() - counterOffset.y();
	

		
		dragged->state.x = (int)((float)x / window->factor);
		dragged->state.y = (int)((float)y / window->factor);
		
		
		
		window->setWidgets(); 
		
		
		
		event->setDropAction(Qt::CopyAction);
		event->accept();			
	}
	
}





void Window::Frame::paintEvent(QPaintEvent *event)
{
	
	QPainter painter(this);
	
	if (!backgroundImage.isNull())
		painter.drawImage(0,0,backgroundImage);
	
}






Window::Label::Label(std::string tag, Window *parent, int x, int y,  int w, int h, std::string resourecName, 
						QString styleSheet) : QLabel((QWidget *)parent->frame), x(x), y(y), w(w), h(h)
{
	
	if (resourecName.empty())
	{
		this->backgroundImage = QImage(w, h, QImage::Format_ARGB32_Premultiplied);	
		this->backgroundImage.fill(Qt::transparent);
	}	
	else
		this->backgroundImage = io->getImage(resourecName);	
		
		
	this->setPixmap(QPixmap::fromImage(backgroundImage));
	
	
	this->resize(this->pixmap().size());
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







