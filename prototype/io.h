#ifndef IO_H
#define IO_H


#include <fstream>
#include <string>
#include <map>

#include <QtWidgets>

#include "counter.h"



class IO
{
	
	public:	
	
		static constexpr int intType = 1;
        static constexpr int stringType = 2;
        static constexpr int boolType = 3;
        static constexpr int doubleType = 4;
        static constexpr int tableType = 5;
        
        
        typedef std::variant<int, std::string, bool, double, Counter::Table> Variant;
        
	
		class LoadGame 
		{
			private:
				Counter::Table loadTable(std::size_t keys);
			public:			
				LoadGame(std::string fileName);				
				virtual ~LoadGame() 
				{
					fs.close();
				}				
		};
			
							      
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
		
		// getter for ownership key
		static unsigned long long getKey();
		
		void saveTable(Counter::Table table);
		void saveGame();
		void loadGame();
		
		static std::string findSide();
		
		
	protected:
			
		static std::fstream fs;	
		
	private:
		
		std::map<std::string, Resource> _resources;
		
		static unsigned long long _ownershipKey;	// holds a 32-bit prime number
		
};


#endif // IO_H
