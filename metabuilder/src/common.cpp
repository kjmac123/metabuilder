#include "metabuilder_pch.h"

#include "dlmalloc.h"

#include "makesetup.h"
#include "makeglobal.h"
#include "metabase.h"

#include <sys/stat.h>

#include <set>
#include <list>
#include <algorithm>

struct mbRandomContext
{
	mbRandomContext();
	U32 seed[4];
};

class StringPtrLess
{
public:
	bool operator()(const std::string* a, const std::string* b) const
	{
		return *a < *b;
	}
};

typedef std::set<std::string*, StringPtrLess> UniqueStringHashTable;

static AppState								g_appState;
static StringVector							g_makefiles;
static std::list<MetaBuilderContext*>		g_contexts; //Has memory ownership of context
static std::list<MetaBuilderContext*>		g_contextStack;
static std::stack<std::string>				g_doFileCurrentDirStack;
static mbRandomContext						g_randomContext;

const char* g_cAndCPPSourceExt[] = { "cpp", "cxx", "c", "cc", "m", "mm", NULL };
const char* g_cAndCPPHeaderExt[] = { "hpp", "hxx", "h", NULL };
const char* g_cAndCPPInlineExt[] = { "inl", NULL };

//-----------------------------------------------------------------------------------------------------------------------------------------

mbRandomContext::mbRandomContext()
{
	seed[0] = 0xfe354cd2;
	seed[1] = 0xabcde012;
	seed[2] = 0x458229cd;
	seed[3] = 0xdeadbeef;
}

//-----------------------------------------------------------------------------------------------------------------------------------------

AppState::AppState()
{
	isProcessingPrimaryMakefile = false;
	makeSetup = NULL;
}
	
AppState::~AppState()
{
	delete makeSetup;
}

void AppState::ProcessSetup()
{
	if (makeSetup)
	{
		metabaseDirAbs = makeSetup->metabaseDir;
		intDir = makeSetup->intDir;
		outDir = makeSetup->outDir;
	}
	
	generator = cmdSetup._generator;

	//cmdline overrides
	if (cmdSetup._metabaseDir.length() > 0)		metabaseDirAbs = cmdSetup._metabaseDir;
	if (cmdSetup._makeOutputTopDir.length() > 0)	makeOutputTopDirAbs = cmdSetup._makeOutputTopDir;

	if (!mbCreateDirChain(cmdSetup._makeOutputTopDir.c_str()))
	{
		MB_LOGERROR("Failed to create output directory %s", cmdSetup._makeOutputTopDir.c_str());
		mbExitError();
	}

	mainMetaMakeFileAbs = Platform::FileGetAbsPath(cmdSetup._inputFile);
	metabaseDirAbs = Platform::FileGetAbsPath(cmdSetup._metabaseDir);
	makeOutputTopDirAbs = Platform::FileGetAbsPath(cmdSetup._makeOutputTopDir);

	//Set defaults if required.
	if (intDir.size() == 0)
	{
		intDir = "int";
	}
	if (outDir.size() == 0)
	{
		outDir = "out";
	}

	lineEndingStyle = E_LineEndingStyle_Default;
	if (cmdSetup.lineEndingStyle.length() > 0)
	{
		if (cmdSetup.lineEndingStyle == "default")
		{
			lineEndingStyle = E_LineEndingStyle_Default;
		}
		else if (cmdSetup.lineEndingStyle == "windows")
		{
			lineEndingStyle = E_LineEndingStyle_Windows;
		}
		else if (cmdSetup.lineEndingStyle == "unix")
		{
			lineEndingStyle = E_LineEndingStyle_UNIX;
		}
		else
		{
			MB_LOGERROR("Unknown line ending style, valid values are \"default\", \"windows\" and \"unix\"");
			mbExitError();
		}
	}
}

void AppState::ProcessGlobal()
{
	OnTargetDirSepChanged();
}

