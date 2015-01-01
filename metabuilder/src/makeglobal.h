#ifndef MAKEGLOBAL_H
#define MAKEGLOBAL_H

#include "block.h"

enum E_LineEndingStyle
{
	E_LineEndingStyle_Unknown = 0,
	E_LineEndingStyle_Windows,
	E_LineEndingStyle_UNIX,
};

class MakeGlobal : public Block
{
public:
	MakeGlobal();
	
    virtual E_BlockType		GetType() const;
	virtual bool			IsA(E_BlockType t) const;		
				
	char					targetDirSep;
	E_LineEndingStyle		lineEndingStyle;

protected:
};

void mbMakeGlobalLuaRegister(lua_State* lua);

#endif
