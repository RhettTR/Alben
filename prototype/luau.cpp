#include <iostream>
#include <fstream>
#include <cstring>
#include <variant>


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
	
	std::vector<std::string> scriptNames = {"module", "counter", "traits", "state", "repository"};
	
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
	
	
	static int setBackgroundMapID(lua_State *L)
	{	
		
		if (lua_isstring(L, -1))
		{	
			CentralFrame::backgroundID = lua_tostring(L, -1);
			lua_pop(L, 1);
		}
		
		
		return 0;
	}
	
	Counter::Table getTable();
	void outTable(Counter::Table);
	
	static int create_class(lua_State *L)
	{	
		
		(void)lua_gettable(L, -2);
		
		if (lua_istable(L, -1))
		{			
			Counter::Table state = getTable();
	
			(void)new Counter(&state);
		}	
		
		lua_pop(L, 2);
		
		return 0;
	}
	
	
	static int create_class_copy(lua_State *L)
	{	
		
		int id = lua_tointeger(L, -1);
		
		(void)lua_gettable(L, -2);
		
		if (lua_istable(L, -1))
		{			
			Counter::Table state = getTable();
			
			(void)new Counter(id, &state);	
		}
		
		lua_pop(L, 2);
		
		return 0;
	}
	
	
	static int create_mask(lua_State *L)
	{
		int y = lua_tonumber(L, -1);
		int x = lua_tonumber(L, -2);
		const char *mask = lua_tostring(L, -3);
		const char *name = lua_tostring(L, -4);
		lua_pop(L, 4);
		(void)new Counter::Mask(name, mask, x, y);
		
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
		const char *id = lua_tostring(L, -2);
		lua_pop(L, 2);
		Counter *found = Counter::findObj(id);
		if (found)
		{
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
			found->state.zorder = zorder;
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
	
	static int root(lua_State *L)
	{		
		int level = lua_tonumber(L, -1);
		lua_pop(L, 1);
		Window::root(level);
		
		return 0;
	}
	
	static int tabs(lua_State *L)
	{			
		int level = lua_tonumber(L, -1);
		lua_pop(L, 1);
		Window::tabs(level);
		
		return 0;
	}
	
	static int tab(lua_State *L)
	{		
		const char *text = lua_tostring(L, -1);	
		int level = lua_tonumber(L, -2);
		lua_pop(L, 2);
		Window::tab(level, string(text));
		
		return 0;
	}
	
	static int listbox(lua_State *L)
	{			
		int level = lua_tonumber(L, -1);
		lua_pop(L, 1);
		Window::listBox(level);
		
		return 0;
	}
	
	static int listitem(lua_State *L)
	{		
		const char *text = lua_tostring(L, -1);	
		int level = lua_tonumber(L, -2);
		lua_pop(L, 2);
		Window::listItem(level, string(text));
		
		return 0;
	}
	
	static int combobox(lua_State *L)
	{			
		int level = lua_tonumber(L, -1);
		lua_pop(L, 1);
		Window::comboBox(level);
		
		return 0;
	}
	
	static int comboitem(lua_State *L)
	{		
		const char *text = lua_tostring(L, -1);	
		int level = lua_tonumber(L, -2);
		lua_pop(L, 2);
		Window::comboItem(level, string(text));
		
		return 0;
	}
	

	
}	// extern "C"




Counter::Table getTable()
{
	
	Counter::Leftside left;
	Counter::Rightside right;
	Counter::Table table;
	
	
	lua_pushnil(L);
	
	while(lua_next(L, -2) != 0) 
	{
	
		// key
		
		if (lua_isnumber(L, -2))
		{
			lua_Integer n = lua_tointeger(L, -2);			
			left = n;
		}
		else	
			if (lua_isstring(L, -2))
			{
				const char *str = lua_tostring(L, -2);
				left = std::string(str);
			}
		
		
		// value
		
		if (lua_isboolean(L, -1))
		{
			int n = lua_toboolean(L, -1);
			if (n)
				right = true; 	
			else
				right = false; 
		}
		else
			if (lua_isnumber(L, -1))
			{
				lua_Number n = lua_tonumber(L, -1);				
				right = n;
			}
			else
				if (lua_isstring(L, -1))
				{
					const char *str = lua_tostring(L, -1);
					right = std::string(str);
				}
				else
					if (lua_istable(L, -1))
						right = getTable();
	
		
		table[left] = right;
		
		
		lua_pop(L, 1);
		
	}
	
	return table;
	
}


template <class... Fs> struct Overload : Fs... { using Fs::operator()...; };
template <class... Fs> Overload(Fs...) -> Overload<Fs...>;


void outTable(Counter::Table t)
{
	for (auto obj = t.begin(); obj != t.end(); ++obj)
	{
		std::visit(
			Overload{
				[] (int k) { printf("%d=", k);  },
				[] (std::string k) { printf("\"%s\"=", k.c_str()); }
			},
			obj->first
		);
		std::visit(
			Overload{
				[] (int k) { printf("%d\n", k); },
				[] (double k) { printf("%f\n", k); },
				[] (bool k) { (k ? printf("true\n") : printf("false\n")); },				
				[] (std::string k) { printf("\"%s\"\n", k.c_str()); },
				[] (Counter::Table k) { outTable(k); }
			},
			obj->second
		);	
	}
}





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


void Luau::doCreate(const char *fromId, const char *trait)
{
	lua_getglobal(L, "create");
	lua_pushstring(L, fromId);
	lua_pushstring(L, trait);
	
	lua_pcall(L, 2, 0, 0);
	
}


bool Luau::beforeDrag(const char *id)
{
	lua_getglobal(L, "beforeDrag");
	lua_pushstring(L, id);
	lua_pcall(L, 1, 1, 0);
	
	
	int r = lua_toboolean (L, -1);
	
	lua_pop(L, 1);
	
	return (r != 0);
	
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



void Luau::copyCounter(const char *fromId, const char *toId, int zorder, int x, int y)
{
	
	lua_getglobal(L, "copyCounter");
	lua_pushstring(L, fromId);
	lua_pushstring(L, toId);
	lua_pushnumber(L, zorder);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pcall(L, 5, 0, 0);
	
}



void Luau::startVM()
{
	
	L = luaL_newstate();
	luaL_openlibs(L);
	
	
	(void)std::setlocale(LC_NUMERIC, "en_US.UTF-8");
	
	compileScript();
	
	
	lua_pushcfunction(L, setCounterDragAlpha, "setCounterDragAlpha");
	lua_setglobal(L, "setCounterDragAlpha");
	
	lua_pushcfunction(L, setBackgroundMapID, "setBackgroundMapID");
	lua_setglobal(L, "setBackgroundMapID");
	
	lua_pushcfunction(L, create_class, "create_class");
	lua_setglobal(L, "create_class");
	
	lua_pushcfunction(L, create_class_copy, "create_class_copy");
	lua_setglobal(L, "create_class_copy");
	
	lua_pushcfunction(L, create_mask, "create_mask");
	lua_setglobal(L, "create_mask");
	
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
	
	// repository
	
	lua_pushcfunction(L, root, "root");
	lua_setglobal(L, "root");
	
	lua_pushcfunction(L, tabs, "tabs");
	lua_setglobal(L, "tabs");
	
	lua_pushcfunction(L, tab, "tab");
	lua_setglobal(L, "tab");
	
	lua_pushcfunction(L, listbox, "listbox");
	lua_setglobal(L, "listbox");
	
	lua_pushcfunction(L, listitem, "listitem");
	lua_setglobal(L, "listitem");
	
	lua_pushcfunction(L, combobox, "combobox");
	lua_setglobal(L, "combobox");
	
	lua_pushcfunction(L, comboitem, "comboitem");
	lua_setglobal(L, "comboitem");
	
	
	
	
	lua_getglobal(L, "module");
	lua_pcall(L, 0, 0, 0);
	
		
}
	
	
void Luau::closeVM()
{
		
	lua_close(L);

}
