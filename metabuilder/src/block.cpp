#include "metabuilder_pch.h"

#include "configparam.h"
#include "block.h"
#include "platformparam.h"

#include <sstream>

static const char* g_stringGroups[] = {
	STRINGGROUP_FILES,
	STRINGGROUP_DEFINES,
	STRINGGROUP_INCLUDEDIRS,
	STRINGGROUP_LIBDIRS,
	STRINGGROUP_LIBS,
	STRINGGROUP_EXEDIRS,
	STRINGGROUP_RESOURCES,
	STRINGGROUP_FRAMEWORKS,
	NULL
};

//-----------------------------------------------------------------------------------------------------------------------------------------

static void AddHeadersAutomatically(StringVector* files)
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

		for (const char** sourceExtCursor = mbGetCAndCPPSourceFileExtensions(); *sourceExtCursor; ++sourceExtCursor)
		{
			if (!strcmp(*sourceExtCursor, fileExt))
			{
				//Add headers
				{
					const char** candidateExt = mbGetCAndCPPHeaderFileExtensions();
					for (const char** candidateExtCursor = candidateExt; *candidateExtCursor; ++candidateExtCursor)
					{
						char candidateRelativeName[MB_MAX_PATH];
						mbPathReplaceFileExtension(candidateRelativeName, filename.c_str(), *candidateExtCursor);

						char candidateFilename[MB_MAX_PATH];
						sprintf(candidateFilename, "%s/%s", ctx->currentMetaMakeDirAbs.c_str(), candidateRelativeName);
						if (mbFileExists(candidateFilename))
						{
							MB_LOGDEBUG("Automatically adding header file %s", candidateRelativeName);
							result.push_back(candidateRelativeName);
							break;
						}
					}
				}

				//Add inline files
				{
					const char** candidateExt = mbGetCAndCPPInlineFileExtensions();
					for (const char** candidateExtCursor = candidateExt; *candidateExtCursor; ++candidateExtCursor)
					{
						char candidateRelativeName[MB_MAX_PATH];
						mbPathReplaceFileExtension(candidateRelativeName, filename.c_str(), *candidateExtCursor);

						char candidateFilename[MB_MAX_PATH];
						sprintf(candidateFilename, "%s/%s", ctx->currentMetaMakeDirAbs.c_str(), candidateRelativeName);
						if (mbFileExists(candidateFilename))
						{
							MB_LOGDEBUG("Automatically adding inline file %s", candidateRelativeName);
							result.push_back(candidateRelativeName);
							break;
						}
					}
				}


				break;
			}
		}
	}
	
	*files = result;
}

static void ProcessWildcards(StringVector* result, const StringVector& input)
{
	int initialResultCount = result->size();
	
	for (int i = 0; i < (int)input.size(); ++i)
	{
		std::string inputFilepath = input[i];

		//Look for wildcard
		if (inputFilepath.find('*') != std::string::npos)
		{
			const char* excludeDirs = NULL;
#if 0
			const char* delimiter = "|excludedirs=";
			char* tmp = (char*)strstr(inputFilepath.c_str(), delimiter);
			if (tmp)
			{
				excludeDirs = tmp + strlen(delimiter);
				*tmp = '\0';
			}
#endif
			std::string dir = mbPathGetDir(inputFilepath);
			std::string filename = mbPathGetFilename(inputFilepath);

			Platform::BuildFileListRecurse(result, dir.c_str(), filename.c_str(), excludeDirs);
			if ((int)result->size() == initialResultCount)
			{
				MB_LOGERROR("No files found matching dir %s and filter %s",  dir.c_str(), filename.c_str());
				mbExitError();
			}
		}
		else
		{
			result->push_back(inputFilepath);
		}
	}
}

static int luaFuncSetOption(lua_State* l)
{
	Block* block = mbGetActiveContext()->ActiveBlock();
    if (!block)
    {
        MB_LOGERROR("must be within block");
        mbExitError();
    }
	
	std::string group, key, value;
	mbLuaToStringExpandMacros(&group, block, l, 1);
	mbLuaToStringExpandMacros(&key, block, l, 2);
	mbLuaToStringExpandMacros(&value, block, l, 3);

	block->SetOption(group, key, value);
	return 0;
}

