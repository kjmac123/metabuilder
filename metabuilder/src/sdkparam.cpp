#include "metabuilder_pch.h"

#include "common.h"

SDKParam::SDKParam()
{
}

SDKParam::~SDKParam()
{
}

E_BlockType SDKParam::Type() const
{
	return E_BlockType_SDKParam;
}

bool SDKParam::IsA(E_BlockType t) const
{
	if (ParamBlock::IsA(t))
		return true;

	return t == E_BlockType_SDKParam;
}

void SDKParam::SetMainConfigName(const char* mainConfigName)
{
	m_mainConfigName = mainConfigName;
}

const std::string& SDKParam::GetMainConfigName() const
{
	return m_mainConfigName;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

static int luaFuncSDKParam(lua_State* l)
{
	Block* activeBlock = mbGetActiveContext()->ActiveBlock();

    const char* sdkConfigName = lua_tostring(l, 1);
	const char* mainConfigName = lua_tostring(l, 2);
	
	//TODO - validation
	
	
	//Create new target instance.
    SDKParam* sdkParam = new SDKParam();
	activeBlock->AddChild(sdkParam);
    sdkParam->SetName(sdkConfigName);
        
	//Config becomes new active block
    mbGetActiveContext()->PushActiveBlock(sdkParam);
    return 0;
}

static int luaFuncPlatformEnd(lua_State* lua)
{
	mbGetActiveContext()->activeBlockStack.pop();
	return 0;
}

void mbSDKParamLuaRegister(lua_State* l)
{
    lua_pushcfunction(l, luaFuncSDKParam);
    lua_setglobal(l, "sdkconfig");

    lua_pushcfunction(l, luaFuncPlatformEnd);
    lua_setglobal(l, "sdkconfig_end");
}
