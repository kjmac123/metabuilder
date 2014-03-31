#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <math.h>
#include <assert.h>

#include <vector>
#include <string>
#include <stack>
#include <map>
#include <set>
#include <list>

#include "stdio.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "core.h"

enum E_BlockType
{
	E_BlockType_Unknown,
	E_BlockType_Block,
    E_BlockType_MakeSetup,
    E_BlockType_ConfigParam,
    E_BlockType_Metabase,
    E_BlockType_Solution,
    E_BlockType_Target,
	E_BlockType_PlatformParam,
	E_BlockType_MakeBlock,
	E_BlockType_Param
};

class Block;
class Metabase;
class Solution;
class ConfigParam;
class MakeSetup;
class PlatformParam;
class ParamBlock;

struct CmdSetup
{
    CmdSetup()
    {}

    std::string _inputFile;
    std::string _generator;
	std::string _metabaseDir;
	std::string _makeOutputDir;
	bool		verbose;
};

struct GeneratorMapping
{
    std::string name;
    std::string luaFileAbs;
};

class AppState
{
public:
	AppState()
	{
		isProcessingPrimaryMakefile = false;
		makeSetup = NULL;
	}
	
	~AppState()
	{
		delete makeSetup;
	}
	
	void Process();
	
	std::string mainSolutionName;
    std::string mainMetaMakeFileAbs;
    std::string generator;
	std::string metabaseDirAbs;
	std::string makeOutputDirAbs;
    std::string	intDir;
    std::string	outDir;
	
	
	CmdSetup	cmdSetup;
	MakeSetup*	makeSetup;
	
	bool		isProcessingPrimaryMakefile;
};

typedef std::vector<std::string>		StringVector;
typedef std::vector<ParamBlock*>		ParamVector;
typedef std::vector<ConfigParam*>		ConfigParamVector;
typedef std::vector<PlatformParam*>		PlatformParamVector;

struct KeyValue
{
    std::string key;
    std::string value;
};
typedef std::vector<KeyValue> KeyValueVector;

typedef std::map<std::string, std::string> KeyValueMap;

typedef void (*PostLoadInitFunc)(lua_State*);



class MetaBuilderContext
{
public:
	MetaBuilderContext();
	~MetaBuilderContext();
	
    Block*    ActiveBlock() const
    {
        return activeBlockStack.size() > 0 ? activeBlockStack.top() : NULL;
    }

    void PushActiveBlock(Block* block)
    {
        activeBlockStack.push(block);
    }
    
    void PopActiveBlock()
    {
        activeBlockStack.pop();
    }

    std::string					currentMetaMakeDirAbs;
	Metabase*					metabase;
    Solution*					solution;
    std::stack<Block*>			activeBlockStack;
};

AppState*			mbGetAppState();

void				mbCommonInit(lua_State* l, const std::string& path);
void				mbPushDir(const std::string& path);
void				mbPopDir();
void				mbCommonLuaRegister(lua_State* lua);

void				mbAddMakeFile(const char* makefile);
const StringVector&	mbGetMakeFiles();

MetaBuilderContext* mbCreateContext();
void				mbDestroyContext(MetaBuilderContext* ctx);
MetaBuilderContext* mbGetMainContext();
MetaBuilderContext* mbGetActiveContext();
void				mbPushActiveContext(MetaBuilderContext* ctx);
void				mbPopActiveContext();
const std::list<MetaBuilderContext*>&
					mbGetContexts();

void				mbExitError();
void				mbLuaDoFile(lua_State* lua, const std::string& filepath, PostLoadInitFunc initFunc);
std::string			mbPathJoin(const std::string& a, const std::string& b);
std::string			mbPathGetDir(const std::string& filePath);
std::string			mbPathGetFilename(const std::string& filePath);
bool				mbPathGetFileExtension(char* result, const char* filename);
bool				mbPathReplaceFileExtension(char* result, const char* filename, const char* newExtension);
#if 0
bool				mbPathRelativeDirTo(
						std::string* result,
						const std::string& from,
						const std::string& to);
#endif
void				mbNormaliseFilePath(char* outFilePath, const char* inFilePath);
void				mbNormaliseFilePath(std::string* inout);

bool				mbFileExists(const std::string& filepath);

std::string			mbGetMakeOutputDirRelativePath(const std::string& path);

struct ActiveBlock
{
	E_BlockType type;
	void* block;
};


//Expects top of stack to contain table.
void				mbRegisterBlock(lua_State* l);
ActiveBlock*		mbGetActiveBlock();

void				mbLuaGetDefines(StringVector* defines, lua_State* lua, E_BlockType blockTypeExpected);

void				mbStringReplace(std::string& str, const std::string& oldStr, const std::string& newStr);

void				mbSetPlatformName(const char* name);
void				mbLuaDump(lua_State* l);

void				mbMergeStringGroups(std::map<std::string, StringVector>* result, const std::map<std::string, StringVector>& stringGroup);
void				mbMergeOptions(std::map<std::string, KeyValueMap>* result,	const std::map<std::string, KeyValueMap>& groupOptionMap);

U32					mbRandomU32();

void				mbCheckExpectedBlock(E_BlockType blockExpected, const char* cmdName);

//No duplicates removed
void				mbJoinArrays(StringVector* a, const StringVector& b);
void				mbMergeArrays(StringVector* a, const StringVector& b);
void				mbRemoveDuplicates(StringVector* strings);
void				mbRemoveDuplicatesAndSort(StringVector* strings);

bool				mbCreateDirChain(const char* osDir_);

void				mbDebugDumpKeyValueGroups(const std::map<std::string, KeyValueMap>& kvGroups);
void				mbDebugDumpGroups(const std::map<std::string, StringVector>& stringGroups);


#include "block.h"
#include "makesetup.h"
#include "configparam.h"
#include "target.h"
#include "solution.h"
#include "metabase.h"
#include "platformparam.h"
#include "writer.h"
#include "writer_msvc.h"
#include "writer_xcode.h"

#endif
