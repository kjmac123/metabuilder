#include "metabuilder_pch.h"

#include "platformparam.h"

//-----------------------------------------------------------------------------------------------------------------------------------------

PlatformParam::PlatformParam()
{
}

E_BlockType PlatformParam::GetType() const
{
	return E_BlockType_PlatformParam;
}

bool PlatformParam::IsA(E_BlockType t) const
{
	if (ParamBlock::IsA(t))
		return true;

	return t == E_BlockType_PlatformParam;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

static int luaFuncPlatform(lua_State* l)
{
	Block* activeBlock = mbGetActiveContext()->ActiveBlock();

    std::string name;
	mbLuaToStringExpandMacros(&name, activeBlock, l, 1);
	
	//Create new target instance.
    PlatformParam* platform = new PlatformParam();
	activeBlock->AddChild(platform);
	platform->SetName(name.c_str());
        
	//Config becomes new active block
    mbGetActiveContext()->PushActiveBlock(platform);
    return 0;
}

static int luaFuncPlatformEnd(lua_State* lua)
{
	mbGetActiveContext()->activeBlockStack.pop();
	return 0;
}

void mbPlatformParamLuaRegister(lua_State* l)
{
    lua_pushcfunction(l, luaFuncPlatform);
    lua_setglobal(l, "platform");

    lua_pushcfunction(l, luaFuncPlatformEnd);
    lua_setglobal(l, "platform_end");
}
