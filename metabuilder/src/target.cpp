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


Target::Target(MetaBuilderBlockBase* parent)
: MetaBuilderBlockBase(parent)
{
}

E_BlockType Target::Type() const
{
	return E_BlockType_Target;
}

void Target::Process()
{
	AddHeadersAutomatically(&m_files);

	for (std::map<std::string, StringVector>::iterator it = m_platformFiles.begin(); it != m_platformFiles.end(); ++it)
	{
		AddHeadersAutomatically(&it->second);
	}
}

void Target::AddFiles(const StringVector& files)
{
	mbJoinArrays(&m_files, files);
}

void Target::AddPlatformFiles(const StringVector& files, const char* platformName)
{
	StringVector* platformFiles;
	std::map<std::string, StringVector>::iterator it = m_platformFiles.find(platformName);
	if (it != m_platformFiles.end())
	{
		platformFiles = &it->second;
	}
	else
	{
		std::pair<std::map<std::string, StringVector>::iterator, bool> result =
			m_platformFiles.insert(std::pair<std::string, StringVector>(platformName, StringVector()));
		platformFiles = &result.first->second;
	}
	
	mbJoinArrays(platformFiles, files);
}

void Target::GetPlatformFiles(StringVector* result, const char* platformName)
{
	BuildUniqueStringList(result, &m_files, &m_platformFiles, platformName);
}

void Target::GetPlatformFrameworks(StringVector* result, const char* platformName)
{
	BuildUniqueStringList(result, NULL, &platformFrameworks, platformName);
}

void Target::GetPlatformResources(StringVector* result, const char* platformName)
{
	BuildUniqueStringList(result, NULL, &platformResources, platformName);
}

void Target::AddHeadersAutomatically(StringVector* files) const
{
	MetaBuilderContext* ctx = mbGetMainContext();
	
	StringVector result;
	result.reserve(files->size()*2);
	for (int i = 0; i < (int)files->size(); ++i)
	{
		const std::string& filename = (*files)[i];
		result.push_back(filename);
		
		char fileExt[MB_MAX_PATH];
		mbPathGetFileExtension(fileExt, filename.c_str());
		const char* sourceFileExtensions[] = {"cpp", "c", "m", "mm", NULL};
		for (const char** sourceExtCursor = sourceFileExtensions; *sourceExtCursor; ++sourceExtCursor)
		{
			if (!stricmp(*sourceExtCursor, fileExt))
			{
				const char* candidateExt[] = {"h", "inl", NULL};
				for (const char** candidateExtCursor = candidateExt; *candidateExtCursor; ++candidateExtCursor)
				{
					char candidateRelativeName[MB_MAX_PATH];
					mbPathReplaceFileExtension(candidateRelativeName, filename.c_str(), *candidateExtCursor);

					char candidateFilename[MB_MAX_PATH];
					sprintf(candidateFilename, "%s/%s", ctx->currentMetaMakeDirAbs.c_str(), candidateRelativeName);
					if (mbFileExists(candidateFilename))
					{
						MB_LOGINFO("Automatically adding header file %s", candidateRelativeName);
						result.push_back(candidateRelativeName);
					}
				}
			}
		}
	}
	
	*files = result;
}

static int luaFuncTarget(lua_State* l)
{
    if (!mbGetActiveContext()->ActiveBlock() || mbGetActiveContext()->ActiveBlock()->Type() != E_BlockType_Solution)
    {
        MB_LOGERROR("ERROR: must be within solution block");
        mbExitError();
    }
    
    Solution* solution = (Solution*)mbGetActiveContext()->ActiveBlock();

    const char* targetName = lua_tostring(l, 1);
    
	//Create new target instance.
    Target* target = new Target(solution);
    target->name = targetName;
    
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


static void FilterFiles(StringVector* result, const StringVector& input)
{
	for (int i = 0; i < (int)input.size(); ++i)
	{
		const std::string& inputFilepath = input[i];

		//Look for wildcard
		if (inputFilepath.find('*') != std::string::npos)
		{
			const char* excludeDirs = NULL;
			const char* delimiter = "|excludedirs=";
			char* tmp = (char*)strstr(inputFilepath.c_str(), delimiter);
			if (tmp)
			{
				excludeDirs = tmp + strlen(delimiter);
				*tmp = '\0';
			}

			std::string dir = mbPathGetDir(inputFilepath);	

			std::string filename = mbPathGetFilename(inputFilepath);
			mbaBuildFileListRecurse(result, dir.c_str(), filename.c_str(), excludeDirs);
		}
		else
		{
			result->push_back(inputFilepath);
		}
	}
}

static int luaFuncTargetFiles(lua_State* l)
{
    Target* target = (Target*)mbGetActiveContext()->ActiveBlock();
	
    luaL_checktype(l, 1, LUA_TTABLE);
    int tableLen =  luaL_len(l, 1);
    
	StringVector inputFiles;
    for (int i = 1; i <= tableLen; ++i)
    {
        lua_rawgeti(l, 1, i);
        const char* filename = lua_tostring(l, -1);
		inputFiles.push_back(filename);
    }
	
	StringVector filteredList;
	FilterFiles(&filteredList, inputFiles);
	target->AddFiles(filteredList);
		
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

static int luaFuncTargetPlatformFiles(lua_State* l)
{
	const char* platformName = lua_tostring(l, 1);
	
    luaL_checktype(l, 2, LUA_TTABLE);
    int tableLen =  luaL_len(l, 2);
    
	StringVector unfilteredList;
    for (int i = 1; i <= tableLen; ++i)
    {
        lua_rawgeti(l, 2, i);
        const char* tmp = lua_tostring(l, -1);
        unfilteredList.push_back(tmp);
    }
	
	StringVector filteredList;
	FilterFiles(&filteredList, unfilteredList);
		
    Target* target = (Target*)mbGetActiveContext()->ActiveBlock();
	target->AddPlatformFiles(filteredList, platformName);
	
    return 0;
}

static int luaFuncTargetPlatformFrameworks(lua_State* l)
{
    Target* target = (Target*)mbGetActiveContext()->ActiveBlock();
	AddPlatformSpecificStrings(&target->platformFrameworks, l);
    return 0;
}

static int luaFuncTargetPlatformResources(lua_State* l)
{
    Target* target = (Target*)mbGetActiveContext()->ActiveBlock();
	AddPlatformSpecificStrings(&target->platformResources, l);
    return 0;
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
        
    lua_pushcfunction(l, luaFuncTargetFiles);
    lua_setglobal(l, "files");

    lua_pushcfunction(l, luaFuncTargetPlatformFrameworks);
    lua_setglobal(l, "platformframeworks");
	
    lua_pushcfunction(l, luaFuncTargetPlatformResources);
    lua_setglobal(l, "platformresources");
	
    lua_pushcfunction(l, luaFuncTargetDepends);
    lua_setglobal(l, "depends");
	
    lua_pushcfunction(l, luaFuncTargetFiles);
    lua_setglobal(l, "files");
	
    lua_pushcfunction(l, luaFuncTargetPlatformFiles);
    lua_setglobal(l, "platformfiles");
}

