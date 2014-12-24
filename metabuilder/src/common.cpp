#include "metabuilder_pch.h"

#include "makesetup.h"
#include "metabase.h"

#include <sys/stat.h>

#include <set>
#include <list>
#include <algorithm>
#include <sstream>

static AppState								g_appState;
static StringVector							g_makefiles;
static std::list<MetaBuilderContext*>		g_contexts; //Has memory ownership of context
static std::list<MetaBuilderContext*>		g_contextStack;
static std::stack<std::string>				g_doFileCurrentDirStack;

//static std::map<std::string, std::string>	g_macroMap;

const char* g_cAndCPPSourceExt[] = { "cpp", "cxx", "c", "cc", "m", "mm", NULL };
const char* g_cAndCPPHeaderExt[] = { "hpp", "hxx", "h", NULL };
const char* g_cAndCPPInlineExt[] = { "inl", NULL };

AppState::AppState()
{
	isProcessingPrimaryMakefile = false;
	makeSetup = NULL;
}
	
AppState::~AppState()
{
	delete makeSetup;
}

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

	if (!mbCreateDirChain(cmdSetup._makeOutputDir.c_str()))
	{
		MB_LOGERROR("Failed to create output directory %s", cmdSetup._makeOutputDir.c_str());
		mbExitError();
	}

	mainMetaMakeFileAbs = mbaFileGetAbsPath(cmdSetup._inputFile);
	metabaseDirAbs = mbaFileGetAbsPath(cmdSetup._metabaseDir);
	makeOutputDirAbs = mbaFileGetAbsPath(cmdSetup._makeOutputDir);
	mbNormaliseFilePath(&mainMetaMakeFileAbs);
	mbNormaliseFilePath(&metabaseDirAbs);
	mbNormaliseFilePath(&makeOutputDirAbs);

	//Set defaults if required.
	if (intDir.size() == 0)
	{
		intDir = "int";
	}
	if (outDir.size() == 0)
	{
		outDir = "out";
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

MetaBuilderContext::MetaBuilderContext()
{
	metabase = NULL;
	solution = NULL;
	isMainMakefile = false;
	
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
	
	delete metabase;
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

void mbDestroyContext(MetaBuilderContext* ctx)
{
	delete ctx;
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
	Block* b = mbGetActiveContext()->ActiveBlock();

	std::string requireFile;
	mbLuaToStringExpandMacros(&requireFile, b, lua, 1);
	mbLuaDoFile(lua, requireFile.c_str(), NULL);
    return 0;
}
/*
int luaFuncAddMacro(lua_State* lua)
{
	const char* key = lua_tostring(lua, 1);
	const char* value = lua_tostring(lua, 2);
	
	if (!key || !value)
	{
		//TODO - error handling
		MB_LOGERROR("Must specify both a key and value when adding a macro");
		mbExitError();
	}

	if (mbGetAppState()->cmdSetup.verbose)
	{
		MB_LOGINFO("Setting global macro %s with a value of %s", key, value);
	}
	g_macroMap[key] = value;

	return 0;
}
*/
static int luaFuncExpandMacro(lua_State* l)
{
	Block* b = mbGetActiveContext()->ActiveBlock();

	const char* str = lua_tostring(l, 1);

	std::string expandedString;
	mbExpandMacros(&expandedString, b, str);

	lua_pushstring(l, expandedString.c_str());
	return 1;
}

static int report (lua_State *L, int status) 
{
  const char *msg;
  if (status) 
  {
		msg = lua_tostring(L, -1);
		if (msg == NULL)
			msg = "(error with no message)";

		fprintf(stderr, "status=%d, %s\n", status, msg);
		lua_pop(L, 1);
	}
	return status;
}



static int luaFuncCheckPlatform(lua_State* l)
{
	Block* b = mbGetActiveContext()->ActiveBlock();

    std::string testPlatform;
	mbLuaToStringExpandMacros(&testPlatform, b, l, 1);
	for (int i = 0; i < (int)mbGetActiveContext()->metabase->supportedPlatforms.size(); ++i)
	{
		const std::string& test = mbGetActiveContext()->metabase->supportedPlatforms[i];
		if (test == testPlatform)
		{
			lua_pushboolean(l, 1);
			return 1;
		}
	}
	
	lua_pushboolean(l, 0);
	return 1;
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
            MB_LOGERROR("%s", str);
        }
		mbExitError();
	}
	
	g_doFileCurrentDirStack.pop();
}

