#ifndef BLOCK_H
#define BLOCK_H

#define STRINGGROUP_FILES					"files"
#define STRINGGROUP_NOPCHFILES				"nopchfiles"
#define STRINGGROUP_DEFINES					"defines"
#define STRINGGROUP_INCLUDEDIRS				"includedirs"
#define STRINGGROUP_LIBDIRS					"libdirs"
#define STRINGGROUP_LIBS					"libs"
#define STRINGGROUP_EXEDIRS					"exedirs"
#define STRINGGROUP_RESOURCES				"resources"
#define STRINGGROUP_FRAMEWORKS				"frameworks"

#define MACRO_SEMANTIC_TARGET_NAME			"MB_TARGET_NAME"
#define MACRO_SEMANTIC_CONFIG_NAME			"MB_CONFIG_NAME"

const char**	mbGetStringGroupNames();
void			mbBlockLuaRegister(lua_State* l);

struct FlatConfig
{
	std::string name;
	
	std::map<std::string, StringVector>	stringGroups;
	std::map<std::string, KeyValueMap>	options;
	
	void Init(const char* configName);
	void Dump();

private:
};

class Block
{
public:
								Block();
	virtual						~Block();

    virtual E_BlockType			GetType() const = 0;
	virtual bool				IsA(E_BlockType t) const;
	
	virtual void				Process();
	
	virtual void				Dump() const;

	virtual void				AddChild(Block* block);
	Block*						GetParent();
	const Block*				GetParent() const;

	virtual void				SetName(const char* name);
	const std::string&			GetName() const;
	
	const char*					GetParentConfig() const;
	const char*					GetParentPlatform() const;
	
	void						AddFiles(const StringVector& files);
	void						AddNoPchFiles(const StringVector& files);
	void						AddResources(const StringVector& files);
	void						AddFrameworks(const StringVector& files);
	void						AddDefines(const StringVector& defines);
	void						AddLibs(const StringVector& libs);
	void						AddIncludeDirs(const StringVector& libs);
	void						AddLibDirs(const StringVector& libs);
	void						AddExeDirs(const StringVector& defines);

	void						AppendOption(const std::string& group, const std::string& key, const std::string& value, char seperator);
	void						SetOption(const std::string& group, const std::string& key, const std::string& value);
	const char*					GetOption(const std::string& group, const std::string& key) const;
	void						GetOptions(std::map<std::string, KeyValueMap>* result) const;
	
	ConfigParam*				AcquireConfigParam(const char* name);
	PlatformParam*				AcquirePlatformParam(const char* name);

	void						GetParams(ParamVector* result, E_BlockType t, const char* platformName, const char* configName, bool recurseChildParams) const;
	ParamBlock*					GetParam(E_BlockType t, const char* name);
	const ParamBlock*			GetParam(E_BlockType t, const char* name) const;
	
	const std::vector<ParamBlock*>&
								GetParamBlocks() const;

	void						SetTargetOutDir(const char* targetOutDir);
	void						SetTargetIntDir(const char* targetIntDir);
	void						SetTargetFilename(const char* targetFilename);

	void						SetMacro(const char* key, const char* value);
	const KeyValueMap&			FlattenMacros() const;

	void						FlattenThis(FlatConfig* result) const;

protected:
	StringVector*				AcquireStringGroup(const char* groupName);
	const StringVector*			GetStringGroup(const char* groupName) const;

	void						SetMacroCacheDirty() const;
	const KeyValueMap&			GetMacros() const;

	std::string					m_name;
	
	Block*						m_parent;

	//First-class data stored here, e.g. files, includes
	std::map<std::string, StringVector>
								m_stringGroups;
	
	//Key-value pairs stored per group
	std::map<std::string, KeyValueMap>
								m_keyValueGroups;
				
	std::vector<ParamBlock*>	m_childParams;

	mutable	KeyValueMap			m_flattenedMacroCache;
	mutable bool				m_macroCacheDirty;
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
