#include "metabuilder_pch.h"

#include "makeglobal.h"

#include "writer_utility.h"

#include <map>

typedef std::map<std::string, std::string> OutputFilePathMapping;

static OutputFilePathMapping g_outputFilePathMapping;

//-----------------------------------------------------------------------------------------------------------------------------------------

static bool mbWriterUtility_FilePathMarkedAsRaw(const char* filepath)
{
	return filepath[0] == '!';
}

static void mbWriterUtility_NormaliseTargetFilePath(char* filepath)
{
	mbNormaliseFilePath(filepath, mbGetAppState()->makeGlobal->GetTargetDirSep());
}

static int mbWriterUtility_GetNumDirLevels(const char* dir)
{
	char dirSep = mbGetAppState()->makeGlobal->GetTargetDirSep();
	int dirCount = 0;

	for (const char* cursor = dir; *cursor; ++cursor)
	{
		if (*cursor == dirSep)
		{
			++dirCount;
		}
	}

	return dirCount;
}

static void mbWriterUtility_FileConvertToAbsolute(char* filepathAbs, const char* baseDir, const char* filepath)
{
	if (filepath[0] == '\0')
	{
		filepathAbs[0] = '\0';
		return;
	}

	//Bail if we've marked this filepath to not be expanded
	if (mbWriterUtility_FilePathMarkedAsRaw(filepath))
	{
		int len = (int)strlen(filepath);
		//As we're truncating, len will account for null termination.
		memcpy(filepathAbs, filepath + 1, len);
		return;
	}

	//Expand!
	sprintf(filepathAbs, "%s%c%s", baseDir, mbGetAppState()->makeGlobal->GetTargetDirSep(), filepath);
}

void mbWriterUtility_BuildPathBack(char* result, int nLevels)
{
	char dirSep = mbGetAppState()->makeGlobal->GetTargetDirSep();
	
	char* cursor = result;
	for (int i = 0; i < nLevels; ++i)
	{
		*cursor++ = '.';
		*cursor++ = '.';
		*cursor++ = dirSep;
	}

	*cursor = '\0';
}

int mbWriterUtility_GetLongestCommonSequenceLengthFromStart(const char* str1, const char* str2)
{
	int commonCount = 0;
	const char* str1Cursor = str1;
	const char* str2Cursor = str2;
	for (; *str1Cursor && *str2Cursor; ++str1Cursor, ++str2Cursor, ++commonCount)
	{
		if (*str1Cursor != *str2Cursor)
			break;
	}
	
	return commonCount;
}


static void mbWriterUtility_GetRelativeFilePath(char* result, const char* filepathUnnormalised, const char* oldBaseDir, const char* newBaseDir)
{    
	char dirSep = mbGetAppState()->makeGlobal->GetTargetDirSep();

	char filepath[MB_MAX_PATH];
	strcpy(filepath, filepathUnnormalised);

	mbWriterUtility_NormaliseTargetFilePath(filepath);

	//If filepath has been flagged to escape any kind of modification
	if (mbWriterUtility_FilePathMarkedAsRaw(filepath))
	{
		strcpy(result, filepath+1);
	}
	else
	{
		char normalisedFilepathAbs[MB_MAX_PATH];
		mbWriterUtility_FileConvertToAbsolute(normalisedFilepathAbs, oldBaseDir, filepath);
		const char* normalisedMakeOutputDirAbs = newBaseDir;

		//baseDirLength is the common path length shared by the makefile output directory and 'filepath'
		int baseDirLength = 0;
		{
			char commonSubSequence[MB_MAX_PATH];
			int commonSubSequenceLength = mbWriterUtility_GetLongestCommonSequenceLengthFromStart(normalisedFilepathAbs, normalisedMakeOutputDirAbs);
			memcpy(commonSubSequence, normalisedFilepathAbs, commonSubSequenceLength);

			//If entire sequence is a directory match
			if (normalisedFilepathAbs[commonSubSequenceLength] == dirSep)
			{
				baseDirLength = commonSubSequenceLength;
			}
			//Partial match, find dir
			else
			{
				commonSubSequence[commonSubSequenceLength] = '\0';

				//Look for last dir sep character in order to ignore a partial path or file match
				char* lastDirSep = strrchr(commonSubSequence, dirSep);
				if (lastDirSep != NULL)
				{
					//Take sequence up to last dir sep as our base dir
					*lastDirSep = '\0';
					baseDirLength = (int)(lastDirSep - commonSubSequence);
				}
			}
		}

		if (baseDirLength > 0)
		{
			const char* filepathBaseRelative = normalisedFilepathAbs + baseDirLength + 1;

			if (*(normalisedMakeOutputDirAbs + baseDirLength) == '\0')
			{
				strcpy(result, filepathBaseRelative);
			}
			else
			{
				const char* pathFromBaseToOutDir = normalisedMakeOutputDirAbs + baseDirLength + 1;
				int nDirLevels = mbWriterUtility_GetNumDirLevels(pathFromBaseToOutDir) + 1;
				//Path back from make output dir to base dir
				char pathBack[MB_MAX_PATH];
				mbWriterUtility_BuildPathBack(pathBack, nDirLevels);
				sprintf(result, "%s%s", pathBack, filepathBaseRelative);
			}
		}
		else
		{
			strcpy(result, normalisedFilepathAbs);

			//MB_LOGINFO("[%s] [%s] %s", oldBaseDir, newBaseDir, result);
		}
	}
}

