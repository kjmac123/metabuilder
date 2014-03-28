#ifndef PLATFORMPARAM_H
#define PLATFORMPARAM_H

class PlatformParam : public ParamBlock
{
public:
	PlatformParam();
	
    virtual E_BlockType		Type() const;
	virtual bool			IsA(E_BlockType t) const;
	
protected:
};

void mbPlatformParamLuaRegister(lua_State* lua);

#endif