static int luaFuncSetMacro(lua_State* l)
{
	Block* block = mbGetActiveContext()->ActiveBlock();
	if (!block)
	{
		MB_LOGERROR("must be within block");
		mbExitError();
	}

	std::string key, value;
	mbLuaToStringExpandMacros(&key, block, l, 1);
	mbLuaToStringExpandMacros(&value, block, l, 2);

	block->SetMacro(key.c_str(), value.c_str());
	return 0;
}

static int luaFuncAddOption(lua_State* l)
{
	Block* block = mbGetActiveContext()->ActiveBlock();
	if (!block)
	{
		MB_LOGERROR("must be within block");
		mbExitError();
	}

	std::string group, key, value;
	mbLuaToStringExpandMacros(&group, block, l, 1);
	mbLuaToStringExpandMacros(&key, block, l, 2);
	mbLuaToStringExpandMacros(&value, block, l, 3);

	block->AppendOption(group, key, value, ' ');
	return 0;
}

static int luaFuncDefines(lua_State* l)
{
    Block* block = mbGetActiveContext()->ActiveBlock();
    if (!block)
    {
        MB_LOGERROR("must be set within a block!");
        mbExitError();
    }
    
    luaL_checktype(l, 1, LUA_TTABLE);
    int tableLen =  luaL_len(l, 1);
    
    StringVector strings;
    for (int i = 1; i <= tableLen; ++i)
    {
        lua_rawgeti(l, 1, i);
		strings.push_back(std::string());
		mbLuaToStringExpandMacros(&strings.back(), block, l, -1);
    }
    block->AddDefines(strings);
    
    return 0;
}

static int luaFuncLibs(lua_State* l)
{
	Block* block = mbGetActiveContext()->ActiveBlock();
    if (!block)
    {
        MB_LOGERROR("must be within block");
        mbExitError();
    }
	
    luaL_checktype(l, 1, LUA_TTABLE);
    int tableLen =  luaL_len(l, 1);
    
	StringVector strings;
    for (int i = 1; i <= tableLen; ++i)
    {
        lua_rawgeti(l, 1, i);
		strings.push_back(std::string());
		mbLuaToStringExpandMacros(&strings.back(), block, l, -1);
    }
	block->AddLibs(strings);
		
    return 0;
}

static int luaFuncIncludeDir(lua_State* l)
{
	Block* block = mbGetActiveContext()->ActiveBlock();
    if (!block)
    {
        MB_LOGERROR("must be within block");
        mbExitError();
    }

	
    luaL_checktype(l, 1, LUA_TTABLE);
    int tableLen =  luaL_len(l, 1);
    
	StringVector strings;
    for (int i = 1; i <= tableLen; ++i)
    {
		lua_rawgeti(l, 1, i);
		strings.push_back(std::string());
		mbLuaToStringExpandMacros(&strings.back(), block, l, -1);
    }
	block->AddIncludeDirs(strings);
		
    return 0;
}

static int luaFuncLibDir(lua_State* l)
{
	Block* block = mbGetActiveContext()->ActiveBlock();
    if (!block)
    {
        MB_LOGERROR("must be within block");
        mbExitError();
    }
	
    luaL_checktype(l, 1, LUA_TTABLE);
    int tableLen =  luaL_len(l, 1);
    
	StringVector strings;
    for (int i = 1; i <= tableLen; ++i)
    {
        lua_rawgeti(l, 1, i);
		strings.push_back(std::string());
		mbLuaToStringExpandMacros(&strings.back(), block, l, -1);
    }
	block->AddLibDirs(strings);
		
    return 0;
}

static int luaFuncExeDirs(lua_State* l)
{
	Block* block = mbGetActiveContext()->ActiveBlock();	
    if (!block)
    {
        MB_LOGERROR("must be within block");
        mbExitError();
    }
	
    luaL_checktype(l, 1, LUA_TTABLE);
    int tableLen =  luaL_len(l, 1);
    
	StringVector strings;
    for (int i = 1; i <= tableLen; ++i)
    {
        lua_rawgeti(l, 1, i);
		strings.push_back(std::string());
		mbLuaToStringExpandMacros(&strings.back(), block, l, -1);
    }
	block->AddExeDirs(strings);
		
    return 0;
}

