#include <iostream>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <QtSvg>


#include "io.h"
#include "scale.h"
#include "luau.h"
#include "settings.h"
#include "toolbar.h"
#include "overlay.h"




using namespace std;
using namespace std::filesystem;


std::fstream IO::fs;

 
unsigned long long IO::_ownershipKey;
bool IO::stepping;
bool IO::recording;



	
void IO::close()
{
	Luau::deleteAll();
	Luau::resetState();
	Luau::logReset();
	Window::getInstance("main")->frame->closeAllOpenStacks();
	
	ToolBar::getInstance("main")->enabled("__forward", false);
	ToolBar::getInstance("main")->enabled("__end", false);
}
	
	
IO::LoadGame::LoadGame(std::string fileName)
{	
	
	bool logfile = false;
	
	
	try
	{
		fs.exceptions(std::ios_base::badbit);
				
		fs.open(fileName, ios::binary | ios::in);
	
		if (fs.is_open()) 
		{
			
			
				
			// read file build table
			while (fs)
			{	
				
				// read key
				std::string key;
							
				if (!std::getline(fs, key, '\0'))
					break;
					
				if (key == "turn")
				{
					// read turn
					Luau::Turn turn;
					 					
					fs.read(reinterpret_cast<char*>(&turn.turn), sizeof turn.turn); 					
					fs.read(reinterpret_cast<char*>(&turn.phase), sizeof turn.phase);
					
					Luau::setTurn(turn);
				}
					
					
				if (key == "deck")
				{
					// read number of keys in this table
					std::size_t size; 
					
					fs.read(reinterpret_cast<char*>(&size), sizeof size);
					
					if (size > 0)
					{	
					
						// read table
						Counter::Table table = loadTable(size);
					
						Luau::loadDeck(table);
						
					}
					
				}		
				
					
				if (key == "counter")
				{	
										
					// read number of keys in this table
					std::size_t size; 
					
					fs.read(reinterpret_cast<char*>(&size), sizeof size);
				
					// read ownershipField
					unsigned long long f;
					
					fs.read(reinterpret_cast<char*>(&f), sizeof f);
					
					// read rights
					Settings::OwnershipRights r;
					
					fs.read(reinterpret_cast<char*>(&r), sizeof r); 
					
					
					if (size > 0)
					{	
					
						// read table
						Counter::Table table = loadTable(size);
					
					
						int id = Luau::loadCounter(table);
					
						Counter::counters[id]->setOwnershipField(f);				
						Counter::counters[id]->setRights(r);
						
					}
						
				}
				
				
				// read log (if any)
				if (key == "log")				
				{
					
					// this is a log file
					logfile = true;
					
					
					std::size_t size;
					fs.read(reinterpret_cast<char*>(&size), sizeof size);
					
					
					if (size > 0)
					{	
					
						// read table
						Counter::Table table = loadTable(size);
					
						Luau::loadLog(table);
						
					}
				}
				
				
			}
				
			
			
	
			
			if (Settings::playerSide == "")
			{
				Settings::playerSide = findSide();
				Luau::updateSide(Settings::playerSide.c_str());
			}
			
			
			if (logfile)
			{
				ToolBar::getInstance("main")->enabled("__forward", true);
				ToolBar::getInstance("main")->enabled("__end", true);
				ToolBar::getInstance("main")->enabled("__abort", true);
				Counter::setDisabled(true);
				IO::stepping = true;				
			}
				
				
				
			// note: close stream is done in the destructor
			
			
		}
			
	}
	catch (const ifstream::failure& e)
	{
		std::cout << e.what() << std::endl;
	}

}



Counter::Table IO::LoadGame::loadTable(std::size_t keys)
{
	
	
	Counter::Leftside left;
	Counter::Rightside right;
	Counter::Table table;
	
	
	
	
	for (std::size_t i = 0; i < keys; i++)
	{
		// key
		
		char type;
	
		if (!fs.read(reinterpret_cast<char*>(&type), 1))
			return table;
		
			
		if ((int)type == stringType)
		{
			std::string key;
			std::getline(fs, key, '\0');
			left = key;
		}
		else
			if ((int)type == intType)
			{
				int key;
				fs.read(reinterpret_cast<char*>(&key), sizeof(key));			
				left = key;
			}
			
		
		
		
		// value
		
		fs.read(reinterpret_cast<char*>(&type), 1);
		
		
		if ((int)type == boolType)
		{
			char value;
			fs.read(reinterpret_cast<char*>(&value), 1);
			if ((bool)value)
				right = true; 	
			else
				right = false; 
		}
		else
			if ((int)type == stringType)
			{
				std::string value;
				std::getline(fs, value, '\0');
				right = value;
			}
			else
				if ((int)type == doubleType)				
				{
					double value;
					fs.read(reinterpret_cast<char*>(&value), sizeof value);
					right = value;
				}
				else
					if ((int)type == tableType)
					{
						std::size_t s;
						fs.read(reinterpret_cast<char*>(&s), sizeof s);	
						right = loadTable(s);
					}


		
		table[left] = right;
	
	}
	
	
	return table;
	
	
}




