#include "metabuilder_pch.h"

#include "common.h"
#include "platform/platform.h"

#include "writer_xcode.h"

#include <set>

static int luaSplit (lua_State *L) {
  const char *s = luaL_checkstring(L, 1);
  const char *sep = luaL_checkstring(L, 2);
  const char *e;
  int i = 1;

  lua_newtable(L);  /* result */

  /* repeat for each separator */
  while ((e = strchr(s, *sep)) != NULL) {
	lua_pushlstring(L, s, e-s);  /* push substring */
	lua_rawseti(L, -2, i++);
	s = e + 1;  /* skip separator */
  }

  /* push last substring */
  lua_pushstring(L, s);
  lua_rawseti(L, -2, i);

  return 1;  /* return the table */
}

static int luaFuncMkdir(lua_State* l)
{
    const char* path = lua_tostring(l, 1);
	
	if (!mbCreateDirChain(path))
	{
		mbExitError();
	}
	
	return 0;
}

static int luaFuncMklink(lua_State* l)
{
    const char* src = lua_tostring(l, 1);
    const char* dst = lua_tostring(l, 2);
	
	if (!src || !dst)
	{
		mbExitError();
	}
		
	if (!mbaCreateLink(src, dst))
	{
		mbExitError();
	}
	
	return 0;
}

static int luaFuncGetFileType(lua_State* l)
{
    const char* filepath = lua_tostring(l, 1);
	E_FileType fileType = mbaGetFileType(filepath);
	
	const char* result = "unknown";
	switch (fileType)
	{
		case E_FileType_File:
			result = "file";
			break;
		case E_FileType_Dir:
			result = "dir";
			break;
		case E_FileType_Missing:
			result = "missing";
			break;
		case E_FileType_Unknown:
		default:
			break;
	}
	lua_pushstring(l, result);
	return 1;
}
#if 0
static int luaFuncGetRelativeDirTo(lua_State* l)
{
    const char* from = lua_tostring(l, 1);
    const char* to = lua_tostring(l, 2);

	std::string result;
	if (mbPathRelativeDirTo(&result, from, to))
	{
		lua_pushstring(l, result.c_str());
		return 1;
	}

	return 0;
}
#endif

static int luaFuncCopyFile(lua_State* l)
{
    const char* fromFilename = lua_tostring(l, 1);
    const char* toFilename = lua_tostring(l, 2);
	if (!fromFilename || !toFilename)
	{
		MB_LOGINFO("Failed to copy file. Insufficient args");
		mbExitError();
		return 0;
	}

	FILE* fromFile = fopen(fromFilename, "rb");
	if (!fromFile)
	{
        MB_LOGINFO("cannot open file %s", fromFilename);
        mbExitError();
	}
	
	FILE* toFile = fopen(toFilename, "wb");
	if (!toFile)
	{
        MB_LOGINFO("cannot open file %s", toFilename);
        mbExitError();
	}
	
	fseek(fromFile, 0, SEEK_END);
	long bytesToCopy = ftell(fromFile);
	fseek(fromFile, 0, SEEK_SET);
	
	char buf[10*1024];
	while(bytesToCopy > 0)
	{
		long bytesThisTime = bytesToCopy > sizeof(buf) ? sizeof(buf) : bytesToCopy;

		long actualBytesRead = fread(buf, 1, bytesThisTime, fromFile);
		if (actualBytesRead != bytesThisTime)
		{
			fclose(toFile);
			fclose(fromFile);
			MB_LOGINFO("Failed to read from file %s", fromFilename);
			mbExitError();
		}
		
		long bytesWritten = fwrite(buf, 1, actualBytesRead, toFile);
		if (bytesWritten != bytesThisTime)
		{
			fclose(toFile);
			fclose(fromFile);
			MB_LOGINFO("Failed to copy to file %s", toFilename);
			mbExitError();
		}
		
		bytesToCopy -= bytesThisTime;
	}
	
	fclose(toFile);
	fclose(fromFile);

	return 0;
}

