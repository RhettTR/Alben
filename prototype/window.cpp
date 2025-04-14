#include "window.h"
#include "io.h"
#include "luau.h"


extern IO *io;


Window::Frame *Window::frame;
Window *getInstance(const char *instance);
std::map<std::string, Window *> Window::instances;




Window::Window(QWidget *parent, QString title, std::string name, std::string background) : QMainWindow(parent)
{
	
	this->setWindowTitle(title);
	
	
    this->move(200, 40);
    
    frame = new Frame(this, background);
    
    this->setCentralWidget((QWidget *)frame);
    
    this->instances[name] = this;
    this->name = name;
  
    
	setAcceptDrops(true); 
	
}




Window::Token::Token(int counterId, Counter::Table *table) : QLabel((QWidget *)frame)
{	
		
	this->setStyleSheet("border-style: none");
	this->baseWidth = 72;
	this->baseHeight = 72;
	this->resize(baseWidth, baseHeight);
	this->counterId	= counterId;
	this->setVisible(true);
	
	this->margin = 18;
		
		
	try
    {
		Counter::Table images = std::get<Counter::Table>(std::get<Counter::Table>((*table)["Image"])["images"]);	
		this->state.image = std::get<std::string>(images[1]);
		this->state.id = counterId;
		
		frame->tokens[this->state.id] = this;
		
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
			
			
	}
	catch (std::bad_variant_access const& e)
    {
        std::cout << e.what() << std::endl;
    }
    
    
	

	this->setImage(state.image);	
	this->move(state.x - (this->width() / 2), state.y - (this->height() / 2));
	
	
}


Window::Token::~Token() 
{	
	std::map<int, Token *>::iterator iter = frame->tokens.find(this->state.id);
	
	if (iter != frame->tokens.end() )
		frame->tokens.erase(iter);
}







void Window::Token::setImage(std::string name)
{
		
	int w = baseWidth + 2*margin;
	int h = baseHeight + 2*margin;
	
	
	this->resize(w,h);
	
	QImage baseImage = io->getImage(name);
	
	
	QImage image = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
		
	image.fill(Qt::transparent);
	
	QPainter *paint = new QPainter(&image);			
	paint->drawImage(margin, margin, baseImage);						
	delete paint;
	
	
	if (overlays != nullptr)		
	{	
		
		for (auto const &label : (*overlays))
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
					paint->drawText(x, y, width(), margin, Qt::AlignVCenter | Qt::AlignHCenter, text);
					delete paint;
				}
				
			}
			catch (std::bad_variant_access const& e)
			{
				std::cout << e.what() << std::endl;
			}	
			
		}
				
	}
	

	this->setPixmap(QPixmap::fromImage(image));
	
}


void Window::Token::set(std::string text)
{
	this->setText(QString::fromStdString(text));
}


Window *Window::getInstance(const char *instance)
{
	
	std::string name = std::string(instance);
	
	for ( auto obj = Window::instances.begin(); obj != Window::instances.end(); ++obj  )
	
		if (obj->first == name)
			return obj->second;
			
	return nullptr;
	
}



Window::Frame::Coordiantes coordinates;


Window::Frame::Frame(Window *parent, std::string background) : QFrame((QWidget *)parent)
{
	setAcceptDrops(true);
	
	backgroundImage = io->getImage(background);
	((Window *)this->parent())->resize(backgroundImage.size()); 
}


void Window::Frame::addtoGrid(int x, int y)
{
	coordinates.push_back({x, y});
}

 
 
inline Window::Token *dragged = nullptr;

 
 
void Window::Frame::mousePressEvent(QMouseEvent *event)
{
	
	
	if (event->button() == Qt::LeftButton)
	{
		
		Token *child = 
			static_cast<Token*>(childAt(event->position().toPoint()));
	
			
		if (child == nullptr)
			return;			
				

		dragged = child;
		
		
	
		
		QImage image = child->pixmap().toImage();
		
		
			
		QPixmap img(image.size());
		
		img.fill(Qt::transparent);		
		QPainter painter;
		painter.begin(&img);
		painter.drawImage(0, 0, image); 				
		painter.end();
		
		
		QByteArray itemData;
		QDataStream dataStream(&itemData, QIODevice::WriteOnly);
		
		
		
		QPoint counterOffset = event->position().toPoint() - child->pos();		
		dataStream << counterOffset;

		QMimeData *mimeData = new QMimeData;
		mimeData->setData("application/x-alben-window", itemData);
			
		
		QDrag *drag = new QDrag(this);
		drag->deleteLater();
		drag->setMimeData(mimeData);
		drag->setPixmap(img);
		drag->setHotSpot(counterOffset);
		
		
		
		drag->exec(Qt::MoveAction);
		
		
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
		
		
		int x = event->position().toPoint().x();
		int y = event->position().toPoint().y();
	
		
		
		
		if (coordinates.empty())
			dragged->move(x - counterOffset.x(), y - counterOffset.y());
		else
		{
			Coordinate coordinate;	
			snaptoGrid (x, y, dragged, coordinate);
			dragged->move(coordinate.x - (int)(dragged->width() / 2), 
						  coordinate.y - (int)(dragged->height() / 2));
			Luau::moved(((Window *)this->parent())->name.c_str(), 
						std::to_string(dragged->state.id).c_str(), 
						coordinate.x, 
						coordinate.y);
		}
			
		event->acceptProposedAction();				
	}
	
}



void Window::Frame::snaptoGrid (int x, int y, Token *dragged, Coordinate &coordinate)
{
	

	int minimum_distance = std::numeric_limits<int>::max();
	
	Coordinate minimum;
		
	for (auto i : coordinates)
	{
		// simplification of sqr(dx² + dy²)
		int sum = abs(x - i.x) + abs(y -i.y);
		
		if (sum < minimum_distance)
		{
			minimum_distance = sum;
			minimum.x = i.x;
			minimum.y = i.y;
			
		}
	}
	
	
	if (minimum_distance < std::numeric_limits<int>::max())
	{
		coordinate.x = minimum.x;
		coordinate.y = minimum.y;
	}
	
}



void Window::Frame::paintEvent(QPaintEvent *event)
{
	
	QPainter painter(this);
	
	painter.drawImage(0,0,backgroundImage);
	
}




Window::Label::Label(std::string id, Frame *parent, int x, int y, int w, int h, QString styleSheet) : QLabel((QWidget *)parent->parent())
{
	parent->labels[id] = this;
	
	setStyleSheet(styleSheet);
	setFixedSize(w, h);
	setVisible(true);
	
	move(x, y);
}


void Window::Label::set(std::string text)
{
	this->setText(QString::fromStdString(text));
}


std::string Window::Label::get()
{
	return this->text().toStdString();
}



Window::CheckBox::CheckBox(std::string id, Window *parent, int x, int y, int w, int h, QString text, std::string luaScript, QString styleSheet) : QCheckBox(text, (QWidget *)parent)
{
	parent->frame->checkboxes[id] = this;
	
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