void mbExitError()
{
	MB_LOGERROR("Exiting with error.");
    exit(1);
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

const char** mbGetCAndCPPSourceFileExtensions()
{
	return g_cAndCPPSourceExt;
}

const char** mbGetCAndCPPHeaderFileExtensions()
{
	return g_cAndCPPHeaderExt;
}

const char** mbGetCAndCPPInlineFileExtensions()
{
	return g_cAndCPPInlineExt;
}

void mbPushDir(const std::string& path)
{
	g_doFileCurrentDirStack.push(path);
}

void mbPopDir()
{
	g_doFileCurrentDirStack.pop();
}






void mbCommonLuaRegister(lua_State* l)
{
    lua_pushcfunction(l, luaFuncGlobalImport);
    lua_setglobal(l, "import");

    lua_pushcfunction(l, luaFuncCheckPlatform);
    lua_setglobal(l, "checkplatform");

	lua_pushcfunction(l, luaFuncExpandMacro);
	lua_setglobal(l, "expandmacro");
}

bool mbStringReplace(std::string& str, const std::string& oldStr, const std::string& newStr)
{
	bool found = false;
    size_t pos = 0;
    while((pos = str.find(oldStr, pos)) != std::string::npos)
    {
        str.replace(pos, oldStr.length(), newStr);
        pos += newStr.length();

		found = true;
    }

	return found;
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
                MB_LOGDEBUG("string: '%s'", lua_tostring(l, i));
                break;
            case LUA_TBOOLEAN:
                MB_LOGDEBUG("boolean %s",lua_toboolean(l, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:
                MB_LOGDEBUG("number: %g", lua_tonumber(l, i));
                break;
            default:
                MB_LOGDEBUG("%s", lua_typename(l, t));
                break;
        }
        MB_LOGDEBUG("  ");
    }
    MB_LOGDEBUG("");
}

void mbMergeStringGroups(std::map<std::string, StringVector>* result, const std::map<std::string, StringVector>& stringGroup)
{
	for (std::map<std::string, StringVector>::const_iterator it = stringGroup.begin(); it != stringGroup.end(); ++it)
	{
		const std::string& inputGroupName = it->first;
		const StringVector& inputStrings = it->second;
		
		//Find group to insert into
		std::map<std::string, StringVector>::iterator resultIt = result->find(inputGroupName);
		if (resultIt == result->end())
		{
			//We've fond a new group - insert empty option map
			std::pair<std::map<std::string, StringVector>::iterator, bool> tmp =
				result->insert(std::make_pair(inputGroupName, StringVector()));
			resultIt = tmp.first;
		}

		StringVector& resultStrings = (*resultIt).second;
		mbMergeArrays(&resultStrings, inputStrings);
	}
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
    if (!mbGetActiveContext()->ActiveBlock() || mbGetActiveContext()->ActiveBlock()->GetType() != blockExpected)
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

void mbMergeArrays(StringVector* a, const StringVector& b)
{
	mbJoinArrays(a, b);
	mbRemoveDuplicates(a);
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


struct StringSortRecord
{
	std::string lowerCaseString;
	std::string originalString;
};

bool mbCompareNoCase(const StringSortRecord& a, const StringSortRecord& b)
{
	return a.lowerCaseString < b.lowerCaseString;
}

void mbRemoveDuplicatesAndSort(StringVector* strings_)
{
	StringVector& strings = *strings_;
	
	std::vector<StringSortRecord> tmp;
	tmp.reserve(strings.size());
		
	std::set<std::string> uniqueStrings;
	for (int i = 0; i < (int)strings.size(); ++i)
	{
		uniqueStrings.insert(strings[i]);
	}

	for (std::set<std::string>::iterator it = uniqueStrings.begin(); it != uniqueStrings.end(); ++it)
	{
		const std::string& currentString = *it;

		tmp.push_back(StringSortRecord());
		StringSortRecord& r = tmp.back();
		r.lowerCaseString = currentString;
		std::transform(r.lowerCaseString.begin(), r.lowerCaseString.end(), r.lowerCaseString.begin(), ::tolower);
		r.originalString = currentString;
	}

	std::sort(tmp.begin(), tmp.end(), mbCompareNoCase);

	strings.clear();
	for (int i = 0; i < (int)tmp.size(); ++i)
	{
		strings.push_back(tmp[i].originalString);
	}
}

bool mbCreateDirChain(const char* osDir_)
{
#ifdef PLATFORM_WINDOWS
	const char sep = '\\';
#else
	const char sep = '/';
#endif
//    Debug::Error("Creating dir chain %s", osDir_);

    char osDir[FILENAME_MAX] = {0};
    mbaNormaliseFilePath(osDir, osDir_);
    
    if (osDir[0] == 0)
        return false;
    
    const char* left = osDir;
    for (const char* right = osDir+1; *right; ++right)
    {
        if (*right == sep)
        {
            *(char*)right = 0;
            const char* partialPath = left;
            
            if (!_mbaCreateDir(partialPath))
                return false;
            
            *(char*)right = sep;
        }
    }
    
    return _mbaCreateDir(osDir);
}

void mbDebugDumpKeyValueGroups(const std::map<std::string, KeyValueMap>& kvGroups)
{
	for (std::map<std::string, KeyValueMap>::const_iterator it = kvGroups.begin(); it != kvGroups.end(); ++it)
	{
		MB_LOGDEBUG("%s (option group) count: %i", it->first.c_str(), it->second.size());
		const KeyValueMap& kvm = it->second;
		for (KeyValueMap::const_iterator it2 = kvm.begin(); it2 != kvm.end(); ++it2)
		{
			MB_LOGDEBUG("  %s : %s", it2->first.c_str(), it2->second.c_str());
		}
	}
}

void mbDebugDumpGroups(const std::map<std::string, StringVector>& stringGroups)
{
	for (std::map<std::string, StringVector>::const_iterator it = stringGroups.begin(); it != stringGroups.end(); ++it)
	{
		MB_LOGDEBUG("%s (string group) count: %i", it->first.c_str(), it->second.size());
		const StringVector& stringVector = it->second;
		for (StringVector::const_iterator it2 = stringVector.begin(); it2 != stringVector.end(); ++it2)
		{
			MB_LOGDEBUG("  %s", it2->c_str());
		}
	}
}

void mbExpandMacros(std::string* result, const std::map<std::string, std::string>& macroMap, const char* str)
{
	char macro[1024];

	*result = str;

	//Only process each macro if we know our string contains at least one.
	const char* macroStart = strstr(str, "${");
	if (macroStart)
	{
		bool found = false;
		for (std::map<std::string, std::string>::const_iterator it = macroMap.begin(); it != macroMap.end(); ++it)
		{
			const std::string& key = it->first;
			const std::string& value = it->second;

			sprintf(macro, "${%s}", key.c_str());

			found = mbStringReplace(*result, macro, value);
			if (found)
				break;
		}

		if (!found)
		{
			macroStart += 2;
			char key[1024];
			const char* macroEnd = strstr(macroStart, "}");
			int length = macroEnd - macroStart;

			memcpy(key, macroStart, length);
			key[length] = '\0';

			const char* envValue = getenv(key);
			if (envValue)
			{
				sprintf(macro, "${%s}", key);
				mbStringReplace(*result, macro, envValue);
			}
		}
	}
}

void mbExpandMacros(std::string* result, Block* block, const char* str)
{
	if (block)
	{
		mbExpandMacros(result, block->FlattenMacros(), str);
	}
	else
	{
		*result = str;
	}
}

const char* mbLuaToStringExpandMacros(std::string* result, Block* block, lua_State* l, int stackPos)
{
	const char* str = lua_tostring(l, stackPos);
	if (!str)
		return NULL;
	
	mbExpandMacros(result, block, str);
//	MB_LOGINFO("%s -> %s", str, result->c_str());
	return result->c_str();
}

