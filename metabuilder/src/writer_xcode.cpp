#include "metabuilder_pch.h"

#include "common.h"


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
	mbLuaToStringExpandMacros(&g_externalFileReference.back().key, l, 1);	//filename
	mbLuaToStringExpandMacros(&g_externalFileReference.back().value, l, 2);	//id
	return 0;
}

static int luaFuncRegisterExternalPBXNativeTargetExternal(lua_State* l)
{
	g_externalNativeTarget.push_back(KeyValue());
	mbLuaToStringExpandMacros(&g_externalNativeTarget.back().key, l, 1);	//target
	mbLuaToStringExpandMacros(&g_externalNativeTarget.back().value, l, 2);	//id
	return 0;
}

static int luaFuncGetExternalPBXFileReferenceExternal(lua_State* l)
{
    std::string filename;
	mbLuaToStringExpandMacros(&filename, l, 1);
	
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
	mbLuaToStringExpandMacros(&targetName, l, 1);
	
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
    lua_setglobal(l, "xcodegenerateid");

    lua_pushcfunction(l, luaFuncRegisterExternalPBXFileReferenceExternal);
    lua_setglobal(l, "xcoderegisterpbxfilereference_external");

    lua_pushcfunction(l, luaFuncRegisterExternalPBXNativeTargetExternal);
    lua_setglobal(l, "xcoderegisterpbxnativetarget_external");
		
    lua_pushcfunction(l, luaFuncGetExternalPBXFileReferenceExternal);
    lua_setglobal(l, "xcodegetpbxfilereference_external");

    lua_pushcfunction(l, luaFuncGetExternalPBXNativeTargetExternal);
    lua_setglobal(l, "xcodegetpbxnativetarget_external");
}
