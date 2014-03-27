#include "metabuilder_pch.h"

#include "common.h"

Solution::Solution(MetaBuilderBlockBase* parent)
: MetaBuilderBlockBase(parent)
{
}

E_BlockType Solution::Type() const
{
	return E_BlockType_Solution;
}

void Solution::Process()
{
	for (int i = 0; i < (int)targetVector.size(); ++i)
	{
		targetVector[i]->Process();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

static int luaFuncSolution(lua_State* lua)
{
    const char* name = lua_tostring(lua, 1);
    
    if (mbGetActiveContext()->solution)
    {
        MB_LOGERROR("ERROR: Multiple solutions found. This is not supported.");
        mbExitError();
    }
	
	MetaBuilderBlockBase* activeBlock = mbGetActiveContext()->ActiveBlock();
	assert(!activeBlock);
    
	mbGetActiveContext()->solution = new Solution(mbGetActiveContext()->metabase);
    mbGetActiveContext()->solution->SetName(name);

	AppState* appState = mbGetAppState();
	if (appState->isProcessingPrimaryMakefile)
	{
		appState->mainSolutionName = name;
	}
    
    mbGetActiveContext()->activeBlockStack.push(mbGetActiveContext()->solution);
    return 0;
}

static int luaFuncSolutionEnd(lua_State* lua)
{
	mbGetActiveContext()->activeBlockStack.pop();
	return 0;
}

void mbSolutionLuaRegister(lua_State* l)
{
    lua_pushcfunction(l, luaFuncSolution);
    lua_setglobal(l, "solution");
    
    lua_pushcfunction(l, luaFuncSolutionEnd);
    lua_setglobal(l, "solution_end");
}
