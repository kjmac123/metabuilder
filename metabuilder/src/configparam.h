#ifndef CONFIGPARAM_H
#define CONFIGPARAM_H

#include "block.h"

class ConfigParam : public ParamBlock
{
public:
	ConfigParam();
	
    virtual E_BlockType		GetType() const;
	virtual bool			IsA(E_BlockType t) const;

	virtual void			SetName(const char* name);
	
protected:
};

//-----------------------------------------------------------------------------------------------------------------------------------------

void mbConfigParamLuaRegister(lua_State* lua);

#endif
