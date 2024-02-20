#include <iostream>
#include <fstream>
#include <cstring>


#include <assert.h>
#include "lua.h"
#include "lualib.h"
#include "luacode.h"


#include "luau.h"
#include "counter.h" 


using namespace std;

struct aScript
{
	std::string chunkName;
	std::string chunk;
};

typedef struct std::vector<aScript> Scripts;



extern CentralFrame *mapFrame;




static lua_State* L;



Scripts scripts;




Luau::Luau()
{
	
}



int readScript(string fileName, string *content)
{
	
	ifstream in(fileName.c_str());

	string line;
	
	if (in.is_open()) 
	{
		while (getline(in,line))		
			content->append(line + "\n");
		
		return 0; 
	}         
	else
		return 1;
	
}


void defineResources()
{
	// give each resorce file an id 
	// the case of similar names in different subdirectories is not dealt with here
	
	
	
}


void defineScripts()
{
	
	scripts.clear();
	
	std::vector<std::string> scriptNames = {"module", "counter", "traits", "state"};
	
	for (auto name : scriptNames)
	{
		
		string content;
		
		int r = readScript("./" + name + ".luau", &content);
		
		if (r)
			continue;
		
		
		
		aScript script = aScript{ name, content };	
		scripts.push_back(script);
	}
	
}


void Luau::compileScript()
{	
	char*  bytecode;
	size_t bytecodeSize = 0;
	
	defineScripts();
	
	
	for (auto &element : scripts) 
	{

		bytecode = luau_compile(element.chunk.c_str(), element.chunk.length(), NULL, &bytecodeSize);
		assert(luau_load(L, element.chunkName.c_str(), bytecode, bytecodeSize, 0) == 0);
		free(bytecode);	
		
		// set global to make other script files use this script file
		lua_setglobal(L, element.chunkName.c_str());
		
		
		cout << "loaded " + element.chunkName << endl;
	}
}




extern "C" {
	
	
	
	static int setCounterDragAlpha(lua_State *L)
	{	
		
		if (lua_isnumber(L, -1))
		{	
			Counter::alpha = lua_tonumber(L, -1);	
			lua_pop(L, 1);
		}
		
		
		return 0;
	}
	
	
	
	static int create_class(lua_State *L)
	{	
		
		const char *id = lua_tostring(L, -1);	   
		size_t length = lua_strlen(L, -1);         
		assert(id[length] == '\0');
		assert(strlen(id) <= length);
		lua_pop(L, 1);
		(void)new Counter(id);
		
		return 0;
	}
	
	
	static int create_class_copy(lua_State *L)
	{	
		
		int y = lua_tonumber(L, -1);
		int x = lua_tonumber(L, -2);
		int zorder = lua_tonumber(L, -3);
		int degrees = lua_tonumber(L, -4);
		const char *image = lua_tostring(L, -5);
		int id = lua_tonumber(L, -6);
		lua_pop(L, 6);
		printf("id image degrees zorder %d %s %d %d %d %d\n", id, image, degrees, zorder, x, y);
		(void)new Counter(id, image, degrees, zorder, x, y);
		
		return 0;
	}
	
	
	
	static int delete_class(lua_State *L)
	{	
		if (lua_isstring(L, -1))
		{	
			const char *id = lua_tostring(L, -1); 
			lua_pop(L, 1);
			Counter *found = Counter::findObj(id);
			if (found)	
			{			
				delete found;
				Counter::setGUI();
			}
		}
		
		
		return 0;
	}
	
	
	
	static int set_counter_image(lua_State *L)
	{		
		if (lua_isstring(L, -1))
		{	
			const char *image = lua_tostring(L, -1);
			const char *id = lua_tostring(L, -2);
			lua_pop(L, 2);
			Counter *found = Counter::findObj(id);
			if (found)
			{
				found->state.image = image;
				found->setImage();
			}
		}
		
		
		return 0;
	}
	
	
	static int set_moved_status(lua_State *L)
	{
			
		bool moved = lua_toboolean(L, -1);
		int y = lua_tonumber(L, -2);
		int x = lua_tonumber(L, -3);
		const char *id = lua_tostring(L, -4);
		lua_pop(L, 4);
		Counter *found = Counter::findObj(id);
		if (found)
		{
			Counter::moveImage_x = x;
			Counter::moveImage_y = y;
			found->state.moved = moved;			
			found->setImage();
		}	
		
		return 0;
	}
	
	static int rotate_counter(lua_State *L)
	{		
			
		int degrees = lua_tonumber(L, -1);
		const char *id = lua_tostring(L, -2);
		lua_pop(L, 2);
		Counter *found = Counter::findObj(id);
		if (found)
		{
			found->state.degrees = degrees;
			found->setImage();
		}
		
		return 0;
	}
	
	static int updateState(lua_State *L)
	{		
		
		const char *image	= lua_tostring(L, -1);	
		int degrees 		= lua_tonumber(L, -2);	
		bool moved 			= lua_toboolean(L, -3);
		int zorder			= lua_tonumber(L, -4);
		int y 				= lua_tonumber(L, -5);
		int x 				= lua_tonumber(L, -6);
		const char *id 		= lua_tostring(L, -7);
		lua_pop(L, 7);

		
		Counter *found = Counter::findObj(id);
		if (found)
		{
			found->state.x = x;
			found->state.y = y;
			found->zorder = zorder;
			found->state.moved = moved;
			found->state.degrees = degrees;
			found->state.image = string(image);
		}
		else
			printf("error\n");
			
		
		return 0;
	}
	
	static int setGUI(lua_State *L)
	{		
		
		Counter::setGUI();
		
		return 0;
	}
	
	
}	// extern "C"



