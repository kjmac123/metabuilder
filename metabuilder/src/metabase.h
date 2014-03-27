#ifndef METABASE_H
#define METABASE_H

#include "common.h"

class Metabase : public MetaBuilderBlockBase
{
public:
	Metabase(MetaBuilderBlockBase* parent);
	
    virtual E_BlockType Type() const;
	
    std::string         name;
	StringVector		supportedPlatforms;
		
	std::string			writerLua;

protected:
};

void mbMetabaseLuaRegister(lua_State* lua);

#endif
