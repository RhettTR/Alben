#include <iostream>
#include <filesystem>


#include "io.h"
#include "scale.h"



using namespace std;
using namespace std::filesystem;




	


IO::IO()		
{
	
	load_resources("./__images");
	load_resources("./images");
	
}



QImage& IO::getImage(string str)
{	
	return _resources[str].image;
}


QSize IO::getSize(string str)
{
	return _resources[str].size;
}




void IO::load_resources(string directory)
{  
	
	try
	{
		
		for (recursive_directory_iterator i(directory), end; i != end; ++i)
		{		 
			if (!is_directory(i->path()))
			{
				string str = i->path().parent_path().string() + "/" + i->path().stem().string();
				
				string base = directory + "/";
				string substr = str.substr(base.length());
				printf("loaded %s\n", substr.c_str());
				
				string filename{i->path().relative_path()};
				
				
				QImageReader reader(QString::fromStdString(filename));
								
				_resources[substr].size = reader.size();
			
				
				
	
				QImage image = reader.read();
								
				_resources[substr].image = image;
				
			}
		}
		
	}
	catch (filesystem_error &e) 
	{
		std::cout << e.code() << '\n';
		std::cout << e.what() << '\n';
	}
			
}