static int luaFuncFiles(lua_State* l)
{
    Block* b = mbGetActiveContext()->ActiveBlock();
	
    luaL_checktype(l, 1, LUA_TTABLE);
    int tableLen =  luaL_len(l, 1);
    
	StringVector strings;
    for (int i = 1; i <= tableLen; ++i)
    {
        lua_rawgeti(l, 1, i);
		strings.push_back(std::string());
		mbLuaToStringExpandMacros(&strings.back(), b, l, -1);
    }
	
	StringVector filteredList;
	ProcessWildcards(&filteredList, strings);
	b->AddFiles(filteredList);
		
    return 0;
}

static int luaFuncNoPchFiles(lua_State* l)
{
    Block* b = mbGetActiveContext()->ActiveBlock();
	
    luaL_checktype(l, 1, LUA_TTABLE);
    int tableLen =  luaL_len(l, 1);
    
	StringVector strings;
    for (int i = 1; i <= tableLen; ++i)
    {
        lua_rawgeti(l, 1, i);
		strings.push_back(std::string());
		mbLuaToStringExpandMacros(&strings.back(), b, l, -1);
    }
	
	StringVector filteredList;
	ProcessWildcards(&filteredList, strings);
	b->AddNoPchFiles(filteredList);
		
    return 0;
}

static int luaFuncFrameworks(lua_State* l)
{
    Block* b = mbGetActiveContext()->ActiveBlock();
	
    luaL_checktype(l, 1, LUA_TTABLE);
    int tableLen =  luaL_len(l, 1);
    
	StringVector strings;
    for (int i = 1; i <= tableLen; ++i)
    {
        lua_rawgeti(l, 1, i);
		strings.push_back(std::string());
		mbLuaToStringExpandMacros(&strings.back(), b, l, -1);
    }
	
	b->AddFrameworks(strings);
	
    return 0;
}

static int luaFuncResources(lua_State* l)
{
    Block* b = mbGetActiveContext()->ActiveBlock();
	
    luaL_checktype(l, 1, LUA_TTABLE);
    int tableLen =  luaL_len(l, 1);
    
	StringVector strings;
    for (int i = 1; i <= tableLen; ++i)
    {
        lua_rawgeti(l, 1, i);
		strings.push_back(std::string());
		mbLuaToStringExpandMacros(&strings.back(), b, l, -1);
	}
	
	StringVector filteredList;
	ProcessWildcards(&filteredList, strings);
	b->AddResources(filteredList);
    return 0;
}

const char** mbGetStringGroupNames()
{
	return g_stringGroups;
}

void mbBlockLuaRegister(lua_State* l)
{
    lua_pushcfunction(l, luaFuncAddOption);
    lua_setglobal(l, "addoption");
	
	lua_pushcfunction(l, luaFuncSetOption);
	lua_setglobal(l, "option");

	lua_pushcfunction(l, luaFuncSetMacro);
	lua_setglobal(l, "macro");
	
	lua_pushcfunction(l, luaFuncDefines);
    lua_setglobal(l, "defines");
	
    lua_pushcfunction(l, luaFuncIncludeDir);
    lua_setglobal(l, "includedirs");

    lua_pushcfunction(l, luaFuncLibDir);
    lua_setglobal(l, "libdirs");
	
    lua_pushcfunction(l, luaFuncLibs);
    lua_setglobal(l, "libs");

    lua_pushcfunction(l, luaFuncExeDirs);
    lua_setglobal(l, "exedirs");
	
    lua_pushcfunction(l, luaFuncFiles);
    lua_setglobal(l, "files");

    lua_pushcfunction(l, luaFuncNoPchFiles);
    lua_setglobal(l, "nopchfiles");

    lua_pushcfunction(l, luaFuncFrameworks);
    lua_setglobal(l, "frameworks");

    lua_pushcfunction(l, luaFuncResources);
    lua_setglobal(l, "resources");
}

//-----------------------------------------------------------------------------------------------------------------------------------------

void FlatConfig::Init()
{
}

void FlatConfig::Dump()
{
	MB_LOGDEBUG("FlatConfig Dumping %s", name.c_str());
	
	MB_LOGDEBUG("stringGroups:");
	mbDebugDumpGroups(stringGroups);

	MB_LOGDEBUG("options:");
	mbDebugDumpKeyValueGroups(options);
	
	MB_LOGDEBUG("END dumping %s", name.c_str());
}

