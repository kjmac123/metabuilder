#ifndef TARGET_H
#define TARGET_H

#include "common.h"

struct TargetDepends
{
	std::string libTargetName;
	std::string libMakefile;
};

class Target : public MetaBuilderBlockBase
{
public:
	Target(MetaBuilderBlockBase* parent);
	
    virtual E_BlockType	Type() const;
	
	virtual void		Process();
	
	void				AddFiles(const StringVector& files);
	void				AddPlatformFiles(const StringVector& files, const char* platformName);
	
	void				GetPlatformFiles(StringVector* files, const char* platformName);
	void				GetPlatformFrameworks(StringVector* frameworks, const char* platformName);
	void				GetPlatformResources(StringVector* resources, const char* platformName);

    std::string         name;
	std::string         targetType;

    std::vector<TargetDepends>
						depends;
	std::map<std::string, StringVector> platformFrameworks;
	std::map<std::string, StringVector> platformResources;
	
	std::string			pch;
			
protected:
	void				AddHeadersAutomatically(StringVector* files) const;
	
	StringVector		m_defines;
    StringVector        m_files;
	std::map<std::string, StringVector>
						m_platformFiles;
};

typedef std::vector<Target*> TargetVector;

void mbTargetLuaRegister(lua_State* lua);

#endif
