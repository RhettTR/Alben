#include <iostream>
#include <fstream>
#include <cstring>
#include <variant>


#include <assert.h>
#include "lua.h"
#include "lualib.h"
#include "luacode.h"


#include "luau.h"
#include "repository.h"
#include "overlay.h"
#include "toolbar.h"
#include "io.h"
#include "window.h"

 


using namespace std;

struct aScript
{
	std::string chunkName;
	std::string chunk;
};

typedef struct std::vector<aScript> Scripts;




extern CentralFrame *mapFrame;
extern ToolBar *mainToolBar;
extern Window *artilleryWindow;
extern void createWindow(QString, std::string, std::string);
extern void show();



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
	
	std::vector<std::string> scriptNames = {"module", "counter", "traits", "state", "repository", "dev"};
	
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



void Luau::callbackScript(std::string script)
{		
	size_t bytecodeSize = 0;
	
	char* bytecode = luau_compile(script.c_str(), script.length(), NULL, &bytecodeSize);
	assert(luau_load(L, "script", bytecode, bytecodeSize, 0) == 0);
	free(bytecode);	
		
	lua_setglobal(L, "script");
	
	lua_getglobal(L, "runScript");
	lua_pushstring(L, "script");
	lua_pcall(L, 1, 0, 0);
	
	// remove the global
	lua_pushnil(L);
    lua_setglobal(L, "script");
 
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
		(void)new Counter::SystemMask(name, mask, x, y);
		
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
				// hook
				Luau::flipped(id, image);
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
		
		const char *id;
		float opacity;
		Counter::Table *overlays = nullptr;
		const char *image;	
		int degrees;	
		bool moved;
		int zorder;
		int y;
		int x;
		
		
		(void)lua_gettable(L, -2);
	
		
		if (lua_istable(L, -1))
		{
			overlays = new Counter::Table();
			
			(*overlays) = getTable();
			lua_pop(L, 2);
			opacity = lua_tonumber(L, -1); 
			image	= lua_tostring(L, -2);			
			degrees = lua_tonumber(L, -3);	
			moved 	= lua_toboolean(L, -4);
			zorder	= lua_tonumber(L, -5);
			y 		= lua_tonumber(L, -6);
			x 		= lua_tonumber(L, -7);
			id 		= lua_tostring(L, -8);
			lua_pop(L, 8);			
		}
		else
		{
			lua_pop(L, 2);
			opacity = lua_tonumber(L, -1); 
			image	= lua_tostring(L, -2);	
			degrees = lua_tonumber(L, -3);	
			moved 	= lua_toboolean(L, -4);
			zorder	= lua_tonumber(L, -5);
			y 		= lua_tonumber(L, -6);
			x 		= lua_tonumber(L, -7);
			id 		= lua_tostring(L, -8);
			lua_pop(L, 8);
		}
		
		
		
		Counter *found = Counter::findObj(id);
		if (found)
		{
			found->state.x = x;
			found->state.y = y;
			found->state.zorder = zorder;
			found->state.moved = moved;
			found->state.degrees = degrees;
			found->state.image = string(image);
			found->state.overlays = overlays;
			found->state.opacity = opacity;		
		}
		else
			printf("error\n");
			
		// force redraw of mask layer
		Overlay::overlay->clearMask();
			
		
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
		Repository::root(level);
		
		return 0;
	}
	
	static int tabs(lua_State *L)
	{			
		int level = lua_tonumber(L, -1);
		lua_pop(L, 1);
		Repository::tabs(level);
		
		return 0;
	}
	
	static int tab(lua_State *L)
	{		
		const char *text = lua_tostring(L, -1);	
		int level = lua_tonumber(L, -2);
		lua_pop(L, 2);
		Repository::tab(level, string(text));
		
		return 0;
	}
	
	static int listbox(lua_State *L)
	{			
		int level = lua_tonumber(L, -1);
		lua_pop(L, 1);
		Repository::listBox(level);
		
		return 0;
	}
	
	static int listitem(lua_State *L)
	{		
		const char *text = lua_tostring(L, -1);	
		int level = lua_tonumber(L, -2);
		lua_pop(L, 2);
		Repository::listItem(level, string(text));
		
		return 0;
	}
	
	static int combobox(lua_State *L)
	{			
		int level = lua_tonumber(L, -1);
		lua_pop(L, 1);
		Repository::comboBox(level);
		
		return 0;
	}
	
	static int comboitem(lua_State *L)
	{		
		const char *text = lua_tostring(L, -1);	
		int level = lua_tonumber(L, -2);
		lua_pop(L, 2);
		Repository::comboItem(level, string(text));
		
		return 0;
	}
	
	static int create_button(lua_State *L)
	{		
		int height = lua_tonumber(L, -1);
		int width  = lua_tonumber(L, -2);
		int y  = lua_tonumber(L, -3);
		int x  = lua_tonumber(L, -4);
		const char *handler = lua_tostring(L, -5);	
		const char *text = lua_tostring(L, -6);
		const char *id = lua_tostring(L, -7); 	
		lua_pop(L, 7);		
		CentralFrame::createButton(id, text, handler, x, y, width, height);
		
		return 0;
	}

	static int delete_button(lua_State *L)
	{		
		const char *id = lua_tostring(L, -1); 	
		lua_pop(L, 1);	
		CentralFrame::deleteButton(id);
		
		return 0;
	}
	
	static int top_zorder(lua_State *L)
	{				
		int zorder = Counter::topZorder();		
		lua_pushnumber(L, zorder);
		
		return 1;
	}
	
	static int next_id(lua_State *L)
	{				
		const char *id = std::to_string(Counter::nextId()).c_str();		
		lua_pushstring(L, id);
		
		return 1;
	}
	
	static int toggle_select_status(lua_State *L)
	{		
		const char *id = lua_tostring(L, -1); 	
		lua_pop(L, 1);	
		Counter::toggleSelect(id);
		
		return 0;
	}
	
	static int toolbar_enable(lua_State *L)
	{		
		const char *id = lua_tostring(L, -1); 	
		lua_pop(L, 1);	
		mainToolBar->enabled(id, true);
		
		return 0;
	}
	
	static int toolbar_disable(lua_State *L)
	{		
		const char *id = lua_tostring(L, -1); 	
		lua_pop(L, 1);	
		mainToolBar->enabled(id, false);
		
		return 0;
	}
	
	static int disable_all(lua_State *L)
	{
		Counter::setDisabled(true);
		
		return 0;
	}
	
	static int stop_stepping(lua_State *L)
	{
		IO::stepping = false;
		Counter::setDisabled(false);
		Counter::resetId();
		Counter::resetZorder();
		
		return 0;
	}
	
	static int not_recording(lua_State *L)
	{
		lua_pushboolean(L, (int)!IO::recording);
				
		return 1;
	}
	
	static int add_toolbar_button(lua_State *L)
	{	
		std::string luaScript(lua_tostring(L, -1));		
		const QString toolTip = QString(lua_tostring(L, -2));
		std::string resourceName(lua_tostring(L, -3));
		
		lua_pop(L, 3);
		
		mainToolBar->addImageButton(resourceName, toolTip, nullptr, luaScript);
		mainToolBar->enabled("Icons/GermanArtillery", true);
		
		return 0;
	}
	
	static int create_window(lua_State *L)
	{		
		std::string background(lua_tostring(L, -1));
		std::string name(lua_tostring(L, -2));
		QString title(lua_tostring(L, -3));
		lua_pop(L, 3);
		
		createWindow(title, name, background);
		
		return 0;
	}
	
	static int delete_window(lua_State *L)
	{					
		int id = lua_tonumber(L, -1);
		const char *instance = lua_tostring(L, -2);  	
		lua_pop(L, 2);	
		
		
		Window *window = Window::getInstance(instance);
		if (window != nullptr)	
		{			
			delete window->frame->tokens[id];
		}
		
		return 0;
	}
	
	static int toggle_window(lua_State *L)
	{
		show();
		
		return 0;
	}
	
	static int set_grid(lua_State *L)
	{
		(void)lua_gettable(L, -2);
		
		if (lua_istable(L, -1))
		{			
			Counter::Table table = getTable();
				
			for (auto obj = table.begin(); obj != table.end(); ++obj)
			{
				int x = 0, y = 0;
				
				std::visit(
					Overload{
						[] (double k) {},
						[] (bool k) {},				
						[] (std::string k) {},
						[&] (Counter::Table k) { x = (int)std::get<double>((k)["x"]);	 	
												 y = (int)std::get<double>((k)["y"]); }
					},
					obj->second
				);
				
				artilleryWindow->frame->addtoGrid(x, y);
			}			
		}	
		
		lua_pop(L, 2);
		
		return 0;;
	}
	
	static int create_token(lua_State *L)
	{	
		int id = lua_tonumber(L, -1);
		
		(void)lua_gettable(L, -2);
		
		if (lua_istable(L, -1))
		{		
			Counter::Table state = getTable();

			(void)new Window::Token(id, &state);
		}	
		
		lua_pop(L, 2);
		
		return 0;
	
	}
	
	static int set_token_image(lua_State *L)
	{		
		std::string name(lua_tostring(L, -1));
		int id = lua_tonumber(L, -2); 
		lua_pop(L, 2);
		
		if (artilleryWindow->frame->tokens.find(id) != artilleryWindow->frame->tokens.end())
			artilleryWindow->frame->tokens[id]->setImage(name);
		
		return 0;
	}
	
	static int create_label(lua_State *L)
	{	
		const char *css = lua_tostring(L, -1);
		int h = lua_tonumber(L, -2);
		int w = lua_tonumber(L, -3);
		int y = lua_tonumber(L, -4);
		int x = lua_tonumber(L, -5);
		const char *id = lua_tostring(L, -6); 
		
		lua_pop(L, 6);
		
		(void)new Window::Label(id, artilleryWindow->frame, x, y, w, h, css);
		
		return 0;
	}
	
	static int set_label_text(lua_State *L)
	{	
		std::string text(lua_tostring(L, -1));
		std::string id(lua_tostring(L, -2)); 
		lua_pop(L, 2);
		
		if (artilleryWindow->frame->labels.find(id) != artilleryWindow->frame->labels.end())
			artilleryWindow->frame->labels[id]->set(text);		
				
		return 0;
	}
	
	static int get_label_text(lua_State *L)
	{	
		std::string id(lua_tostring(L, -1)); 
		lua_pop(L, 1);
		
		const char *text = artilleryWindow->frame->labels[id]->get().c_str();			
		lua_pushstring(L, text);	
				
		return 1;
	}
	
	static int create_checkbox(lua_State *L)
	{	
		const char *css = lua_tostring(L, -1);
		std::string script(lua_tostring(L, -2));	
		const char *text = lua_tostring(L, -3);
		int h = lua_tonumber(L, -4);
		int w = lua_tonumber(L, -5);
		int y = lua_tonumber(L, -6);
		int x = lua_tonumber(L, -7);
		const char *id = lua_tostring(L, -8); 
		
		lua_pop(L, 8);
		
		(void)new Window::CheckBox(id, Window::getInstance("artillery-window"), x, y, w, h, text, script, css);
		
		return 0;
	}
	
	static int get_checkbox_checked(lua_State *L)
	{	
		std::string box(lua_tostring(L, -1)); 
		const char *window = lua_tostring(L, -2); 
		lua_pop(L, 2);
		
		bool value = Window::getInstance(window)->frame->checkboxes[box]->get();			
		lua_pushboolean(L, (int)value);	
				
		return 1;
	}
	
	static int update_token(lua_State *L)
	{
		
		int id;	
		Counter::Table *overlays = nullptr;
		const char *image;	
		int y;
		int x;
		
		
		(void)lua_gettable(L, -2);
	
		
		if (lua_istable(L, -1))
		{
			overlays = new Counter::Table();
			
			(*overlays) = getTable();
			lua_pop(L, 2);
			
			image	= lua_tostring(L, -1);			
			y 		= lua_tonumber(L, -2);
			x 		= lua_tonumber(L, -3);
			id 		= lua_tonumber(L, -4);
			lua_pop(L, 4);			
		}
		else
			return 0;
				
		
		artilleryWindow->frame->tokens[id]->state.x = x;
		artilleryWindow->frame->tokens[id]->state.y = y;		
		artilleryWindow->frame->tokens[id]->state.image = string(image);
		artilleryWindow->frame->tokens[id]->overlays = overlays;
		
		artilleryWindow->frame->tokens[id]->setImage(image);
		
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
		
		if (lua_type(L, -2) == LUA_TSTRING)
		{
			const char *str = lua_tostring(L, -2);
			left = std::string(str);
		}
		else
			if (lua_isnumber(L, -2))
			{
				lua_Integer n = lua_tointeger(L, -2);			
				left = n;
					
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
			if (lua_type(L, -1) == LUA_TSTRING)
			{
				const char *str = lua_tostring(L, -1);
				right = std::string(str);
			}
			else
				if (lua_isnumber(L, -1))
				{
					lua_Number n = lua_tonumber(L, -1);				
					right = n;
				}
				else
					if (lua_isfunction (L, -1))
					{
						lua_pop(L, 1);
						continue;
					}
					else
						if (lua_istable(L, -1))
							right = getTable();



		
		table[left] = right;
		
		
		lua_pop(L, 1);
		
	}
		
	return table;
	
}




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




