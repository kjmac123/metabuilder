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
	
	
	void						AddFiles(const StringVector& files);
	void						GetFiles(StringVector* result) const;

	void						AddResources(const StringVector& files);
	void						GetResources(StringVector* result) const;

	void						AddFrameworks(const StringVector& files);
//	void						GetFrameworks(StringVector* result) const;

	void						AddDefines(const StringVector& defines);
//	void						GetDefines(StringVector* result, const char* configName) const;
	
	void						AddLibs(const StringVector& libs);
//	void						GetLibs(StringVector* result, const char* configName) const;

	void						AddIncludeDirs(const StringVector& libs);
//	void						GetIncludeDirs(StringVector* result, const char* configName) const;

	void						AddLibDirs(const StringVector& libs);
//	void						GetLibDirs(StringVector* result, const char* configName) const;

	void						GetStringGroups(std::map<std::string, StringVector>* result, const char* configName) const;

	void						SetOption(const std::string& group, const std::string& key, const std::string& value);
	void						GetOptions(std::map<std::string, KeyValueMap>* result, const char* configName) const;

	void						AddExeDirs(const StringVector& defines);
//	void						GetExeDirs(StringVector* result, const char* configName) const;
	
	ConfigParam*				AcquireConfigParam(const char* name);
	SDKParam*					AcquireSDKParam(const char* name);
	PlatformParam*				AcquirePlatformParam(const char* name);

	void						GetParams(ParamVector* result, E_BlockType t, bool recurseChildParams) const;
	ParamBlock*					GetParam(E_BlockType t, const char* name);
	const ParamBlock*			GetParam(E_BlockType t, const char* name) const;
	
	const std::vector<ParamBlock*>
								GetParamBlocks() const;

	void						GetPlatformParams(PlatformParamVector* result, bool recurseChildParams) const { return GetParams((ParamVector*)result, E_BlockType_PlatformParam, recurseChildParams); }
	void						GetConfigParams(ConfigParamVector* result, bool recurseChildParams) const { return GetParams((ParamVector*)result, E_BlockType_ConfigParam, recurseChildParams); }
	void						GetSDKParams(SDKParamVector* result, bool recurseChildParams) const { return GetParams((ParamVector*)result, E_BlockType_SDKParam, recurseChildParams); }
	
	const PlatformParam*		GetPlatformParam(const char* name) const { return (PlatformParam*)GetParam(E_BlockType_PlatformParam, name); }
	const ConfigParam*			GetConfigParam(const char* name) const { return (ConfigParam*)GetParam(E_BlockType_ConfigParam, name); }
	const SDKParam*				GetSDKParam(const char* name) const { return (SDKParam*)GetParam(E_BlockType_SDKParam, name); }
				
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

#endif
