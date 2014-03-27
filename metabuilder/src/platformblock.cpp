#include "metabuilder_pch.h"

#include "common.h"

PlatformBlock::PlatformBlock(MetaBuilderBlockBase* parent)
: MetaBuilderBlockBase(parent)
{
}

E_BlockType PlatformBlock::Type() const
{
	return E_BlockType_Platform;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

static int luaFuncPlatform(lua_State* l)
{
	MetaBuilderBlockBase* activeBlock = mbGetActiveContext()->ActiveBlock();

    const char* name = lua_tostring(l, 1);
	
	//Create new target instance.
    PlatformBlock* b = new PlatformBlock(activeBlock);
    b->SetName(name);
        
	//Config becomes new active block
    mbGetActiveContext()->PushActiveBlock(b);
    return 0;
}

static int luaFuncPlatformEnd(lua_State* lua)
{
	mbGetActiveContext()->activeBlockStack.pop();
	return 0;
}

void mbPlatformBlockLuaRegister(lua_State* l)
{
    lua_pushcfunction(l, luaFuncPlatform);
    lua_setglobal(l, "platform");

    lua_pushcfunction(l, luaFuncPlatformEnd);
    lua_setglobal(l, "platform_end");
}
