#ifndef SCALE_H
#define SCALE_H

#include "io.h"


#include <string>


class Scale
{
	
	public:
	
		static constexpr float ratio = 2.0/3.0;
		static constexpr int rotation = 0; 
		
		Scale();
		
		QImage& getScaledImage(std::string str);
		QSize getScaledSize(std::string str);

		void resourceScaleRotate(std::string id);
		void coordinatesScaleRotate();
		
		
		
	private:
		
		std::map<std::string, IO::Resource> _scaledResources;	
		
};



#endif // SCALE_H
