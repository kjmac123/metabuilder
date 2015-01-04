#include "metabuilder_pch.h"

#include "configparam.h"

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

//-----------------------------------------------------------------------------------------------------------------------------------------

static int luaFuncConfig(lua_State* l)
{
	Block* activeBlock = mbGetActiveContext()->ActiveBlock();

	std::string name;
	mbLuaToStringExpandMacros(&name, activeBlock, l, 1);
	ConfigParam* config = activeBlock->AcquireConfigParam(name.c_str());
    
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
