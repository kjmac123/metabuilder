#ifndef METABASE_H
#define METABASE_H

#include "common.h"

class Metabase : public MakeBlock
{
public:
	Metabase();
	
    virtual E_BlockType Type() const;
	virtual bool		IsA(E_BlockType t) const;		
	
	StringVector		supportedPlatforms;
		
	std::string			writerLua;

protected:
};

void mbMetabaseLuaRegister(lua_State* lua);

#endif
