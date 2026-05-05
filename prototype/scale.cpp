#include "counter.h"
#include "overlay.h"
#include "scale.h"
#include "io.h"
#include "window.h"


extern IO *io;



int Scale::rotation = 0;
float Scale::minScaleGrid = 0.4;



Scale::Scale()
{
}


QImage& Scale::getScaledImage(std::string str)
{
	return _scaledResources[str].image;
}


QSize Scale::getScaledSize(std::string str)
{
	return _scaledResources[str].size;
}


void Scale::rotate(Counter *counter, int degree, int &x, int &y)
{
	
	
	float rad = degree * (M_PI/180);
				
	float s = sin(rad);
	float c = cos(rad);

	
	QSize baseSize = io->getSize(Window::getInstance("main")->frame->backgroundID);

	
	baseSize *= Window::getInstance(counter->state.tag.c_str())->frame->scaleFraction;
	

	int cx = std::round((float)baseSize.width() / 2.0);
	int cy = std::round((float)baseSize.height() / 2.0);
	
	
	
	x = x - cx;
	y = y - cy;
	
	
	
	x += counter->scaledBuffer.width() / 2.0;
	y += counter->scaledBuffer.height() / 2.0;
	

	
	
	float nx = x * c - y * s;
	float ny = x * s + y * c;
	
	
	
	QSize size = getScaledSize(Window::getInstance("main")->frame->backgroundID);
	
	cx = std::round((float)size.width() / 2.0);
	cy = std::round((float)size.height() / 2.0); 
	 
	
	x = nx + cx;
	y = ny + cy;
	
	
	
	x -= counter->scaledBuffer.width() / 2.0;
	y -= counter->scaledBuffer.height() / 2.0;
	
	
}


void Scale::unrotate(Counter *counter, int degree, int &x, int &y)
{
	
	
	float rad = degree * (M_PI/180);
				
	float s = sin(rad);
	float c = cos(rad);


	
	
	QSize size = getScaledSize(Window::getInstance("main")->frame->backgroundID);
	
			   
	int cx = std::round((float)size.width() / 2.0);
	int cy = std::round((float)size.height() / 2.0);

	x = x - cx;
	y = y - cy;
	
	
	
	x += counter->scaledBuffer.width() / 2.0;
	y += counter->scaledBuffer.height() / 2.0;

	

	
	float nx = x * c - y * s;
	float ny = x * s + y * c;
	
	
	QSize baseSize = io->getSize(Window::getInstance("main")->frame->backgroundID);

	
	baseSize *= Window::getInstance(counter->state.tag.c_str())->frame->scaleFraction;
	

	cx = std::round((float)baseSize.width() / 2.0);
	cy = std::round((float)baseSize.height() / 2.0);
	
	
	
	x = nx + cx;
	y = ny + cy;
	
	
	x -= counter->scaledBuffer.width() / 2.0;
	y -= counter->scaledBuffer.height() / 2.0;
	
	
}


void Scale::turn(int degree, int &x, int &y)
{
	
	float rad = degree * (M_PI/180);
				
	float s = sin(rad);
	float c = cos(rad);
	
	
	float nx = x * c - y * s;
	float ny = x * s + y * c;
	

	
	x = nx;
	y = ny;
	
}


CentralFrame::Point Scale::getScaleRotateCoordinate(Counter *counter, int x, int y)
{
	
	CentralFrame::Point point = {x, y};
	
	//point = point * Scale::scaleFraction;
	point = point * Window::getInstance(counter->state.tag.c_str())->frame->scaleFraction;
	
	if (Scale::rotation != 0)
		rotate(counter, Scale::rotation, point.x, point.y);
		
	
	return point;
}


void Scale::resourceScaleRotate(std::string tag, std::string name)
{
	
	QImage image = io->getImage(name);
	QImage imageScaled;
	QPixmap overlayScaled;
	
	
    if (Scale::rotation != 0)
		image = image.transformed(QTransform().rotate(Scale::rotation));
	
	
	QSize size(std::round(image.width() * Window::getInstance(tag.c_str())->frame->scaleFraction), 
			   std::round(image.height() * Window::getInstance(tag.c_str())->frame->scaleFraction));
	
	imageScaled = image.scaled( size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	

	
	_scaledResources[name].image = imageScaled;
	_scaledResources[name].size = QSize(imageScaled.width(), imageScaled.height());
	
	
	if (tag == "main")
	{
		Window::getInstance("main")->container->setFixedSize(getScaledSize(Window::getInstance("main")->frame->backgroundID));
		Window::getInstance("main")->frame->setFixedSize(getScaledSize(Window::getInstance("main")->frame->backgroundID));
		Overlay::overlay->setFixedSize(getScaledSize(Window::getInstance("main")->frame->backgroundID));
	}
	
	
}
