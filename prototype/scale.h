#ifndef SCALE_H
#define SCALE_H

#include "io.h"
#include "frame.h"


#include <string>


class Scale
{
	
	public:
	
		Scale();
		
			
		static float scaleFraction;
		static int rotation; 
		
		QImage& getScaledImage(std::string str);
		QSize getScaledSize(std::string str);
		
		void rotate(Counter *counter, int degree, int &x, int &y);
		void unrotate(Counter *counter, int degree, int &x, int &y);
		void turn(int degree, int &x, int &y);
		CentralFrame::Point getScaleRotateCoordinate(Counter *counter, int x, int y);

		void resourceScaleRotate(std::string id);
		
	

	private:
		
		std::map<std::string, IO::Resource> _scaledResources;	
		
};



#endif // SCALE_H
