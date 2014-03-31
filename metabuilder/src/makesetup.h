#ifndef MAKESETUP_H
#define MAKESETUP_H

class MakeSetup : public Block
{
public:
	MakeSetup();
	
    virtual E_BlockType		GetType() const;
	virtual bool			IsA(E_BlockType t) const;		
				
	std::string				_metabaseDir;
    std::string				_intDir;
    std::string				_outDir;

protected:
};

void mbMakeSetupLuaRegister(lua_State* lua);

#endif