static int mbWriterUtility_LuaNormaliseTargetFilePath(lua_State* l)
{
	char dirSep = mbGetAppState()->makeGlobal->GetTargetDirSep();
	std::string filepathUnnormalised;
	mbLuaToStringExpandMacros(&filepathUnnormalised, NULL, l, 1);
	
	char result[MB_MAX_PATH];
	mbNormaliseFilePath(result, filepathUnnormalised.c_str(), dirSep);
	lua_pushstring(l, result);

	return 1;
}

static int mbWriterUtility_LuaNormaliseHostFilePath(lua_State* l)
{
	std::string filepathUnnormalised;
	mbLuaToStringExpandMacros(&filepathUnnormalised, NULL, l, 1);

	char result[MB_MAX_PATH];
	Platform::NormaliseFilePath(result, filepathUnnormalised.c_str());
	lua_pushstring(l, result);

	return 1;
}

static int mbWriterUtility_LuaNormaliseWindowsFilePath(lua_State* l)
{
	std::string filepathUnnormalised;
	mbLuaToStringExpandMacros(&filepathUnnormalised, NULL, l, 1);

	char result[MB_MAX_PATH];
	mbNormaliseFilePath(result, filepathUnnormalised.c_str(), '\\');
	lua_pushstring(l, result);

	return 1;
}

static int mbWriterUtility_LuaNormaliseUnixFilePath(lua_State* l)
{
	std::string filepathUnnormalised;
	mbLuaToStringExpandMacros(&filepathUnnormalised, NULL, l, 1);

	char result[MB_MAX_PATH];
	mbNormaliseFilePath(result, filepathUnnormalised.c_str(), '/');
	lua_pushstring(l, result);

	return 1;
}

static int mbWriterUtility_LuaNormaliseMbFilePath(lua_State* l)
{
	return mbWriterUtility_LuaNormaliseUnixFilePath(l);
}

static int mbWriterUtility_LuaSetOutputFilePathMapping(lua_State* l)
{
	std::string key;
	mbLuaToStringExpandMacros(&key, NULL, l, 1);

	std::string val;
	mbLuaToStringExpandMacros(&val, NULL, l, 2);

	//MB_LOGINFO("MAPPING %s %s", key.c_str(), val.c_str());
	g_outputFilePathMapping.insert(std::make_pair(key, val));
	return 0;
}

static int mbWriterUtility_LuaGetOutputRelativeFilePath(lua_State* l)
{
	std::string filepathUnnormalised;
	mbLuaToStringExpandMacros(&filepathUnnormalised, NULL, l, 1);

	OutputFilePathMapping::iterator it = g_outputFilePathMapping.find(filepathUnnormalised);
	if (it == g_outputFilePathMapping.end())
	{
		MetaBuilderContext* ctx = mbGetActiveContext();
        MB_ASSERT(ctx);
		const char* oldBaseDir = ctx->currentMetaMakeDirAbs.c_str();
		const char* newBaseDir = ctx->makeOutputDirAbs.c_str();

		char result[MB_MAX_PATH];
		mbWriterUtility_GetRelativeFilePath(result, filepathUnnormalised.c_str(), oldBaseDir, newBaseDir);
		lua_pushstring(l, result);
	}
	else
	{
		lua_pushstring(l, it->second.c_str());
	}
	
	return 1;
}

static int mbWriterUtility_LuaGetAbsoluteFilePath(lua_State* l)
{
	std::string filepath;
	mbLuaToStringExpandMacros(&filepath, NULL, l, 1);

	MetaBuilderContext* ctx = mbGetActiveContext();
	char result[MB_MAX_PATH];
	mbWriterUtility_FileConvertToAbsolute(result, ctx->currentMetaMakeDirAbs.c_str(), filepath.c_str());
	lua_pushstring(l, result);
	return 1;
}

void mbWriterUtilityLuaRegister(lua_State* l, LuaModuleFunctions* luaFn)
{
	luaFn->AddFunction("normalisetargetfilepath",	mbWriterUtility_LuaNormaliseTargetFilePath);
	luaFn->AddFunction("normalisehostfilepath",		mbWriterUtility_LuaNormaliseHostFilePath);
	luaFn->AddFunction("normalisewindowsfilepath",	mbWriterUtility_LuaNormaliseWindowsFilePath);
	luaFn->AddFunction("normaliseunixfilepath",		mbWriterUtility_LuaNormaliseUnixFilePath);
	luaFn->AddFunction("normalisembfilepath",		mbWriterUtility_LuaNormaliseMbFilePath);
	luaFn->AddFunction("setoutputfilepathmapping",	mbWriterUtility_LuaSetOutputFilePathMapping);
	luaFn->AddFunction("getoutputrelfilepath",		mbWriterUtility_LuaGetOutputRelativeFilePath);
	luaFn->AddFunction("getabsfilepath",			mbWriterUtility_LuaGetAbsoluteFilePath);
}
