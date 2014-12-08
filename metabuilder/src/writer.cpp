#include "metabuilder_pch.h"

#include "common.h"
#include "platform/platform.h"

#include "writer_xcode.h"

#include <set>

static std::vector<KeyValue>	g_registeredTargets;

static int luaFuncGlobalImport(lua_State* l)
{
    std::string requireFile;
	mbLuaToStringExpandMacros(&requireFile, l, 1);

    mbLuaDoFile(l, requireFile, NULL);
    return 0;
}

static int luaSplit (lua_State* l) 
{
  const char *s = luaL_checkstring(l, 1);
  const char *sep = luaL_checkstring(l, 2);
  const char *e;
  int i = 1;

  lua_newtable(l); 

  //for each separator
  while ((e = strchr(s, *sep)) != NULL) 
  {
		lua_pushlstring(l, s, e-s);  //push substring
		lua_rawseti(l, -2, i++);
		s = e + 1;  //skip separator
  }

  //push last substring
  lua_pushstring(l, s);
  lua_rawseti(l, -2, i);
  return 1;
}

static int luaFuncMkdir(lua_State* l)
{
    std::string path;
	mbLuaToStringExpandMacros(&path, l, 1);
	
	if (!mbCreateDirChain(path.c_str()))
	{
		mbExitError();
	}
	
	return 0;
}

