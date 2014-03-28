#ifndef FLATTEN_H
#define FLATTEN_H

struct FlatConfig
{
	std::string name;
	
	std::map<std::string, StringVector>	stringGroups;
	std::map<std::string, KeyValueMap>	options;
	
	void Dump();
};

struct FlatSDKConfig : FlatConfig
{
	std::string mainConfigName;
	
	void Dump();	
};

void mbFlattenTargetForConfig(FlatConfig* result, Target* target, const char* configName);
void mbFlattenTargetForSDKConfig(FlatSDKConfig* result, Target* target, const char* sdkConfigName);

#endif
