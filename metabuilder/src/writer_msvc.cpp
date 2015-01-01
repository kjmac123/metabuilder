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
	mbLuaToStringExpandMacros(&g_externalProjectReference.back().key, nullptr, l, 1);	// target;
	mbLuaToStringExpandMacros(&g_externalProjectReference.back().value, nullptr, l, 2);	// id;
	return 0;
}

static int luaFuncGetProjectID(lua_State* l)
{
    std::string targetName;
	mbLuaToStringExpandMacros(&targetName, nullptr, l, 1);
	
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

void mbWriterMSVCLuaRegister(lua_State* l, LuaModuleFunctions* luaFn)
{
	luaFn->AddFunction("msvcgenerateid",		luaFuncWriterMSVCGenerateID);
	luaFn->AddFunction("msvcregisterprojectid", luaFuncRegisterProjectID);
	luaFn->AddFunction("msvcgetprojectid",		luaFuncGetProjectID);
}
