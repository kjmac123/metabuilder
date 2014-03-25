#include "metabuilder_pch.h"

#include "common.h"

Config::Config(MetaBuilderBlockBase* parent)
: MetaBuilderBlockBase(parent)
{
}

E_BlockType Config::Type() const
{
	return E_BlockType_Config;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

static int luaFuncConfig(lua_State* l)
{
	MetaBuilderBlockBase* activeBlock = mbGetActiveContext()->ActiveBlock();

    const char* name = lua_tostring(l, 1);
	Config* config = activeBlock->AcquireConfig(name);
    
	//Config becomes new active block
    mbGetActiveContext()->PushActiveBlock(config);
    return 0;
}

static int luaFuncConfigEnd(lua_State* lua)
{
	mbGetActiveContext()->activeBlockStack.pop();
	return 0;
}

void mbConfigLuaRegister(lua_State* l)
{
    lua_pushcfunction(l, luaFuncConfig);
    lua_setglobal(l, "config");

    lua_pushcfunction(l, luaFuncConfigEnd);
    lua_setglobal(l, "config_end");
}
