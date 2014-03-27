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
    E_BlockType_MakeSetup,
    E_BlockType_Config,
    E_BlockType_Metabase,
    E_BlockType_Solution,
    E_BlockType_Target,
	E_BlockType_Platform
};

class Metabase;
class Solution;
class Config;
class MakeSetup;
class PlatformBlock;

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

typedef std::vector<std::string>	StringVector;
typedef std::vector<Config*>		ConfigVector;
typedef std::vector<PlatformBlock*>	PlatformBlockVector;

struct KeyValue
{
    std::string key;
    std::string value;
};
typedef std::vector<KeyValue> KeyValueVector;

typedef std::map<std::string, std::string> KeyValueMap;

typedef void (*PostLoadInitFunc)(lua_State*);

//Generic operations add information to the current context via this interface.
class MetaBuilderBlockBase
{
public:
								MetaBuilderBlockBase(MetaBuilderBlockBase* parent);
	virtual						~MetaBuilderBlockBase();

    virtual E_BlockType			Type() const = 0;
	
	virtual void				Process();
	
	void						SetName(const char* name);
	const std::string&			GetName() const;
	
	void						AddFiles(const StringVector& files);
	void						GetFiles(StringVector* result) const;

	void						AddResources(const StringVector& files);
	void						GetResources(StringVector* result) const;

	void						AddFrameworks(const StringVector& files);
	void						GetFrameworks(StringVector* result) const;

	void						AddDefines(const StringVector& defines);
	void						GetDefines(StringVector* result, const char* configName) const;
	
	void						AddLibs(const StringVector& libs);
	void						GetLibs(StringVector* result, const char* configName) const;

	void						AddSharedLibs(const StringVector& libs);
	void						GetSharedLibs(StringVector* result, const char* configName) const;

	void						AddIncludeDirs(const StringVector& libs);
	void						GetIncludeDirs(StringVector* result, const char* configName) const;

	void						AddLibDirs(const StringVector& libs);
	void						GetLibDirs(StringVector* result, const char* configName) const;

	void						SetOption(const std::string& group, const std::string& key, const std::string& value);
	void						GetOptions(std::map<std::string, KeyValueMap>* result, const std::string* configName) const;

	void						AddExeDirs(const StringVector& defines);
	void						GetExeDirs(StringVector* result, const char* configName) const;
				
	Config*						AcquireConfig(const char* name);
	void						GetConfigs(ConfigVector* configs) const;
	Config*						GetConfig(const char* name);
	const Config*				GetConfig(const char* name) const;

	PlatformBlock*				AcquirePlatformBlock(const char* name);
	void						GetPlatformBlocks(PlatformBlockVector* blocks) const;
	PlatformBlock*				GetPlatformBlock(const char* name);
	const PlatformBlock*		GetPlatformBlock(const char* name) const;
	
	MetaBuilderBlockBase*		GetParent();
	
protected:
	StringVector*				AcquireStringGroup(const char* groupName);
	const StringVector*			GetStringGroup(const char* groupName) const;
//	void						SetString(const std::string& group, const std::string& str);
//	void						GetString(StringVector* result, const std::string* configName) const;

	MetaBuilderBlockBase*		m_parent;
	
	std::string					m_name;
	
	std::map<std::string, StringVector>
								m_stringGroups;
	
	//Key-value pairs stored per group
	std::map<std::string, KeyValueMap>
								m_keyValueGroups;
				
	StringVector				m_exeDirs;								

	std::vector<MetaBuilderBlockBase*>
								m_children;
};

class MetaBuilderContext
{
public:
	MetaBuilderContext();
	~MetaBuilderContext();
	
    MetaBuilderBlockBase*    ActiveBlock() const
    {
        return activeBlockStack.size() > 0 ? activeBlockStack.top() : NULL;
    }

    void PushActiveBlock(MetaBuilderBlockBase* block)
    {
        activeBlockStack.push(block);
    }
    
    void PopActiveBlock()
    {
        activeBlockStack.pop();
    }

    std::string                             currentMetaMakeDirAbs;
		  
	Metabase*								metabase;

    Solution*                               solution;
	
    std::stack<MetaBuilderBlockBase*>       activeBlockStack;
};

AppState*			mbGetAppState();

void				mbCommonInit(lua_State* l, const std::string& path);
void				mbPushDir(const std::string& path);
void				mbPopDir();
void				mbCommonLuaRegister(lua_State* lua);

void				mbAddMakeFile(const char* makefile);
const StringVector&	mbGetMakeFiles();

MetaBuilderContext* mbCreateContext();
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
void				mbMergeOptions(std::map<std::string, KeyValueMap>* result,	const std::map<std::string, KeyValueMap>& groupOptionMap);

U32					mbRandomU32();

void				mbCheckExpectedBlock(E_BlockType blockExpected, const char* cmdName);

//No duplicates removed
void				mbJoinArrays(StringVector* a, const StringVector& b);
void				mbRemoveDuplicates(StringVector* strings);
void				mbRemoveDuplicatesAndSort(StringVector* strings);

bool				mbCreateDirChain(const char* osDir_);

void				mbDebugDumpKeyValueGroups(const std::map<std::string, KeyValueMap>& kvGroups);


#include "makesetup.h"
#include "config.h"
#include "target.h"
#include "solution.h"
#include "metabase.h"
#include "platformblock.h"
#include "writer.h"
#include "writer_msvc.h"
#include "writer_xcode.h"

#endif
