#ifndef MAKEGLOBAL_H
#define MAKEGLOBAL_H

#include "block.h"

class MakeGlobal : public Block
{
public:
	MakeGlobal();
	
    virtual E_BlockType		GetType() const;
	virtual bool			IsA(E_BlockType t) const;

	//void					SetTargetDirSep(char dirSep);
	//char					GetTargetDirSep() const;
				
protected:
	//char					m_targetDirSep;
};

//-----------------------------------------------------------------------------------------------------------------------------------------

void mbMakeGlobalLuaRegister(lua_State* lua);

#endif
