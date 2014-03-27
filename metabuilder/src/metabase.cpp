#include "metabuilder_pch.h"

#include "common.h"

Metabase::Metabase(MetaBuilderBlockBase* parent)
: MetaBuilderBlockBase(parent)
{
}

E_BlockType Metabase::Type() const
{
	return E_BlockType_Metabase;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

static int luaFuncMetabase(lua_State* lua)
{
	const char* generatorName = lua_tostring(lua, 1);
	
	MetaBuilderBlockBase* activeBlock = mbGetActiveContext()->ActiveBlock();
	assert(activeBlock == NULL);
    
    Metabase* generator = new Metabase(activeBlock);
    generator->SetName(generatorName);
	mbGetActiveContext()->metabase = generator;
    
    mbGetActiveContext()->PushActiveBlock(generator);

    return 0;
}

static int luaFuncMetabaseEnd(lua_State* lua)
{
	mbGetActiveContext()->activeBlockStack.pop();
	return 0;
}

static int luaFuncMetabaseSupportedPlatforms(lua_State* l)
{
    if (!mbGetActiveContext()->ActiveBlock() || mbGetActiveContext()->ActiveBlock()->Type() != E_BlockType_Metabase)
    {
        MB_LOGERROR("must be within solution block");
        mbExitError();
    }
    
    Metabase* gen = (Metabase*)mbGetActiveContext()->ActiveBlock();
	
    luaL_checktype(l, 1, LUA_TTABLE);
    int tableLen =  luaL_len(l, 1);
    
	StringVector strings;
    for (int i = 1; i <= tableLen; ++i)
    {
        lua_rawgeti(l, 1, i);
        const char* platform = lua_tostring(l, -1);
		gen->supportedPlatforms.push_back(platform);
    }
		
	return 0;
}

static int luaFuncMetabaseWriter(lua_State* lua)
{

    if (!mbGetActiveContext()->ActiveBlock() || mbGetActiveContext()->ActiveBlock()->Type() != E_BlockType_Metabase)
    {
        MB_LOGERROR("must be within solution block");
        mbExitError();
    }
    
    Metabase* gen = (Metabase*)mbGetActiveContext()->ActiveBlock();
	const char* writer = lua_tostring(lua, 1);
	gen->writerLua = writer;
	
	return 0;
}

void mbMetabaseLuaRegister(lua_State* l)
{
    lua_pushcfunction(l, luaFuncMetabase);
    lua_setglobal(l, "metabase");
	
	lua_pushcfunction(l, luaFuncMetabaseEnd);
    lua_setglobal(l, "metabase_end");
		
	lua_pushcfunction(l, luaFuncMetabaseSupportedPlatforms);
    lua_setglobal(l, "supportedplatforms");

	lua_pushcfunction(l, luaFuncMetabaseWriter);
    lua_setglobal(l, "writer");
}

