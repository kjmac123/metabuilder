#include "metabuilder_pch.h"

#include "common.h"

#define STRINGGROUP_FILES					"files"
#define STRINGGROUP_DEFINES					"defines"
#define STRINGGROUP_INCLUDEDIRS				"includedirs"
#define STRINGGROUP_LIBDIRS					"libdirs"
#define STRINGGROUP_LIBS					"libs"
#define STRINGGROUP_EXEDIRS					"exedirs"
#define STRINGGROUP_RESOURCES				"resources"
#define STRINGGROUP_FRAMEWORKS				"frameworks"

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

void Block::GetFiles(StringVector* result/*, const char* configName*/) const
{
	const StringVector* existing = GetStringGroup(STRINGGROUP_FILES);
	if (existing)
	{
		mbJoinArrays(result, *existing);
	}
	
	/*
	if (configName)
	{
		ConfigParam* config = GetConfigParam(configName);
		if (config->GetName() == configName)
		{
			config->GetFiles(result, NULL);
		}
	}
	*/
}

void Block::AddResources(const StringVector& resources)
{
	StringVector* existing = AcquireStringGroup(STRINGGROUP_RESOURCES);
	mbJoinArrays(existing, resources);
};

void Block::GetResources(StringVector* result/*, const char* configName*/) const
{
	const StringVector* existing = GetStringGroup(STRINGGROUP_RESOURCES);
	if (existing)
	{
		mbJoinArrays(result, *existing);
	}
	
	/*
	if (configName)
	{
		ConfigParam* config = GetConfigParam(configName);
		if (config->GetName() == configName)
		{
			config->GetResources(result, NULL);
		}
	}
	*/
}

void Block::AddFrameworks(const StringVector& frameworks)
{
	StringVector* existing = AcquireStringGroup(STRINGGROUP_FRAMEWORKS);
	mbJoinArrays(existing, frameworks);
};

/*
void Block::GetFrameworks(StringVector* result) const
{
	const StringVector* existing = GetStringGroup(STRINGGROUP_FRAMEWORKS);
	if (existing)
	{
		mbJoinArrays(result, *existing);
	}
	*/
	/*
	if (configName)
	{
		ConfigParam* config = GetConfigParam(configName);
		if (config->GetName() == configName)
		{
			config->GetFrameworks(result, NULL);
		}
	}
	*/
//}

void Block::AddDefines(const StringVector& defines)
{
	StringVector* existing = AcquireStringGroup(STRINGGROUP_DEFINES);
	mbJoinArrays(existing, defines);
};
/*
void Block::GetDefines(StringVector* result, const char* configName) const
{
	const StringVector* existing = GetStringGroup(STRINGGROUP_DEFINES);
	if (existing)
	{
		mbJoinArrays(result, *existing);
	}
	
	if (configName)
	{
		const ConfigParam* config = GetConfigParam(configName);
		if (config)
		{
			if (config->GetName() == configName)
			{
				config->GetDefines(result, NULL);
			}
		}
	}
}
*/

void Block::AddIncludeDirs(const StringVector& includeDirs)
{
	StringVector* existing = AcquireStringGroup(STRINGGROUP_INCLUDEDIRS);
	mbJoinArrays(existing, includeDirs);
};
/*
void Block::GetIncludeDirs(StringVector* result, const char* configName) const
{
	const StringVector* existing = GetStringGroup(STRINGGROUP_INCLUDEDIRS);
	if (existing)
	{
		mbJoinArrays(result, *existing);
	}
	
	if (configName)
	{
		const ConfigParam* config = GetConfigParam(configName);
		if (config)
		{
			if (config->GetName() == configName)
			{
				config->GetIncludeDirs(result, NULL);
			}
		}
	}
}
*/
void Block::AddLibDirs(const StringVector& libDirs)
{
	StringVector* existing = AcquireStringGroup(STRINGGROUP_LIBDIRS);
	mbJoinArrays(existing, libDirs);

};
/*
void Block::GetLibDirs(StringVector* result, const char* configName) const
{
	const StringVector* existing = GetStringGroup(STRINGGROUP_LIBDIRS);
	if (existing)
	{
		mbJoinArrays(result, *existing);
	}
	
	if (configName)
	{
		const ConfigParam* config = GetConfigParam(configName);
		if (config)
		{
			if (config->GetName() == configName)
			{
				config->GetLibDirs(result, NULL);
			}
		}
	}
}
*/
void Block::AddLibs(const StringVector& libs)
{
	StringVector* existing = AcquireStringGroup(STRINGGROUP_LIBS);
	mbJoinArrays(existing, libs);
};

