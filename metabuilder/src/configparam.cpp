#include "metabuilder_pch.h"

#include "common.h"

ConfigParam::ConfigParam()
{
}

E_BlockType ConfigParam::GetType() const
{
	return E_BlockType_ConfigParam;
}

bool ConfigParam::IsA(E_BlockType t) const
{
	if (ParamBlock::IsA(t))
		return true;

	return t == E_BlockType_ConfigParam;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

static int luaFuncConfig(lua_State* l)
{
	Block* activeBlock = mbGetActiveContext()->ActiveBlock();

    const char* name = lua_tostring(l, 1);
	ConfigParam* config = activeBlock->AcquireConfigParam(name);
    
	//Config becomes new active block
    mbGetActiveContext()->PushActiveBlock(config);
    return 0;
}

static int luaFuncConfigEnd(lua_State* lua)
{
	mbGetActiveContext()->activeBlockStack.pop();
	return 0;
}

void mbConfigParamLuaRegister(lua_State* l)
{
    lua_pushcfunction(l, luaFuncConfig);
    lua_setglobal(l, "config");

    lua_pushcfunction(l, luaFuncConfigEnd);
    lua_setglobal(l, "config_end");
}
