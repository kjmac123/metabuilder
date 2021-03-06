#include "metabuilder_pch.h"

#include "platform/platform.h"
#include "solution.h"

Target::Target()
{
}

E_BlockType Target::GetType() const
{
	return E_BlockType_Target;
}

bool Target::IsA(E_BlockType t) const
{
	if (MakeBlock::IsA(t))
		return true;

	return t == E_BlockType_Target;
}

void Target::Process()
{
	Block::Process();
}

void Target::SetName(const char* name)
{
	MakeBlock::SetName(name);
	SetMacro(MACRO_SEMANTIC_TARGET_NAME, name);
}

void Target::FlattenFiles(StringVector* result, const char* platformName) const
{
	FlattenStringGroup(result, STRINGGROUP_FILES, platformName);
}

void Target::FlattenNoPchFiles(StringVector* result, const char* platformName) const
{
	FlattenStringGroup(result, STRINGGROUP_NOPCHFILES, platformName);
}

void Target::FlattenFrameworks(StringVector* result, const char* platformName) const
{
	FlattenStringGroup(result, STRINGGROUP_FRAMEWORKS, platformName);
}

void Target::FlattenResources(StringVector* result, const char* platformName) const
{
	FlattenStringGroup(result, STRINGGROUP_RESOURCES, platformName);
}

void Target::Flatten(FlatConfig* result, const char* platformName, const char* configName) const
{
	MB_LOGDEBUG("Flattening Target %s platform %s config %s", GetName().c_str(), platformName ? platformName : "{ALL}", configName ? configName : "{ALL}");
	//TODO - move this
	if (configName)
	{
		result->name = configName;
	}
	
	//Build a list so that we can traverse our hierarchy top to bottom.
	ParamVector params;
	
	std::vector<const Block*> blocks;
	for (const Block* block = this; block; block = block->GetParent())
	{
		blocks.push_back(block);
	}
	//Special case - if we're not processing the primary make file then also include
	//the contents of the primary makefile's solution
	if (!mbGetAppState()->isProcessingPrimaryMakefile)
	{
		blocks.push_back(mbGetAppState()->mainSolution);
	}

	for (size_t iBlock = 0; iBlock < blocks.size(); ++iBlock)
	{
		blocks[iBlock]->GetParams(&params, E_BlockType_Unknown, platformName, configName, true);
	}

	//Merge non config specifc params
	for (int iBlock = static_cast<int>(blocks.size())-1; iBlock >= 0; --iBlock)
	{
		const Block* block = blocks[iBlock];
		MB_LOGDEBUG("Merging block %s", block->GetName().c_str());
		block->FlattenThis(result);
	}

	//Merge config specific params
	for (int iBlock = static_cast<int>(params.size())-1; iBlock >= 0; --iBlock)
	{
		const Block* block = params[iBlock];
		MB_LOGDEBUG("Merging config block %s parent config %s", block->GetName().c_str(), block->GetParentPlatform() ? block->GetParentPlatform() : "none");
		block->FlattenThis(result);
	}
	
	result->Dump();
}

void Target::FlattenStringGroup(StringVector* result, const char* stringGroup, const char* platformName) const
{
	FlatConfig f;
	Flatten(&f, platformName, NULL);
	
	std::map<std::string, StringVector>::iterator it = f.stringGroups.find(stringGroup);
	if (it == f.stringGroups.end())
		return;
	
	const StringVector& strings = it->second;
	mbJoinArrays(result, strings);	
}

//-----------------------------------------------------------------------------------------------------------------------------------------

static int luaFuncTarget(lua_State* l)
{
    if (!mbGetActiveContext()->ActiveBlock() || mbGetActiveContext()->ActiveBlock()->GetType() != E_BlockType_Solution)
    {
        MB_LOGERROR("ERROR: must be within solution block");
        mbExitError();
    }
    
    Solution* solution = (Solution*)mbGetActiveContext()->ActiveBlock();

    std::string name;
	mbLuaToStringExpandMacros(&name, solution, l, 1);
    
	//Create new target instance.
    Target* target = new Target();
	solution->AddChild(target);
	target->SetName(name.c_str());
    
	solution->targetVector.push_back(target);

	//Target becomes new active block
    mbGetActiveContext()->PushActiveBlock(target);

    return 0;
}

static int luaFuncTargetEnd(lua_State*)
{
	mbGetActiveContext()->activeBlockStack.pop();
	return 0;
}

static int luaFuncTargetType(lua_State* l)
{
    Target* target = (Target*)mbGetActiveContext()->ActiveBlock();
    mbLuaToStringExpandMacros(&target->targetType, target, l, 1);
    return 0;
}

static int luaFuncTargetSubsystem(lua_State* l)
{
	Target* target = (Target*)mbGetActiveContext()->ActiveBlock();
	mbLuaToStringExpandMacros(&target->targetSubsystem, target, l, 1);
	return 0;
}

static int luaFuncTargetPCH(lua_State*l )
{
    Target* target = (Target*)mbGetActiveContext()->ActiveBlock();
	mbLuaToStringExpandMacros(&target->pch, target, l, 1);
    return 0;
}

static int luaFuncTargetDepends(lua_State* l)
{
    Target* target = (Target*)mbGetActiveContext()->ActiveBlock();
	target->depends.push_back(TargetDepends());
	mbLuaToStringExpandMacros(&target->depends.back().libTargetName, target, l, 1);	// libTargetName;
	mbLuaToStringExpandMacros(&target->depends.back().libMakefile, target, l, 2);	// libMakefile;
	
	//Record this makefile, we'll process it later.
	mbAddMakeFile(target->depends.back().libMakefile.c_str());
    return 0;
}

void mbTargetLuaRegister(lua_State* l)
{
    lua_pushcfunction(l, luaFuncTarget);
    lua_setglobal(l, "target");

    lua_pushcfunction(l, luaFuncTargetEnd);
    lua_setglobal(l, "target_end");

    lua_pushcfunction(l, luaFuncTargetType);
    lua_setglobal(l, "target_type");
	
	lua_pushcfunction(l, luaFuncTargetSubsystem);
	lua_setglobal(l, "target_subsystem");
	
	lua_pushcfunction(l, luaFuncTargetPCH);
    lua_setglobal(l, "pch");
        	
    lua_pushcfunction(l, luaFuncTargetDepends);
    lua_setglobal(l, "depends");
}


