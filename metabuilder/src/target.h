#ifndef TARGET_H
#define TARGET_H

#include "common.h"

struct TargetDepends
{
	std::string libTargetName;
	std::string libMakefile;
};

class Target : public MakeBlock
{
public:
						Target();
	
    virtual E_BlockType	Type() const;
	virtual bool		IsA(E_BlockType t) const;	
	
	virtual void		Process();
		
	void				GetPlatformFiles(StringVector* files, const char* platformName) const;
	void				GetPlatformFrameworks(StringVector* frameworks, const char* platformName);
	void				GetPlatformResources(StringVector* resources, const char* platformName);
	
	std::string         targetType;

    std::vector<TargetDepends>
						depends;

	std::string			pch;
			
protected:
};

typedef std::vector<Target*> TargetVector;

void mbTargetLuaRegister(lua_State* lua);

#endif
