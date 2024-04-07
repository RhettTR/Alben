#ifndef IO_H
#define IO_H


#include <string>
#include <map>

#include <QtWidgets>




class IO
{
	
	public:					     					      
							      
		IO();
		
		void load_resources(std::string directory);

		
		struct Resource
		{	
			QImage image;
			QSize  size;	
		}; 
		
		
		// getters for resources
		QImage& getImage(std::string str);
		QSize getSize(std::string str);
		
		
	private:
		
		std::map<std::string, Resource> _resources;
		
};


#endif // IO_H
