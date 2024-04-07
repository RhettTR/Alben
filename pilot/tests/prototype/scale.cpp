//#include <math.h>

#include "counter.h"
#include "scale.h"
#include "io.h"


extern IO *io;



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



void Scale::resourceScaleRotate(std::string name)
{
	
	QImage image = io->getImage(name);	
	QImage scaled;
	
	
    if (Scale::rotation != 0)
		image = image.transformed(QTransform().rotate(Scale::rotation));
	
	
	QSize size(image.width() * Scale::ratio, image.height() * Scale::ratio);
	
	scaled = image.scaled( size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	
	_scaledResources[name].image = scaled;
	_scaledResources[name].size = QSize(scaled.width(), scaled.height());
	
}



void Scale::coordinatesScaleRotate()
{
	
	for (auto obj = Counter::counters.begin(); obj !=Counter::counters.end(); ++obj)
	{
		
		int x = obj->second->state.x * Scale::ratio;
		int y = obj->second->state.y * Scale::ratio;
		
		
		if (Scale::rotation != 0)
		{
			
			float rad = Scale::rotation * (M_PI/180);		
			
			
			
			QSize oldSize = io->getSize(CentralFrame::backgroundID);
			
			
			oldSize *= Scale::ratio;
			
			int cx = oldSize.width() / 2.0;
			int cy = oldSize.height() / 2.0;
			
			
			x += obj->second->scaledBuffer.width() / 2.0;
			y += obj->second->scaledBuffer.height() / 2.0;
			
			
			float r = sqrt( pow(abs(cx - x), 2) + pow(abs(cy - y), 2) );
			
			
			float angle = acos( (float)abs(cx - x) / r);
			
			
			
			QSize mapSize = getScaledSize(CentralFrame::backgroundID);
			
			cx = mapSize.width() / 2.0;
			cy = mapSize.height() / 2.0;
				
			
			angle = M_PI - (angle + rad);
			
			
			
			
			x = cx + r * cos(angle) - (obj->second->scaledBuffer.width() / 2.0);
			y = cy - r * sin(angle) - (obj->second->scaledBuffer.height() / 2.0);
			
		
		}
		
		
        obj->second->state.x = x;
        obj->second->state.y = y;
        
		
		
	}
	
	Counter::setGUI();
	
}