Luau::PopupEntries entries;


void getEntries()
{
	
	lua_pushnil(L);
	
	Luau::Popupentry e = {"", ""};
	
	while(lua_next(L, -2) != 0) 
	{
		
		// skip number keys
		if (lua_isnumber(L, -2))
		{
			lua_pop(L, 1);
			continue;
		}
		
		
		if (lua_isstring(L, -1))
		{	
			
			std::string s(lua_tostring(L, -2));
			
			if (s == "menuname" || s == "menuclick")			
			{
				if (s == "menuname") 
					e.entryname = std::string(lua_tostring(L, -1));
				if (s == "menuclick")
					e.entryaction = lua_tostring(L, -1);
				if (!e.entryname.empty() && !strlen(e.entryaction) == 0)
				{
					entries.push_back(e);
					e = {"", ""};
				}
			}
		}	
		else 
		if (lua_istable(L, -1))
			getEntries();
			
			
		lua_pop(L, 1);
		
	}
		
	
}


    
Luau::PopupEntries Luau::getTraits(const char *window, const char *id)
{
	
	lua_getglobal(L, "getTraits");
	lua_pushstring(L, window);
	lua_pushstring(L, id);
	lua_pcall(L, 2, 1, 0);
	
	
	entries.clear();
	
	getEntries();
	
	lua_pop(L, 1);
	
	
	
	
	return entries;
	
	
}



void Luau::doAction(const char *window, const char *id, const char *name)
{
	
	lua_getglobal(L, "action");
	lua_pushstring(L, window);
	lua_pushstring(L, id);
	lua_pushstring(L, name);
	lua_pcall(L, 3, 0, 0);
	
}




void Luau::doEvent(const char *eventName, const char *id, const char *trait, const char *key, int value)
{
	
	lua_getglobal(L, "events");
	lua_pushstring(L, eventName);
	lua_pushstring(L, id);
	lua_pushstring(L, trait);
	lua_pushstring(L, key);
	lua_pushnumber(L, value);
	
	
	lua_pcall(L, 5, 0, 0);
	
}


void Luau::doCreate(const char *fromId, const char *trait, const char *toId, int degrees, int zorder, int x, int y)
{
	lua_getglobal(L, "create");
	lua_pushstring(L, fromId);
	lua_pushstring(L, trait);
	lua_pushstring(L, toId);
	lua_pushnumber(L, degrees);
	lua_pushnumber(L, zorder);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	
	
	lua_pcall(L, 7, 0, 0);
	
}


void Luau::undo()
{
	
	lua_getglobal(L, "undo");
	lua_pcall(L, 0, 0, 0);
	
}



void Luau::redo()
{
	
	lua_getglobal(L, "redo");
	lua_pcall(L, 0, 0, 0);
	
}



void Luau::copyCounter(const char *fromId, const char *toId, int degrees, int zorder, int x, int y)
{
	
	lua_getglobal(L, "copyCounter");
	lua_pushstring(L, fromId);
	lua_pushstring(L, toId);
	lua_pushnumber(L, degrees);
	lua_pushnumber(L, zorder);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pcall(L, 6, 0, 0);
	
}



void Luau::startVM()
{
	
	L = luaL_newstate();
	luaL_openlibs(L);
	
	
	(void)std::setlocale(LC_NUMERIC, "en_US.UTF-8");
	
	compileScript();
	
	
	lua_pushcfunction(L, setCounterDragAlpha, "setCounterDragAlpha");
	lua_setglobal(L, "setCounterDragAlpha");
	
	lua_pushcfunction(L, create_class, "create_class");
	lua_setglobal(L, "create_class");
	
	lua_pushcfunction(L, create_class_copy, "create_class_copy");
	lua_setglobal(L, "create_class_copy");
	
	lua_pushcfunction(L, delete_class, "delete_class");
	lua_setglobal(L, "delete_class");
	
	lua_pushcfunction(L, set_counter_image, "set_counter_image");
	lua_setglobal(L, "set_counter_image");
	
	lua_pushcfunction(L, set_moved_status, "set_moved_status");
	lua_setglobal(L, "set_moved_status");
	
	lua_pushcfunction(L, rotate_counter, "rotate_counter");
	lua_setglobal(L, "rotate_counter");
	
	lua_pushcfunction(L, updateState, "updateState");
	lua_setglobal(L, "updateState");
	
	lua_pushcfunction(L, setGUI, "setGUI");
	lua_setglobal(L, "setGUI");
	
	lua_getglobal(L, "module");
	lua_pcall(L, 0, 0, 0);
	
		
}
	
	
void Luau::closeVM()
{
		
	lua_close(L);

}
