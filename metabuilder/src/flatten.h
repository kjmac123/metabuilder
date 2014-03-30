#ifndef FLATTEN_H
#define FLATTEN_H

struct FlatConfig
{
	std::string name;
	
	std::map<std::string, StringVector>	stringGroups;
	std::map<std::string, KeyValueMap>	options;
	
	void Dump();
};

void mbFlattenTargetForWriter(FlatConfig* result, Target* target, const char* platformName, const char* configName);

#endif