void luaRegisterWriterFuncs(lua_State* l)
{
	mbWriterXcodeLuaRegister(l);
	mbWriterMSVCLuaRegister(l);
	
    lua_pushcfunction(l, luaSplit);
    lua_setglobal(l, "split");
	
    lua_pushcfunction(l, luaFuncMkdir);
    lua_setglobal(l, "mkdir");
	
    lua_pushcfunction(l, luaFuncMklink);
    lua_setglobal(l, "mklink");

    lua_pushcfunction(l, luaFuncGetFileType);
    lua_setglobal(l, "getfiletype");
	
    lua_pushcfunction(l, luaFuncCopyFile);
    lua_setglobal(l, "copyfile");
}

static void mbWriterSetOptions(lua_State* l, const std::map<std::string, KeyValueMap>& options)
{
	//	mbDebugDumpKeyValueGroups(options);

	lua_createtable(l, 0, 0);
	{
		int jOptionGroup = 0;
		for (std::map<std::string, KeyValueMap>::const_iterator optionGroupIt = options.begin(); optionGroupIt != options.end(); ++optionGroupIt, ++jOptionGroup)
		{
			const std::string& groupName = optionGroupIt->first;
			const KeyValueMap& options = optionGroupIt->second;

			lua_createtable(l, 0, 0);
			{
				int jOption = 0;
				for (KeyValueMap::const_iterator optionIt = options.begin(); optionIt != options.end(); ++optionIt, ++jOption)
				{
					const char* key = optionIt->first.c_str();
					const char* value = optionIt->second.c_str();
					
					if (value[0] == '\0')
					{
						//No key/value pair
						lua_pushstring(l, key);
					}
					else
					{
						//Key/value pair
						char buf[4096];
						sprintf(buf, "%s=%s", key, value);
						lua_pushstring(l, buf);
					}

					lua_rawseti(l, -2, jOption+1);
				}
			}
			lua_setfield(l, -2, groupName.c_str());
		}
	}
	
	lua_setfield(l, -2, "options");
}

static void mbWriterSetStringGroups(lua_State* l, const std::map<std::string, StringVector>& stringGroups)
{
	const char** groupNames = mbGetStringGroupNames();
	for (const char** cursor = groupNames; *cursor; ++cursor)
	{
		const char* groupName = *cursor;
		lua_createtable(l, 0, 0);
		lua_setfield(l, -2, groupName);
	}
	
	int jOptionGroup = 0;
	for (std::map<std::string, StringVector>::const_iterator optionGroupIt = stringGroups.begin(); optionGroupIt != stringGroups.end(); ++optionGroupIt, ++jOptionGroup)
	{
		const std::string& groupName = optionGroupIt->first;
		const StringVector& strings = optionGroupIt->second;

		lua_createtable(l, 0, 0);
		{
			for (int jString = 0; jString < (int)strings.size(); ++jString)
			{
				const char* value = strings[jString].c_str();
				lua_pushstring(l, value);
				lua_rawseti(l, -2, jString+1);
			}
		}
		lua_setfield(l, -2, groupName.c_str());
	}
}

static void mbWriterWriteConfigTable(lua_State* l, const FlatConfig& flatConfig)
{
	//Set config name
	lua_pushstring(l, flatConfig.name.c_str());
	lua_setfield(l, -2, "name");
	
	mbWriterSetStringGroups(l, flatConfig.stringGroups);
	mbWriterSetOptions(l, flatConfig.options);
}