void AppState::OnTargetDirSepChanged()
{
	mbNormaliseFilePath(&mainMetaMakeFileAbs,	makeGlobal->GetTargetDirSep());
	mbNormaliseFilePath(&metabaseDirAbs,		makeGlobal->GetTargetDirSep());
	mbNormaliseFilePath(&makeOutputTopDirAbs,	makeGlobal->GetTargetDirSep());

	MetaBuilderContext* ctx = mbGetActiveContext();
	if (ctx)
	{
		ctx->OnTargetDirSepChanged();
	}
}

//-----------------------------------------------------------------------------------------------------------------------------------------

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

Block* MetaBuilderContext::ActiveBlock() const
{
	return activeBlockStack.size() > 0 ? activeBlockStack.top() : NULL;
}

void MetaBuilderContext::PushActiveBlock(Block* block)
{
    activeBlockStack.push(block);
}
    
void MetaBuilderContext::PopActiveBlock()
{
    activeBlockStack.pop();
}

void MetaBuilderContext::OnTargetDirSepChanged()
{
	mbNormaliseFilePath(&currentMetaMakeDirAbs, mbGetAppState()->makeGlobal->GetTargetDirSep());
	mbNormaliseFilePath(&makeOutputDirAbs, mbGetAppState()->makeGlobal->GetTargetDirSep());
}

//-----------------------------------------------------------------------------------------------------------------------------------------

LuaModuleFunctions::LuaModuleFunctions()
{
	m_nFunctions = 0;
	memset(m_luaFunctions, 0, sizeof(m_luaFunctions));
}

void LuaModuleFunctions::AddFunction(const char* name, lua_CFunction fn)
{
	MB_ASSERT(m_nFunctions != MB_LUAMODULE_MAX_FUNCTIONS);
	m_luaFunctions[m_nFunctions].name = name;
	m_luaFunctions[m_nFunctions].func = fn;
	++m_nFunctions;
}

void LuaModuleFunctions::RegisterLuaGlobal(lua_State* l)
{
	for (int i = 0; i < m_nFunctions; ++i)
	{
		lua_pushcfunction(l, m_luaFunctions[i].func);
		lua_setglobal(l, m_luaFunctions[i].name);
	}
}

void LuaModuleFunctions::RegisterLuaModule(lua_State* l, const char* moduleName)
{
	luaL_newlib(l, m_luaFunctions);  //Create module
	lua_setglobal(l, moduleName);
}

//-----------------------------------------------------------------------------------------------------------------------------------------

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

static int luaFuncExpandMacro(lua_State* l)
{
	Block* b = mbGetActiveContext()->ActiveBlock();

	const char* str = lua_tostring(l, 1);

	std::string expandedString;
	mbExpandMacros(&expandedString, b, str);

	lua_pushstring(l, expandedString.c_str());
	return 1;
}

static int luaFuncLogInfo(lua_State* l)
{
	Block* b = mbGetActiveContext()->ActiveBlock();

	const char* str = lua_tostring(l, 1);

	std::string expandedString;
	mbExpandMacros(&expandedString, b, str);

	lua_pushstring(l, expandedString.c_str());
	MB_LOGINFO("%s", expandedString.c_str());
	return 0;
}

static int luaFuncLogProfile(lua_State* l)
{
#ifdef MB_ENABLE_PROFILING
	Block* b = mbGetActiveContext()->ActiveBlock();

	const char* str = lua_tostring(l, 1);

	std::string expandedString;
	mbExpandMacros(&expandedString, b, str);

	lua_pushstring(l, expandedString.c_str());
	MB_LOGINFO("%s", expandedString.c_str());
#endif
	return 0;
}

static int luaFuncLogError(lua_State* l)
{
	Block* b = mbGetActiveContext()->ActiveBlock();

	const char* str = lua_tostring(l, 1);

	std::string expandedString;
	mbExpandMacros(&expandedString, b, str);

	lua_pushstring(l, expandedString.c_str());
	MB_LOGERROR("%s", expandedString.c_str());
	return 0;
}