//-----------------------------------------------------------------------------------------------------------------------------------------

Block::Block()
{
	m_parent = NULL;
	m_keyValueGroups.insert(std::make_pair("__macros", KeyValueMap()));
	m_macroCacheDirty = true;
}

Block::~Block()
{
	for (int i = 0; i < (int)m_childParams.size(); ++i)
	{
		delete m_childParams[i];
	}
}

bool Block::IsA(E_BlockType t) const
{
	return t == E_BlockType_Block;
}

void Block::Process()
{
	std::map<std::string, StringVector>::iterator it = m_stringGroups.find(STRINGGROUP_FILES);
	if (it != m_stringGroups.end())
	{
		AddHeadersAutomatically(&it->second);
	}
	
	for (int i = 0; i < (int)m_childParams.size(); ++i)
	{
		m_childParams[i]->Process();
	}
}

void Block::Dump() const
{
	MB_LOGDEBUG("Dumping %s", m_name.c_str());
	
	MB_LOGDEBUG("m_stringGroups:");
	mbDebugDumpGroups(m_stringGroups);

	MB_LOGDEBUG("m_keyValueGroups:");
	mbDebugDumpKeyValueGroups(m_keyValueGroups);
	
	MB_LOGDEBUG("END dumping %s", m_name.c_str());
}

void Block::SetName(const char* name)
{
	m_name = name;
}

const std::string& Block::GetName() const
{
	return m_name;
}

const char* Block::GetParentConfig() const
{
	for (const Block* block = this; block; block = block->m_parent)
	{
		if (block && block->GetType() == E_BlockType_ConfigParam)
		{
			if (block->m_name.length() == 0)
				return NULL;
			
			return block->m_name.c_str();
		}
	 }
	 
	return NULL;
}

const char* Block::GetParentPlatform() const
{
	for (const Block* block = this; block; block = block->m_parent)
	{
		if (block && block->GetType() == E_BlockType_PlatformParam)
		{
			if (block->m_name.length() == 0)
				return NULL;
			
			return block->m_name.c_str();
		}
	 }
	 
	return NULL;
}

void Block::AddChild(Block* block)
{
	block->m_parent = this;
	
	if (block->IsA(E_BlockType_Param))
	{
		m_childParams.push_back((ParamBlock*)block);
	}
}

Block* Block::GetParent()
{
	return m_parent;
}

const Block* Block::GetParent() const
{
	return m_parent;
}

void Block::AddFiles(const StringVector& files)
{
	StringVector* existing = AcquireStringGroup(STRINGGROUP_FILES);
	mbJoinArrays(existing, files);
};

void Block::AddNoPchFiles(const StringVector& files)
{
	StringVector* existing = AcquireStringGroup(STRINGGROUP_NOPCHFILES);
	mbJoinArrays(existing, files);
};

void Block::AddResources(const StringVector& resources)
{
	StringVector* existing = AcquireStringGroup(STRINGGROUP_RESOURCES);
	mbJoinArrays(existing, resources);
};

void Block::AddFrameworks(const StringVector& frameworks)
{
	StringVector* existing = AcquireStringGroup(STRINGGROUP_FRAMEWORKS);
	mbJoinArrays(existing, frameworks);
};

void Block::AddDefines(const StringVector& defines)
{
	StringVector* existing = AcquireStringGroup(STRINGGROUP_DEFINES);
	mbJoinArrays(existing, defines);
};

void Block::AddIncludeDirs(const StringVector& includeDirs)
{
	StringVector* existing = AcquireStringGroup(STRINGGROUP_INCLUDEDIRS);
	mbJoinArrays(existing, includeDirs);
};

void Block::AddLibDirs(const StringVector& libDirs)
{
	StringVector* existing = AcquireStringGroup(STRINGGROUP_LIBDIRS);
	mbJoinArrays(existing, libDirs);

};

void Block::AddLibs(const StringVector& libs)
{
	StringVector* existing = AcquireStringGroup(STRINGGROUP_LIBS);
	mbJoinArrays(existing, libs);
};

const std::vector<ParamBlock*>& Block::GetParamBlocks() const
{
	return m_childParams;
}

