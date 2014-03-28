#ifndef SDKBLOCK_H
#define SDKBLOCK_H

class SDKParam : public ParamBlock
{
public:
							SDKParam();
	virtual					~SDKParam();
	
    virtual E_BlockType		Type() const;
	virtual bool			IsA(E_BlockType t) const;
	
	void					SetMainConfigName(const char* mainConfigName);
	const std::string&		GetMainConfigName() const;

protected:
	std::string				m_mainConfigName;
};

void mbSDKParamLuaRegister(lua_State* lua);

#endif
