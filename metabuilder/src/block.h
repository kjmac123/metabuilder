#ifndef BLOCK_H
#define BLOCK_H

class Block
{
public:
								Block();
	virtual						~Block();

    virtual E_BlockType			Type() const = 0;
	virtual bool				IsA(E_BlockType t) const;
	
	virtual void				Process();
	
	virtual void				Dump() const;

	virtual void				AddChild(Block* block);
	Block*						GetParent();

	void						SetName(const char* name);
	const std::string&			GetName() const;
	
	const char*					GetParentConfig() const;
	const char*					GetParentPlatform() const;
	
	void						AddFiles(const StringVector& files);
	void						GetFiles(StringVector* result) const;

	void						AddResources(const StringVector& files);
	void						GetResources(StringVector* result) const;

	void						AddFrameworks(const StringVector& files);
	void						GetFrameworks(StringVector* result) const;
	
	void						AddDefines(const StringVector& defines);
	
	void						AddLibs(const StringVector& libs);

	void						AddIncludeDirs(const StringVector& libs);

	void						AddLibDirs(const StringVector& libs);

	void						GetStringGroups(std::map<std::string, StringVector>* result) const;

	void						SetOption(const std::string& group, const std::string& key, const std::string& value);
	void						GetOptions(std::map<std::string, KeyValueMap>* result) const;

	void						AddExeDirs(const StringVector& defines);
	
	ConfigParam*				AcquireConfigParam(const char* name);
	PlatformParam*				AcquirePlatformParam(const char* name);

	void						GetParams(ParamVector* result, E_BlockType t, const char* platformName, const char* configName, bool recurseChildParams) const;
	ParamBlock*					GetParam(E_BlockType t, const char* name);
	const ParamBlock*			GetParam(E_BlockType t, const char* name) const;
	
	const std::vector<ParamBlock*>&
								GetParamBlocks() const;

	void						GetPlatformParams(PlatformParamVector* result, const char* platformName, bool recurseChildParams) const { return GetParams((ParamVector*)result, E_BlockType_PlatformParam, platformName, NULL, recurseChildParams); }
	
	const PlatformParam*		GetPlatformParam(const char* name) const { return (PlatformParam*)GetParam(E_BlockType_PlatformParam, name); }
	const ConfigParam*			GetConfigParam(const char* name) const { return (ConfigParam*)GetParam(E_BlockType_ConfigParam, name); }
				
protected:
	StringVector*				AcquireStringGroup(const char* groupName);
	const StringVector*			GetStringGroup(const char* groupName) const;

	std::string					m_name;
	
	Block*						m_parent;

	//First-class data stored here, e.g. files, includes
	std::map<std::string, StringVector>
								m_stringGroups;
	
	//Key-value pairs stored per group
	std::map<std::string, KeyValueMap>
								m_keyValueGroups;
				
	std::vector<ParamBlock*>	m_childParams;
};

//Generic operations add information to the current context via this interface.
class MakeBlock : public Block
{
public:
								MakeBlock();
	virtual						~MakeBlock();
	
	virtual void				AddChild(Block* block);
	
	virtual bool				IsA(E_BlockType t) const;	

	virtual void				Process();
	
	virtual void				Dump() const;
				
		
protected:
	std::vector<MakeBlock*>		m_childMakeBlocks;
};


class ParamBlock : public Block
{
public:
								ParamBlock();
	virtual						~ParamBlock();
	
	virtual bool				IsA(E_BlockType t) const;
	
	virtual void				Process();
	
	virtual void				Dump() const;	
};

const char** mbGetStringGroupNames();

#endif
