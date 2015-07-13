#include "metabuilder_pch.h"

#include "solution.h"
#include "metabase.h"

Solution::Solution()
{
}

Solution::~Solution()
{
}

E_BlockType Solution::GetType() const
{
	return E_BlockType_Solution;
}

bool Solution::IsA(E_BlockType t) const
{
	if (MakeBlock::IsA(t))
		return true;

	return t == E_BlockType_Solution;
}

void Solution::Process()
{
	for (size_t i = 0; i < targetVector.size(); ++i)
	{
		targetVector[i]->Process();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

static int luaFuncSolution(lua_State* l)
{
	//Finalise processing of metabase setup, which must happen before we process the solution.
	{
		AppState* appState = mbGetAppState();
		if (appState->isProcessingPrimaryMakefile)
		{
			appState->ProcessSetup();

			std::string metabase = appState->cmdSetup._generator + ".lua";
			//Process metabase
			mbLuaDoFile(l, FilePath(metabase), NULL);

			appState->ProcessGlobal();
		}
	}

	Block* activeBlock = mbGetActiveContext()->ActiveBlock();
	assert(!activeBlock);

    std::string name;
	mbLuaToStringExpandMacros(&name, activeBlock, l, 1);
    
    if (mbGetActiveContext()->solution)
    {
        MB_LOGERROR("ERROR: Multiple solutions found. This is not supported.");
        mbExitError();
    }
	    
	mbGetActiveContext()->solution = new Solution();
	mbGetActiveContext()->metabase->AddChild(mbGetActiveContext()->solution);
	mbGetActiveContext()->solution->SetName(name.c_str());

	AppState* appState = mbGetAppState();
	if (appState->isProcessingPrimaryMakefile)
	{
		appState->mainSolution = mbGetActiveContext()->solution;
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
