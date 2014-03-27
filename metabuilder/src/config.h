#ifndef CONFIG_H
#define CONFIG_H

class Config : public MetaBuilderBlockBase
{
public:
	Config(MetaBuilderBlockBase* parent);
	
    virtual E_BlockType		Type() const;

protected:
};

void mbConfigLuaRegister(lua_State* lua);

#endif
