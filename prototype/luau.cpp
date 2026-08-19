#include <iostream>
#include <fstream>
#include <cstring>
#include <variant>


#include <assert.h>
#include "lua.h"
#include "lualib.h"
#include "luacode.h"

#include "configure.h"
#include "luau.h"
#include "overlay.h"
#include "toolbar.h"
#include "io.h"
#include "window.h"
#include "scale.h"


 


using namespace std;

struct aScript
{
	std::string chunkName;
	std::string chunk;
};

typedef struct std::vector<aScript> Scripts;




extern IO *io;
extern Window *repositoryWindow;
extern Window *logWindow;
extern Configure *config;



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
	
	std::vector<std::string> scriptNames = 
		{"module", "report", "counter", "traits", "state", "repository", "zone", "dev"};
	
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
		int res = luau_load(L, element.chunkName.c_str(), bytecode, bytecodeSize, 0);	
		free(bytecode);
		
		
		if (res != 0) 
		{
			// https://sleitnick.github.io/luau-api/reference.html#luau_load
			size_t len;
			const char* msg = lua_tolstring(L, -1, &len);
			lua_pop(L, 1);
			printf("failed to compile: %s\n", msg);
			std::exit(1);
		}	
		
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



void Luau::error(int no, int count, ...) 
{
	
	lua_getglobal(L, "error");

	lua_pushnumber(L, no);
	
	
	va_list args;
    va_start(args, count);
    
    for (int i = 0; i < count; i++) 
    {
		
        lua_pushstring(L, va_arg(args, const char *));
	}
    
    va_end(args);
	
	
	
	lua_pcall(L, 1 + count, 0, 0);
}







