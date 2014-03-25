#include "metabuilder_pch.h"

#include "common.h"

#include <sys/stat.h>

#include <set>
#include <list>

static AppState								g_appState;
static StringVector							g_makefiles;
static std::list<MetaBuilderContext*>		g_contexts; //Has memory ownership of context
static std::list<MetaBuilderContext*>		g_contextStack;
static std::stack<std::string>				g_doFileCurrentDirStack;

void AppState::Process()
{
	if (makeSetup)
	{
		metabaseDirAbs = makeSetup->_metabaseDir;
		intDir = makeSetup->_intDir;
		outDir = makeSetup->_outDir;
	}
	
	generator = cmdSetup._generator;

	//cmdline overrides
	if (cmdSetup._metabaseDir.length() > 0)		metabaseDirAbs = cmdSetup._metabaseDir;
	if (cmdSetup._makeOutputDir.length() > 0)	makeOutputDirAbs = cmdSetup._makeOutputDir;

	mainMetaMakeFileAbs = mbaFileGetAbsPath(cmdSetup._inputFile);
	metabaseDirAbs = mbaFileGetAbsPath(cmdSetup._metabaseDir);
	makeOutputDirAbs = mbaFileGetAbsPath(cmdSetup._makeOutputDir);
	mbNormaliseFilePath(&mainMetaMakeFileAbs);
	mbNormaliseFilePath(&metabaseDirAbs);
	mbNormaliseFilePath(&makeOutputDirAbs);

	//Set defaults if required.
	if (intDir.size() == 0)
	{
		intDir = "tmp/int";
	}
	if (outDir.size() == 0)
	{
		outDir = "tmp/out";
	}
}

MetaBuilderBlockBase::MetaBuilderBlockBase(MetaBuilderBlockBase* parent)
{
	m_parent = parent;
}

MetaBuilderBlockBase::~MetaBuilderBlockBase()
{
}

void MetaBuilderBlockBase::Process()
{
}

Config* MetaBuilderBlockBase::AcquireConfig(const char* configName)
{
	for (int i = 0; i < (int)m_configs.size(); ++i)
	{
		Config* config = m_configs[i];
		if (config->name == configName)
			return config;
	}

    Config* config = new Config(this);
    config->name = configName;
	m_configs.push_back(config);
	return config;
}

const ConfigVector& MetaBuilderBlockBase::GetConfigs() const
{
	return m_configs;
}

Config* MetaBuilderBlockBase::GetConfig(const char* configName)
{
	for (int i = 0; i < (int)m_configs.size(); ++i)
	{
		if (m_configs[i]->name == configName)
			return m_configs[i];
	}
	
	return NULL;
}

MetaBuilderBlockBase* MetaBuilderBlockBase::GetParent()
{
	return m_parent;
}

void MetaBuilderBlockBase::AddDefines(const StringVector& defines)
{
	for (size_t i = 0; i < defines.size(); ++i)
	{
		m_defines.push_back(defines[i]);
	}
};

void MetaBuilderBlockBase::GetDefines(StringVector* result, const char* configName) 
{
	for (size_t i = 0; i < m_defines.size(); ++i)
	{
		result->push_back(m_defines[i]);
	}
	
	if (configName)
	{
		for (size_t i = 0; i < m_configs.size(); ++i)
		{
			if (m_configs[i]->name == configName)
			{
				m_configs[i]->GetDefines(result, NULL);
				break;
			}
		}
	}
}

void MetaBuilderBlockBase::AddIncludeDirs(const StringVector& includeDirs)
{
	for (size_t i = 0; i < includeDirs.size(); ++i)
	{
		m_includedirs.push_back(includeDirs[i]);
	}
};

void MetaBuilderBlockBase::GetIncludeDirs(StringVector* result, const char* configName)
{
	for (size_t i = 0; i < m_includedirs.size(); ++i)
	{
		result->push_back(m_includedirs[i]);
	}
	
	if (configName)
	{
		for (size_t i = 0; i < m_configs.size(); ++i)
		{
			if (m_configs[i]->name == configName)
			{
				m_configs[i]->GetIncludeDirs(result, NULL);
				break;
			}
		}
	}
}

void MetaBuilderBlockBase::AddLibDirs(const StringVector& libDirs)
{
	for (int i = 0; i < (int)libDirs.size(); ++i)
	{
		m_librarydirs.push_back(libDirs[i]);
	}
};