void mbWriterDo(MetaBuilderContext* ctx)
{
	mbPushActiveContext(ctx);

	lua_State *l;
    l = luaL_newstate();
	luaL_checkstack(l, MB_LUA_STACK_MAX, "Out of stack!");

	luaL_openlibs(l);

	AppState* appState = mbGetAppState();

    Metabase* metabase = ctx->metabase;
    if (!metabase)
    {
        MB_LOGERROR("unknown IDE specified.");
        mbExitError();
    }
	metabase->Process();
			
	Solution* solution = ctx->solution;
	
	//Global information table
	{
		lua_createtable(l, 0, 1);

		lua_pushstring(l, mbGetAppState()->metabaseDirAbs.c_str());
		lua_setfield(l, -2, "metabasedirabs");
				
		{
			char buf[MB_MAX_PATH];
			sprintf(buf, "%s/%s/%s", appState->makeOutputDirAbs.c_str(), appState->mainSolutionName.c_str(), metabase->GetName().c_str());
			lua_pushstring(l, buf);
			lua_setfield(l, -2, "makeoutputdirabs");
			
			if (!mbCreateDirChain(buf))
			{
				mbExitError();
			}
		}

		lua_pushstring(l, appState->mainMetaMakeFileAbs.c_str());
		lua_setfield(l, -2, "mainmetamakefileabs");

		lua_pushstring(l, ctx->currentMetaMakeDirAbs.c_str());
		lua_setfield(l, -2, "currentmetamakedirabs");

		lua_pushstring(l, appState->intDir.c_str());
		lua_setfield(l, -2, "intdir");

		lua_pushstring(l, appState->outDir.c_str());
		lua_setfield(l, -2, "outdir");
		
		//Write out the set of options associated with our metabase node.
		{
			std::map<std::string, KeyValueMap> options;
			metabase->GetOptions(&options);
			mbWriterSetOptions(l, options);
		}
	
		lua_pushboolean(l, appState->cmdSetup.verbose);
		lua_setfield(l, -2, "verbose");
		
		lua_setglobal(l, "writer_global");
	}
	
	//Solution table
	{
		lua_createtable(l, 0, 4);
				
		lua_pushstring(l, solution->GetName().c_str());
		lua_setfield(l, -2, "name");

		//Target table
		int nTargets = (int)solution->targetVector.size();
		lua_createtable(l, 0, nTargets);
				
		for (int iTarget = 0; iTarget < nTargets; ++iTarget)
		{
			Target* target = solution->targetVector[iTarget];

			lua_createtable(l, 0, 2);
			{
				lua_pushstring(l, target->GetName().c_str());
				lua_setfield(l, -2, "name");

				lua_pushstring(l, target->targetType.c_str());
				lua_setfield(l, -2, "targetType");

				lua_pushstring(l, target->pch.c_str());
				lua_setfield(l, -2, "pch");
				
				//All regular files across all platforms for this target
				{
					StringVector allFiles;
					
					target->GetPlatformFiles(&allFiles, NULL);
					
					for (int jPlatform = 0; jPlatform < (int)ctx->metabase->supportedPlatforms.size(); ++jPlatform)
					{
						target->GetPlatformResources(&allFiles, ctx->metabase->supportedPlatforms[jPlatform].c_str());
						target->GetPlatformFrameworks(&allFiles, ctx->metabase->supportedPlatforms[jPlatform].c_str());
					}

					lua_createtable(l, 0, 0);
					{
						for (int jFile = 0; jFile < (int)allFiles.size(); ++jFile)
						{
							const std::string& filepath = allFiles[jFile];
							
							lua_pushstring(l, filepath.c_str());
							lua_rawseti(l, -2, jFile+1);
						}
					}
					lua_setfield(l, -2, "allfiles");
				}

				//Configs
				{
					StringVector configNames;
					{
						ConfigParamVector configs;
						target->GetParams((ParamVector*)&configs, E_BlockType_ConfigParam, NULL, NULL, true);
						for (int jConfig = 0; jConfig < (int)configs.size(); ++jConfig)
						{
							configNames.push_back(configs[jConfig]->GetName());
						}
						mbRemoveDuplicates(&configNames);
					}
					
					//Write out configs, with embedded expanded out data per SDK platform.
					lua_createtable(l, 0, 0);
					{
						for (int jConfig = 0; jConfig < (int)configNames.size(); ++jConfig)
						{
							const char* configName = configNames[jConfig].c_str();
							FlatConfig flatConfig;
							for (int kPlatform = 0; kPlatform < (int)ctx->metabase->supportedPlatforms.size(); ++kPlatform)
							{
								const char* platformName = ctx->metabase->supportedPlatforms[kPlatform].c_str();
								mbFlattenTargetForWriter(&flatConfig, target, platformName, configName);
							}
							
							lua_createtable(l, 0, 0);
							mbWriterWriteConfigTable(l, flatConfig);
							lua_rawseti(l, -2, jConfig+1);
						}
					}
					lua_setfield(l, -2, "configs");
				}

				//Files
				lua_createtable(l, 0, 0);
				{
					std::set<std::string> uniqueFiles;
					for (int jPlatform = 0; jPlatform < (int)ctx->metabase->supportedPlatforms.size(); ++jPlatform)
					{
						StringVector files;
						target->GetPlatformFiles(&files, ctx->metabase->supportedPlatforms[jPlatform].c_str());
						for (int jFile = 0; jFile < (int)files.size(); ++jFile)
						{
							uniqueFiles.insert(files[jFile]);
						}
					}
										
					//Write out a flat list of unique files to build.
					int jFile = 0;
					for (std::set<std::string>::iterator it = uniqueFiles.begin(); it != uniqueFiles.end(); ++it, ++jFile)
					{
						const char* str = it->c_str();
						lua_pushstring(l, str);
						lua_rawseti(l, -2, jFile+1);
					}
				}
				lua_setfield(l, -2, "files");

				//Frameworks
				std::set<std::string> uniqueFrameworks;
				for (int jPlatform = 0; jPlatform < (int)ctx->metabase->supportedPlatforms.size(); ++jPlatform)
				{
					StringVector frameworks;
					target->GetPlatformFrameworks(&frameworks, ctx->metabase->supportedPlatforms[jPlatform].c_str());
					for (int jFile = 0; jFile < (int)frameworks.size(); ++jFile)
					{
						uniqueFrameworks.insert(frameworks[jFile]);
					}
				}
				lua_createtable(l, 0, 0);
				{
					int jFile = 0;
					for (std::set<std::string>::iterator it = uniqueFrameworks.begin(); it != uniqueFrameworks.end(); ++it, ++jFile)
					{
						const char* str = it->c_str();
						lua_pushstring(l, str);
						lua_rawseti(l, -2, jFile+1);
					}
				}
				lua_setfield(l, -2, "frameworks");
				
				//Resources
				{
					StringVector uniqueResources;
					{
						for (int jPlatform = 0; jPlatform < (int)ctx->metabase->supportedPlatforms.size(); ++jPlatform)
						{
							target->GetPlatformResources(&uniqueResources, ctx->metabase->supportedPlatforms[jPlatform].c_str());
						}
						mbRemoveDuplicatesAndSort(&uniqueResources);
					}
					lua_createtable(l, 0, 0);
					{
						for (int jFile = 0; jFile < (int)uniqueResources.size(); ++jFile)
						{
							const char* str = uniqueResources[jFile].c_str();
							lua_pushstring(l, str);
							lua_rawseti(l, -2, jFile+1);
						}
					}
					lua_setfield(l, -2, "resources");
				}
				
				//Depends
				lua_createtable(l, 0, 0);
				{
					for (int jConfig = 0; jConfig < (int)target->depends.size(); ++jConfig)
					{
						const TargetDepends& depends = target->depends[jConfig];
						std::string dependsFilePath = mbPathGetDir(depends.libMakefile);
						char buf[4096];
						sprintf(buf, "%s/%s", dependsFilePath.c_str(), depends.libTargetName.c_str());
						lua_pushstring(l,  buf);
						lua_rawseti(l, -2, jConfig+1);
					}
				}
				lua_setfield(l, -2, "depends");
				
			}
			lua_rawseti(l, -2, iTarget+1);
		}
		lua_setfield(l, -2, "targets");

		lua_setglobal(l, "writer_solution");
	}

	if (metabase->writerLua.length() == 0)
    {
        MB_LOGERROR("no writer specified.");
        mbExitError();
    }
	
    mbLuaDoFile(l, metabase->writerLua, luaRegisterWriterFuncs);
    
    lua_close(l);
	
	mbPopActiveContext();
}
