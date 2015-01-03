#include "metabuilder_pch.h"

#include "makesetup.h"
#include "makeglobal.h"

#include "writer_utility.h"


static bool mbWriterUtility_FilePathMarkedAsRaw(const char* filepath)
{
	return filepath[0] == '!';
}

static void mbWriterUtility_NormaliseTargetFilePath(char* filepath)
{
	char dirSep = mbGetAppState()->makeGlobal->GetTargetDirSep();
	mbNormaliseFilePath(filepath, dirSep);
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
		int len = strlen(filepath);
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

		//baseDir is the common path fragment shared by the makefile output directory and 'filepath'
		char baseDir[MB_MAX_PATH];
		int baseDirLength = 0;
		baseDir[0] = '\0';
		{
			int commonSubSequenceLength = mbWriterUtility_GetLongestCommonSequenceLengthFromStart(normalisedFilepathAbs, normalisedMakeOutputDirAbs);
			char commonSubSequence[MB_MAX_PATH];
			memcpy(commonSubSequence, normalisedFilepathAbs, commonSubSequenceLength);
			commonSubSequence[commonSubSequenceLength] = '\0';

			//Look for last dir sep character in order to ignore a partial path or file match
			char* lastDirSep = strrchr(commonSubSequence, dirSep);
			if (lastDirSep != nullptr)
			{
				//Take sequence up to last dir sep as our base dir
				*lastDirSep = '\0';
				baseDirLength = (int)(lastDirSep - commonSubSequence);
			}
		}

		if (baseDir != nullptr)
		{
			const char* pathFromBaseToOutDir = normalisedMakeOutputDirAbs + baseDirLength + 1;
			int nDirLevels = mbWriterUtility_GetNumDirLevels(pathFromBaseToOutDir) + 1;
			//Path back from make output dir to base dir
			char pathBack[MB_MAX_PATH];
			mbWriterUtility_BuildPathBack(pathBack, nDirLevels);

			const char* filepathBaseRelative = normalisedFilepathAbs + baseDirLength + 1;
			sprintf(result, "%s%s", pathBack, filepathBaseRelative);
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
	const char* filepathUnnormalised = lua_tostring(l, 1);
	
	char result[MB_MAX_PATH];
	mbNormaliseFilePath(result, filepathUnnormalised, dirSep);
	lua_pushstring(l, result);

	return 1;
}

static int mbWriterUtility_LuaNormaliseHostFilePath(lua_State* l)
{
	char dirSep = mbGetAppState()->makeGlobal->GetTargetDirSep();
	const char* filepathUnnormalised = lua_tostring(l, 1);

	char result[MB_MAX_PATH];
	Platform::NormaliseFilePath(result, filepathUnnormalised);
	lua_pushstring(l, result);

	return 1;
}

static int mbWriterUtility_LuaGetOutputRelativeFilePath(lua_State* l)
{
	const char* filepathUnnormalised = lua_tostring(l, 1);

	if (strstr(filepathUnnormalised, "writer_xcode"))
	{
		int x= 0;x;
	}

	MetaBuilderContext* ctx = mbGetActiveContext();
	const char* oldBaseDir = ctx->currentMetaMakeDirAbs.c_str();
	const char* newBaseDir = ctx->makeOutputDirAbs.c_str();

	char result[MB_MAX_PATH];
	mbWriterUtility_GetRelativeFilePath(result, filepathUnnormalised, oldBaseDir, newBaseDir);
	lua_pushstring(l, result);
	return 1;
}

static int mbWriterUtility_LuaGetAbsoluteFilePath(lua_State* l)
{
	const char* filepath = lua_tostring(l, 1);

	MetaBuilderContext* ctx = mbGetActiveContext();
	char result[MB_MAX_PATH];
	mbWriterUtility_FileConvertToAbsolute(result, filepath, ctx->currentMetaMakeDirAbs.c_str());
	lua_pushstring(l, result);
	return 1;
}

void mbWriterUtilityLuaRegister(lua_State* l, LuaModuleFunctions* luaFn)
{
	luaFn->AddFunction("normalisetargetfilepath",	mbWriterUtility_LuaNormaliseTargetFilePath);
	luaFn->AddFunction("normalisehostfilepath",		mbWriterUtility_LuaNormaliseHostFilePath);
	luaFn->AddFunction("getoutputrelfilepath",		mbWriterUtility_LuaGetOutputRelativeFilePath);
	luaFn->AddFunction("getabsfilepath",			mbWriterUtility_LuaGetAbsoluteFilePath);
}
