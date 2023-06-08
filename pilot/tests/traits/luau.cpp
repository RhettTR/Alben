#include <iostream>
#include <fstream>
#include <cstring>


#include <assert.h>
#include "lua.h"
#include "lualib.h"
#include "luacode.h"


#include "luau.h"
#include "counter.h" 
#include "window.h"


using namespace std;

struct aScript
{
	std::string chunkName;
	std::string chunk;
};

typedef struct std::vector<aScript> Scripts;






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
	
	std::vector<std::string> scriptNames = {"module", "counter", "traits"};
	
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
	
	
	static int setCounterRotateDegrees(lua_State *L)
	{	
		
		if (lua_isnumber(L, -1))
		{	
			int degrees = lua_tonumber(L, -1);
			Counter::incDegrees = degrees;	
			lua_pop(L, 1);
		}
		
		
		return 0;
	}
	
	
	static int create_class(lua_State *L)
	{	
		
		if (lua_isstring(L, -1))
		{	
			const char *name = lua_tostring(L, -1);   
			size_t length = lua_strlen(L, -1);         
			assert(name[length] == '\0');
			assert(strlen(name) <= length);
			lua_pop(L, 1);
			(void)new Counter(name);
		}
		
		
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
				delete( found );
		}
		
		
		return 0;
	}
	
	
	
	static int set_counter_image(lua_State *L)
	{	
		
		if (lua_isstring(L, -1))
		{	
			const char *name = lua_tostring(L, -1);
			const char *id = lua_tostring(L, -2);
			lua_pop(L, 2);
			Counter *found = Counter::findObj(id);
			if (found)
				found->setImage(name);
		}
		
		
		return 0;
	}
	
	
	
	static int rotate_counter(lua_State *L)
	{	
		
		if (lua_isstring(L, -1))
		{	
			const char *id = lua_tostring(L, -1);
			printf("id %s\n", id);
			lua_pop(L, 1);
			Counter *found = Counter::findObj(id);
			if (found)
			{
				found->state.degrees = found->incrementDegrees();
				found->rotateImage();
			}
		}
		
		
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


static string popupParentName;
    
Luau::PopupEntries Luau::getTraits(const char *name)
{
	
	popupParentName = string(name);
	
	lua_getglobal(L, "getTraits");
	lua_pushstring(L, name);
	lua_pcall(L, 1, 1, 0);
	
	
	entries.clear();
	
	getEntries();
	
	lua_pop(L, 1);
	
	
	
	
	return entries;
	
	
}


void Luau::doAction(const char *name)
{
	
	lua_getglobal(L, "action");
	lua_pushstring(L, popupParentName.c_str());
	lua_pushstring(L, name);
	lua_pcall(L, 2, 0, 0);
	
}



void Luau::startVM()
{
	
	L = luaL_newstate();
	luaL_openlibs(L);
	
	
	(void)std::setlocale(LC_NUMERIC, "en_US.UTF-8");
	
	compileScript();
			
	lua_pushcfunction(L, setCounterRotateDegrees, "setCounterRotateDegrees");
	lua_setglobal(L, "setCounterRotateDegrees");
	
	lua_pushcfunction(L, create_class, "create_class");
	lua_setglobal(L, "create_class");
	
	lua_pushcfunction(L, delete_class, "delete_class");
	lua_setglobal(L, "delete_class");
	
	lua_pushcfunction(L, set_counter_image, "set_counter_image");
	lua_setglobal(L, "set_counter_image");
	
	lua_pushcfunction(L, rotate_counter, "rotate_counter");
	lua_setglobal(L, "rotate_counter");
	
	
	
	
	lua_getglobal(L, "module");
	lua_pcall(L, 0, 0, 0);
	
		
}
	
	
void Luau::closeVM()
{
		
	lua_close(L);

}