void MetaBuilderBlockBase::GetLibDirs(StringVector* result, const char* configName)
{
	for (size_t i = 0; i < m_librarydirs.size(); ++i)
	{
		result->push_back(m_librarydirs[i]);
	}
	
	if (configName)
	{
		for (size_t i = 0; i < m_configs.size(); ++i)
		{
			if (m_configs[i]->name == configName)
			{
				m_configs[i]->GetLibDirs(result, NULL);
				break;
			}
		}
	}
}

void MetaBuilderBlockBase::AddLibs(const StringVector& libs)
{
	for (int i = 0; i < (int)libs.size(); ++i)
	{
		m_libs.push_back(libs[i]);
	}
};

void MetaBuilderBlockBase::GetLibs(StringVector* result, const char* configName) 
{
	for (size_t i = 0; i < m_libs.size(); ++i)
	{
		result->push_back(m_libs[i]);
	}
	
	if (configName)
	{
		for (size_t i = 0; i < m_configs.size(); ++i)
		{
			if (m_configs[i]->name == configName)
			{
				m_configs[i]->GetLibs(result, NULL);
				break;
			}
		}
	}
}

void MetaBuilderBlockBase::SetOption(const std::string& group, const std::string& key, const std::string& value)
{
	std::map<std::string, KeyValueMap>::iterator it = m_groupOptions.find(group);
	
	if (it == m_groupOptions.end())
	{
		//Insert new vector as one does not exist already for this platform.
		std::pair<std::map<std::string, KeyValueMap>::iterator, bool> result =
			m_groupOptions.insert(std::make_pair(group, KeyValueMap()));
		
		it = result.first;
	}

	KeyValueMap& kvmap = (*it).second;
	kvmap[key] = value;
}

void MetaBuilderBlockBase::GetOptions(std::map<std::string, KeyValueMap>* result, const std::string* configName) const
{
	mbMergeOptions(result, m_groupOptions);
	
	if (configName)
	{
		for (size_t i = 0; i < m_configs.size(); ++i)
		{
			if (m_configs[i]->name == *configName)
			{
				m_configs[i]->GetOptions(result, NULL);
				break;
			}
		}
	}
}

void MetaBuilderBlockBase::AddExeDirs(const StringVector& exeDirs)
{
	for (size_t i = 0; i < exeDirs.size(); ++i)
	{
		m_exeDirs.push_back(exeDirs[i]);
	}
}

