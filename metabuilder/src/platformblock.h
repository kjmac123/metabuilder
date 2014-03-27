#ifndef PLATFORMBLOCK_H
#define PLATFORMBLOCK_H

class PlatformBlock : public MetaBuilderBlockBase
{
public:
	PlatformBlock(MetaBuilderBlockBase* parent);
	
    virtual E_BlockType		Type() const;

protected:
};

void mbPlatformBlockLuaRegister(lua_State* lua);

#endif