/*
void Block::GetLibs(StringVector* result, const char* configName) const
{
	const StringVector* existing = GetStringGroup(STRINGGROUP_LIBS);
	if (existing)
	{
		mbJoinArrays(result, *existing);
	}
	
	if (configName)
	{
		const ConfigParam* config = GetConfigParam(configName);
		if (config)
		{
			if (config->GetName() == configName)
			{
				config->GetLibs(result, NULL);
			}
		}
	}
}
*/

const std::vector<ParamBlock*> Block::GetParamBlocks() const
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

void Block::GetStringGroups(std::map<std::string, StringVector>* result, const char* configName) const
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

void Block::GetOptions(std::map<std::string, KeyValueMap>* result, const char* configName) const
{
	mbMergeOptions(result, m_keyValueGroups);
}

void Block::AddExeDirs(const StringVector& exeDirs)
{
	StringVector* existing = AcquireStringGroup(STRINGGROUP_EXEDIRS);
	mbJoinArrays(existing, exeDirs);

}
/*
void Block::GetExeDirs(StringVector* result, const char* configName) const
{
	const StringVector* existing = GetStringGroup(STRINGGROUP_EXEDIRS);
	if (existing)
	{
		mbJoinArrays(result, *existing);
	}
	
	if (configName)
	{
		const ConfigParam* config = GetConfigParam(configName);
		if (config)
		{
			if (config->GetName() == configName)
			{
				config->GetExeDirs(result, NULL);
			}
		}
	}
}
*/



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

SDKParam* Block::AcquireSDKParam(const char* configName)
{
	SDKParam* config = (SDKParam*)GetParam(E_BlockType_SDKParam, configName);
	if (config)
		return config;

	config = new SDKParam();
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


void Block::GetParams(ParamVector* result, E_BlockType t, bool recurseChildParams) const
{
	for (int i = 0; i < (int)m_childParams.size(); ++i)
	{
		Block* child = m_childParams[i];
		if (child->Type() == t)
		{
			result->push_back((ParamBlock*)child);
		}
	}
	
	if (recurseChildParams)
	{
		for (int i = 0; i < (int)m_childParams.size(); ++i)
		{
			Block* child = m_childParams[i];
			child->GetParams(result, t, recurseChildParams);
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
/*
void Block::GetPlatformParams(PlatformParamVector* result) const
{
	for (int i = 0; i < (int)m_childParams.size(); ++i)
	{
		Block* child = m_childParams[i];
		if (child->Type() == E_BlockType_PlatformParam)
		{
			result->push_back((PlatformParam*)child);
		}
	}
}

PlatformParam* Block::GetPlatformParam(const char* configName)
{
	for (int i = 0; i < (int)m_childParams.size(); ++i)
	{
		Block* child = m_childParams[i];
		if (child->Type() == E_BlockType_PlatformParam && child->GetName() == configName)
			return (PlatformParam*)child;
	}
	
	return NULL;
}

const PlatformParam* Block::GetPlatformParam(const char* configName) const
{
	for (int i = 0; i < (int)m_childParams.size(); ++i)
	{
		Block* child = m_childParams[i];
		if (child->Type() == E_BlockType_PlatformParam && child->GetName() == configName)
			return (PlatformParam*)child;
	}
	
	return NULL;
}
*/


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
