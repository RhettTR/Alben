#include <iostream>
#include <fstream>
#include <cstring>
#include <variant>


#include <assert.h>
#include "lua.h"
#include "lualib.h"
#include "luacode.h"


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
extern Window *getMain();



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
		{"module", "counter", "traits", "state", "repository", "zone", "dev"};
	
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
			Counter::alpha = lua_tonumber(L, -1);	
			
		lua_pop(L, 1);
		
		return 0;
	}
	
	
	static int setBackgroundMapID(lua_State *L)
	{	
		
		if (lua_isstring(L, -1))	
			Window::getInstance("main")->frame->backgroundID = std::string(lua_tostring(L, -1));
			
		lua_pop(L, 1);
		
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
	
	static int update_class(lua_State *L)
	{	
		
		int id = lua_tointeger(L, -1);
		
		(void)lua_gettable(L, -2);
		
		if (lua_istable(L, -1))
		{			
			Counter::Table state = getTable();
			
			Counter *found = Counter::counters[id];
			
			if (found)
			{
				// memory leak ??
				found->table = state;
				
				found->state.x = (int)std::get<double>((state)["x"]);	 	
				found->state.y = (int)std::get<double>((state)["y"]);
				found->state.tag = std::get<std::string>((state)["window"]);
				
				Counter::Table images = std::get<Counter::Table>(std::get<Counter::Table>((state)["Image"])["images"]);
				int index = (int)std::get<double>(std::get<Counter::Table>((state)["Image"])["imageIndex"]);
				found->image = std::get<std::string>(images[1]);	
				found->state.image = std::get<std::string>(images[index]);
				
				found->state.zorder = (int)std::get<double>((state)["zorder"]);
				

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
				
				
				
				//found->parentWindow->frame->repaint();
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
		
		Counter::setGUI();
		
		return 0;
	}
	
	static int root(lua_State *L)
	{	
		
		const char *tag = lua_tostring(L, -1);
		int level = lua_tonumber(L, -2);	
		lua_pop(L, 2);
		
		Window *window = Window::getInstance(tag);		
		
		window->root(level);
		
		return 0;
	}
	
	static int tabs(lua_State *L)
	{			
		const char *tag = lua_tostring(L, -1);
		int level = lua_tonumber(L, -2);	
		lua_pop(L, 2);
		
		Window *window = Window::getInstance(tag);
		
		window->tabs(level);
		
		return 0;
	}
	
	static int tab(lua_State *L)
	{		
		const char *tag = lua_tostring(L, -1);
		const char *text = lua_tostring(L, -2);
		int level = lua_tonumber(L, -3);	
		lua_pop(L, 3);
		
		Window *window = Window::getInstance(tag);
		
		window->tab(level, string(text));
		
		return 0;
	}
	
	static int listbox(lua_State *L)
	{			
		const char *tag = lua_tostring(L, -1);
		int level = lua_tonumber(L, -2);	
		lua_pop(L, 2);
		
		Window *window = Window::getInstance(tag);
		
		window->listBox(level);
		
		return 0;
	}
	
	static int listitem(lua_State *L)
	{		
		const char *tag = lua_tostring(L, -1);
		const char *text = lua_tostring(L, -2);
		int level = lua_tonumber(L, -3);	
		lua_pop(L, 3);
		
		Window *window = Window::getInstance(tag);
		
		window->listItem(level, string(text));
		
		return 0;
	}
	
	static int combobox(lua_State *L)
	{				
		const char *tag = lua_tostring(L, -1);
		int level = lua_tonumber(L, -2);	
		lua_pop(L, 2);
		
		Window *window = Window::getInstance(tag);
		
		window->comboBox(level);
		
		return 0;
	}
	
	static int comboitem(lua_State *L)
	{		
		const char *tag = lua_tostring(L, -1);
		const char *text = lua_tostring(L, -2);
		int level = lua_tonumber(L, -3);	
		lua_pop(L, 3);
		
		Window *window = Window::getInstance(tag);
		
		window->comboItem(level, string(text));
		
		return 0;
	}
	
	static int image_item(lua_State *L)
	{		
		const char *tag = lua_tostring(L, -1);
		const char *image = lua_tostring(L, -2);	
		lua_pop(L, 2);
		
		Window *window = Window::getInstance(tag);
	
		window->imageItem(string(image));
		
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
	
	static int bottom_zorder(lua_State *L)
	{				
		int zorder = Counter::bottomZorder();		
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
		const QString name = QString(lua_tostring(L, -1));
		std::string tag(lua_tostring(L, -2)); 
		
		lua_pop(L, 2);
		
		ToolBar *customBar = new ToolBar(tag, name);
		getMain()->addToolBarBreak(Qt::TopToolBarArea);
		getMain()->addToolBar(Qt::TopToolBarArea, customBar);
		
		return 0;
	}
	
	static int add_toolbar_button(lua_State *L)
	{	
		std::string luaScript(lua_tostring(L, -1));		
		const QString toolTip = QString(lua_tostring(L, -2));
		QString buttonText(lua_tostring(L, -3));
		std::string resourceName(lua_tostring(L, -4));
		const char *id = lua_tostring(L, -5);
		const char *tag = lua_tostring(L, -6);
		
		lua_pop(L, 6);
		
		ToolBar *toolbar = ToolBar::getInstance(tag);
		
		toolbar->addImageButton(id, resourceName, buttonText, toolTip, nullptr, luaScript);
		toolbar->enabled(resourceName, true);
		
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
		
		toolbar->setImageButton(id, resourceName, buttonText);
		
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
		
		const char *type = lua_tostring(L, -1);	
		bool anyScroll = lua_toboolean(L, -2);	
		std::string background(lua_tostring(L, -3));
		QString title(lua_tostring(L, -4));
		const char *tag = lua_tostring(L, -5);
		lua_pop(L, 5);
		
		Window *window = new Window(Window::getInstance("main"), title, std::string(tag), std::string(type));
		
		window->frame->setBackground(background);		
		
		
		if (anyScroll)
		{
			window->scrollArea = new QScrollArea;
			window->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
			window->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
			window->scrollArea->setWidget(window->frame);
			
			window->setCentralWidget(window->scrollArea);
		}
		else
			window->scrollArea = nullptr;
		
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
	
	static int set_window(lua_State *L)
	{		
		int h = lua_tonumber(L, -1);
		int w = lua_tonumber(L, -2);
		int y = lua_tonumber(L, -3);
		int x = lua_tonumber(L, -4);
		const char *tag = lua_tostring(L, -5);

		lua_pop(L, 5);
		
		Window *window = Window::getInstance(tag);
		window->resize(w, h);
		window->move(x, y);
		//window->setSingleRowed(x, y, w , h);
		
		return 0;
	}
	
	static int add_window_button(lua_State *L)
	{		
		std::string luaScript(lua_tostring(L, -1));
		const char *resourceName = lua_tostring(L, -2);
		int y = lua_tonumber(L, -3);
		int x = lua_tonumber(L, -4);
		std::string widget(lua_tostring(L, -5));
		const char *tag = lua_tostring(L, -6);

		lua_pop(L, 6);
		
		Window *window = Window::getInstance(tag);
		
		QImage image = io->getImage(std::string(resourceName));
		
		
		Window::PushButton *button = 
			new Window::PushButton(widget, image, "", x, y, luaScript, window);
		
		
		return 0;
	}
	
	static int add_window_label(lua_State *L)
	{	
		const char *css = lua_tostring(L, -1);
		std::string resourceName(lua_tostring(L, -2));
		int h = lua_tonumber(L, -3);
		int w = lua_tonumber(L, -4);
		int y = lua_tonumber(L, -5);
		int x = lua_tonumber(L, -6);
		std::string widgetTag(lua_tostring(L, -7));
		const char *windowTag = lua_tostring(L, -8); 
		
		lua_pop(L, 8);
		
		Window *window = Window::getInstance(windowTag);
	
		new Window::Label(widgetTag, window, x, y, w, h, resourceName, css);
		
		return 0;
	}
	
	static int set_label_text(lua_State *L)
	{	
		std::string text(lua_tostring(L, -1));
		std::string id(lua_tostring(L, -2));
		const char *tag = lua_tostring(L, -3); 
		lua_pop(L, 3);
	
		Window *window = Window::getInstance(tag);
	
		((Window::Label *)window->widgets[id])->setText(text);
				
		return 0;
	}
	
	static int get_label_text(lua_State *L)
	{	
		std::string id(lua_tostring(L, -1)); 
		lua_pop(L, 1);
		
		//const char *text = ((Window::Frame *)Window::getInstance("artillery-window")->frame)->labels[id]->get().c_str();			
		const char *text = "";
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
		
		(void)new Window::CheckBox(id, (Window *)Window::getInstance("artillery-window"), x, y, w, h, text, script, css);
		
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
	
	static int get_size(lua_State *L)
	{	
		Counter *counter;
		
		
		if (lua_isnumber(L, -1))
		{
			int id  = lua_tonumber(L, -1);
			counter = Counter::counters[id];
		}
		else	
		{
			const char *name = lua_tostring(L, -1);
			counter = Counter::repository[std::string(name)];
		}
			
		lua_pop(L, 1);
		
		QSize size = Counter::getSize(counter);
		
		
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
	
	static int set_zoom_range(lua_State *L)
	{
		int defaultZoom = lua_tonumber(L, -1);
		lua_pop(L, 1);
	
		int top = lua_gettop(L);
		
		for (int i = 1; i <= top; i++) 
		{
			int zoom = lua_tointeger(L, i);
		
			ToolBar::getInstance("main")->ToolBar::sizeBox->insert(zoom);
						
					
		}

		lua_pop(L, top);
				
		ToolBar::getInstance("main")->ToolBar::sizeBox->setDefault(defaultZoom);
		float fraction = (float)defaultZoom / 100.0;
		ToolBar::getInstance("main")->ToolBar::sizeBox->parent->zoom(fraction);	
		
		return 0;
	}
	
	static int create_area(lua_State *L)
	{	
		
		CentralFrame::area = new CentralFrame::Area(Window::getInstance("main")->frame);
		
		return 0;
	}
	
	static int set_grid(lua_State *L)
	{	
		(void)lua_gettable(L, -2);
		
		if (lua_istable(L, -1))
		{			
			Counter::Table grid = getTable();
			
			CentralFrame::setGrid(&grid);
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
	
	Luau::Popupentry e = {"", "", "", "", "", false};
	
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
							e = {"", "", "", "", "", false};	
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
	
	
	Counter::Table state = getTable();
	
	lua_pop(L, 1);
		
	
	return state;
	
}




Counter::Table Luau::getTrait(const char *window, const char *id)
{
	
	lua_getglobal(L, "getTraits");
	lua_pushstring(L, window);
	lua_pushstring(L, id);
		
	lua_pcall(L, 2, 1, 0);
	
	
	Counter::Table state = getTable();
	
	lua_pop(L, 1);
		
	
	return state;
	
}


Counter::Table Luau::getDecks()
{
	lua_getglobal(L, "getKeys");	
	lua_pcall(L, 0, 1, 0);
	
	
	Counter::Table keys = getTable();
	lua_pop(L, 1);

	return keys;
}


Counter::Table Luau::getDeck(const char *id)
{
	lua_getglobal(L, "getDeck");
	lua_pushstring(L, id);	
	lua_pcall(L, 1, 1, 0);
	
	
	Counter::Table table = getTable();
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





void Luau::doAction(const char *window, const char *id, const char *trait, const char *name)
{
	
	lua_getglobal(L, "action");
	lua_pushstring(L, window);
	lua_pushstring(L, id);
	lua_pushstring(L, trait);
	lua_pushstring(L, name);
	lua_pcall(L, 4, 0, 0);
	
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


bool Luau::afterDrag(const char *window, const char *id, int dx, int dy, int &x, int &y)
{	
	lua_getglobal(L, "afterDrag");
	lua_pushstring(L, window);
	lua_pushstring(L, id);
	lua_pushnumber(L, dx);
	lua_pushnumber(L, dy);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pcall(L, 6, 3, 0);	

	y = (int)lua_tonumber (L, -1);
	x = (int)lua_tonumber (L, -2);
	bool flag = lua_toboolean (L, -3);

	lua_pop(L, 3);
	
	return flag;
	
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


bool Luau::selectable(const char *id)
{
	lua_getglobal(L, "selectable");
	lua_pushstring(L, id);
	lua_pcall(L, 1, 1, 0);
	
	int result = lua_toboolean (L, -1);
	
	lua_pop(L, 1);
	
	return (result != 0);	
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


void Luau::pointoffset(const char *tag, int x, int y, int dx, int dy, float &xoff, float &yoff, int &open, int &shown)
{
	lua_getglobal(L, "pointoffset");
	lua_pushstring(L, tag);
	lua_pushnumber(L, x);
	lua_pushnumber(L, dx);
	lua_pushnumber(L, y);
	lua_pushnumber(L, dy);
	lua_pcall(L, 5, 4, 0);
	
	shown = (int)lua_tonumber (L, -1);
	open = (int)lua_tonumber (L, -2);
	xoff = (int)lua_tonumber (L, -3);
	yoff = (int)lua_tonumber (L, -4);	
	
	lua_pop(L, 4);
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



void Luau::copyCounter(const char *fromId, const char *toId, int zorder, const char *window, int x, int y)
{
	
	lua_getglobal(L, "copyCounter");
	lua_pushstring(L, fromId);
	lua_pushstring(L, toId);
	lua_pushnumber(L, zorder);
	lua_pushstring(L, window);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pcall(L, 6, 0, 0);
	
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


void Luau::updatePos(const char *window, const char *id, int x, int y)
{
	lua_getglobal(L, "updatePos");
	lua_pushstring(L, window);
	lua_pushstring(L, id);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pcall(L, 4, 0, 0);
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

QString Luau::mapPlace(const char *window, int dx, int dy, int x, int y)
{
	lua_getglobal(L, "mapPlace");
	
	lua_pushstring(L, window);
	lua_pushnumber(L, dx);
	lua_pushnumber(L, dy);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	
	lua_pcall(L, 5, 1, 0);
	
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
	
	lua_pushcfunction(L, setBackgroundMapID, "setBackgroundMapID");
	lua_setglobal(L, "setBackgroundMapID");
	
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
	
	lua_pushcfunction(L, image_item, "image_item");
	lua_setglobal(L, "image_item");
	
	// gui
	
	lua_pushcfunction(L, create_button, "create_button");
	lua_setglobal(L, "create_button");
	
	lua_pushcfunction(L, delete_button, "delete_button");
	lua_setglobal(L, "delete_button");
	
	lua_pushcfunction(L, top_zorder, "top_zorder");
	lua_setglobal(L, "top_zorder");
	
	lua_pushcfunction(L, bottom_zorder, "bottom_zorder");
	lua_setglobal(L, "bottom_zorder");
	
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
	
	lua_pushcfunction(L, create_toolbar, "create_toolbar");
	lua_setglobal(L, "create_toolbar");
	
	lua_pushcfunction(L, add_toolbar_button, "add_toolbar_button");
	lua_setglobal(L, "add_toolbar_button");
	
	lua_pushcfunction(L, set_toolbar_button, "set_toolbar_button");
	lua_setglobal(L, "set_toolbar_button");
	
	lua_pushcfunction(L, add_toolbar_label, "add_toolbar_label");
	lua_setglobal(L, "add_toolbar_label");
	
	lua_pushcfunction(L, set_toolbar_label, "set_toolbar_label");
	lua_setglobal(L, "set_toolbar_label");
	
	lua_pushcfunction(L, add_toolbar_separator, "add_toolbar_separator");
	lua_setglobal(L, "add_toolbar_separator");
	
	// window
	
	lua_pushcfunction(L, create_window, "create_window");
	lua_setglobal(L, "create_window");
	
	lua_pushcfunction(L, toggle_window, "toggle_window");
	lua_setglobal(L, "toggle_window");
	
	lua_pushcfunction(L, set_window, "set_window");
	lua_setglobal(L, "set_window");
	
	lua_pushcfunction(L, add_window_button, "add_window_button");
	lua_setglobal(L, "add_window_button");
	
	lua_pushcfunction(L, add_window_label, "add_window_label");
	lua_setglobal(L, "add_window_label");

	lua_pushcfunction(L, set_label_text, "set_label_text");
	lua_setglobal(L, "set_label_text");
	
	lua_pushcfunction(L, get_label_text, "get_label_text");
	lua_setglobal(L, "get_label_text");
	
	lua_pushcfunction(L, create_checkbox, "create_checkbox");
	lua_setglobal(L, "create_checkbox");
	
	lua_pushcfunction(L, get_checkbox_checked, "get_checkbox_checked");
	lua_setglobal(L, "get_checkbox_checked");
	
	// hoover window
	
	lua_pushcfunction(L, hoover_ShowMap, "hoover_ShowMap");
	lua_setglobal(L, "hoover_ShowMap");
	
	lua_pushcfunction(L, hoover_ShowPlace, "hoover_ShowPlace");
	lua_setglobal(L, "hoover_ShowPlace");
	
	// programmatic snapping
	
	lua_pushcfunction(L, get_size, "get_size");
	lua_setglobal(L, "get_size");
	
	// resource keys for wildcards
	
	lua_pushcfunction(L, get_resource, "get_resource");
	lua_setglobal(L, "get_resource");
	
	// setup
	
	lua_pushcfunction(L, load_setup, "load_setup");
	lua_setglobal(L, "load_setup");
	
	// repository
	
	lua_pushcfunction(L, repository_visibility, "repository_visibility");
	lua_setglobal(L, "repository_visibility");
	
	// toolbar zoom
	
	lua_pushcfunction(L, set_zoom_range, "set_zoom_range");
	lua_setglobal(L, "set_zoom_range");
	
	// area
	
	lua_pushcfunction(L, create_area, "create_area");
	lua_setglobal(L, "create_area");
	
	// grid rendering
	
	lua_pushcfunction(L, set_grid, "set_grid");
	lua_setglobal(L, "set_grid");
	
	lua_pushcfunction(L, show_grid, "show_grid");
	lua_setglobal(L, "show_grid");
	
	lua_pushcfunction(L, specify_grid, "specify_grid");
	lua_setglobal(L, "specify_grid");
	
	// painting
	
	lua_pushcfunction(L, toggle_painting, "toggle_painting");
	lua_setglobal(L, "toggle_painting");
	
	lua_pushcfunction(L, is_painting, "is_painting");
	lua_setglobal(L, "is_painting");
	
	
	
	lua_getglobal(L, "module");
	lua_pcall(L, 0, 0, 0);
	
	
	
	io->transfer_resource_keys();
	
		
}
	
	
void Luau::closeVM()
{
		
	lua_close(L);

}