static int luaSplit(lua_State* l)
{
	const char *s = luaL_checkstring(l, 1);
	const char *sep = luaL_checkstring(l, 2);
	const char *e;
	int i = 1;

	lua_newtable(l);

	//for each separator
	while ((e = strchr(s, *sep)) != NULL)
	{
		lua_pushlstring(l, s, e - s);  //push substring
		lua_rawseti(l, -2, i++);
		s = e + 1;  //skip separator
	}

	//push last substring
	lua_pushstring(l, s);
	lua_rawseti(l, -2, i);
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

void mbLuaDoFile(lua_State* l, const std::string& filepath_, PostLoadInitFunc initFunc)
{
	char normalisedFilePath[MB_MAX_PATH];
	Platform::NormaliseFilePath(normalisedFilePath, filepath_.c_str());
	const std::string& currentDir = g_doFileCurrentDirStack.top();

    std::string absPath;
    //Try relative to make file first.
    {
		char tmp[MB_MAX_PATH];
		mbHostPathJoin(tmp, currentDir.c_str(), normalisedFilePath);
        if (mbFileExists(tmp))
        {
            absPath = tmp;
        }
    }
	
	//Attempt to open directly
    if (absPath.length() == 0)
	{
		if (mbFileExists(normalisedFilePath))
		{
			absPath = Platform::FileGetAbsPath(normalisedFilePath);
		}
	}

    //Fall back to lua base dir
    if (absPath.length() == 0)
    {
		char tmpJoin[MB_MAX_PATH];
		mbHostPathJoin(tmpJoin, mbGetAppState()->metabaseDirAbs.c_str(), normalisedFilePath);
		absPath = tmpJoin;
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

void mbHostPathJoin(char* result, const char* a, const char* b)
{
	if (a[0] == '\0')
	{
		strcpy(result, b);
	}
	
	if (b[0] == '\0')
	{
		strcpy(result, a);
	}

	//Trim leading slash
	if (b[0] == '/' || b[0] == '\\')
	{
		++b;
	}

	//Trim trailing slash
	int aLen = strlen(a);

	char trailingSlashToRestore = 0;
	char* trailingSlashPtr = NULL;
	if (a[aLen-1] == '/' || a[aLen-1] == '\\')
	{
		trailingSlashPtr = const_cast<char*>(a)+aLen-1;
		trailingSlashToRestore = *trailingSlashPtr;
		*trailingSlashPtr = '\0';
	}

	sprintf(result, "%s%c%s", a, Platform::GetDirSep(), b);

	if (trailingSlashPtr)
	{
		*trailingSlashPtr = trailingSlashToRestore;
	}
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

void mbNormaliseFilePath(char* filepath, char dirSep)
{
	bool preceedingSlash = false;

	char dirSepToReplace = dirSep == '\\' ? '/' : '\\';

	char* cursor = filepath;
	for (; *cursor; ++cursor)
	{
		char c = *cursor;
		//Normalise slashes
		if (c == dirSepToReplace)
			c = dirSep;

		//Ignore duplicate slashes
		if (c == dirSep)
		{
			if (preceedingSlash)
				continue;
			preceedingSlash = true;
		}
		else
		{
			preceedingSlash = false;
		}

		*cursor = c;
	}
	*cursor = '\0';
}

void mbNormaliseFilePath(char* outFilePath, const char* inFilePath, char dirSep)
{
	strcpy(outFilePath, inFilePath);
	mbNormaliseFilePath(outFilePath, dirSep);
}

void mbNormaliseFilePath(std::string* inout, char dirSep)
{
    char tmp[MB_MAX_PATH];
	mbNormaliseFilePath(tmp, inout->c_str(), dirSep);
	*inout = tmp;
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

int luaCreateTable(lua_State* l)
{
	int narr = (int)lua_tonumber(l, 1);
	int nrec = (int)lua_tonumber(l, 2);

	lua_createtable(l, narr, nrec);

	return 1;
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

U32 mbRandomU32()
{ 
	return mbRandomU32(g_randomContext);
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
	a->reserve(a->size() + b.size());
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
	
	UniqueStringHashTable uniqueStrings;
	for (int i = 0; i < (int)strings.size(); ++i)
	{
		UniqueStringHashTable::const_iterator it = uniqueStrings.find(&strings[i]);
		if (it == uniqueStrings.end())
		{
			std::pair<UniqueStringHashTable::iterator, bool> result = uniqueStrings.insert(&strings[i]);
			if (!result.second)
				continue; //Already added.
				
			tmp.push_back(strings[i]);
		}
	}
	
	std::swap(tmp, strings);
}

struct StringSortRecord
{
	std::string lowerCaseString;
	std::string originalString;
};

bool mbCompareNoCase(const StringSortRecord* a, const StringSortRecord* b)
{
	return a->lowerCaseString < b->lowerCaseString;
}

void mbRemoveDuplicatesAndSort(StringVector* strings_)
{
	StringVector& strings = *strings_;
	
	std::vector<StringSortRecord*> tmp;
	tmp.reserve(strings.size());
		
	UniqueStringHashTable uniqueStrings;
	for (int i = 0; i < (int)strings.size(); ++i)
	{
		uniqueStrings.insert(&strings[i]);
	}

	for (UniqueStringHashTable::const_iterator it = uniqueStrings.begin(); it != uniqueStrings.end(); ++it)
	{
		const std::string* currentString = *it;

		StringSortRecord* r = new StringSortRecord();
		tmp.push_back(r);
		r->lowerCaseString = *currentString;
		std::transform(r->lowerCaseString.begin(), r->lowerCaseString.end(), r->lowerCaseString.begin(), ::tolower);
		r->originalString = *currentString;
	}

	std::sort(tmp.begin(), tmp.end(), mbCompareNoCase);

	strings.clear();
	for (int i = 0; i < (int)tmp.size(); ++i)
	{
		StringSortRecord* r = tmp[i];
		strings.push_back(r->originalString);
		delete r;
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
    Platform::NormaliseFilePath(osDir, osDir_);
    
    if (osDir[0] == 0)
        return false;
    
    const char* left = osDir;
    for (const char* right = osDir+1; *right; ++right)
    {
        if (*right == sep)
        {
            *(char*)right = 0;
            const char* partialPath = left;
            
            if (!Platform::CreateDir(partialPath))
                return false;
            
            *(char*)right = sep;
        }
    }
    
    return Platform::CreateDir(osDir);
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
	const char* macroStart = strstr(str, "#{");
	if (macroStart)
	{
		bool found = false;
		for (std::map<std::string, std::string>::const_iterator it = macroMap.begin(); it != macroMap.end(); ++it)
		{
			const std::string& key = it->first;
			const std::string& value = it->second;

			sprintf(macro, "#{%s}", key.c_str());

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
				sprintf(macro, "#{%s}", key);
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
	{
		return NULL;
	}
	
	mbExpandMacros(result, block, str);
//	MB_LOGINFO("%s -> %s", str, result->c_str());
	return result->c_str();
}

void* mbLuaAllocator(void* ud, void* ptr, size_t osize, size_t nsize)
{
	if (nsize == 0)
		dlfree(ptr);
	else
	{
		if (osize == 0)
		{
			return dlmalloc(nsize);
		}
		else
		{
			return dlrealloc(ptr, nsize);
		}
	}

	return NULL;
}

void mbCommonLuaRegister(lua_State* l, LuaModuleFunctions* luaFn)
{
	luaFn->AddFunction("import",		luaFuncGlobalImport);
	luaFn->AddFunction("checkplatform",	luaFuncCheckPlatform);
	luaFn->AddFunction("expandmacro",	luaFuncExpandMacro);
	luaFn->AddFunction("loginfo",		luaFuncLogInfo);
	luaFn->AddFunction("logerror",		luaFuncLogError);
	luaFn->AddFunction("logprofile",	luaFuncLogProfile);
	luaFn->AddFunction("split",			luaSplit);
	luaFn->AddFunction("createtable",	luaCreateTable);
}