void MetaBuilderBlockBase::GetExeDirs(StringVector* result, const char* configName)
{
	for (size_t i = 0; i < m_exeDirs.size(); ++i)
	{
		result->push_back(m_exeDirs[i]);
	}
	
	if (configName)
	{
		for (size_t i = 0; i < m_configs.size(); ++i)
		{
			if (m_configs[i]->name == configName)
			{
				m_configs[i]->GetExeDirs(result, NULL);
				break;
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

MetaBuilderContext::MetaBuilderContext()
{
	metabase = NULL;
	solution = NULL;
	
	g_contexts.push_back(this);
}

MetaBuilderContext::~MetaBuilderContext()
{
	for (std::list<MetaBuilderContext*>::iterator it = g_contexts.begin(); it != g_contexts.end(); ++it)
	{
		if (*it == this)
		{
			g_contexts.erase(it);
			break;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

void mbAddMakeFile(const char* makefile)
{
	g_makefiles.push_back(makefile);
}

const StringVector& mbGetMakeFiles()
{
	return g_makefiles;
}

MetaBuilderContext* mbCreateContext()
{
	return new MetaBuilderContext;
}

MetaBuilderContext* mbGetMainContext()
{
	return g_contextStack.front();
}

MetaBuilderContext* mbGetActiveContext()
{
	return g_contextStack.back();
}

void mbPushActiveContext(MetaBuilderContext* ctx)
{
	g_contextStack.push_back(ctx);
}

void mbPopActiveContext()
{
	g_contextStack.pop_back();
}

const std::list<MetaBuilderContext*>& mbGetContexts()
{
	return g_contexts;
}

static int luaFuncGlobalImport(lua_State* lua)
{
    const char* requireFile = lua_tostring(lua, 1);

    mbLuaDoFile(lua, requireFile, NULL);

    return 0;
}

static int luaFuncDefines(lua_State* lua)
{
    MetaBuilderBlockBase* block = mbGetActiveContext()->ActiveBlock();
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


static int report (lua_State *L, int status) {
  const char *msg;
  if (status) {
    msg = lua_tostring(L, -1);
    if (msg == NULL) msg = "(error with no message)";
    fprintf(stderr, "status=%d, %s\n", status, msg);
    lua_pop(L, 1);
  }
  return status;
}

static int luaFuncSetOption(lua_State* l)
{
	MetaBuilderBlockBase* block = mbGetActiveContext()->ActiveBlock();
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

static int luaFuncCheckPlatform(lua_State* l)
{
    const char* testPlatform = lua_tostring(l, 1);
	for (int i = 0; i < mbGetActiveContext()->metabase->currentPlatforms.size(); ++i)
	{
		const std::string& test = mbGetActiveContext()->metabase->currentPlatforms[i];
		if (test == testPlatform)
		{
			lua_pushboolean(l, 1);
			return 1;
		}
	}
	
	lua_pushboolean(l, 0);
	return 1;
}

static int luaFuncLibs(lua_State* l)
{
	MetaBuilderBlockBase* block = mbGetActiveContext()->ActiveBlock();
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
	MetaBuilderBlockBase* block = mbGetActiveContext()->ActiveBlock();
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
	MetaBuilderBlockBase* block = mbGetActiveContext()->ActiveBlock();	
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
	MetaBuilderBlockBase* block = mbGetActiveContext()->ActiveBlock();	
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

void mbLuaDoFile(lua_State* l, const std::string& filepath, PostLoadInitFunc initFunc)
{
	const std::string& currentDir = g_doFileCurrentDirStack.top();

    std::string absPath;
    //Try relative to make file first.
    {
        std::string tmp = mbPathJoin(currentDir, filepath);
        if (mbFileExists(tmp))
        {
            absPath = tmp;
        }
    }
	
	//Attempt to open directly
    if (absPath.length() == 0)
	{
		if (mbFileExists(filepath))
		{
			absPath = mbaFileGetAbsPath(filepath);
		}
	}

    //Fall back to lua base dir
    if (absPath.length() == 0)
    {
        absPath = mbPathJoin(mbGetAppState()->metabaseDirAbs, filepath);
    }
	
	std::string newDir = mbPathGetDir(absPath.c_str());
	g_doFileCurrentDirStack.push(newDir);
	
	if (report(l, luaL_loadfile(l, absPath.c_str())))
		mbExitError();
	
	if (initFunc)
	{
		initFunc(l);
	}
   	
	if (lua_pcall(l, 0,0,0) != 0)
	{
        // the top of the stack should be the error string
        if (lua_isstring(l, lua_gettop(l)))
        {
            // get the top of the stack as the error and pop it off
            const char* str = lua_tostring(l, lua_gettop(l));
            lua_pop(l, 1);
            MB_LOGINFO("%s", str);
        }
		mbExitError();
	}
	
	g_doFileCurrentDirStack.pop();
}

void mbExitError()
{
    _exit(1);    
}

std::string mbPathJoin(const std::string& a, const std::string& b)
{
	if (a.length() > 0)
		return a + std::string("/") + b;
		
	return b;
}

std::string mbPathGetDir(const std::string& filePath)
{
    std::string tmp;
    int len = (int)filePath.length();
    char* chars = (char*)filePath.c_str();
    for (int i = len-1; i >= 0; --i)
    {
        if (chars[i] == '/')
        {
            chars[i] = 0;
            tmp = chars;
            chars[i] = '/';
            return tmp;
        }
    }

    return "";
}

std::string	mbPathGetFilename(const std::string& filePath)
{
    std::string tmp;
    int len = (int)filePath.length();
    char* chars = (char*)filePath.c_str();
    for (int i = len-1; i >= 0; --i)
    {
        if (chars[i] == '/')
        {
            chars += i+1;
			return chars;
        }
    }

    return filePath;
}

bool mbPathGetFileExtension(char* result, const char* filename)
{
    int len = (int)strlen(filename);
    const char* chars = filename;
    for (int i = len-1; i >= 0; --i)
    {
        if (chars[i] == '.')
        {
            chars += i+1;
			strcpy(result, chars);
			return true;
        }
    }

	return false;
}

bool mbPathReplaceFileExtension(char* result, const char* filename, const char* newExtension)
{
	strcpy(result, filename);
	
    int len = (int)strlen(result);
	char* chars = result;
    for (int i = len-1; i >= 0; --i)
    {
        if (chars[i] == '.')
        {
            chars += i+1;
			strcpy(chars, newExtension);
			return true;
        }
    }

	return false;
}

#if 0
bool mbPathRelativeDirTo(
	std::string* result,
	const std::string& from,
	const std::string& to)
{
	//Find longest common sequence
	int minLen = from.length() < to.length() ? (int)from.length() : (int)to.length();
	
	int commonLen = 0;
	for (; commonLen < minLen; ++commonLen)
	{
		if (from[commonLen] != to[commonLen])
		{
			--commonLen;
			break;
		}
	}
	//Nothing in common
	if (commonLen <= 0)
	{
		return false;
	}
	
	//Must end on dir sep
	if (from[commonLen] != '/')
	{
		return false;
	}
	
	//Go up folders from our 'from' until we get to the common folder.
	char resultTmp[MB_MAX_PATH] = {0};
	for (int i = from.size()-1; i >= commonLen; --i)
	{
		if (from[i] == '/')
		{
			strcat(resultTmp, "../");
		}
	}
	
	//Now we can move forward into the 'to' folder.
	const char* toFolderCommonRelative = to.c_str()+commonLen+1;
	
	//Now combine the two
	strcat(resultTmp, toFolderCommonRelative);
			
	*result = resultTmp;

	return true;
}
#endif

bool mbFileExists(const std::string& filepath)
{
    FILE* f = fopen(filepath.c_str(), "rb");
    if (f)
    {
        fclose(f);
        return true;
    }
    
    return false;
}

void mbNormaliseFilePath(char* outFilePath, const char* inFilePath)
{
    bool preceedingSlash = false;
    
    outFilePath[0] = 0;
    char* outCursor = outFilePath;
    for (const char* inCursor = inFilePath; *inCursor; ++inCursor)
    {
		char c = *inCursor;
        //Normalise slashes
        if (c == '\\')
            c = '/';
        
        //Ignore duplicate slashes
        if (c == '/')
        {
            if (preceedingSlash)
                continue;
            preceedingSlash = true;
        }
        else
        {
            preceedingSlash = false;
        }
        
        *outCursor = c;
        ++outCursor;
    }
	*outCursor = '\0';
}

void mbNormaliseFilePath(std::string* inout)
{
    char tmp[MB_MAX_PATH];
	mbNormaliseFilePath(tmp, inout->c_str());
	*inout = tmp;
}


std::string mbGetMakeOutputDirRelativePath(const std::string& path)
{
	//TODO. Use abs path for now
	return mbaFileGetAbsPath(path);
}

AppState* mbGetAppState()
{
	return &g_appState;
}

void mbCommonInit(lua_State* l, const std::string& path)
{
	g_doFileCurrentDirStack.push(path);
	
	mbLuaDoFile(l, "metabase.lua", NULL);
}

void mbPushDir(const std::string& path)
{
	g_doFileCurrentDirStack.push(path);
}

void mbPopDir()
{
	g_doFileCurrentDirStack.pop();
}




void mbLuaGetDefines(StringVector* defines, lua_State* lua, E_BlockType blockTypeExpected)
{
    if (!mbGetActiveContext()->ActiveBlock() || mbGetActiveContext()->ActiveBlock()->Type() != blockTypeExpected)
    {
        MB_LOGINFO("ERROR: must be within expected block");
        mbExitError();
    }
	   
    luaL_checktype(lua, 1, LUA_TTABLE);
    int tableLen =  luaL_len(lua, 1);
    
    for (int i = 1; i <= tableLen; ++i)
    {
        lua_rawgeti(lua, 1, i);
        const char* tmp = lua_tostring(lua, -1);
        defines->push_back(tmp);
    }
}

void mbCommonLuaRegister(lua_State* l)
{
    lua_pushcfunction(l, luaFuncGlobalImport);
    lua_setglobal(l, "import");

    lua_pushcfunction(l, luaFuncCheckPlatform);
    lua_setglobal(l, "checkplatform");
	
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
}

void mbStringReplace(std::string& str, const std::string& oldStr, const std::string& newStr)
{
    size_t pos = 0;
    while((pos = str.find(oldStr, pos)) != std::string::npos)
    {
        str.replace(pos, oldStr.length(), newStr);
        pos += newStr.length();
    }
}

void mbLuaDump(lua_State* l)
{
    int i;
    int top = lua_gettop(l);
 
    printf("total in stack %d",top);
 
    for (i = 1; i <= top; i++)
    {
        int t = lua_type(l, i);
        switch (t) {
            case LUA_TSTRING:
                MB_LOGINFO("string: '%s'", lua_tostring(l, i));
                break;
            case LUA_TBOOLEAN:
                MB_LOGINFO("boolean %s",lua_toboolean(l, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:
                MB_LOGINFO("number: %g", lua_tonumber(l, i));
                break;
            default:
                MB_LOGINFO("%s", lua_typename(l, t));
                break;
        }
        MB_LOGINFO("  ");
    }
    MB_LOGINFO("");
}

void mbMergeOptions(std::map<std::string, KeyValueMap>* result,	const std::map<std::string, KeyValueMap>& groupOptionMap)
{
	for (std::map<std::string, KeyValueMap>::const_iterator it = groupOptionMap.begin(); it != groupOptionMap.end(); ++it)
	{
		const std::string& inputGroupName = it->first;
		const KeyValueMap& inputGroupOptions = it->second;
		
		//Find group to insert into
		std::map<std::string, KeyValueMap>::iterator resultIt = result->find(inputGroupName);
		if (resultIt == result->end())
		{
			//We've fond a new group - insert empty option map
			std::pair<std::map<std::string, KeyValueMap>::iterator, bool> tmp =
				result->insert(std::make_pair(inputGroupName, KeyValueMap()));
			resultIt = tmp.first;
		}

		KeyValueMap& resultGroupOptions = (*resultIt).second;
		//Copy values between maps
		for (KeyValueMap::const_iterator copyIt = inputGroupOptions.begin(); copyIt != inputGroupOptions.end(); ++copyIt)
		{
			const std::string& key = copyIt->first;
			const std::string& value = copyIt->second;
			
			resultGroupOptions[key] = value;
		}
	}
}


struct mbRandomContext
{
	mbRandomContext();
	U32 seed[4];
};


mbRandomContext::mbRandomContext()
{
	seed[0] = 0xfe354cd2;
	seed[1] = 0xabcde012;
	seed[2] = 0x458229cd;
	seed[3] = 0xdeadbeef;
}

static mbRandomContext g_randomContext;

U32 mbRandomU32(mbRandomContext& ctx);

U32 mbRandomU32()
{ 
	return mbRandomU32(g_randomContext);
}

U32 mbRandomU32(mbRandomContext& ctx)
{
// Originally by David Jones, UCL in http://www.cs.ucl.ac.uk/staff/d.jones/GoodPracticeRNG.pdf
// Public domain code for JKISS RNG
	U32& x = ctx.seed[0];
	U32& y = ctx.seed[1];
	U32& z = ctx.seed[2];
	U32& c = ctx.seed[3];

	U64 t;
	x = 314527869 * x + 1234567; 
	y ^= y << 5; 
	y ^= y >> 7; 
	y ^= y << 22; 
	t = 4294584393ULL * z + c; 
	c = t >> 32; 
	z = (U32)t;
	return x + y + z; 
}

void mbCheckExpectedBlock(E_BlockType blockExpected, const char* cmdName)
{
    if (!mbGetActiveContext()->ActiveBlock() || mbGetActiveContext()->ActiveBlock()->Type() != blockExpected)
    {
		MB_LOGERROR("Operation %s is not permitted within the current block type.", cmdName);
        mbExitError();
    }
}

void mbJoinArrays(StringVector* a, const StringVector& b)
{
	for (int i = 0; i < (int)b.size(); ++i)
	{
		a->push_back(b[i]);
	}
}

void mbRemoveDuplicates(StringVector* strings_)
{
	StringVector& strings = *strings_;
	
	StringVector tmp;
	tmp.reserve(strings.size());
		
	std::set<std::string> uniqueStrings;
	for (int i = 0; i < (int)strings.size(); ++i)
	{
		std::set<std::string>::iterator it = uniqueStrings.find(strings[i]);
		if (it == uniqueStrings.end())
		{
			std::pair<std::set<std::string>::iterator, bool> result = uniqueStrings.insert(strings[i]);
			if (!result.second)
				continue; //Already added.
				
			tmp.push_back(strings[i]);
		}
	}
	
	strings = tmp;
}

void mbRemoveDuplicatesAndSort(StringVector* strings_)
{
	StringVector& strings = *strings_;
	
	StringVector tmp;
	tmp.reserve(strings.size());
		
	std::set<std::string> uniqueStrings;
	for (int i = 0; i < (int)strings.size(); ++i)
	{
		uniqueStrings.insert(strings[i]);
	}
	
	for (std::set<std::string>::iterator it = uniqueStrings.begin(); it != uniqueStrings.end(); ++it)
	{
		tmp.push_back(*it);
	}
	
	strings = tmp;
}
