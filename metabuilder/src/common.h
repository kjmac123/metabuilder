#ifndef COMMON_H
#define COMMON_H

#include <assert.h>

#include <vector>
#include <string>
#include <stack>
#include <map>
#include <set>
#include <list>

#include "stdio.h"

#ifndef LUA_AS_CPP
extern "C"
{
#endif
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#ifndef LUA_AS_CPP
}
#endif

#include "core.h"

enum E_BlockType
{
	E_BlockType_Unknown,
	E_BlockType_Block,
    E_BlockType_MakeSetup,
	E_BlockType_MakeGlobal,
    E_BlockType_ConfigParam,
    E_BlockType_Metabase,
    E_BlockType_Solution,
    E_BlockType_Target,
	E_BlockType_PlatformParam,
	E_BlockType_MakeBlock,
	E_BlockType_Param
};

enum E_LineEndingStyle
{
	E_LineEndingStyle_Default = 0,
	E_LineEndingStyle_Windows,
	E_LineEndingStyle_UNIX,
};

class Block;
class Metabase;
class Solution;
class ConfigParam;
class MakeSetup;
class MakeGlobal;
class PlatformParam;
class ParamBlock;

struct CmdSetup
{
    CmdSetup()
    {}

    std::string				_inputFile;
    std::string				_generator;
	std::string				_metabaseDir;
	std::string				_makeOutputTopDir;
	std::string				lineEndingStyle;
	bool					verbose;
};

struct GeneratorMapping
{
    std::string name;
    std::string luaFileAbs;
};

class AppState
{
public:
	AppState();	
	~AppState();
	
	void ProcessSetup();
	void ProcessGlobal();
	
	std::string				mainSolutionName;
    std::string				mainMetaMakeFileAbs;
    std::string				generator;
	std::string				metabaseDirAbs;
	std::string				makeOutputTopDirAbs;
    std::string				intDir;
    std::string				outDir;
	E_LineEndingStyle		lineEndingStyle;
	
	CmdSetup				cmdSetup;
	MakeSetup*				makeSetup;
	MakeGlobal*				makeGlobal;
	
	bool					isProcessingPrimaryMakefile;
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
		return activeBlockStack.size() > 0 ? activeBlockStack.top() : nullptr;
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
	std::string					makeOutputDirAbs;
	Metabase*					metabase;
    Solution*					solution;
    std::stack<Block*>			activeBlockStack;
	bool						isMainMakefile;
};

class LuaModuleFunctions
{
public:
					LuaModuleFunctions();
	void			AddFunction(const char* name, lua_CFunction fn);
	void			RegisterLuaGlobal(lua_State* l);
	void			RegisterLuaModule(lua_State* l, const char* moduleName);

private:
	luaL_Reg		m_luaFunctions[MB_LUAMODULE_MAX_FUNCTIONS+1]; //+1 to allow for sentinel
	int				m_nFunctions;
};

AppState*			mbGetAppState();

void				mbCommonInit(lua_State* l, const std::string& path);

const char**		mbGetCAndCPPSourceFileExtensions();
const char**		mbGetCAndCPPHeaderFileExtensions();
const char**		mbGetCAndCPPInlineFileExtensions();

void				mbPushDir(const std::string& path);
void				mbPopDir();
void				mbCommonLuaRegister(lua_State* lua, LuaModuleFunctions* luaFn);

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

void				mbNormaliseFilePath(char* outFilePath, const char* inFilePath, char dirSep);
void				mbNormaliseFilePath(char* inout, char dirSep);
void				mbNormaliseFilePath(std::string* inout, char dirSep);

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

bool				mbStringReplace(std::string& str, const std::string& oldStr, const std::string& newStr);

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

void				mbExpandMacros(std::string* result, const std::map<std::string, std::string>& macroMap, const char* str);
void				mbExpandMacros(std::string* result, Block* block, const char* str);
const char*			mbLuaToStringExpandMacros(std::string* result, Block* block, lua_State* l, int stackPos);

void*				mbLuaAllocator(void* ud, void* ptr, size_t osize, size_t nsize);


#endif