QString activeDirectory = QDir::homePath() + "/Documents";


void IO::loadGame()
{
	
	// no undo
	
	QString fileName = QFileDialog::getOpenFileName(nullptr, "Open Game",
													activeDirectory,
													"Load Files (*.vsav *.vlog);;All Files(*.*)");
	
	
	if (!fileName.isNull())
	{
		close();		
	
		LoadGame *load = new LoadGame(fileName.toStdString());	
		delete load;
		
		Counter::resetId();
		Counter::resetZorder();
		
		Counter::setGUI();
		
	}
	
	
}


void IO::loadSetUp(string filename)
{
	QFileInfo fileInfo(QString::fromStdString(filename));

	QString base = fileInfo.baseName();
	

	
	string completeFileName = "./setups/" + base.toStdString() + ".vsav";
	
	LoadGame *load = new LoadGame(completeFileName);	
	delete load;
	
	Counter::resetId();
	Counter::resetZorder();
	
	Counter::setGUI();
	
}





IO::IO()		
{
	
	load_resources("./__images");
	load_resources("./images");
	
	IO::stepping = false;
	IO::recording = false;
	
	
	// NOTE!! _ownershipKey must be loaded from a user profile, not set like this
	
	_ownershipKey = 0xc2158b49;		// 32-bit prime
									// openssl prime -generate -bits 32 -hex
									
	//_ownershipKey = 0xddb49da1;
									
	
}



QImage& IO::getImage(string str)
{	
	return _resources[str].image;
}


QSize IO::getSize(string str)
{
	return _resources[str].size;
}

bool IO::isResource(string str)
{
	if (_resources.find(str) == _resources.end()) 
		return false;
		
	return true;	
}


unsigned long long IO::getKey()
{
	return _ownershipKey;
}


std::string IO::findSide()
{
	
	std::string side = "";
	
	for (auto obj = Counter::counters.begin(); obj != Counter::counters.end(); ++obj)
	{
		Counter *counter = obj->second;
		Counter::Table table = obj->second->table;
		
		if (table.find("Side") != table.end())
		{
			unsigned long long field = counter->getOwnershipField();
			
			if (field != 0)
				if (field % getKey() == 0)
				{
					side = std::get<std::string>(table["Side"]);
					return side;
				}
		}
		
	}
	
	return side;
	
}

//QSvgRenderer
//QSvgWidget

