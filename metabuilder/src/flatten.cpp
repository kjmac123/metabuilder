#include "metabuilder_pch.h"

#include "common.h"

void FlatConfig::Dump()
{
	MB_LOGINFO("FlatConfig Dumping %s", name.c_str());
	
	MB_LOGINFO("stringGroups:");
	mbDebugDumpGroups(stringGroups);

	MB_LOGINFO("options:");
	mbDebugDumpKeyValueGroups(options);
	
	MB_LOGINFO("END dumping %s", name.c_str());
}

static void mbFlattenMakeBlock(FlatConfig* result, Block* block)
{
	//Add param strings and options from block to result.
	block->GetStringGroups(&result->stringGroups);
	block->GetOptions(&result->options);
}

void mbFlattenTargetForWriter(FlatConfig* result, Target* target, const char* platformName, const char* configName)
{
	MB_LOGINFO("Flattening Target %s platform %s config %s", target->GetName().c_str(), platformName ? platformName : "{ALL}", configName ? configName : "{ALL}");
	//TODO - move this
	if (configName)
	{
		result->name = configName;
	}
	
	//Build a list so that we can traverse our hierarchy top to bottom.
	ParamVector params;
	
	std::vector<Block*> blocks;
	for (Block* block = target; block; block = block->GetParent())
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
		Block* block = blocks[iBlock];
		MB_LOGINFO("Merging block %s", block->GetName().c_str());
		mbFlattenMakeBlock(result, block);
	}

	//Merge config specific params
	for (int iBlock = (int)params.size()-1; iBlock >= 0; --iBlock)
	{
		Block* block = params[iBlock];
		MB_LOGINFO("Merging config block %s parent config %s", block->GetName().c_str(), block->GetParentPlatform() ? block->GetParentPlatform() : "none");
		mbFlattenMakeBlock(result, block);
	}
	
	result->Dump();
}
