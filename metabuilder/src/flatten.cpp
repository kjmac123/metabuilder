#include "metabuilder_pch.h"

#include "common.h"

//Constraints
//Configs cannot have children
//Platform params can only have children of type config or SDKConfig

void FlatConfig::Dump()
{
	MB_LOGINFO("FlatConfig Dumping %s", name.c_str());
	
	MB_LOGINFO("stringGroups:");
	mbDebugDumpGroups(stringGroups);

	MB_LOGINFO("options:");
	mbDebugDumpKeyValueGroups(options);
	
	MB_LOGINFO("END dumping %s", name.c_str());
}

void FlatSDKConfig::Dump()
{
	MB_LOGINFO("FlatSDKConfig Dumping %s", name.c_str());
	
	MB_LOGINFO("mainconfigname: %s", mainConfigName.c_str());
	
	MB_LOGINFO("stringGroups:");
	mbDebugDumpGroups(stringGroups);

	MB_LOGINFO("options:");
	mbDebugDumpKeyValueGroups(options);
	
	MB_LOGINFO("END dumping %s", name.c_str());
}

//Flatten a single block for a given config/SDKConfig
//This means that all of the internal param nodes will get merged together. Note that this function is recursive.
static void mbFlattenMakeBlockForConfig(FlatConfig* result, Block* block, const char* configName, bool flattenForSDKConfig)
{
	//1) Add param strings and options from block to result.
	block->GetStringGroups(&result->stringGroups, configName);
	block->GetOptions(&result->options, configName);

	//2) Add param strings and options from top level config params to result.
	{
		ConfigParamVector configParams;
		block->GetConfigParams(&configParams, false);
		for (int i = 0; i < (int)configParams.size(); ++i)
		{
			mbFlattenMakeBlockForConfig(result, configParams[i], configName, flattenForSDKConfig);
		}
	}
	
	//3) Add param strings and options from top level SDK configs to result, if we're processing SDK configs.
	if (flattenForSDKConfig)
	{
		SDKParamVector sdkParams;
		block->GetSDKParams(&sdkParams, false);
		for (int i = 0; i < (int)sdkParams.size(); ++i)
		{
			mbFlattenMakeBlockForConfig(result, sdkParams[i], configName, flattenForSDKConfig);
		}
	}
	
	//4) Add param strings and options from flattened top level platform params to result.
	{
		PlatformParamVector platformParams;
		block->GetPlatformParams(&platformParams, false);
		for (int i = 0; i < (int)platformParams.size(); ++i)
		{
			mbFlattenMakeBlockForConfig(result, platformParams[i], configName, flattenForSDKConfig);
		}
	}
}

static void mbFlattenTargetForConfig(FlatConfig* result, Target* target, const char* configName, bool flattenForSDKConfig)
{
	//Build a list so that we can traverse our hierarchy top to bottom.
	std::vector<Block*> blocks;
	for (Block* block = target; block; block = block->GetParent())
	{
		blocks.push_back(block);
	}

	for (int iBlock = (int)blocks.size()-1; iBlock >= 0; --iBlock)
	{
		mbFlattenMakeBlockForConfig(result, blocks[iBlock], configName, flattenForSDKConfig);
	}
}

void mbFlattenTargetForConfig(FlatConfig* result, Target* target, const char* configName)
{
	mbFlattenTargetForConfig(result, target, configName, false);
	result->Dump();
}

void mbFlattenTargetForSDKConfig(FlatSDKConfig* result, Target* target, const char* sdkConfigName)
{
	mbFlattenTargetForConfig(result, target, sdkConfigName, true);
	result->mainConfigName = sdkConfigName;
	result->Dump();
}











