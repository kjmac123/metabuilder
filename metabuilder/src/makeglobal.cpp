#include "metabuilder_pch.h"

#include "makeglobal.h"

//-----------------------------------------------------------------------------------------------------------------------------------------

MakeGlobal::MakeGlobal()
{
	targetDirSep = '/';
}

E_BlockType MakeGlobal::GetType() const
{
	return E_BlockType_MakeGlobal;
}

bool MakeGlobal::IsA(E_BlockType t) const
{
	if (Block::IsA(t))
		return true;

	return t == E_BlockType_MakeGlobal;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

static int luaFuncMakeGlobal(lua_State* l)
{
	MakeGlobal* makeGlobal = mbGetAppState()->makeGlobal;

	//MakeSetup becomes new active block
    mbGetActiveContext()->PushActiveBlock(makeGlobal);
    return 0;
}

static int luaFuncMakeGlobalEnd(lua_State* l)
{
	mbGetActiveContext()->activeBlockStack.pop();

	return 0;
}

static int luaFuncDirSep(lua_State* l)
{
	const char* str = lua_tostring(l, 1);
	if (strlen(str) != 1)
	{
		MB_LOGERROR("Dir seperator must be a single character only.");
		mbExitError();
	}

	mbGetAppState()->makeGlobal->targetDirSep = str[0];
	return 0;
}

void mbMakeGlobalLuaRegister(lua_State* l)
{
    lua_pushcfunction(l, luaFuncMakeGlobal);
    lua_setglobal(l, "makeglobal");

    lua_pushcfunction(l, luaFuncMakeGlobalEnd);
    lua_setglobal(l, "makeglobal_end");
	
	lua_pushcfunction(l, luaFuncDirSep);
	lua_setglobal(l, "dirsep");
}
