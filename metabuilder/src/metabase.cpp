#include "metabuilder_pch.h"

#include "metabase.h"

Metabase::Metabase()
{
}

E_BlockType Metabase::GetType() const
{
	return E_BlockType_Metabase;
}

bool Metabase::IsA(E_BlockType t) const
{
	if (MakeBlock::IsA(t))
		return true;

	return t == E_BlockType_Metabase;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

static int luaFuncMetabase(lua_State* l)
{	
	Block* activeBlock = mbGetActiveContext()->ActiveBlock();
	assert(activeBlock == NULL);
 
	std::string generatorName;
	mbLuaToStringExpandMacros(&generatorName, activeBlock, l, 1);

    Metabase* generator = new Metabase();
	if (activeBlock)
	{
		activeBlock->AddChild(generator);
	}
	generator->SetName(generatorName.c_str());
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
	MetaBuilderContext* ctx = mbGetActiveContext();
	Block* b = mbGetActiveContext()->ActiveBlock();
	if (b != ctx->metabase)
	{
		MB_LOGERROR("must be within metabase block");
		mbExitError();
	}
	Metabase* metabase = (Metabase*)b;
	
    luaL_checktype(l, 1, LUA_TTABLE);
    int tableLen =  luaL_len(l, 1);
    
    for (int i = 1; i <= tableLen; ++i)
    {
        lua_rawgeti(l, 1, i);
		metabase->supportedPlatforms.push_back(std::string());
		mbLuaToStringExpandMacros(&metabase->supportedPlatforms.back(), metabase, l, -1);
    }
		
	return 0;
}

static int luaFuncMetabaseWriter(lua_State* l)
{
	MetaBuilderContext* ctx = mbGetActiveContext();
	Block* b = mbGetActiveContext()->ActiveBlock();
	if (b != ctx->metabase)
	{
		MB_LOGERROR("must be within metabase block");
		mbExitError();
	}
	Metabase* metabase = (Metabase*)b;

	std::string writer;
	mbLuaToStringExpandMacros(&writer, metabase, l, 1);
	metabase->writerLua = writer;
	
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