static int luaFuncMklink(lua_State* l)
{
    std::string src, dst;
	if (!mbLuaToStringExpandMacros(&src, l, 1) || !mbLuaToStringExpandMacros(&dst, l, 2))
	{
		MB_LOGERROR("Must specify both source and destination when creating link");
		mbExitError();
	}
		
	if (!mbaCreateLink(src.c_str(), dst.c_str()))
	{
		MB_LOGERROR("Failed to create link %s->%s", src.c_str(), dst.c_str());
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

static int luaFuncReportOutputFile(lua_State* l)
{
    std::string filepath;
	mbLuaToStringExpandMacros(&filepath, l, 1);
	MB_LOGINFO("Wrote file %s", filepath.c_str());
	
	return 0;
}

static int luaFuncFatalError(lua_State* l)
{
    const char* str = lua_tostring(l, 1);
	if (str)
	{
		MB_LOGERROR("%s", str);
	}
	mbExitError();
	return 0;
}

#if 0
static int luaFuncGetRelativeDirTo(lua_State* l)
{
    const char* from = mbLuaToStringExpandMacros(l, 1);
    const char* to = mbLuaToStringExpandMacros(l, 2);

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
	std::string fromFilename, toFilename;
	
	if (!mbLuaToStringExpandMacros(&fromFilename, l, 1) || !mbLuaToStringExpandMacros(&toFilename, l, 2))
	{
		MB_LOGERROR("Failed to copy file. Insufficient args");
		mbExitError();
		return 0;
	}

	FILE* fromFile = fopen(fromFilename.c_str(), "rb");
	if (!fromFile)
	{
        MB_LOGERROR("cannot open file %s", fromFilename.c_str());
        mbExitError();
	}
	
	FILE* toFile = fopen(toFilename.c_str(), "wb");
	if (!toFile)
	{
        MB_LOGERROR("cannot open file %s", toFilename.c_str());
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
			MB_LOGERROR("Failed to read from file %s", fromFilename.c_str());
			mbExitError();
		}
		
		long bytesWritten = fwrite(buf, 1, actualBytesRead, toFile);
		if (bytesWritten != bytesThisTime)
		{
			fclose(toFile);
			fclose(fromFile);
			MB_LOGERROR("Failed to copy to file %s", toFilename.c_str());
			mbExitError();
		}
		
		bytesToCopy -= bytesThisTime;
	}
	
	fclose(toFile);
	fclose(fromFile);

	return 0;
}

static int luaFuncWriterRegisterTarget(lua_State* l)
{
	g_registeredTargets.push_back(KeyValue());
	mbLuaToStringExpandMacros(&g_registeredTargets.back().key, l, 1);	// target name;
	mbLuaToStringExpandMacros(&g_registeredTargets.back().value, l, 2);	// target filepath;
	MB_LOGINFO("Registered target - name: %s location: %s", g_registeredTargets.back().key.c_str(), g_registeredTargets.back().value.c_str());
	return 0;
}

static int luaFuncWriterGetTarget(lua_State* l)
{
    std::string target;
	mbLuaToStringExpandMacros(&target, l, 1);
	
	for (int i = 0; i < (int)g_registeredTargets.size(); ++i)
	{
		if (g_registeredTargets[i].key == target)
		{
			lua_pushstring(l, g_registeredTargets[i].value.c_str());
			return 1;
		}
	}

	MB_LOGERROR("Failed to find target %s", target.c_str());
	mbExitError();
	return 0;
}

void luaRegisterWriterFuncs(lua_State* l)
{
	mbWriterXcodeLuaRegister(l);
	mbWriterMSVCLuaRegister(l);

    lua_pushcfunction(l, luaFuncGlobalImport);
    lua_setglobal(l, "import");
	
    lua_pushcfunction(l, luaSplit);
    lua_setglobal(l, "split");

	lua_pushcfunction(l, luaFuncAddMacro);
	lua_setglobal(l, "globalmacro");

	lua_pushcfunction(l, luaFuncExpandMacro);
	lua_setglobal(l, "expandmacro");
	
    lua_pushcfunction(l, luaFuncMkdir);
    lua_setglobal(l, "mkdir");
	
    lua_pushcfunction(l, luaFuncMklink);
    lua_setglobal(l, "mklink");

    lua_pushcfunction(l, luaFuncGetFileType);
    lua_setglobal(l, "getfiletype");
	
    lua_pushcfunction(l, luaFuncCopyFile);
    lua_setglobal(l, "copyfile");

    lua_pushcfunction(l, luaFuncReportOutputFile);
    lua_setglobal(l, "reportoutputfile");

    lua_pushcfunction(l, luaFuncFatalError);
    lua_setglobal(l, "mbwriter_fatalerror");

    lua_pushcfunction(l, luaFuncWriterRegisterTarget);
    lua_setglobal(l, "mbwriter_registertarget");

    lua_pushcfunction(l, luaFuncWriterGetTarget);
    lua_setglobal(l, "mbwriter_gettarget");
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
			const KeyValueMap& keyValueMap = optionGroupIt->second;

			lua_createtable(l, 0, 0);
			{
				int jOption = 0;
				for (KeyValueMap::const_iterator optionIt = keyValueMap.begin(); optionIt != keyValueMap.end(); ++optionIt, ++jOption)
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

		lua_pushboolean(l, ctx->isMainMakefile);
		lua_setfield(l, -2, "ismainmakefile");
		
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
				
				{
					StringVector allFiles;
					
					target->FlattenFiles(&allFiles, NULL);
					mbRemoveDuplicatesAndSort(&allFiles);
						
					lua_createtable(l, 0, 0);
					{
						for (int jFile = 0; jFile < (int)allFiles.size(); ++jFile)
						{
							const std::string& filepath = allFiles[jFile];
//							MB_LOGINFO(filepath.c_str());
							
							lua_pushstring(l, filepath.c_str());
							lua_rawseti(l, -2, jFile+1);
						}
					}
					lua_setfield(l, -2, "allsourcefiles");
					
					target->FlattenResources(&allFiles, NULL);
					target->FlattenFrameworks(&allFiles, NULL);
					mbRemoveDuplicatesAndSort(&allFiles);
					
					lua_createtable(l, 0, 0);
					{
						for (int jFile = 0; jFile < (int)allFiles.size(); ++jFile)
						{
							const std::string& filepath = allFiles[jFile];
//							MB_LOGINFO(filepath.c_str());
							
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
								target->Flatten(&flatConfig, platformName, configName);
							}
							
							lua_createtable(l, 0, 0);
							mbWriterWriteConfigTable(l, flatConfig);
							lua_rawseti(l, -2, jConfig+1);
						}
					}
					lua_setfield(l, -2, "configs");
				}
				
				//Files
				{
					StringVector uniqueFiles;
					{
						for (int jPlatform = 0; jPlatform < (int)ctx->metabase->supportedPlatforms.size(); ++jPlatform)
						{
							const char* platformName = ctx->metabase->supportedPlatforms[jPlatform].c_str();
							target->FlattenFiles(&uniqueFiles, platformName);
						}
						mbRemoveDuplicatesAndSort(&uniqueFiles);
					}
					lua_createtable(l, 0, 0);
					{
						for (int jFile = 0; jFile < (int)uniqueFiles.size(); ++jFile)
						{
							const char* str = uniqueFiles[jFile].c_str();
							lua_pushstring(l, str);
							lua_rawseti(l, -2, jFile+1);
						}
					}
					lua_setfield(l, -2, "files");
				}

				//No-pch Files
				{
					StringVector uniqueFiles;
					{
						for (int jPlatform = 0; jPlatform < (int)ctx->metabase->supportedPlatforms.size(); ++jPlatform)
						{
							const char* platformName = ctx->metabase->supportedPlatforms[jPlatform].c_str();
							target->FlattenNoPchFiles(&uniqueFiles, platformName);
						}
						mbRemoveDuplicatesAndSort(&uniqueFiles);
					}
					lua_createtable(l, 0, 0);
					{
						for (int jFile = 0; jFile < (int)uniqueFiles.size(); ++jFile)
						{
							const char* str = uniqueFiles[jFile].c_str();
							lua_pushstring(l, str);
							lua_rawseti(l, -2, jFile+1);
						}
					}
					lua_setfield(l, -2, "nopchfiles");
				}


				//Frameworks
				{
					StringVector uniqueFrameworks;
					{
						for (int jPlatform = 0; jPlatform < (int)ctx->metabase->supportedPlatforms.size(); ++jPlatform)
						{
							const char* platformName = ctx->metabase->supportedPlatforms[jPlatform].c_str();
							target->FlattenFrameworks(&uniqueFrameworks, platformName);
						}
						mbRemoveDuplicatesAndSort(&uniqueFrameworks);
					}
					lua_createtable(l, 0, 0);
					{
						for (int jFile = 0; jFile < (int)uniqueFrameworks.size(); ++jFile)
						{
							const char* str = uniqueFrameworks[jFile].c_str();
							lua_pushstring(l, str);
							lua_rawseti(l, -2, jFile+1);
						}
					}
					lua_setfield(l, -2, "frameworks");
				}
				
				//Resources
				{
					StringVector uniqueResources;
					{
						for (int jPlatform = 0; jPlatform < (int)ctx->metabase->supportedPlatforms.size(); ++jPlatform)
						{
							const char* platformName = ctx->metabase->supportedPlatforms[jPlatform].c_str();
							target->FlattenResources(&uniqueResources, platformName);
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
				
				//Target options
				{
					FlatConfig flatTarget;

					for (int jPlatform = 0; jPlatform < (int)ctx->metabase->supportedPlatforms.size(); ++jPlatform)
					{
						const char* platformName = ctx->metabase->supportedPlatforms[jPlatform].c_str();
						target->Flatten(&flatTarget, platformName, NULL);
					}
					mbWriterSetOptions(l, flatTarget.options);
				}
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
