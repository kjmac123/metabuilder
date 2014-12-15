#ifndef PLATFORMPARAM_H
#define PLATFORMPARAM_H

#include "block.h"

class PlatformParam : public ParamBlock
{
public:
	PlatformParam();
	
    virtual E_BlockType		GetType() const;
	virtual bool			IsA(E_BlockType t) const;
	
protected:
};

void mbPlatformParamLuaRegister(lua_State* lua);

#endif