extern "C" {
	
	
	
	static int setCounterDragAlpha(lua_State *L)
	{	
		
		if (lua_isnumber(L, -1))
			Counter::alpha = lua_tonumber(L, -1);	
			
		lua_pop(L, 1);
		
		return 0;
	}
	
	
	static int set_background_mapID(lua_State *L)
	{	
		
		const char *id = lua_tostring(L, -1); 
		const char *window = lua_tostring(L, -2); 	
		
		Window::getInstance(window)->frame->backgroundID = std::string(id);
			
		lua_pop(L, 2);
		
		return 0;
	}
	
	
	static int set_padding(lua_State *L)
	{		
		
		int bottom = lua_tonumber(L, -1);
		int right  = lua_tonumber(L, -2);
		int top    = lua_tonumber(L, -3);
		int left   = lua_tonumber(L, -4);
		QString color(lua_tostring(L, -5));
		string resourceId = string(lua_tostring(L, -6));
				
		lua_pop(L, 6);
		
		
		IO::addPadding(resourceId, QColor::fromString(color), left, top, right, bottom);
		
		return 0; 
	}
	
	
	static int set_mask(lua_State *L)
	{		
		
		int height = lua_tonumber(L, -1);
		int width  = lua_tonumber(L, -2);
		int top    = lua_tonumber(L, -3);
		int left   = lua_tonumber(L, -4);
		QString color(lua_tostring(L, -5));
		string resourceId = string(lua_tostring(L, -6));
				
		lua_pop(L, 6);
		
		
		IO::addMask(resourceId, QColor::fromString(color), left, top, width, height);
		
		return 0; 
	}
	
	
	static int set_image(lua_State *L)
	{		
		
		string toImage = string(lua_tostring(L, -1));
		string fromImage = string(lua_tostring(L, -2));
				
		lua_pop(L, 2);
		
		
		IO::changeImage(fromImage, toImage);
		
		return 0; 
	}	
	
	
	Counter::Table getTable(qint32 &);
	void outTable(Counter::Table);
	
	static int create_class(lua_State *L)
	{	
		
		if (!lua_istable(L, -2))
			Luau::error(2, 0);			
		else
		{
			(void)lua_gettable(L, -2);
			
			if (lua_istable(L, -1))
			{			
				qint32 dummy;
				Counter::Table state = getTable(dummy);

				(void)new Counter(&state);
			}
			else
				Luau::error(3, 0);
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
			qint32 dummy;		
			Counter::Table state = getTable(dummy);
		
			(void)new Counter(id, &state);	
		}
			
		
		lua_pop(L, 2);
		
		return 0;
	}
	
	static int update_class(lua_State *L)
	{	
		
		int id = lua_tointeger(L, -1);
		
		(void)lua_gettable(L, -2);
		
		if (lua_istable(L, -1))
		{	
			qint32 dummy;		
			Counter::Table state = getTable(dummy);
			
			Counter *found = Counter::counters[id];
			
			if (found)
			{
				// memory leak ??
				found->table = state;
				
				
				found->state.x = (int)std::get<double>((state)["x"]);	 	
				found->state.y = (int)std::get<double>((state)["y"]);
				found->state.cx = (int)std::get<double>((state)["cx"]);	 	
				found->state.cy = (int)std::get<double>((state)["cy"]);
				found->state.tag = std::get<std::string>((state)["window"]);
				found->state.zorder = (int)std::get<double>((state)["zorder"]);
	
				
				Counter::Table images = std::get<Counter::Table>(std::get<Counter::Table>((state)["Image"])["images"]);
				int index = (int)std::get<double>(std::get<Counter::Table>((state)["Image"])["imageIndex"]);
				found->image = std::get<std::string>(images[1]);	
				found->state.image = std::get<std::string>(images[index]);
				
				
				

				// optional fields
 	
				if ((state).find("MarkMoved") != (state).end())	
					found->state.moved = std::get<bool>(std::get<Counter::Table>((state)["MarkMoved"])["moved"]);
				else
					found->state.moved = false;
					
				if ((state).find("Rotate") != (state).end())		
					found->state.degrees = (int)std::get<double>(std::get<Counter::Table>((state)["Rotate"])["degrees"]);
				else
					found->state.degrees = 0;
					
				if ((state).find("Overlays") != (state).end())		
				{
					Counter::Table *overlays = new Counter::Table();	
					(*overlays) = std::get<Counter::Table>((state)["Overlays"]);	
					found->state.overlays = overlays;
				}
				else
					found->state.overlays = nullptr;
				
				if ((state).find("Visibility") != (state).end())		
					found->state.opacity = (float)std::get<double>(std::get<Counter::Table>((state)["Visibility"])["opacity"]);
				else
					found->state.opacity = 1.0;
					
				found->doesNotStack = ((state).find("DoesNotStack") != (state).end());
				
				if ((state).find("Side") != (state).end())					
					found->side = std::get<std::string>(std::get<Counter::Table>((state)["Side"])["side"]);
				else
					found->side = "none";
				
				
								
				found->setImage();
				
				found->parentWindow->frame->update();
				
				
			}			
		}
		
		lua_pop(L, 2);
		
		return 0;
	}
	
	
	static int create_mask(lua_State *L)
	{
		const char *halign = lua_tostring(L, -1);
		const char *valign = lua_tostring(L, -2);
		const char *mask = lua_tostring(L, -3);
		const char *name = lua_tostring(L, -4);
		lua_pop(L, 4);
		(void)new Counter::SystemMask(name, mask, valign, halign);
		
		return 0;
	}
	
	
	static int delete_class(lua_State *L)
	{	
		if (lua_isstring(L, -1))
		{	
			const char *id = lua_tostring(L, -1); 
			lua_pop(L, 1);
//printf("find %s\n", id);		
			Counter *found = Counter::findObj(id);
			if (found)	
			{			
				delete found;
				//Counter::setGUI();				
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
		
	static int setGUI(lua_State *L)
	{		
		
		const char *tag = lua_tostring(L, -1);	
		lua_pop(L, 1);
		
		Counter::setGUI(tag);
		
		return 0;
	}
	
	static int get_size(lua_State *L)
	{		
		
		std::string resourceName(lua_tostring(L, -1));
		
		lua_pop(L, 1);
		
		
		int w = 0;
		int h = 0;
		
		if (!resourceName.empty())
		{
			QSize size = io->getSize(resourceName);
			w = size.width();
			h = size.height();
		}
		
	
		lua_pushnumber(L, w);
		lua_pushnumber(L, h);	
				
		return 2;
	}
	
	
	static int set_root(lua_State *L)
	{	
		
		const char *tag = lua_tostring(L, -1);	
		lua_pop(L, 1);
		
		Window::rootTag = string(tag);		
		
		
		return 0;
	}
	
	static int root(lua_State *L)
	{	
	
		int level = lua_tonumber(L, -1);	
		lua_pop(L, 1);
		
		Window *window = Window::getInstance(Window::rootTag.c_str());		
		
		window->root(level);
		
		return 0;
	}
	
	static int tabs(lua_State *L)
	{			
		int level = lua_tonumber(L, -1);	
		lua_pop(L, 1);
		
		Window *window = Window::getInstance(Window::rootTag.c_str());
		
		window->tabs(level);
		
		return 0;
	}
	
	static int tab(lua_State *L)
	{		
		const char *text = lua_tostring(L, -1);
		int level = lua_tonumber(L, -2);	
		lua_pop(L, 2);
		
		Window *window = Window::getInstance(Window::rootTag.c_str());
		
		window->tab(level, string(text));
		
		return 0;
	}
	
	static int listbox(lua_State *L)
	{			
		int level = lua_tonumber(L, -1);	
		lua_pop(L, 1);
		
		Window *window = Window::getInstance(Window::rootTag.c_str());
		
		window->listBox(level);
		
		return 0;
	}
	
	static int listitem(lua_State *L)
	{		
		const char *text = lua_tostring(L, -1);
		int level = lua_tonumber(L, -2);	
		lua_pop(L, 2);
		
		Window *window = Window::getInstance(Window::rootTag.c_str());
		
		window->listItem(level, string(text));
		
		return 0;
	}
	
	static int combobox(lua_State *L)
	{				
		int level = lua_tonumber(L, -1);	
		lua_pop(L, 1);
		
		Window *window = Window::getInstance(Window::rootTag.c_str());
		
		window->comboBox(level);
		
		return 0;
	}
	
	static int comboitem(lua_State *L)
	{		
		const char *text = lua_tostring(L, -1);
		int level = lua_tonumber(L, -1);	
		lua_pop(L, 2);
		
		Window *window = Window::getInstance(Window::rootTag.c_str());
		
		window->comboItem(level, string(text));
		
		return 0;
	}
	
	static int image_item(lua_State *L)
	{		
		const char *image = lua_tostring(L, -1);	
		lua_pop(L, 1);
		
		Window *window = Window::getInstance(Window::rootTag.c_str());
	
		window->imageItem(string(image));
		
		return 0;
	}
	
	static int html_item(lua_State *L)
	{		
		const char *file = lua_tostring(L, -1);	
		lua_pop(L, 1);
		
		Window *window = Window::getInstance(Window::rootTag.c_str());
	
		window->htmlItem(string(file));
		
		return 0;
	}
	
	static int scale_frame(lua_State *L)
	{		
		float fraction = lua_tonumber(L, -1);
		const char *tag = lua_tostring(L, -2);
			
		lua_pop(L, 2);
		
		Window *window = Window::getInstance(tag);	
		window->scaleFrame(fraction);
		
		return 0;
	}
	
	static int scale_pane(lua_State *L)
	{		
		float fraction = lua_tonumber(L, -1);	
		lua_pop(L, 1);
		
		Window *window = Window::getInstance(Window::rootTag.c_str());
		window->scalePane(fraction);
		
		return 0;
	}
	
	static int set_row_length(lua_State *L)
	{		
		int length = lua_tonumber(L, -1);		
		lua_pop(L, 1);
		
		Overlay::FlowLayout::maxColumns = length;
		
		return 0;
	}
		
	static int is_owner(lua_State *L)
	{
		std::string id(lua_tostring(L, -1));
		lua_pop(L, 1); 
		
		bool owner = Settings::isOwner(id);
		
		lua_pushboolean(L, (int)owner);	
		
		return 1;
	}
	
	static int is_side(lua_State *L)
	{
		std::string side(lua_tostring(L, -1));
		lua_pop(L, 1); 
		
		bool owner = Settings::isSide(side);
		
		lua_pushboolean(L, (int)owner);	
		
		return 1;
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
	
	static int inc_id(lua_State *L)
	{				
		(void)Counter::incId();
				
		return 0;
	}
	
	static int inc_top_zorder(lua_State *L)
	{				
		(void)Counter::incTopZorder();
				
		return 0;
	}
	
	static int inc_bottom_zorder(lua_State *L)
	{				
		(void)Counter::incBottomZorder();
				
		return 0;
	}
	
	static int top_zorder(lua_State *L)
	{				
		int zorder = Counter::topZorder();		
		lua_pushnumber(L, zorder);
		
		return 1;
	}
	
	static int bottom_zorder(lua_State *L)
	{				
		int zorder = Counter::bottomZorder();		
		lua_pushnumber(L, zorder);
		
		return 1;
	}
	
	static int next_id(lua_State *L)
	{		
		std::string str = std::to_string(Counter::nextId());
				
		const char *id = str.c_str();		
		lua_pushstring(L, id);
		
		return 1;
	}
	
	static int reset_idz(lua_State *L)
	{		
		Counter::resetId();
		Counter::resetZorder();
		
		return 0;
	}
	
	static int toggle_select_status(lua_State *L)
	{		
		const char *id = lua_tostring(L, -1); 	
		lua_pop(L, 1);	
		Counter::toggleSelect(id);
		
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
	
	static int create_toolbar(lua_State *L)
	{	
		int height = lua_tonumber(L, -1);
		const QString name = QString(lua_tostring(L, -2));
		std::string tag(lua_tostring(L, -3));
		std::string window(lua_tostring(L, -4)); 
		
		lua_pop(L, 4);
		
		ToolBar *customBar = new ToolBar(window, tag, name, height);
		Window::getInstance(customBar->window.c_str())->addToolBarBreak(Qt::TopToolBarArea);
		Window::getInstance(customBar->window.c_str())->addToolBar(Qt::TopToolBarArea, customBar);
		
		return 0;
	}
	
	static int add_toolbar_button(lua_State *L)
	{	
		int size = lua_gettop(L);
		std::string side = "";
		
		std::string luaScript(lua_tostring(L, -1));		
		const QString toolTip = QString(lua_tostring(L, -2));
		QString buttonText(lua_tostring(L, -3));
		std::string resourceName(lua_tostring(L, -4));
		const char *id = lua_tostring(L, -5);
		const char *tag = lua_tostring(L, -6);
		if (size == 7)
			side = std::string(lua_tostring(L, -7));
		
		lua_pop(L, size);
		
		ToolBar *toolbar = ToolBar::getInstance(tag);		


		toolbar->addImageButton(side, id, resourceName, buttonText, toolTip, nullptr, luaScript);		
		
		return 0;
	}
	
	static int set_toolbar_button(lua_State *L)
	{	
		QString buttonText(lua_tostring(L, -1));
		std::string resourceName(lua_tostring(L, -2));
		std::string id(lua_tostring(L, -3));
		const char *tag = lua_tostring(L, -4);
		
		lua_pop(L, 4);
		
		ToolBar *toolbar = ToolBar::getInstance(tag);
		
		toolbar->setLabelImage(id, resourceName, buttonText);
		
		return 0;
	}
	
	static int enable_toolbar_button(lua_State *L)
	{
		bool value = lua_toboolean(L, -1);
		std::string id(lua_tostring(L, -2));
		const char *tag = lua_tostring(L, -3);
		
		lua_pop(L, 3);
		
		ToolBar *toolbar = ToolBar::getInstance(tag);
		ToolBar::ToolButton *button = toolbar->getToolButton(id);
		button->setEnabled(value);
		
		return 0;
	}
	
	static int add_toolbar_label(lua_State *L)
	{	
		QString css(lua_tostring(L, -1));
		int h = lua_tonumber(L, -2);
		int w = lua_tonumber(L, -3);
		std::string resourceName(lua_tostring(L, -4));
		std::string id(lua_tostring(L, -5));
		const char *tag = lua_tostring(L, -6);
		
		lua_pop(L, 6);
		
		ToolBar *toolbar = ToolBar::getInstance(tag);
		
		toolbar->addLabel(id, resourceName, "", w, h, css);
		
		return 0;
	}
	
	static int set_toolbar_label(lua_State *L)
	{	
		QString text(lua_tostring(L, -1));
		std::string id(lua_tostring(L, -2));
		const char *tag = lua_tostring(L, -3); 
		lua_pop(L, 3);
	
		ToolBar *toolbar = ToolBar::getInstance(tag);
	
		toolbar->setLabel(id, text);
				
		return 0;
	}
	
	static int add_toolbar_separator(lua_State *L)
	{	
	
		const char *tag = lua_tostring(L, -1);	
		lua_pop(L, 1);
		
		ToolBar *toolbar = ToolBar::getInstance(tag);
		
		toolbar->addseparator();
		
		return 0;
	}
	
	static int toolbar_enable(lua_State *L)
	{		
		const char *id = lua_tostring(L, -1);
		const char *tag = lua_tostring(L, -2); 	
		lua_pop(L, 2);	
		
		ToolBar *toolbar = ToolBar::getInstance(tag);
		
		toolbar->enabled(id, true);
		
		return 0;
	}
	
	static int set_toolbar_zoom(lua_State *L)
	{
		int defaultZoom = lua_tonumber(L, -1);
		lua_pop(L, 1);
	
		int top = lua_gettop(L);
		
		for (int i = 1; i <= top; i++) 
		{
			int zoom = lua_tointeger(L, i);
		
			ToolBar::getInstance("main")->sizeBox->insert(zoom);
						
					
		}

		lua_pop(L, top);
				
		ToolBar::getInstance("main")->ToolBar::sizeBox->setDefault(defaultZoom);
		float fraction = (float)defaultZoom / 100.0;
		ToolBar::getInstance("main")->ToolBar::sizeBox->parent->zoom(fraction);	
		
		return 0;
	}
	
	static int toggle_pin(lua_State *L)
	{		
		string id = string(lua_tostring(L, -1));
		const char *tag = lua_tostring(L, -2); 	
		lua_pop(L, 2);	
		
		ToolBar *toolbar = ToolBar::getInstance(tag);
		
		toolbar->toolbarPinned = !toolbar->toolbarPinned;
		
		Window *window = (Window *)toolbar->parent();
	
		if (toolbar->toolbarPinned)
		{
			toolbar->setImageButton(id, "__pin", "");
			Qt::WindowFlags flags = window->windowFlags();
			window->setWindowFlags(flags | Qt::WindowStaysOnTopHint);
			window->show();
		}
		else
		{
			toolbar->setImageButton(id, "__unpin", "");
			Qt::WindowFlags flags = window->windowFlags();
			window->setWindowFlags(flags & ~Qt::WindowStaysOnTopHint);
			window->show();
		}
		
		return 0;
	}
	
	static int show_menu(lua_State *L)
	{		
		const char *menuid = lua_tostring(L, -1); 
		const char *tag = lua_tostring(L, -2); 	
		lua_pop(L, 2);	
		
		ToolBar *toolbar = ToolBar::getInstance(tag);
		
		toolbar->showMenu(menuid);
		
		return 0;
	}
	
	static int toolbar_disable(lua_State *L)
	{		
		const char *id = lua_tostring(L, -1);
		const char *tag = lua_tostring(L, -2); 	
		lua_pop(L, 2);
		
		ToolBar *toolbar = ToolBar::getInstance(tag);
		
		toolbar->enabled(id, false);
		
		return 0;
	}
	
	static int create_window(lua_State *L)
	{	
		
		int h = lua_tonumber(L, -1);
		int w = lua_tonumber(L, -2); 
		const char *type = lua_tostring(L, -3);	
		bool anyScroll = lua_toboolean(L, -4);	
		std::string background(lua_tostring(L, -5));
		QString title(lua_tostring(L, -6));
		const char *tag = lua_tostring(L, -7);
		lua_pop(L, 7);
		
		Window *window = new Window(Window::getInstance("main"), title, std::string(tag), std::string(type), anyScroll, w, h);	
			
		window->frame->setBackground(background);
		
		//window->setStyleSheet("background-color:" + QString::fromStdString(background) + ";");
		
		
		return 0;
	}
	
	static int toggle_window(lua_State *L)
	{
		const char *tag = lua_tostring(L, -1);
		lua_pop(L, 1);
		
		Window *window = Window::getInstance(tag);
		if (window != nullptr)
			window->showWindow();
		
		return 0;
	}
	
	static int set_frame(lua_State *L)
	{		
		int h = lua_tonumber(L, -1);
		int w = lua_tonumber(L, -2);
		int y = lua_tonumber(L, -3);
		int x = lua_tonumber(L, -4);
		const char *tag = lua_tostring(L, -5);

		lua_pop(L, 5);
		
		Window *window = Window::getInstance(tag);
		window->frame->startWidth = w;
		window->frame->startHeight = h;
		window->frame->resize(w, h);
		window->move(x, y);
		//window->setSingleRowed(x, y, w , h);
		
		return 0;
	}
	
	static int set_font_options(lua_State *L)
	{
		std::string color(lua_tostring(L, -1));
		std::string weight(lua_tostring(L, -2));
		int size = lua_tonumber(L, -3);
		QString font(lua_tostring(L, -4));
		const char *tag = lua_tostring(L, -5);
		
		lua_pop(L, 5);
		
		Window *window = Window::getInstance(tag);
		window->setOptions(font, size, weight, color);
		
		return 0;		
	}
	
	static int add_window_button(lua_State *L)
	{		
		std::string luaScript(lua_tostring(L, -1));
		std::string resourceName(lua_tostring(L, -2));
		int y = lua_tonumber(L, -3);
		int x = lua_tonumber(L, -4);		
		std::string tag(lua_tostring(L, -5));
		const char *windowtag = lua_tostring(L, -6);

		lua_pop(L, 6);
		
		Window *window = Window::getInstance(windowtag);
		
		
		// x,y are center coordinates, transform them to upper-left
		
		if (!resourceName.empty())
		{
			QSize size = io->getSize(resourceName);
			x = x - std::round(size.width() / 2);
			y = y - std::round(size.height() / 2);
		}
		
		 
		(void)new Window::PushButton(window, tag, resourceName, "", x, y, luaScript);
		
		
		return 0;
	}
	
	static int add_window_image(lua_State *L)
	{	
		const char *resourceName = lua_tostring(L, -1);
		int h = lua_tonumber(L, -2);
		int w = lua_tonumber(L, -3);
		int y = lua_tonumber(L, -4);
		int x = lua_tonumber(L, -5);				
		std::string widgetTag(lua_tostring(L, -6));		
		const char *windowTag = lua_tostring(L, -7); 
		
		lua_pop(L, 7);
		
		Window *window = Window::getInstance(windowTag);
		
		
		// x,y are center coordinates, transform them to upper-left
		QSize size;
		
		if (!std::string(resourceName).empty())
			size = io->getSize(resourceName);
		else
			size = QSize(w, h);
			 
		x = x - std::round(size.width() / 2);
		y = y - std::round(size.height() / 2);
		
	
		new Window::Label(window, widgetTag, x, y, w, h, std::string(resourceName));
		
		return 0;
	}
	
	static int add_window_label(lua_State *L)
	{	
		int h = lua_tonumber(L, -1);
		int w = lua_tonumber(L, -2);
		int y = lua_tonumber(L, -3);
		int x = lua_tonumber(L, -4);				
		std::string widgetTag(lua_tostring(L, -5));	
		const char *pageTag = lua_tostring(L, -6);	
		const char *windowTag = lua_tostring(L, -7);
			
		lua_pop(L, 7);
		
		
		Window *window = Window::getInstance(windowTag);
		Configure::ChoicePage *page = Configure::getPage(pageTag);

		new Window::Text(page, window, widgetTag, x, y, w, h);
		
		return 0;
	}
	
	static int set_text(lua_State *L)
	{	
		QString text(lua_tostring(L, -1));
		std::string id(lua_tostring(L, -2));
		const char *tag = lua_tostring(L, -3);
		   
		lua_pop(L, 3);
		
		
		Window *window = Window::getInstance(tag);
		 
		
		try
		{
			window->widgets.at(id);
		}
		catch (const std::out_of_range &e)
		{
			Luau::error(39, 2, id, window); 
			return 0;
		}
		
	
		((Window::Text *)window->widgets[id])->set(text);
				
		return 0;
	}
	
	static int set_label_text(lua_State *L)
	{	
		QString text(lua_tostring(L, -1));
		std::string id(lua_tostring(L, -2));
		const char *tag = lua_tostring(L, -3); 
		lua_pop(L, 3);
		
		
		
		Window *window = Window::getInstance(tag);
		
		try
		{
			window->widgets.at(id);
		}
		catch (const std::out_of_range &e)
		{
			Luau::error(39, 2, id, window); 
			return 0;
		}
		
	
		((Window::Label *)window->widgets[id])->setText(text);
				
		return 0;
	}
	
	static int get_label_text(lua_State *L)
	{	
		std::string id(lua_tostring(L, -1)); 
		lua_pop(L, 1);
		
		const char *text = "";
		lua_pushstring(L, text);	
				
		return 1;
	}
	
	static int set_label_options(lua_State *L)
	{
		QString alingment(lua_tostring(L, -1));
		QString border(lua_tostring(L, -2));
		std::string id(lua_tostring(L, -3));
		const char *tag = lua_tostring(L, -4);
		
		lua_pop(L, 4);
		
		Window *window = Window::getInstance(tag);
		((Window::Text *)window->widgets[id])->setOptions(alingment, border);
		
		return 0;		
	}
	
	static int create_choicepage(lua_State *L)
	{
		std::string tag(lua_tostring(L, -1));	
		lua_pop(L, 1);
		
		new Configure::ChoicePage(config, tag);
		
		return 0;		
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
		const char *tag = lua_tostring(L, -9); 
		
		lua_pop(L, 9);
		
		(void)new Window::CheckBox(id, (Window *)Window::getInstance(tag), x, y, w, h, text, script, css);
		
		return 0;
	}
	
	static int get_checkbox_checked(lua_State *L)
	{	
		std::string box(lua_tostring(L, -1)); 
		const char *window = lua_tostring(L, -2); 
		lua_pop(L, 2);
		
		//bool value = ((Window::Frame *)((Window *)Window::getInstance(window))->frame)->checkboxes[box]->get();			
		bool value = true;
		lua_pushboolean(L, (int)value);	
				
		return 1;
	}
	
	static int get_text_input(lua_State *L)
	{	
		QString start(lua_tostring(L, -1));
		QString title(lua_tostring(L, -2));
		lua_pop(L, 2);
		
		QString text = Window::textInput(title, "Text value:", start);
		lua_pushstring(L, text.toStdString().c_str());	
				
		return 1;
	}
	
	static int create_combobox(lua_State *L)
	{	
		
		int y = lua_tonumber(L, -1);
		int x = lua_tonumber(L, -2);
		QString placeholderText(lua_tostring(L, -3));				
		std::string widgetTag(lua_tostring(L, -4));
		const char *parentTag = lua_tostring(L, -5); 		
		const char *windowTag = lua_tostring(L, -6);
		
		
		lua_pop(L, 6);
		
		Window *window = Window::getInstance(windowTag);
		QWidget *page = (QWidget *)Configure::getPage(parentTag);
		
		if (page == nullptr)
			page = (QWidget *)window;
	
		Window::ChoiceBox *box = new Window::ChoiceBox(page, window, widgetTag, x, y);
		
		box->setPlaceholderText(placeholderText);
		
		return 0;
	}
	
	
	static int add_combobox(lua_State *L)
	{
		QString text(lua_tostring(L, -1));
		QString data(lua_tostring(L, -2));
		std::string id(lua_tostring(L, -3));		
		const char *windowTag = lua_tostring(L, -4);
		 
		lua_pop(L, 4);
		
		Window *window = Window::getInstance(windowTag);
		
		((Window::ChoiceBox *)window->widgets[id])->add(text, data);
		
		
		return 0;
	}
	
	static int get_title(lua_State *L)
	{	
		const char *tag = lua_tostring(L, -1);
		lua_pop(L, 1);
		
		Window *window = Window::getInstance(tag);
		
		lua_pushstring(L, window->title.c_str());	
				
		return 1;
	}	
	
	static int hoover_ShowMap(lua_State *L)
	{	
		
		if (lua_isboolean(L, -1))
			Counter::hooverShowMap = lua_toboolean(L, -1);	
			
		lua_pop(L, 1);
		
		return 0;
	}
	
	static int hoover_ShowPlace(lua_State *L)
	{	
		
		if (lua_isboolean(L, -1))
			Counter::hooverShowPlace = lua_toboolean(L, -1);	
			
		lua_pop(L, 1);
			
		return 0;
	}
	
	static int get_corner(lua_State *L)
	{	
		Counter *counter;
		QSize size;
		
		if (lua_isnumber(L, -1))
		{
			int id  = lua_tonumber(L, -1);
			try
			{
				Counter::counters.at(id);
			}
			catch (const std::out_of_range &e)
			{
				Luau::error(30, 1, std::to_string(id).c_str()); 
				std::exit(1);
			}
			
			size = Counter::getCorner(id);
		}
		else
		if 	(lua_isstring(L, -1))
		{
			std::string name = std::string(lua_tostring(L, -1));
			try
			{
				Counter::repository.at(name);
			}
			catch (const std::out_of_range &e)
			{
				Luau::error(30, 1, name.c_str()); 
				std::exit(1);
			}
			
			size = Counter::getCorner(name);
		}
		else
		{
			Luau::error(29, 0); 
			std::exit(1);
		}
		
		lua_pop(L, 1);
		
		
		lua_pushnumber(L, size.width());
		lua_pushnumber(L, size.height());
			
		return 2;
	}
	
	static int get_resource(lua_State *L)
	{		
		io->transfer_resource_keys();	
		return 0;
	}
	
	static int load_setup(lua_State *L)
	{	
		
		std::string filename(lua_tostring(L, -1)); 			
		lua_pop(L, 1);
		
		io->loadSetUp(filename);
			
		return 0;
	}
	
	static int repository_visibility(lua_State *L)
	{		
		if (lua_isboolean(L, -1))			
			repositoryWindow->visibility(lua_toboolean(L, -1));	
			
		lua_pop(L, 1);
		
		return 0;
	}
	
	static int log_visibility(lua_State *L)
	{	
		if (lua_isboolean(L, -1))			
			logWindow->visibility(lua_toboolean(L, -1));	
			
		lua_pop(L, 1);
		
		return 0;
	}
	
	static int set_map_zoom(lua_State *L)
	{
		float maxZoom = lua_tonumber(L, -1);
		float minZoom = lua_tonumber(L, -2);
		const char *tag = lua_tostring(L, -3); 
		lua_pop(L, 3);
		
		Window *window = Window::getInstance(tag);
	
		window->minZoom = minZoom;
		window->maxZoom = maxZoom;				
		
		
		return 0;
	}
	
	static int set_grid(lua_State *L)
	{	
		std::string zone(lua_tostring(L, -1)); 
		lua_pop(L, 1);	
		
		const char *tag = lua_tostring(L, -1); 
		lua_pop(L, 1);	
		Window *window = Window::getInstance(tag);
		
		(void)lua_gettable(L, -2);
		
		if (lua_istable(L, -1))
		{	
			qint32 dummy;		
			Counter::Table grid = getTable(dummy);
			
			window->frame->setGrid(zone, &grid);
		}	
		
		lua_pop(L, 2);
		
		
		return 0;
	}
	
	static int set_borders(lua_State *L)
	{	
		std::string zone(lua_tostring(L, -1)); 
		lua_pop(L, 1);
		
		const char *tag = lua_tostring(L, -1); 
		lua_pop(L, 1);	
		Window *window = Window::getInstance(tag);
		
		(void)lua_gettable(L, -2);
		
		if (lua_istable(L, -1))
		{			
			qint32 dummy;
			Counter::Table borders = getTable(dummy);
			
			window->frame->setBorders(zone, &borders);
		}	
		
		lua_pop(L, 2);
		
		
		return 0;
	}
	
	static int show_grid(lua_State *L)
	{	
		
		if (lua_isboolean(L, -1))
			CentralFrame::showGrid = lua_toboolean(L, -1);	
			
		lua_pop(L, 1);
		
		return 0;
	}

	static int specify_grid(lua_State *L)
	{	
		
		int alpha = 255, red = 255, green = 0, blue = 0;
		
		if (lua_isnumber(L, -1))
		{	
			alpha = lua_tonumber(L, -1);
			lua_pop(L, 1);	
		}
		
		if (lua_isnumber(L, -1))
		{	
			blue = lua_tonumber(L, -1);
			lua_pop(L, 1);	
		}
		
		if (lua_isnumber(L, -1))
		{	
			green = lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
		
		if (lua_isnumber(L, -1))
		{	
			red = lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
		
		CentralFrame::gridColor = QColor(red, green, blue, alpha);
		
		
		if (lua_isnumber(L, -1))
		{	
			CentralFrame::gridRadius = lua_tonumber(L, -1);	
			lua_pop(L, 1);
		}
		
		if (lua_isnumber(L, -1))
		{	
			Scale::minScaleGrid = lua_tonumber(L, -1);	
			lua_pop(L, 1);
		}	
			
		
		
		return 0;
	}
		
	static int toggle_painting(lua_State *L)
	{		
		CentralFrame::allowPainting = !CentralFrame::allowPainting;
		
		return 0;
	}
	
	static int is_painting(lua_State *L)
	{
		lua_pushboolean(L, (int)CentralFrame::allowPainting);
				
		return 1;
	}
	
	static int roll_die(lua_State *L)
	{		
		Window::rollDie();
		
		return 0;
	}
	
	static int get_nick(lua_State *L)
	{
		std::string text = "[" + Settings::myNick() + "] ";
		
		lua_pushstring(L, text.c_str());

		return 1;
	}
	
	static int log_print(lua_State *L)
	{	
		std::string text(lua_tostring(L, -1));
		lua_pop(L, 1);	
		
		logWindow->textbox->appendHtml(QString::fromStdString(text));

		return 0;
	}
	
	static int net_connected(lua_State *L)
	{	
		
		bool value = io->server->connected();
		lua_pushboolean(L, (int)value);	
				
		return 1;
	}
	
	static int ask_sync(lua_State *L)
	{	
		Window *parent = Window::getInstance("main");
		
		QMessageBox askBox(parent);
		askBox.setWindowTitle("Confirm synchronizing");
		askBox.setText("Do you want to set your map to your opponent's map?<br>All content of your own map will be lost.");
		askBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		askBox.setDefaultButton(QMessageBox::No);
		
		bool answer = false;
		
		if (askBox.exec() == QMessageBox::Yes)
		{	
			io->closeGame();
			answer = true;
		}
		
		
		lua_pushboolean(L, (int)answer);	
				
		return 1;
	}
	
	static int send_table(lua_State *L)
	{	
		
		qint32 type = lua_tointeger(L, -1); 
		lua_pop(L, 1);
		
		int id = lua_tointeger(L, -1);
		
		(void)lua_gettable(L, -2);
		
		if (lua_istable(L, -1))
		{	
			qint32 tableSize = 0;		
			Counter::Table table = getTable(tableSize);
			IO::server->writeData(table, tableSize, type);				
		}
		else
			Luau::error(31, 1, id);
			
			
		
		lua_pop(L, 2);
		
		return 0;
	}
	
	static int remove_undo(lua_State *L)
	{	
		Luau::resetState();
		
		return 0;
	}
	
	static int get_top_stack(lua_State *L)
	{	
		
		int cy = lua_tointeger(L, -1);
		int cx = lua_tointeger(L, -2);
		std::string window(lua_tostring(L, -3)); 
		
		lua_pop(L, 3);
		
		int id = Counter::getTop(window, cx, cy);
			
		lua_pushnumber(L, id);
			
		return 1;
	}
	
	
	
	
	
}	// extern "C"







Counter::Table getTable(qint32 &size)
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
			size += std::string(str).size() + sizeof(int);
		}
		else
			if (lua_isnumber(L, -2))
			{
				lua_Integer n = lua_tointeger(L, -2);			
				left = n;
				size += sizeof(lua_Integer) + sizeof(int);	
			}
			
		
		
		
		// value
		
		if (lua_isboolean(L, -1))
		{
			int n = lua_toboolean(L, -1);
			if (n)
				right = true; 	
			else
				right = false;
			size += sizeof(lua_Number) + sizeof(int);
		}
		else
			if (lua_type(L, -1) == LUA_TSTRING)
			{
				const char *str = lua_tostring(L, -1);
				right = std::string(str);
				size += std::string(str).size() + sizeof(int);
			}
			else
				if (lua_isnumber(L, -1))
				{
					lua_Number n = lua_tonumber(L, -1);				
					right = n;
					size += sizeof(lua_Number) + sizeof(int);
				}
				else
					if (lua_isfunction (L, -1))
					{
						lua_pop(L, 1);
						continue;
					}
					else
						if (lua_istable(L, -1))
						{
							size += sizeof(qint32) + sizeof(int);
							right = getTable(size);
						}



		
		table[left] = right;
		
		
		lua_pop(L, 1);
		
	}
		
	return table;
	
}



/*
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
*/




Luau::PopupEntries entries;


	
void getEntries()
{

	lua_pushnil(L);
	
	Luau::Popupentry e = {"", "", "", "", "", "", false};
	
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
			
			if (lua_isstring(L, -2))
			{
				std::string s(lua_tostring(L, -2));
			
				if (s == "menuname" || s == "menutrait" || s == "menuclick" || s == "menuid")
				{		
					
					if (s == "menuname")
					{ 
						if (!e.entryname.empty())
						{
							entries.push_back(e);
							e = {"", "", "", "", "", "", false};	
						}							
						e.entryname = std::string(lua_tostring(L, -1));
					}
					if (s == "menutrait")
						e.entrytrait = std::string(lua_tostring(L, -1));
					if (s == "menuclick")
						e.entryaction = std::string(lua_tostring(L, -1));
					if (s == "menuid")
						e.entryid = std::string(lua_tostring(L, -1));
						
				}
				if (s == "menufield")
					e.entryfield = std::string(lua_tostring(L, -1));
				if (s == "menuactions")
					e.entryactions = std::string(lua_tostring(L, -1));
			}
			
			
		}
		
		if (lua_isfunction(L, -1))
		{
			
			if (lua_isstring(L, -2))
			{
				std::string s(lua_tostring(L, -2));
			
				if (s == "menushow")
					e.entrytest = true;	
			}
			
		}	
		
		if (lua_istable(L, -1))
			getEntries();		
			
		
		lua_pop(L, 1);
			
	}
	
	
	if (!e.entryname.empty())
		entries.push_back(e);
							
	
}



    
Luau::PopupEntries Luau::getTraits(const char *tag, const char *id)
{

	lua_getglobal(L, "getTraits");
	lua_pushstring(L, tag);
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
	
	
	qint32 dummy;
	Counter::Table state = getTable(dummy);
	
	lua_pop(L, 1);
		
	
	return state;
	
}




Counter::Table Luau::getTrait(const char *window, const char *id)
{
	
	lua_getglobal(L, "getTraits");
	lua_pushstring(L, window);
	lua_pushstring(L, id);
		
	lua_pcall(L, 2, 1, 0);
	
	
	qint32 dummy;
	Counter::Table state = getTable(dummy);
	
	lua_pop(L, 1);
		
	
	return state;
	
}


bool Luau::findTrait(const char *id, const char *trait, const char *field, std::string &value)
{
	
	lua_getglobal(L, "findTrait");
	lua_pushstring(L, id);
	lua_pushstring(L, trait);
	lua_pushstring(L, field);
		
	lua_pcall(L, 3, 2, 0);

	
	int result = lua_toboolean (L, -1);
	value = std::string(lua_tostring (L, -2));
	
	lua_pop(L, 2);
	
	
	return (result != 0);
		
	
}



Counter::Table Luau::getDecks()
{
	lua_getglobal(L, "getKeys");	
	lua_pcall(L, 0, 1, 0);
	
	
	qint32 dummy;
	Counter::Table keys = getTable(dummy);
	lua_pop(L, 1);

	return keys;
}


Counter::Table Luau::getDeck(const char *id)
{
	lua_getglobal(L, "getDeck");
	lua_pushstring(L, id);	
	lua_pcall(L, 1, 1, 0);
	
	
	qint32 dummy;
	Counter::Table table = getTable(dummy);
	lua_pop(L, 1);
	
	return table;
}


Luau::Turn Luau::getTurn()
{
	lua_getglobal(L, "getTurn");
	lua_pcall(L, 0, 2, 0);
	
	Turn turn;
	
	turn.phase = lua_tointeger(L, -1);
	turn.turn = lua_tointeger(L, -2);
	lua_pop(L, 2);		
	
	return turn;
}


void Luau::setTurn(Turn turn)
{
	lua_getglobal(L, "setTurn");
	lua_pushnumber(L, turn.turn);
	lua_pushnumber(L, turn.phase);
	lua_pcall(L, 2, 0, 0);
}



void Luau::noAction(bool value)
{
	lua_getglobal(L, "setNoAction");
	lua_pushboolean(L, (value == true ? 1 : 0));
	lua_pcall(L, 1, 0, 0);
}


void Luau::ifAction()
{
	lua_getglobal(L, "ifAction");
	lua_pcall(L, 0, 0, 0);
}


void Luau::updateId()
{
	lua_getglobal(L, "updateid");
	lua_pcall(L, 0, 0, 0);
}


void Luau::updateTopZorder()
{
	lua_getglobal(L, "updatetopzorder");
	lua_pcall(L, 0, 0, 0);
}


void Luau::updateBottomZorder()
{
	lua_getglobal(L, "updatebottomzorder");
	lua_pcall(L, 0, 0, 0);
}


Counter::Table Luau::findAllSides()
{
	lua_getglobal(L, "findallsides");
	lua_pcall(L, 0, 1, 0);
	
	qint32 dummy;
	Counter::Table table = getTable(dummy);
	lua_pop(L, 1);
	
	return table;
}


void Luau::doAction(const char *window, const char *id, const char *trait, const char *field, const char *name, const char *actionId)
{
	
	lua_getglobal(L, "action");
	lua_pushstring(L, window);
	lua_pushstring(L, id);
	lua_pushstring(L, trait);
	lua_pushstring(L, field);
	lua_pushstring(L, name);
	
	if (actionId == nullptr)
		lua_pcall(L, 5, 0, 0);
	else
	{
		lua_pushstring(L, actionId);
		lua_pcall(L, 6, 0, 0);
	}
	
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


void Luau::doLog(const char *key, const char *text)
{
	
	lua_getglobal(L, "logging");
	lua_pushstring(L, key);
	lua_pushstring(L, text);
	
	
	lua_pcall(L, 2, 0, 0);
	
}



bool Luau::doTest(const char *tag, const char *trait, const char *id)
{
	
	lua_getglobal(L, "tests");
	lua_pushstring(L, tag);
	lua_pushstring(L, trait);
	lua_pushstring(L, id);	
	
	lua_pcall(L, 3, 1, 0);
	
	int result = lua_toboolean (L, -1);
	
	lua_pop(L, 1);
	
	return (result != 0);
	
}



void Luau::doCreate(const char *fromId, const char *trait)
{
	lua_getglobal(L, "create");
	lua_pushstring(L, fromId);
	lua_pushstring(L, trait);
	
	lua_pcall(L, 2, 0, 0);
	
}


void Luau::doDelete(const char *id)
{
	lua_getglobal(L, "delete");
	lua_pushstring(L, id);
	
	lua_pcall(L, 1, 0, 0);
}



void Luau::reportText(const char *type, const char *text)
{
	lua_getglobal(L, "reporttext");
	lua_pushstring(L, type);
	lua_pushstring(L, text);
	
	lua_pcall(L, 2, 0, 0);
}


void Luau::reportMove(int id, const char *name, const char *fromtag, const char *totag, int fromX, int fromY, int toX, int toY)
{

	lua_getglobal(L, "reportmove");
	lua_pushnumber(L, id);
	lua_pushstring(L, name);
	lua_pushstring(L, fromtag);
	lua_pushstring(L, totag);
	lua_pushnumber(L, fromX);
	lua_pushnumber(L, fromY);
	lua_pushnumber(L, toX);
	lua_pushnumber(L, toY);
	
	lua_pcall(L, 8, 0, 0);
	
}





// hooks

bool Luau::beforeDrag(const char *id)
{
	lua_getglobal(L, "beforeDrag");
	lua_pushstring(L, id);
	lua_pcall(L, 1, 1, 0);
	
	
	int r = lua_toboolean (L, -1);
	
	lua_pop(L, 1);
	
	return (r != 0);
	
}


bool Luau::afterDrag(const char *window, const char *id, int dx, int dy, int &x, int &y, int &cx, int &cy)
{	
	lua_getglobal(L, "afterDrag");
	lua_pushstring(L, window);
	lua_pushstring(L, id);
	lua_pushnumber(L, dx);
	lua_pushnumber(L, dy);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pcall(L, 6, 5, 0);	

	cy = (int)lua_tonumber (L, -1);
	cx = (int)lua_tonumber (L, -2);
	y = (int)lua_tonumber (L, -3);
	x = (int)lua_tonumber (L, -4);
	bool flag = lua_toboolean (L, -5);

	lua_pop(L, 5);
	
	return flag;
	
}

void Luau::dropped(const char *tag, const char *id, int x, int y)
{
	lua_getglobal(L, "dropped");
	lua_pushstring(L, tag);
	lua_pushstring(L, id);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pcall(L, 4, 0, 0);	
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


bool Luau::selectable(const char *id)
{
	lua_getglobal(L, "selectable");
	lua_pushstring(L, id);
	lua_pcall(L, 1, 1, 0);
	
	int result = lua_toboolean (L, -1);
	
	lua_pop(L, 1);
	
	return (result != 0);	
}


void Luau::feed(int x, int y)
{
	lua_getglobal(L, "feed");
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pcall(L, 2, 0, 0);	
}


bool Luau::isFeeding()
{
	lua_getglobal(L, "isFeeding");
	lua_pcall(L, 0, 1, 0);
	
	int result = lua_toboolean (L, -1);
	
	lua_pop(L, 1);
	
	return (result != 0);
}


void Luau::stopFeed()
{
	lua_getglobal(L, "stopFeed");
	lua_pcall(L, 0, 0, 0);
}


bool Luau::menu(const char *id, const char *name, const char *marker)
{
	lua_getglobal(L, "menu");
	lua_pushstring(L, id);
	lua_pushstring(L, name);
	lua_pushstring(L, marker);
	lua_pcall(L, 3, 1, 0);
	
	int result = lua_toboolean (L, -1);
	
	lua_pop(L, 1);
	
	return (result != 0);	
}


bool Luau::menucounter(int id)
{
	lua_getglobal(L, "menucounter");
	lua_pushnumber(L, id);
	lua_pcall(L, 1, 1, 0);
	
	int result = lua_toboolean (L, -1);
	
	lua_pop(L, 1);
	
	return (result != 0);	
}


void Luau::pointoffset(const char *tag, int cx, int cy, float &xoff, float &yoff, int &xopen, int &yopen, int &shown)
{
	lua_getglobal(L, "pointoffset");
	lua_pushstring(L, tag);
	lua_pushnumber(L, cx);
	lua_pushnumber(L, cy);
	lua_pcall(L, 3, 5, 0);
	
	shown = (int)lua_tonumber (L, -1);
	yopen = (int)lua_tonumber (L, -2);
	xopen = (int)lua_tonumber (L, -3);
	yoff = lua_tonumber (L, -4);
	xoff = lua_tonumber (L, -5);	
	
	lua_pop(L, 5);
}



void Luau::refresh()
{
	lua_getglobal(L, "refresh");
	lua_pcall(L, 0, 0, 0);
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



void Luau::copyCounter(std::string fromId, const char *toId, int zorder, std::string window, int x, int y, int cx, int cy)
{
	
	lua_getglobal(L, "copyCounter");
	
	if (!io->isResource(fromId))
	{
		fromId = "'" + fromId + "'";
		Luau::error(7, 1, fromId.c_str());
		std::exit(1);
	}
	
	lua_pushstring(L, fromId.c_str());
	lua_pushstring(L, toId);
	lua_pushnumber(L, zorder);
	lua_pushstring(L, window.c_str());
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pushnumber(L, cx);
	lua_pushnumber(L, cy);
	lua_pcall(L, 8, 0, 0);
	
}


void Luau::moveCounter(const char *window, const char *id, int zorder, int x, int y, int cx, int cy)
{
	
	lua_getglobal(L, "moveCounter");
	lua_pushstring(L, window);
	lua_pushstring(L, id);
	lua_pushnumber(L, zorder);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pushnumber(L, cx);
	lua_pushnumber(L, cy);
	lua_pcall(L, 7, 0, 0);
	
}


void Luau::copyCard(const char *fromId, const char *toId, const char *oldtoId, int zorder, int x, int y)
{
	
	lua_getglobal(L, "copyCard");
	lua_pushstring(L, fromId);
	lua_pushstring(L, toId);
	lua_pushstring(L, oldtoId);
	lua_pushnumber(L, zorder);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pcall(L, 6, 0, 0);
	
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


void Luau::updateCounter(Counter::Table table)
{
	lua_getglobal(L, "updateCounter");
	
	stackTable(table);

	lua_pcall(L, 1, 0, 0);

}



void Luau::confirm()
{
	lua_getglobal(L, "confirm");
	lua_pcall(L, 0, 0, 0);
}


void Luau::ask()
{
	lua_getglobal(L, "ask");
	lua_pcall(L, 0, 0, 0);
}



void Luau::synchronize()
{
	lua_getglobal(L, "sync");
	lua_pcall(L, 0, 0, 0);
}



void Luau::done()
{
	lua_getglobal(L, "done");
	lua_pcall(L, 0, 0, 0);
}



void Luau::stagedone()
{
	lua_getglobal(L, "stagedone");
	lua_pcall(L, 0, 0, 0);
}



void Luau::loadDeck(Counter::Table table)
{
	lua_getglobal(L, "loadDeck");
	
	stackTable(table);

	lua_pcall(L, 1, 0, 0);
}




void Luau::loadLog(Counter::Table table)
{
	lua_getglobal(L, "loadLog");
	
	stackTable(table);

	lua_pcall(L, 1, 0, 0);	
}




//

void Luau::setResourceKey(const char *id)
{
	lua_getglobal(L, "setResourceKey");
	lua_pushstring(L, id);
	lua_pcall(L, 1, 0, 0);
}


void Luau::updatePos(const char *window, const char *id, int zorder, int x, int y, int cx, int cy)
{
	lua_getglobal(L, "updatePos");
	lua_pushstring(L, window);
	lua_pushstring(L, id);
	lua_pushnumber(L, zorder);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pushnumber(L, cx);
	lua_pushnumber(L, cy);
	lua_pcall(L, 7, 0, 0);
}


void Luau::updateMoved(const char *id, bool moved)
{
	lua_getglobal(L, "updateMoved");
	lua_pushstring(L, id);
	lua_pushboolean(L, (moved == true ? 1 : 0));
	lua_pcall(L, 2, 0, 0);
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


void Luau::resetBase()
{
	lua_getglobal(L, "resetBase");
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

QString Luau::mapPlace(const char *window, int cx, int cy)
{
	lua_getglobal(L, "mapPlace");
	
	lua_pushstring(L, window);
	lua_pushnumber(L, cx);
	lua_pushnumber(L, cy);
	
	lua_pcall(L, 3, 1, 0);
	
	const char *str = lua_tostring(L, -1); 		
	lua_pop(L, 1);
	
	return QString::fromUtf8(str);	
}







void Luau::startVM()
{
	
	L = luaL_newstate();
	luaL_openlibs(L);
	
	
	(void)std::setlocale(LC_NUMERIC, "en_US.UTF-8");
	
	compileScript();
	

	lua_pushcfunction(L, setCounterDragAlpha, "setCounterDragAlpha");
	lua_setglobal(L, "setCounterDragAlpha");
	
	lua_pushcfunction(L, set_background_mapID, "set_background_mapID");
	lua_setglobal(L, "set_background_mapID");
	
	lua_pushcfunction(L, set_padding, "set_padding");
	lua_setglobal(L, "set_padding");
	
	lua_pushcfunction(L, set_mask, "set_mask");
	lua_setglobal(L, "set_mask");
	
	lua_pushcfunction(L, set_image, "set_image");
	lua_setglobal(L, "set_image");
	
	lua_pushcfunction(L, create_class, "create_class");
	lua_setglobal(L, "create_class");
	
	lua_pushcfunction(L, create_class_copy, "create_class_copy");
	lua_setglobal(L, "create_class_copy");
	
	lua_pushcfunction(L, update_class, "update_class");
	lua_setglobal(L, "update_class");
	
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
	
	lua_pushcfunction(L, setGUI, "setGUI");
	lua_setglobal(L, "setGUI");
	
	lua_pushcfunction(L, get_size, "get_size");
	lua_setglobal(L, "get_size");
	
	// repository
	
	lua_pushcfunction(L, set_root, "set_root");
	lua_setglobal(L, "set_root");
	
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
	
	lua_pushcfunction(L, image_item, "image_item");
	lua_setglobal(L, "image_item");
	
	lua_pushcfunction(L, html_item, "html_item");
	lua_setglobal(L, "html_item");
	
	lua_pushcfunction(L, scale_frame, "scale_frame");
	lua_setglobal(L, "scale_frame");
	
	lua_pushcfunction(L, scale_pane, "scale_pane");
	lua_setglobal(L, "scale_pane");
	
	lua_pushcfunction(L, repository_visibility, "repository_visibility");
	lua_setglobal(L, "repository_visibility");
	
	lua_pushcfunction(L, log_visibility, "log_visibility");
	lua_setglobal(L, "log_visibility");
	
	lua_pushcfunction(L, set_row_length, "set_row_length");
	lua_setglobal(L, "set_row_length");
	
	lua_pushcfunction(L, is_owner, "is_owner");
	lua_setglobal(L, "is_owner");
	
	lua_pushcfunction(L, is_side, "is_side");
	lua_setglobal(L, "is_side");
	
	// gui
	
	lua_pushcfunction(L, create_button, "create_button");
	lua_setglobal(L, "create_button");
	
	lua_pushcfunction(L, delete_button, "delete_button");
	lua_setglobal(L, "delete_button");
	
	lua_pushcfunction(L, inc_id, "inc_id");
	lua_setglobal(L, "inc_id");
	
	lua_pushcfunction(L, inc_top_zorder, "inc_top_zorder");
	lua_setglobal(L, "inc_top_zorder");
	
	lua_pushcfunction(L, inc_bottom_zorder, "inc_bottom_zorder");
	lua_setglobal(L, "inc_bottom_zorder");
	
	lua_pushcfunction(L, top_zorder, "top_zorder");
	lua_setglobal(L, "top_zorder");

	lua_pushcfunction(L, bottom_zorder, "bottom_zorder");
	lua_setglobal(L, "bottom_zorder");
	
	lua_pushcfunction(L, next_id, "next_id");
	lua_setglobal(L, "next_id");
	
	lua_pushcfunction(L, reset_idz, "reset_idz");
	lua_setglobal(L, "reset_idz");
	
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
	
	lua_pushcfunction(L, create_toolbar, "create_toolbar");
	lua_setglobal(L, "create_toolbar");
	
	lua_pushcfunction(L, add_toolbar_button, "add_toolbar_button");
	lua_setglobal(L, "add_toolbar_button");
	
	lua_pushcfunction(L, set_toolbar_button, "set_toolbar_button");
	lua_setglobal(L, "set_toolbar_button");
	
	lua_pushcfunction(L, enable_toolbar_button, "enable_toolbar_button");
	lua_setglobal(L, "enable_toolbar_button");
	
	lua_pushcfunction(L, add_toolbar_label, "add_toolbar_label");
	lua_setglobal(L, "add_toolbar_label");
	
	lua_pushcfunction(L, set_toolbar_label, "set_toolbar_label");
	lua_setglobal(L, "set_toolbar_label");
	
	lua_pushcfunction(L, add_toolbar_separator, "add_toolbar_separator");
	lua_setglobal(L, "add_toolbar_separator");

	lua_pushcfunction(L, toggle_pin, "toggle_pin");
	lua_setglobal(L, "toggle_pin");
	
	lua_pushcfunction(L, show_menu, "show_menu");
	lua_setglobal(L, "show_menu");
	
	lua_pushcfunction(L, set_toolbar_zoom, "set_toolbar_zoom");
	lua_setglobal(L, "set_toolbar_zoom");	

	// window
	
	lua_pushcfunction(L, create_window, "create_window");
	lua_setglobal(L, "create_window");
	
	lua_pushcfunction(L, toggle_window, "toggle_window");
	lua_setglobal(L, "toggle_window");
	
	lua_pushcfunction(L, set_frame, "set_frame");
	lua_setglobal(L, "set_frame");
	
	lua_pushcfunction(L, set_font_options, "set_font_options");
	lua_setglobal(L, "set_font_options");
	
	lua_pushcfunction(L, add_window_button, "add_window_button");
	lua_setglobal(L, "add_window_button");
	
	lua_pushcfunction(L, add_window_image, "add_window_image");
	lua_setglobal(L, "add_window_image");
	
	lua_pushcfunction(L, set_text, "set_text");
	lua_setglobal(L, "set_text");
	
	lua_pushcfunction(L, add_window_label, "add_window_label");
	lua_setglobal(L, "add_window_label");

	lua_pushcfunction(L, set_label_text, "set_label_text");
	lua_setglobal(L, "set_label_text");
	
	lua_pushcfunction(L, get_label_text, "get_label_text");
	lua_setglobal(L, "get_label_text");
	
	lua_pushcfunction(L, set_label_options, "set_label_options");
	lua_setglobal(L, "set_label_options");
	
	lua_pushcfunction(L, create_choicepage, "create_choicepage");
	lua_setglobal(L, "create_choicepage");
	
	lua_pushcfunction(L, create_checkbox, "create_checkbox");
	lua_setglobal(L, "create_checkbox");
	
	lua_pushcfunction(L, get_checkbox_checked, "get_checkbox_checked");
	lua_setglobal(L, "get_checkbox_checked");
	
	lua_pushcfunction(L, create_combobox, "create_combobox");
	lua_setglobal(L, "create_combobox");
	
	lua_pushcfunction(L, add_combobox, "add_combobox");
	lua_setglobal(L, "add_combobox");
	
	lua_pushcfunction(L, get_text_input, "get_text_input");
	lua_setglobal(L, "get_text_input");
	
	lua_pushcfunction(L, get_title, "get_title");
	lua_setglobal(L, "get_title");
	
	// hoover window
	
	lua_pushcfunction(L, hoover_ShowMap, "hoover_ShowMap");
	lua_setglobal(L, "hoover_ShowMap");
	
	lua_pushcfunction(L, hoover_ShowPlace, "hoover_ShowPlace");
	lua_setglobal(L, "hoover_ShowPlace");
	
	// programmatic snapping
	
	lua_pushcfunction(L, get_corner, "get_corner");
	lua_setglobal(L, "get_corner");
	
	// resource keys for wildcards
	
	lua_pushcfunction(L, get_resource, "get_resource");
	lua_setglobal(L, "get_resource");
	
	// setup
	
	lua_pushcfunction(L, load_setup, "load_setup");
	lua_setglobal(L, "load_setup");
	
	// zoom
	
	lua_pushcfunction(L, set_map_zoom, "set_map_zoom");
	lua_setglobal(L, "set_map_zoom");
	
	// grid rendering
	
	lua_pushcfunction(L, set_grid, "set_grid");
	lua_setglobal(L, "set_grid");
	
	lua_pushcfunction(L, set_borders, "set_borders");
	lua_setglobal(L, "set_borders");
	
	lua_pushcfunction(L, show_grid, "show_grid");
	lua_setglobal(L, "show_grid");
	
	lua_pushcfunction(L, specify_grid, "specify_grid");
	lua_setglobal(L, "specify_grid");
	
	// painting
	
	lua_pushcfunction(L, toggle_painting, "toggle_painting");
	lua_setglobal(L, "toggle_painting");
	
	lua_pushcfunction(L, is_painting, "is_painting");
	lua_setglobal(L, "is_painting");
	
	// die rolling
	
	lua_pushcfunction(L, roll_die, "roll_die");
	lua_setglobal(L, "roll_die");
	
	// logging
	
	lua_pushcfunction(L, get_nick, "get_nick");
	lua_setglobal(L, "get_nick");
	
	lua_pushcfunction(L, log_print, "log_print");
	lua_setglobal(L, "log_print");
	
	// net
	
	lua_pushcfunction(L, net_connected, "net_connected");
	lua_setglobal(L, "net_connected");
	
	lua_pushcfunction(L, ask_sync, "ask_sync");
	lua_setglobal(L, "ask_sync");
	
	lua_pushcfunction(L, send_table, "send_table");
	lua_setglobal(L, "send_table");
	
	lua_pushcfunction(L, remove_undo, "remove_undo");
	lua_setglobal(L, "remove_undo");
	
	// stack
	
	lua_pushcfunction(L, get_top_stack, "get_top_stack");
	lua_setglobal(L, "get_top_stack");
	
	
	
	
	

	lua_getglobal(L, "module");
	int res = lua_pcall(L, 0, 1, 0);
	
		
	// printing any Luau/C++ run time errors while going through script
	// https://sleitnick.github.io/luau-api/reference.html#lua_pcall
	
	if (res != LUA_OK) 
	{
		const char *err = lua_tostring(L, -1);
		lua_pop(L, 1);
		printf("Runtime error: %s\n", err);
		std::exit(1);
	}
	
	
	
	
	io->transfer_resource_keys();
	
		
}
	
	
void Luau::closeVM()
{
		
	lua_close(L);

}