/*
	//Build a list containing ALL configs
	SDKConfigParamVector allConfigs;
	for (Block* block = this; block; block = block->GetParent())
	{
		block->GetConfigParams(&allConfigs, true);
	}
	
	//Build a list containing just the sdk configs
	SDKConfigParamVector sdkConfigs;
	for (int i = 0; i < (int)allConfigs.size(); ++i)
	{
		ConfigParam* config = allConfigs[i];
		MB_LOGINFO(config->GetName().c_str());
		//SDK configs are marked using Apple style [] sytax. e.g. "[sdk=iphoneos*]"
		if (strstr(config->GetName().c_str(), "[sdk="))
		{
			sdkConfigs.push_back(config);
		}
	}
	
	for (int iSDKConfig = 0; iSDKConfig < (int)sdkConfigs.size(); ++iSDKConfig)
	{
		char sdkMainName[2048] = {0};
		ConfigParam* sdkConfig = sdkConfigs[iSDKConfig];
		//Build a string that will map to the main config names to link the two together.
		char* tmp = strstr((char*)sdkConfig->GetName().c_str(), "[sdk="); //(slightly evil cast)
		if (tmp)
		{
			*tmp = '\0';
			strcpy(sdkMainName, sdkConfig->GetName().c_str());
			*tmp = '[';
		}
		
		//If we've no main name then this sdk config applies to all main configs.
		if (sdkMainName[0] == '\0')
		{
			result->push_back(sdkConfig);
		}
		else
		{
			//If our target has a config that matches the one that the SDK config seeks to match then we'll use it.
			ConfigParam* mainConfig = GetConfigParam(sdkMainName);
			if (mainConfig)
			{
				result->push_back(sdkConfig);
			}
			else
			{
				if (mbGetAppState()->cmdSetup.verbose)
				{
					MB_LOGINFO("SDK config %s does not match any main config on the target %s", sdkMainName, m_name.c_str());
				}
			}
		}
	}
*/

/*
static void mbFlattenOptions(
	std::map<std::string, KeyValueMap>* result,
	Metabase* generator,
	Solution* solution,
	Target* target,
	const std::string* configName)
{
	if (generator)
		generator->GetOptions(result, configName);
	if (solution)
		solution->GetOptions(result, configName);
	if (target)
		target->GetOptions(result, configName);
}
*/
/*
void mbBuildFlatStringListDefines(
	StringVector* strings,
	Block* block_,
	const char* configName)
{
	std::vector<Block*> blocks;
	for (Block* block = block_; block; block = block->GetParent())
	{
		blocks.push_back(block);
	}
	
	for (int iBlock = (int)blocks.size()-1; iBlock >= 0; --iBlock)
	{
		blocks[iBlock]->GetDefines(strings, configName);
	}
	
	mbRemoveDuplicates(strings);
}
*/
//Build list of configs matching given name. We'll collapse multiple results together in a seperate phase.
static void BuildConfigList(ParamBlock* paramBlock, const char* configName)
{
	
}

static void mbFlattenMakeNode(FlatConfig* result, MakeBlock* makeBlock, const char* configName, bool isSDKConfig)
{
/*
	//Find the deepest config that matches our config name. Could for instance be parented to a platform node within our makenode.
	{
		const std::string<ParamBlock*>& paramBlocks = makeNode->GetParamBlocks();
		
	}
	
	//Flatten macros
	StringVector defines;
	makeNode->GetDefines();
	mbBuildFlatStringListDefines(&defines,	target,	configName);

	//Flatten include dirs
	StringVector includeDirs;
	mbBuildFlatStringListGetIncludeDirs(&includeDirs, target, configName);

	//Flatten lib dirs
	StringVector libsDirs;
	mbBuildFlatStringListGetLibDirs(&libsDirs, target, configName);

	//Flatten libs
	StringVector libs;
	mbBuildFlatStringListGetLibs(&libs,	target, configName);

	//Flatten exe dirs
	StringVector exeDirs;
	mbBuildFlatStringListGetExeDirs(&exeDirs, target, configName);

	std::map<std::string, KeyValueMap> options;
	mbFlattenConfigOptions(&options,
		metabase,
		solution,
		target,
		&configName);
	
	//Options
	std::map<std::string, KeyValueMap> options;
	mbFlattenConfigOptions(&options,
		metabase,
		solution,
		target,
		&configName);
		*/
}

/*
void mbFlattenConfig(FlatConfig* result, MakeBlock* makeBlock, const char* configName, bool isSDKConfig)
{
	//Walk make-nodes from root to leaf
	std::vector<Block*> blocks;
	for (Block* block = makeBlock; block; block = block->GetParent())
	{
		blocks.push_back(block);
	}

	for (int iBlock = (int)blocks.size()-1; iBlock >= 0; --iBlock)
	{
		mbFlattenMakeNode(result, blocks[i], configName);
	}
	StringVector defines;
	StringVector includeDirs;
	StringVector libsDirs;
	StringVector exeDirs;
	StringVector libs;
		mbRemoveDuplicates(result->);
}
		*/

