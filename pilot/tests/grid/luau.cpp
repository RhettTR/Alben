#include <iostream>
#include <fstream>
#include <cstring>

#include <assert.h>
#include "lua.h"
#include "lualib.h"
#include "luacode.h"


#include "luau.h" 




static lua_State* L;


static int radius = 5;
static float red = 1.0;
static float green = 0.0;
static float blue = 0.0;
static float alpha = 0.5;


static GdkRGBA color;



	

Grid *grid;

float alphadnd;



Luau::Luau(Grid *g)
{
	
	grid = g;
	alphadnd = 1.0;
	
}


void Luau::compileScript()
{	
	char*  bytecode;
	size_t bytecodeSize = 0;
	
	
	std::string content;	
	std::ifstream ifs;
	
	ifs.open("module.luau", std::ifstream::in);
	content.assign( (std::istreambuf_iterator<char>(ifs) ),
                    (std::istreambuf_iterator<char>()    ) );
	ifs.close();
	
	bytecode = luau_compile(content.c_str(), content.length(), NULL, &bytecodeSize);
	assert(luau_load(L, "module", bytecode, bytecodeSize, 0) == 0);
	free(bytecode);
	
	lua_setglobal(L, "module");
	
}


float Luau::getAlphadnd()
{
	
	return alphadnd;
	
}


extern "C" {
	
	
	static int setCounterDragAlpha(lua_State *L)
	{	
		
		if (lua_isstring(L, -1))
		{	
			const char *val = lua_tostring(L, -1);	
			lua_pop(L, 1);
			alphadnd = atof(val);
		}
		
		
		return 0;
	}
	
	
	//default values
	float a = 32.0;
	float b = 64.0;
	float xoff = 0.0;
	float yoff = 32.0;
	
	static int hexagonalGrid(lua_State *L)
	{	
		
		if (lua_isstring(L, -1))
		{	
			const char *val = lua_tostring(L, -1);	
			lua_pop(L, 1);
			yoff = atof(val);
		}
		
		if (lua_isstring(L, -1))
		{	
			const char *val = lua_tostring(L, -1);	
			lua_pop(L, 1);
			xoff = atof(val);
		}
		
		if (lua_isstring(L, -1))
		{	
			const char *val = lua_tostring(L, -1);	
			lua_pop(L, 1);
			b = atof(val);
		}
		
		if (lua_isstring(L, -1))
		{	
			const char *val = lua_tostring(L, -1);	
			lua_pop(L, 1);
			a = atof(val);
		}
		
		grid->hexagonalGrid(a, b, xoff, yoff, 600, 400); 
		
		return 0;
	}
	
	
	

	
	static int gridSpecify(lua_State *L)
	{
		
		if (lua_isstring(L, -1))
		{	
			const char *val = lua_tostring(L, -1);	
			lua_pop(L, 1);
			alpha = atof(val);
		}
		
		if (lua_isstring(L, -1))
		{	
			const char *val = lua_tostring(L, -1);	
			lua_pop(L, 1);
			blue = atof(val);
		}
		
		if (lua_isstring(L, -1))
		{	
			const char *val = lua_tostring(L, -1);	
			lua_pop(L, 1);
			green = atof(val);
		}
		
		if (lua_isstring(L, -1))
		{	
			const char *val = lua_tostring(L, -1);	
			lua_pop(L, 1);
			red = atof(val);
		}
		
		if (lua_isnumber(L, -1))
		{	
			radius = lua_tonumber(L, -1);	
			lua_pop(L, 1);
		}
		
		color = {red, green, blue, alpha };
		
		
		
		grid->setCirle (radius, &color);
		
		
		return 0;
			
	}
	
	
	
	
	static int gridShow(lua_State *L)	
	{
		bool show;
		
		if (lua_isboolean(L, -1))
		{	
			show = lua_toboolean(L, -1);	
			lua_pop(L, 1);			
		}
		
		grid->setShowGrid(show);
		
		
		gtk_widget_queue_draw (grid->getGridWindow());
			
			
		return 0;
		
	}
	
	
	
	void Luau::startVM()
	{
		
		L = luaL_newstate();
		luaL_openlibs(L);
		
		compileScript();
				
		lua_pushcfunction(L, setCounterDragAlpha, "setCounterDragAlpha");
		lua_setglobal(L, "setCounterDragAlpha");
		
		lua_pushcfunction(L, hexagonalGrid, "hexagonalGrid");
		lua_setglobal(L, "hexagonalGrid");
		
		lua_pushcfunction(L, gridSpecify, "gridSpecify");
		lua_setglobal(L, "gridSpecify");
		
		lua_pushcfunction(L, gridShow, "gridShow");
		lua_setglobal(L, "gridShow");
		
		
		lua_getglobal(L, "module");
		lua_pcall(L, 0, 0, 0);
			
	}
		
	
		
	void Luau::closeVM()
	{
			
		lua_close(L);

	}
	
	
}	// extern "C"
