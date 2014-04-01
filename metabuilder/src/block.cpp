#include "metabuilder_pch.h"

#include "common.h"

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Free functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
						MB_LOGDEBUG("Automatically adding header file %s", candidateRelativeName);
						result.push_back(candidateRelativeName);
					}
				}
			}
		}
	}
	
	*files = result;
}


static void ProcessWildcards(StringVector* result, const StringVector& input)
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

static int luaFuncSetOption(lua_State* l)
{
	Block* block = mbGetActiveContext()->ActiveBlock();
    if (!block)
    {
        MB_LOGERROR("must be within block");
        mbExitError();
    }
	
    const char* group = lua_tostring(l, 1);
    const char* key = lua_tostring(l, 2);
	const char* value = lua_tostring(l, 3);
	if (!value)
	{
		value = "";
	}
	block->SetOption(group, key, value);
	return 0;
}

static int luaFuncDefines(lua_State* lua)
{
    Block* block = mbGetActiveContext()->ActiveBlock();
    if (!block)
    {
        MB_LOGERROR("must be set within a block!");
        mbExitError();
    }
    
    luaL_checktype(lua, 1, LUA_TTABLE);
    int tableLen =  luaL_len(lua, 1);
    
    StringVector defines;
    for (int i = 1; i <= tableLen; ++i)
    {
        lua_rawgeti(lua, 1, i);
        const char* tmp = lua_tostring(lua, -1);
        defines.push_back(tmp);
    }
    block->AddDefines(defines);
    
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
        const char* str = lua_tostring(l, -1);
        strings.push_back(str);
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
        const char* dir = lua_tostring(l, -1);
        strings.push_back(dir);
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
        const char* dir = lua_tostring(l, -1);
        strings.push_back(dir);
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
        const char* dir = lua_tostring(l, -1);
        strings.push_back(dir);
    }
	block->AddExeDirs(strings);
		
    return 0;
}

static int luaFuncFiles(lua_State* l)
{
    Block* b = mbGetActiveContext()->ActiveBlock();
	
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
	ProcessWildcards(&filteredList, inputFiles);
	b->AddFiles(filteredList);
		
    return 0;
}

static int luaFuncFrameworks(lua_State* l)
{
    Block* b = mbGetActiveContext()->ActiveBlock();
	
    luaL_checktype(l, 1, LUA_TTABLE);
    int tableLen =  luaL_len(l, 1);
    
	StringVector inputFiles;
    for (int i = 1; i <= tableLen; ++i)
    {
        lua_rawgeti(l, 1, i);
        const char* filename = lua_tostring(l, -1);
		inputFiles.push_back(filename);
    }
	
	b->AddFrameworks(inputFiles);
	
    return 0;
}

static int luaFuncResources(lua_State* l)
{
    Block* b = mbGetActiveContext()->ActiveBlock();
	
    luaL_checktype(l, 1, LUA_TTABLE);
    int tableLen =  luaL_len(l, 1);
    
	StringVector tableContents;
    for (int i = 1; i <= tableLen; ++i)
    {
        lua_rawgeti(l, 1, i);
        const char* tmp = lua_tostring(l, -1);
		tableContents.push_back(tmp);    }
	
	
	StringVector filteredList;
	ProcessWildcards(&filteredList, tableContents);
	b->AddResources(filteredList);
	
    return 0;
}

const char** mbGetStringGroupNames()
{
	return g_stringGroups;
}

void mbBlockLuaRegister(lua_State* l)
{
    lua_pushcfunction(l, luaFuncSetOption);
    lua_setglobal(l, "option");
	
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

    lua_pushcfunction(l, luaFuncFrameworks);
    lua_setglobal(l, "frameworks");

    lua_pushcfunction(l, luaFuncResources);
    lua_setglobal(l, "resources");
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FlatConfig
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FlatConfig::Dump()
{
	MB_LOGDEBUG("FlatConfig Dumping %s", name.c_str());
	
	MB_LOGDEBUG("stringGroups:");
	mbDebugDumpGroups(stringGroups);

	MB_LOGDEBUG("options:");
	mbDebugDumpKeyValueGroups(options);
	
	MB_LOGDEBUG("END dumping %s", name.c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Block
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Block::Block()
{
	m_parent = NULL;
}

Block::~Block()
{
	for (int i = 0; i < m_childParams.size(); ++i)
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
	
	for (int i = 0; i < m_childParams.size(); ++i)
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

void Block::SetOption(const std::string& group, const std::string& key, const std::string& value)
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
	kvmap[key] = value;
}

void Block::GetOptions(std::map<std::string, KeyValueMap>* result) const
{
	mbMergeOptions(result, m_keyValueGroups);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MakeBlock
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MakeBlock::MakeBlock()
{
}

MakeBlock::~MakeBlock()
{
	for (int i = 0; i < m_childMakeBlocks.size(); ++i)
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
	
	for (int i = 0; i < m_childMakeBlocks.size(); ++i)
	{
		m_childMakeBlocks[i]->Process();
	}
}
	
void MakeBlock::Dump() const
{
	Block::Dump();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ParamBlock
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

