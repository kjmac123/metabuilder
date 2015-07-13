#ifndef TARGET_H
#define TARGET_H

#include "block.h"

struct TargetDepends
{
	std::string		libTargetName;
	FilePath		libMakefile;
};

class Target : public MakeBlock
{
public:
						Target();
	
    virtual E_BlockType	GetType() const;
	virtual bool		IsA(E_BlockType t) const;	
	
	virtual void		Process();

	virtual void		SetName(const char* name);
	
	//Config and platform specific
	void				Flatten(FlatConfig* result, const char* platformName, const char* configName) const;
	//Non config specific
	void				FlattenFiles(StringVector* files, const char* platformName) const;
	void				FlattenNoPchFiles(StringVector* files, const char* platformName) const;
	void				FlattenFrameworks(StringVector* frameworks, const char* platformName) const;
	void				FlattenResources(StringVector* resources, const char* platformName) const;
	
	std::string         targetType;
	std::string			targetSubsystem;

    std::vector<TargetDepends>
						depends;

	std::string			pch;
			
protected:
	void				FlattenStringGroup(StringVector* result, const char* stringGroup, const char* platformName) const;
};

typedef std::vector<Target*> TargetVector;

//-----------------------------------------------------------------------------------------------------------------------------------------

void mbTargetLuaRegister(lua_State* lua);

#endif