Counter::Table Luau::getTraits(const char *window, int id)
{
	
	lua_getglobal(L, "getTraits");
	lua_pushstring(L, window);
	if (strcmp(window, "State") == 0)
		lua_pushnumber(L, id);
	else
		lua_pushstring(L, std::to_string(id).c_str());
	lua_pcall(L, 2, 1, 0);
	
	
	Counter::Table state = getTable();
	
	lua_pop(L, 1);
	
	
	
	return state;
	
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


bool Luau::afterDrag(const char *id, int &x, int &y)
{
	lua_getglobal(L, "afterDrag");
	lua_pushstring(L, id);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pcall(L, 3, 3, 0);
	
	
	y = (int)lua_tonumber (L, -1);
	x = (int)lua_tonumber (L, -2);
	int flag = lua_toboolean (L, -3);
	
	lua_pop(L, 3);
	
	return (flag != 0);
	
}

void Luau::dropped(const char *name, const char *fromid, const char *toid, int x, int y)
{
	lua_getglobal(L, "dropped");
	lua_pushstring(L, name);
	lua_pushstring(L, fromid);
	lua_pushstring(L, toid);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pcall(L, 5, 0, 0);	
}


void Luau::moved(const char *name, const char *toid, int x, int y)
{
	lua_getglobal(L, "moved");
	lua_pushstring(L, name);
	lua_pushstring(L, toid);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pcall(L, 4, 0, 0);	
}


void Luau::deleted(const char *id)
{
	lua_getglobal(L, "deleted");
	lua_pushstring(L, id);
	lua_pcall(L, 1, 0, 0);	
}


void Luau::flipped(const char *id, const char *image)
{
	lua_getglobal(L, "flipped");
	lua_pushstring(L, id);
	lua_pushstring(L, image);
	lua_pcall(L, 2, 0, 0);	
}


void Luau::handlers(const char *id, const char *func)
{
	lua_getglobal(L, "handlers");
	lua_pushstring(L, id);
	lua_pushstring(L, func);
	lua_pcall(L, 2, 0, 0);	
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


void stackTable(Counter::Table table)
{
	lua_newtable(L);
	
	for (auto obj = table.begin(); obj != table.end(); ++obj)
	{
		std::visit(
			Overload{
				[] (int k) { lua_pushnumber(L, k); },
				[] (std::string k) { lua_pushstring(L, k.c_str()); }
			},
			obj->first
		);
		std::visit(
			Overload{
				[] (double k) { lua_pushnumber(L, k);  },
				[] (bool k) { lua_pushboolean(L, (int)k); },				
				[] (std::string k) { lua_pushstring(L, k.c_str()); },
				[&] (Counter::Table k) { stackTable(k); }
			},
			obj->second
		);
		lua_settable(L, -3);	
	}
}



int Luau::loadCounter(Counter::Table table)
{
	lua_getglobal(L, "loadCounter");
	
	stackTable(table);

	lua_pcall(L, 1, 1, 0);
	
	
	int id = lua_tointeger(L, -1); 		
	
	lua_pop(L, 1);
	
	
	return id;
	
}



void Luau::loadLog(Counter::Table table)
{
	lua_getglobal(L, "loadLog");
	
	stackTable(table);

	lua_pcall(L, 1, 0, 0);	
}



void Luau::updatePos(const char *id, int x, int y)
{
	lua_getglobal(L, "updatePos");
	lua_pushstring(L, id);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pcall(L, 3, 0, 0);
}


void Luau::updateMoved(const char *id, bool moved)
{
	lua_getglobal(L, "updateMoved");
	lua_pushstring(L, id);
	lua_pushboolean(L, (moved == true ? 1 : 0));
	lua_pcall(L, 2, 0, 0);
}


void Luau::updateSide(const char *side)
{
	lua_getglobal(L, "updateSide");
	lua_pushstring(L, side);
	lua_pcall(L, 1, 0, 0);
}


void Luau::saveStage()
{
	lua_getglobal(L, "saveStage");
	lua_pcall(L, 0, 0, 0);
}


void Luau::deleteAll()
{
	lua_getglobal(L, "deleteAll");
	lua_pcall(L, 0, 0, 0);
}


void Luau::resetState()
{
	lua_getglobal(L, "resetState");
	lua_pcall(L, 0, 0, 0);
}


void Luau::getRange(int &savedPointer, int &stagePointer)
{
	lua_getglobal(L, "getRange");
	lua_pcall(L, 0, 2, 0);
	
	savedPointer = lua_tointeger(L, -1);
	stagePointer = lua_tointeger(L, -2); 		
	
	lua_pop(L, 2);	
}


void Luau::logReset()
{
	lua_getglobal(L, "logReset");
	lua_pcall(L, 0, 0, 0);	
}

void Luau::logStep(bool oneStep)
{
	lua_getglobal(L, "logStep");
	lua_pushboolean(L, (int)oneStep);
	lua_pcall(L, 1, 0, 0);	
}

void Luau::logAbort()
{
	lua_getglobal(L, "logAbort");
	lua_pcall(L, 0, 0, 0);	
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
	
	// gui
	
	lua_pushcfunction(L, create_button, "create_button");
	lua_setglobal(L, "create_button");
	
	lua_pushcfunction(L, delete_button, "delete_button");
	lua_setglobal(L, "delete_button");
	
	lua_pushcfunction(L, top_zorder, "top_zorder");
	lua_setglobal(L, "top_zorder");
	
	lua_pushcfunction(L, next_id, "next_id");
	lua_setglobal(L, "next_id");
	
	lua_pushcfunction(L, toggle_select_status, "toggle_select_status");
	lua_setglobal(L, "toggle_select_status");
		
	lua_pushcfunction(L, disable_all, "disable_all");
	lua_setglobal(L, "disable_all");
	
	lua_pushcfunction(L, stop_stepping, "stop_stepping");
	lua_setglobal(L, "stop_stepping");
	
	// toolbar
	
	lua_pushcfunction(L, toolbar_enable, "toolbar_enable");
	lua_setglobal(L, "toolbar_enable");
	
	lua_pushcfunction(L, toolbar_disable, "toolbar_disable");
	lua_setglobal(L, "toolbar_disable");
	
	lua_pushcfunction(L, not_recording, "not_recording");
	lua_setglobal(L, "not_recording");
	
	// window
	
	lua_pushcfunction(L, add_toolbar_button, "add_toolbar_button");
	lua_setglobal(L, "add_toolbar_button");
	
	lua_pushcfunction(L, create_window, "create_window");
	lua_setglobal(L, "create_window");
	
	lua_pushcfunction(L, delete_window, "delete_window");
	lua_setglobal(L, "delete_window");
	
	lua_pushcfunction(L, toggle_window, "toggle_window");
	lua_setglobal(L, "toggle_window");
	
	lua_pushcfunction(L, set_grid, "set_grid");
	lua_setglobal(L, "set_grid");
	
	lua_pushcfunction(L, create_token, "create_token");
	lua_setglobal(L, "create_token");
	
	lua_pushcfunction(L, set_token_image, "set_token_image");
	lua_setglobal(L, "set_token_image");
	
	lua_pushcfunction(L, create_label, "create_label");
	lua_setglobal(L, "create_label");
	
	lua_pushcfunction(L, set_label_text, "set_label_text");
	lua_setglobal(L, "set_label_text");
	
	lua_pushcfunction(L, get_label_text, "get_label_text");
	lua_setglobal(L, "get_label_text");
	
	lua_pushcfunction(L, create_checkbox, "create_checkbox");
	lua_setglobal(L, "create_checkbox");
	
	lua_pushcfunction(L, get_checkbox_checked, "get_checkbox_checked");
	lua_setglobal(L, "get_checkbox_checked");
	
	lua_pushcfunction(L, update_token, "update_token");
	lua_setglobal(L, "update_token");
	
	
	lua_getglobal(L, "module");
	lua_pcall(L, 0, 0, 0);
	
		
}
	
	
void Luau::closeVM()
{
		
	lua_close(L);

}
