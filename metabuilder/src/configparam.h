#ifndef CONFIGPARAM_H
#define CONFIGPARAM_H

class ConfigParam : public ParamBlock
{
public:
	ConfigParam();
	
    virtual E_BlockType		GetType() const;
	virtual bool			IsA(E_BlockType t) const;
	
protected:
};

void mbConfigParamLuaRegister(lua_State* lua);

#endif
