#include "metabuilder_pch.h"

std::vector<KeyValue> g_externalFileReference;
std::vector<KeyValue> g_externalNativeTarget;

int luaFuncWriterXcodeGenerateID(lua_State* l)
{
	char hashStr[256];
	sprintf(hashStr, "%08x%08x%08x", mbRandomU32(), mbRandomU32(), mbRandomU32());
	
	lua_pushstring(l, hashStr);
	return 1;
}

static int luaFuncRegisterExternalPBXFileReferenceExternal(lua_State* l)
{
	g_externalFileReference.push_back(KeyValue());
	mbLuaToStringExpandMacros(&g_externalFileReference.back().key, nullptr, l, 1);	//filename
	mbLuaToStringExpandMacros(&g_externalFileReference.back().value, nullptr, l, 2);	//id
	return 0;
}

static int luaFuncRegisterExternalPBXNativeTargetExternal(lua_State* l)
{
	g_externalNativeTarget.push_back(KeyValue());
	mbLuaToStringExpandMacros(&g_externalNativeTarget.back().key, nullptr, l, 1);	//target
	mbLuaToStringExpandMacros(&g_externalNativeTarget.back().value, nullptr, l, 2);	//id
	return 0;
}

static int luaFuncGetExternalPBXFileReferenceExternal(lua_State* l)
{
    std::string filename;
	mbLuaToStringExpandMacros(&filename, nullptr, l, 1);
	
	for (int i = 0; i < (int)g_externalFileReference.size(); ++i)
	{
		if (g_externalFileReference[i].key == filename)
		{
			lua_pushstring(l, g_externalFileReference[i].value.c_str());
			return 1;
		}
	}
	return 0;
}

static int luaFuncGetExternalPBXNativeTargetExternal(lua_State* l)
{
    std::string targetName;
	mbLuaToStringExpandMacros(&targetName, nullptr, l, 1);
	
	for (int i = 0; i < (int)g_externalNativeTarget.size(); ++i)
	{
		if (g_externalNativeTarget[i].key == targetName)
		{
			lua_pushstring(l, g_externalNativeTarget[i].value.c_str());
			return 1;
		}
	}
	return 0;
}

void mbWriterXcodeLuaRegister(lua_State* l)
{
    lua_pushcfunction(l, luaFuncWriterXcodeGenerateID);
    lua_setglobal(l, "mbwriter_xcodegenerateid");

    lua_pushcfunction(l, luaFuncRegisterExternalPBXFileReferenceExternal);
    lua_setglobal(l, "mbwriter_xcoderegisterpbxfilereference_external");

    lua_pushcfunction(l, luaFuncRegisterExternalPBXNativeTargetExternal);
    lua_setglobal(l, "mbwriter_xcoderegisterpbxnativetarget_external");
		
    lua_pushcfunction(l, luaFuncGetExternalPBXFileReferenceExternal);
    lua_setglobal(l, "mbwriter_xcodegetpbxfilereference_external");

    lua_pushcfunction(l, luaFuncGetExternalPBXNativeTargetExternal);
    lua_setglobal(l, "mbwriter_xcodegetpbxnativetarget_external");
}