StringVector* Block::AcquireStringGroup(const char* groupName)
{
	std::map<std::string, StringVector>::iterator it = m_stringGroups.find(groupName);
	
	if (it == m_stringGroups.end())
	{
		//Insert new vector as one does not exist already for this platform.
		std::pair<std::map<std::string, StringVector>::iterator, bool> result =
			m_stringGroups.insert(std::make_pair(groupName, StringVector()));
		
		it = result.first;
	}
	
	return &it->second;
}

const StringVector* Block::GetStringGroup(const char* groupName) const
{
	std::map<std::string, StringVector>::const_iterator it = m_stringGroups.find(groupName);
	
	if (it == m_stringGroups.end())
	{
		return NULL;
	}
	
	return &it->second;
}

void Block::AppendOption(const std::string& group, const std::string& key, const std::string& value, char seperator)
{
	std::map<std::string, KeyValueMap>::iterator it = m_keyValueGroups.find(group);
	
	if (it == m_keyValueGroups.end())
	{
		//Insert new vector as one does not exist already for this platform.
		std::pair<std::map<std::string, KeyValueMap>::iterator, bool> result =
			m_keyValueGroups.insert(std::make_pair(group, KeyValueMap()));
		
		it = result.first;
	}

	KeyValueMap& kvmap = (*it).second;
	
	KeyValueMap::iterator kvit = kvmap.find(key);
	if (kvit == kvmap.end())
	{
		kvmap.insert(std::make_pair(key, value));
	}
	else
	{
		const std::string& oldValue = kvit->second;
		
		std::stringstream ss;
		ss << oldValue;
		ss << seperator;
		ss << value;

		kvit->second = ss.str();
	}
}

void Block::SetOption(const std::string& group_, const std::string& key_, const std::string& value_)
{
	std::string group, key, value;
	mbExpandMacros(&group, this, group_.c_str());
	mbExpandMacros(&key, this, key_.c_str());
	mbExpandMacros(&value, this, value_.c_str());

	std::map<std::string, KeyValueMap>::iterator it = m_keyValueGroups.find(group);

	if (it == m_keyValueGroups.end())
	{
		//Insert new vector as one does not exist already for this platform.
		std::pair<std::map<std::string, KeyValueMap>::iterator, bool> result =
			m_keyValueGroups.insert(std::make_pair(group, KeyValueMap()));

		it = result.first;
	}

	KeyValueMap& kvmap = (*it).second;
	kvmap[key] = value;
}

void Block::GetOptions(std::map<std::string, KeyValueMap>* result) const
{
	mbMergeOptions(result, m_keyValueGroups);
}

void Block::SetMacro(const char* key, const char* value)
{
	SetOption("__macros", key, value);
	SetMacroCacheDirty();
}

const KeyValueMap& Block::GetMacros() const
{
	std::map<std::string, KeyValueMap>::const_iterator it = m_keyValueGroups.find("__macros");
	assert(it != m_keyValueGroups.end());
	return it->second;
}

const KeyValueMap& Block::FlattenMacros() const
{
	if (!m_macroCacheDirty)
	{
		return m_flattenedMacroCache;
	}

	std::map<std::string, KeyValueMap>::const_iterator it = m_keyValueGroups.find("__macros");
	assert(it != m_keyValueGroups.end());

	m_flattenedMacroCache.clear();
	for (const Block* block = this; block; block = block->GetParent())
	{
		const KeyValueMap& blockMacros = block->GetMacros();
		m_flattenedMacroCache.insert(blockMacros.begin(), blockMacros.end());
	}

	m_macroCacheDirty = false;
	return m_flattenedMacroCache;
}

void Block::AddExeDirs(const StringVector& exeDirs)
{
	StringVector* existing = AcquireStringGroup(STRINGGROUP_EXEDIRS);
	mbJoinArrays(existing, exeDirs);
}

ConfigParam* Block::AcquireConfigParam(const char* configName)
{
	ConfigParam* config = (ConfigParam*)GetParam(E_BlockType_ConfigParam, configName);
	if (config)
		return config;

	config = new ConfigParam();
	AddChild(config);
    config->m_name = configName;
	return config;
}

PlatformParam* Block::AcquirePlatformParam(const char* configName)
{
	PlatformParam* config = (PlatformParam*)GetParam(E_BlockType_PlatformParam, configName);
	if (config)
		return config;

	config = new PlatformParam();
	AddChild(config);
    config->m_name = configName;
	return config;
}

