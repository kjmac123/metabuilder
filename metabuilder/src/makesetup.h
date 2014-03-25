#ifndef MAKESETUP_H
#define MAKESETUP_H

class MakeSetup : public MetaBuilderBlockBase
{
public:
	MakeSetup(MetaBuilderBlockBase* parent);
	
    virtual E_BlockType		Type() const;
				
	std::string				_metabaseDir;
    std::string				_intDir;
    std::string				_outDir;

protected:
};

void mbMakeSetupLuaRegister(lua_State* lua);

#endif
