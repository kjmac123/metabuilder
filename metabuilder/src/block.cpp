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

const char** mbGetStringGroupNames()
{
	return g_stringGroups;
}

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
						if (mbGetAppState()->cmdSetup.verbose)
						{
							MB_LOGINFO("Automatically adding header file %s", candidateRelativeName);
						}
						result.push_back(candidateRelativeName);
					}
				}
			}
		}
	}
	
	*files = result;
}

Block::Block()
{
	m_parent = NULL;
}

Block::~Block()
{
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
	MB_LOGINFO("Dumping %s", m_name.c_str());
	
	MB_LOGINFO("m_stringGroups:");
	mbDebugDumpGroups(m_stringGroups);

	MB_LOGINFO("m_keyValueGroups:");
	mbDebugDumpKeyValueGroups(m_keyValueGroups);
	
	MB_LOGINFO("END dumping %s", m_name.c_str());
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
		if (block && block->Type() == E_BlockType_ConfigParam)
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
		if (block && block->Type() == E_BlockType_PlatformParam)
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

void Block::AddFiles(const StringVector& files)
{
	StringVector* existing = AcquireStringGroup(STRINGGROUP_FILES);
	mbJoinArrays(existing, files);
};

void Block::GetFiles(StringVector* result) const
{
	const StringVector* existing = GetStringGroup(STRINGGROUP_FILES);
	if (existing)
	{
		mbJoinArrays(result, *existing);
	}
}

void Block::AddResources(const StringVector& resources)
{
	StringVector* existing = AcquireStringGroup(STRINGGROUP_RESOURCES);
	mbJoinArrays(existing, resources);
};

void Block::GetResources(StringVector* result) const
{
	const StringVector* existing = GetStringGroup(STRINGGROUP_RESOURCES);
	if (existing)
	{
		mbJoinArrays(result, *existing);
	}
}

void Block::AddFrameworks(const StringVector& frameworks)
{
	StringVector* existing = AcquireStringGroup(STRINGGROUP_FRAMEWORKS);
	mbJoinArrays(existing, frameworks);
};

void Block::GetFrameworks(StringVector* result) const
{
	const StringVector* existing = GetStringGroup(STRINGGROUP_FRAMEWORKS);
	if (existing)
	{
		mbJoinArrays(result, *existing);
	}
}

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

void Block::GetStringGroups(std::map<std::string, StringVector>* result) const
{
	mbMergeStringGroups(result, m_stringGroups);
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
				
		E_BlockType childType = child->Type();
		
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
		if (child->Type() == t && child->GetName() == name)
			return (ParamBlock*)child;
	}
	
	return NULL;
}

const ParamBlock* Block::GetParam(E_BlockType t, const char* name) const
{
	return const_cast<Block*>(this)->GetParam(t, name);
}

MakeBlock::MakeBlock()
{
}

MakeBlock::~MakeBlock()
{
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
}
	
void MakeBlock::Dump() const
{
	Block::Dump();
}



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
