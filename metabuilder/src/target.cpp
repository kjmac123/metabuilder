#include "metabuilder_pch.h"

#include "common.h"
#include <set>

#include "platform/platform.h"


static void BuildUniqueStringList(
	StringVector* result,
	StringVector* commonStrings,
	std::map<std::string, StringVector>* platformStrings,
	const char* platformName)
{
	std::set<std::string> uniqueStrings;
	
	if (commonStrings)
	{
		int n = (int)commonStrings->size();
		for (int i = 0; i < n; ++i)
		{
			uniqueStrings.insert((*commonStrings)[i]);
		}
	}

	if (platformStrings)
	{
		//Merge strings for the given platform into the main list.
		if (platformName)
		{
			std::map<std::string, StringVector>::iterator it = platformStrings->find(platformName);
			if (it != platformStrings->end())
			{
				StringVector& platformStrings = it->second;
				int n = (int)platformStrings.size();
				for (int i = 0; i < n; ++i)
				{
					uniqueStrings.insert(platformStrings[i]);
				}
			}
		}
		//Copy all platforms
		else
		{
			for (std::map<std::string, StringVector>::iterator it = platformStrings->begin(); it != platformStrings->end(); ++it)
			{
				StringVector& platformStrings = it->second;
				int n = (int)platformStrings.size();
				for (int i = 0; i < n; ++i)
				{
					uniqueStrings.insert(platformStrings[i]);
				}
			}
		}
	}

	//Convert set to array
	for (std::set<std::string>::iterator it = uniqueStrings.begin(); it != uniqueStrings.end(); ++it)
	{
		result->push_back(*it);
	}
}


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

void Target::FlattenFiles(StringVector* result, const char* platformName) const
{
	FlattenStringGroup(result, STRINGGROUP_FILES, platformName);
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
		
		for (int i = 0; i < blocks.size(); ++i)
		{
			block->GetParams(&params, E_BlockType_Unknown, platformName, configName, true);
		}
	}

	//Merge non config specifc params
	for (int iBlock = (int)blocks.size()-1; iBlock >= 0; --iBlock)
	{
		const Block* block = blocks[iBlock];
		MB_LOGDEBUG("Merging block %s", block->GetName().c_str());
		block->FlattenThis(result);
	}

	//Merge config specific params
	for (int iBlock = (int)params.size()-1; iBlock >= 0; --iBlock)
	{
		const Block* block = params[iBlock];
		MB_LOGDEBUG("Merging config block %s parent config %s", block->GetName().c_str(), block->GetParentPlatform() ? block->GetParentPlatform() : "none");
		block->FlattenThis(result);
	}
	
	result->Dump();
}

void Target::FlattenStringGroup(StringVector* result, const char* stringGroup, const char* platformName) const
{
	MetaBuilderContext* ctx = mbGetActiveContext();
	
	FlatConfig f;
	Flatten(&f, platformName, NULL);
	
	std::map<std::string, StringVector>::iterator it = f.stringGroups.find(stringGroup);
	if (it == f.stringGroups.end())
		return;
	
	const StringVector& strings = it->second;
	mbJoinArrays(result, strings);	
}

static int luaFuncTarget(lua_State* l)
{
    if (!mbGetActiveContext()->ActiveBlock() || mbGetActiveContext()->ActiveBlock()->GetType() != E_BlockType_Solution)
    {
        MB_LOGERROR("ERROR: must be within solution block");
        mbExitError();
    }
    
    Solution* solution = (Solution*)mbGetActiveContext()->ActiveBlock();

    const char* name = lua_tostring(l, 1);
    
	//Create new target instance.
    Target* target = new Target();
	solution->AddChild(target);
    target->SetName(name);
    
	solution->targetVector.push_back(target);

	//Target becomes new active block
    mbGetActiveContext()->PushActiveBlock(target);

    return 0;
}

static int luaFuncTargetEnd(lua_State* lua)
{
	mbGetActiveContext()->activeBlockStack.pop();
	return 0;
}

static int luaFuncTargetType(lua_State* l)
{
    Target* target = (Target*)mbGetActiveContext()->ActiveBlock();

    const char* targetType = lua_tostring(l, 1);
    
    target->targetType = targetType;

    return 0;
}

static int luaFuncTargetPCH(lua_State* l)
{
    Target* target = (Target*)mbGetActiveContext()->ActiveBlock();

    const char* pch = lua_tostring(l, 1);
    
    target->pch = pch;

    return 0;
}

static void AddPlatformSpecificStrings(std::map<std::string, StringVector>* platformStringMap, lua_State* l)
{
	const char* platformName = lua_tostring(l, 1);
	
	std::map<std::string, StringVector>::iterator it = platformStringMap->find(platformName);
	
	if (it == platformStringMap->end())
	{
		//Insert new vector as one does not exist already for this platform.
		std::pair<std::map<std::string, StringVector>::iterator, bool> result =
			platformStringMap->insert(std::make_pair(platformName, StringVector()));
		
		it = result.first;
	}

	StringVector& platformFiles = (*it).second;
	
    luaL_checktype(l, 2, LUA_TTABLE);
    int tableLen =  luaL_len(l, 2);
    
    for (int i = 1; i <= tableLen; ++i)
    {
        lua_rawgeti(l, 2, i);
        const char* tmp = lua_tostring(l, -1);
        platformFiles.push_back(tmp);
    }
}

static int luaFuncTargetDepends(lua_State* l)
{
    Target* target = (Target*)mbGetActiveContext()->ActiveBlock();
    const char* libTargetName = lua_tostring(l, 1);
	const char* libMakefile = lua_tostring(l, 2);
	
	target->depends.push_back(TargetDepends());
	target->depends.back().libTargetName = libTargetName;
	target->depends.back().libMakefile = libMakefile;
	
	//Record this makefile, we'll process it later.
	mbAddMakeFile(libMakefile);

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
	
    lua_pushcfunction(l, luaFuncTargetPCH);
    lua_setglobal(l, "pch");
        	
    lua_pushcfunction(l, luaFuncTargetDepends);
    lua_setglobal(l, "depends");
}