//When a config or param is specified, then not only will this include content specific to that platform or config, but also all generic platform or config data
void Block::GetParams(ParamVector* result, E_BlockType t, const char* platformName, const char* configName, bool recurseChildParams) const
{
	std::vector<Block*> childrenToProcess;
	
	for (int i = 0; i < (int)m_childParams.size(); ++i)
	{
		Block* child = m_childParams[i];
				
		E_BlockType childType = child->GetType();
		
		if (t != E_BlockType_Unknown && childType != t)
			continue;
		
		if (childType == E_BlockType_PlatformParam)
		{
			if (platformName)
			{
				//If we've specified that we want to include only the given platform, and the name of this platform
				//block does not match this then skip this node.
				if (child->GetName() != platformName)
					continue;
			}
			else
			{
				//Specifying null indicates we want all blocks. To exclude platform specific blocks, pass in "" as the platform
			}
		}

		if (childType == E_BlockType_ConfigParam)
		{
			if (configName)
			{
				//If we've specified that we want to include only the given config, and the name of this config
				//block does not match this then skip this node.
				if (child->GetName() != configName)
					continue;
			}
			else
			{
				//Specifying null indicates we want all blocks. To exclude config specific blocks, pass in "" as the config
			}
		}
		
		if (platformName)
		{
			const char* childPlatform = child->GetParentPlatform();
			assert(!childPlatform || strcmp(platformName, childPlatform) == 0);
		}
		if (configName)
		{
			const char* childConfig = child->GetParentConfig();
			assert(!childConfig || strcmp(configName, childConfig) == 0);
		}

		childrenToProcess.push_back(child);
		result->push_back((ParamBlock*)child);
	}
	
	if (recurseChildParams)
	{
		for (int i = 0; i < (int)childrenToProcess.size(); ++i)
		{
			Block* child = childrenToProcess[i];
			child->GetParams(result, t, platformName, configName, recurseChildParams);
		}
	}
}

ParamBlock* Block::GetParam(E_BlockType t, const char* name)
{
	for (int i = 0; i < (int)m_childParams.size(); ++i)
	{
		Block* child = m_childParams[i];
		if (child->GetType() == t && child->GetName() == name)
			return (ParamBlock*)child;
	}
	
	return NULL;
}

const ParamBlock* Block::GetParam(E_BlockType t, const char* name) const
{
	return const_cast<Block*>(this)->GetParam(t, name);
}

void Block::FlattenThis(FlatConfig* result) const
{
	//Add param strings and options from block to result.
	mbMergeStringGroups(&result->stringGroups, m_stringGroups);
	GetOptions(&result->options);
}

void Block::SetMacroCacheDirty() const
{
	m_macroCacheDirty = true;
	for (int i = 0; i < (int)m_childParams.size(); ++i)
	{
		m_childParams[i]->SetMacroCacheDirty();
	}
}

//-----------------------------------------------------------------------------------------------------------------------------------------

MakeBlock::MakeBlock()
{
}

MakeBlock::~MakeBlock()
{
	for (int i = 0; i < (int)m_childMakeBlocks.size(); ++i)
	{
		delete m_childMakeBlocks[i];
	}
}

bool MakeBlock::IsA(E_BlockType t) const
{
	if (Block::IsA(t))
		return true;

	return t == E_BlockType_MakeBlock;
}

void MakeBlock::AddChild(Block* block)
{
	Block::AddChild(block);
	
	if (block->IsA(E_BlockType_MakeBlock))
	{
		m_childMakeBlocks.push_back((MakeBlock*)block);
	}
}

void MakeBlock::Process()
{
	Block::Process();
	
	for (int i = 0; i < (int)m_childMakeBlocks.size(); ++i)
	{
		m_childMakeBlocks[i]->Process();
	}
}
	
void MakeBlock::Dump() const
{
	Block::Dump();
}

//-----------------------------------------------------------------------------------------------------------------------------------------

ParamBlock::ParamBlock()
{
}

ParamBlock::~ParamBlock()
{
}

bool ParamBlock::IsA(E_BlockType t) const
{
	if (Block::IsA(t))
		return true;

	return t == E_BlockType_Param;
}

void ParamBlock::Process()
{
	Block::Process();
}
	
void ParamBlock::Dump() const
{
	Block::Dump();
}


