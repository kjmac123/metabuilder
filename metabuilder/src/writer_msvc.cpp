#include "metabuilder_pch.h"

std::vector<KeyValue> g_externalProjectReference;

int luaFuncWriterMSVCGenerateID(lua_State* l)
{
	U32 a = mbRandomU32();
	U32 b = mbRandomU32() & 0x0000ffff;
	U32 c = mbRandomU32() & 0x0000ffff;
	U32 d = mbRandomU32() & 0x0000ffff;
	U32 e = mbRandomU32();
	U32 f = mbRandomU32() & 0x0000ffff;
				
	char hashStr[256];
	
	sprintf(hashStr, "%08X-%04X-%04X-%04X-%08X%04X", a, b, c, d, e, f);
	
	lua_pushstring(l, hashStr);
	return 1;
}

static int luaFuncRegisterProjectID(lua_State* l)
{
	g_externalProjectReference.push_back(KeyValue());
	mbLuaToStringExpandMacros(&g_externalProjectReference.back().key, l, 1);	// target;
	mbLuaToStringExpandMacros(&g_externalProjectReference.back().value, l, 2);	// id;
	return 0;
}

static int luaFuncGetProjectID(lua_State* l)
{
    std::string targetName;
	mbLuaToStringExpandMacros(&targetName, l, 1);
	
	for (int i = 0; i < (int)g_externalProjectReference.size(); ++i)
	{
		if (g_externalProjectReference[i].key == targetName)
		{
			lua_pushstring(l, g_externalProjectReference[i].value.c_str());
			return 1;
		}
	}
	
	MB_LOGERROR("failed to find project %s", targetName.c_str());
	mbExitError();
	return 0;
}

void mbWriterMSVCLuaRegister(lua_State* l)
{
    lua_pushcfunction(l, luaFuncWriterMSVCGenerateID);
    lua_setglobal(l, "msvcgenerateid");

    lua_pushcfunction(l, luaFuncRegisterProjectID);
    lua_setglobal(l, "msvcregisterprojectid");

    lua_pushcfunction(l, luaFuncGetProjectID);
    lua_setglobal(l, "msvcgetprojectid");
}
