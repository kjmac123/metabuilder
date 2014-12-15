#include "metabuilder_pch.h"

#include "makesetup.h"

MakeSetup::MakeSetup()
{
}

E_BlockType MakeSetup::GetType() const
{
	return E_BlockType_MakeSetup;
}

bool MakeSetup::IsA(E_BlockType t) const
{
	if (Block::IsA(t))
		return true;

	return t == E_BlockType_MakeSetup;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

static int luaFuncMakeSetup(lua_State* l)
{
    std::string metabase;
	mbLuaToStringExpandMacros(&metabase, l, 1);

	MakeSetup* makeSetup = mbGetAppState()->makeSetup;
	//Only process for primary make file
	if (mbGetAppState()->isProcessingPrimaryMakefile)
	{
		makeSetup->_metabaseDir = metabase;
	}
	
	//MakeSetup becomes new active block
    mbGetActiveContext()->PushActiveBlock(makeSetup);
    return 0;
}

static int luaFuncMakeSetupEnd(lua_State* l)
{
	mbGetActiveContext()->activeBlockStack.pop();

	AppState* appState = mbGetAppState();
	if (appState->isProcessingPrimaryMakefile)
	{
		appState->Process();
		
		std::string metabase = appState->cmdSetup._generator + ".lua";
		//Process metabase
		mbLuaDoFile(l, metabase.c_str(), NULL);
	}
	return 0;
}

static int luaFuncMakeSetupIntermediateDir(lua_State* l)
{
    std::string intermediateDir;
	mbLuaToStringExpandMacros(&intermediateDir, l, 1);
    
    if (mbGetActiveContext()->ActiveBlock() != mbGetAppState()->makeSetup)
    {
        MB_LOGERROR("Intermediate dir must be set within correct block.");
        mbExitError();
    }
    
	mbGetAppState()->makeSetup->_intDir = intermediateDir;
    return 0;
}

static int luaFuncMakeSetupOutputDir(lua_State* l)
{
	std::string outputDir;
	mbLuaToStringExpandMacros(&outputDir, l, 1);
    
    if (mbGetActiveContext()->ActiveBlock() != mbGetAppState()->makeSetup)
    {
        MB_LOGERROR("Output dir must be set within correct block.");
        mbExitError();
    }
    
	mbGetAppState()->makeSetup->_outDir = outputDir;
    return 0;
}


void mbMakeSetupLuaRegister(lua_State* l)
{
    lua_pushcfunction(l, luaFuncMakeSetup);
    lua_setglobal(l, "makesetup");

    lua_pushcfunction(l, luaFuncMakeSetupEnd);
    lua_setglobal(l, "makesetup_end");
	

    lua_pushcfunction(l, luaFuncMakeSetupIntermediateDir);
    lua_setglobal(l, "intdir");
    
    lua_pushcfunction(l, luaFuncMakeSetupOutputDir);
    lua_setglobal(l, "outdir");
}