void IO::load_resources(string directory)
{  
	
	try
	{
		
		for (recursive_directory_iterator i(directory), end; i != end; ++i)
		{		 
			if (!is_directory(i->path()))
			{
				string str = i->path().parent_path().string();
				
				// convert back-slashes to fore-slashes
				std::replace(str.begin(), str.end(), '\\', '/');	
				
				str = str + "/" + i->path().stem().string();
				
				string base = directory + "/";
				string substr = str.substr(base.length());
				printf("loaded %s\n", substr.c_str());
				
				string filename = string(i->path().relative_path().generic_string());
	
				
				
				QImageReader reader(QString::fromStdString(filename));
								
				_resources[substr].size = reader.size();
			
				
				/*
				QImage image(reader.size(), QImage::Format_ARGB32);
				
				if (i->path().extension() == ".svg")
				{
					QSvgRenderer renderer(QString(i->path().c_str()));
					image.fill(Qt::transparent);
					QPainter *painter = new QPainter(&image);
						renderer.render(painter);
					delete painter;
				}
				else
				*/ 
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



void IO::transfer_resource_keys()
{
	for (auto resource = _resources.begin(); resource != _resources.end(); ++resource)
		Luau::setResourceKey(resource->first.c_str());
}



void IO::saveTable(Counter::Table table)
{
	for (auto obj = table.begin(); obj != table.end(); ++obj)
	{
		std::visit(
			Overload{
				[] (int k) { fs.write(reinterpret_cast<const char*>(&intType), 1);
							 fs.write(reinterpret_cast<const char*>(&k), sizeof k); },
				[] (std::string k) { fs.write(reinterpret_cast<const char*>(&stringType), 1);
								     fs.write(k.c_str(), k.size() + 1); }
			},
			obj->first
		);
		std::visit(
			Overload{
				[] (double k) { fs.write(reinterpret_cast<const char*>(&doubleType), 1);
								fs.write(reinterpret_cast<const char*>(&k), sizeof k); },
				[] (bool k) { fs.write(reinterpret_cast<const char*>(&boolType), 1);
							  fs.write(reinterpret_cast<const char*>(&k), 1); },				
				[] (std::string k) { fs.write(reinterpret_cast<const char*>(&stringType), 1);
									 fs.write(k.c_str(), k.size() + 1); },
				[&] (Counter::Table k) { fs.write(reinterpret_cast<const char*>(&tableType), 1);
									     std::size_t s = k.size();
										 fs.write(reinterpret_cast<const char*>(&s), sizeof s);
										 saveTable(k); }
			},
			obj->second
		);	
	}
}




QString IO::saveGame(QString saveAs, QString saveTo, QString suffix)
{
	
	QFileDialog dialog = QFileDialog(nullptr, saveAs,
									 activeDirectory,
									 saveTo);
										 
	dialog.setDefaultSuffix(suffix);
	dialog.setAcceptMode(QFileDialog::AcceptSave);
										 
	QStringList fileNames;
	
	if (dialog.exec() == QFileDialog::Accepted)
	{
		
		fileNames = dialog.selectedFiles();
		
		if (fileNames.count() == 0)
			return "";
		
		
		QFileInfo fileInfo(fileNames[0]);
		
		QDir dir = fileInfo.absoluteDir();
		
		activeDirectory = dir.absolutePath();
		
		
		
		
		fs.open(fileNames[0].toStdString(), ios::binary | ios::out);
		
		
		
		
		// save turn and phase
		
		std::string key = "turn";
		fs.write(key.c_str(), key.size() + 1);
		
		Luau::Turn turn = Luau::getTurn();
		
		fs.write(reinterpret_cast<const char*>(&turn.turn), sizeof turn.turn);
		fs.write(reinterpret_cast<const char*>(&turn.phase), sizeof turn.phase);
        
		
		
		// save decks
		
		Counter::Table keys = Luau::getDecks();
		
		
		for (auto const& [k, v] : keys)
		{
			std::string key = "deck";
			fs.write(key.c_str(), key.size() + 1);
			
			
			std::string index = std::get<std::string>(v);
			
			
			Counter::Table table = Luau::getDeck(index.c_str());
			
			std::size_t s = table.size();
			fs.write(reinterpret_cast<const char*>(&s), sizeof s);		
			
			
			saveTable(table);
			
		}
		
		
		
		
		// save counters
		
		
		for (auto obj = Counter::counters.begin(); obj != Counter::counters.end(); ++obj)
		{
			
			decltype(obj->second) counter = obj->second;
			
			
			std::string key = "counter";
			fs.write(key.c_str(), key.size() + 1);
		
			
			Counter::Table table = Luau::getTraits("", counter->id);
			
			if (!table.empty())
			{
			
				// save top level table size
				std::size_t s = table.size();
				fs.write(reinterpret_cast<const char*>(&s), sizeof s);
				
				// save the ownershipField
				unsigned long long f = counter->getOwnershipField();
				Settings::OwnershipRights r = Settings::myOwnershipRights;
				
				// case where opponent has dragged your conter on board
				// set your OwnershipField and rights
				if (f == 0)
					if (table.find("Side") != table.end())
						if (std::get<std::string>(table["Side"]) == Settings::playerSide)
						{
							counter->setOwnershipField(IO::getKey() * 0xef06eea1);
							f = counter->getOwnershipField();						
						}
				fs.write(reinterpret_cast<const char*>(&f), sizeof f);
				
				// save rights			
				fs.write(reinterpret_cast<const char*>(&r), sizeof r); 
				
				
				saveTable(table);
			}						
				
		}
		
		
		
		fs.close();
		
		return fileNames[0];
		
	}
	
	
	return nullptr;										
			
}




void IO::saveLog(QString file)
{
	
	if (file.isNull() || file.isEmpty())
		return;
		
	// append
	fs.open(file.toStdString(), ios::binary | ios::out | ios::app);
	
	
	// get log range
	int savedPointer = 0, stagePointer = 0;
	
	Luau::getRange(savedPointer, stagePointer);
	
	
	
	for (int i = savedPointer; i < stagePointer; i++)
	{
		
		std::string key = "log";
		fs.write(key.c_str(), key.size() + 1);
		
		
		Counter::Table table = Luau::getTraits("State", i+1);
	
		// save top level table size
		std::size_t s = table.size();
		fs.write(reinterpret_cast<const char*>(&s), sizeof s);
		
		
		saveTable(table);
	}
	
	fs.close();
	
}
