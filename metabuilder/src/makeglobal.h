#ifndef MAKEGLOBAL_H
#define MAKEGLOBAL_H

#include "block.h"

class MakeGlobal : public Block
{
public:
	MakeGlobal();
	
    virtual E_BlockType		GetType() const;
	virtual bool			IsA(E_BlockType t) const;		
				
	char					targetDirSep;

protected:
};

//-----------------------------------------------------------------------------------------------------------------------------------------

void mbMakeGlobalLuaRegister(lua_State* lua);

#endif
