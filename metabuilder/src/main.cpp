#include "metabuilder_pch.h"

#include "makeglobal.h"
#include "writer.h"
#include "solution.h"
#include "configparam.h"
#include "metabase.h"
#include "platformparam.h"

#include "ezOptionParser.hpp"
#include "corestring.h"

//TODO
//	-	Resolve issues with adding / overriding members of lists. Need to store additional metadata with macros, indicating whether
//		they were to replace or append when we come around to evaluating them

//-----------------------------------------------------------------------------------------------------------------------------------------


static void ParseArgs(CmdSetup* appOptions, int argc, const char* argv[])
{
	ez::ezOptionParser opt;
    
	opt.overview = "Metabuilder";
	opt.syntax = "metabuilder [OPTIONS]";
	opt.example = "metabuilder --input <makefile> --outputdir <dir> --metabase <dir>\n";
	opt.footer = "";
	
	opt.add(
            "", // Default.
            0, // Required?
            0, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Display usage instructions\n", // Help description.
            "-h",     // Flag token.
            "-help",  // Flag token.
            "--help", // Flag token.
            "--usage" // Flag token.
            );
	
	opt.add(
            "", // Default.
            0, // Required?
            0, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Display usage instructions\n", // Help description.
            "-v",     // Flag token.
            "-verbose",  // Flag token.
            "--verbose" // Flag token.
            );
				
	opt.add(
            "", // Default.
            true, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Generator", // Help description.
            "--gen"
            );
    
	opt.add(
            "", // Default.
            true, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Metabase", // Help description.
            "--metabase"
            );
    
	opt.add(
            "", // Default.
            true, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Makefile", // Help description.
            "--input"     // Flag token.
            );
			
	opt.add(
            "", // Default.
            true, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Output location for written projects", // Help description.
            "--outdir"     // Flag token.
            );
			
	opt.add(
            "", // Default.
            false, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Line endings style to use", // Help description.
            "--endstyle"     // Flag token.
            );
	
	opt.parse(argc, argv);
	std::string usage;
	std::vector<std::string> badOptions;

	if(!opt.gotExpected(badOptions)) {
		for (size_t i = 0; i < badOptions.size(); ++i)
		{
			MB_LOGERROR("ERROR: Got unexpected number of arguments for option %s\n", badOptions[i].c_str());
		}

		opt.getUsage(usage);
		MB_LOGERROR("%s", usage.c_str());
		mbExitError();
	}
	
	std::vector<std::string> requiredOptions;
	if(!opt.gotRequired(requiredOptions)) 
	{
		for(size_t i=0; i < requiredOptions.size(); ++i)
		{
			MB_LOGERROR("ERROR: required option missing: %s", requiredOptions[i].c_str());
		}

		mbExitError();
	}

	if (opt.isSet("-h")) 
	{
		opt.getUsage(usage);
		std::cout << usage;
		mbExitError();
	}
	
	opt.get("--input")->getString(appOptions->_inputFile);
	opt.get("--gen")->getString(appOptions->_generator);
	opt.get("--metabase")->getString(appOptions->_metabaseDir);
	opt.get("--outdir")->getString(appOptions->_makeOutputTopDir);
	opt.get("--endstyle")->getString(appOptions->lineEndingStyle);
	appOptions->verbose = opt.isSet("-v") != 0;
}

int main(int argc, const char * argv[])
{
    mbCore_Init();

	MB_LOGINFO("Metabuilder");

	MB_LOGSETTIMEENABLED(true);

	AppState* appState = mbGetAppState();
	appState->makeGlobal = new MakeGlobal();

	ParseArgs(&appState->cmdSetup, argc, (const char**)argv);

	//---------- GLOBAL SCOPE FUNCTIONS ----------
	   
	// build context objects
    std::string inputFileAbs = Platform::FileGetAbsPath(appState->cmdSetup._inputFile);
		
	mbAddMakeFile(inputFileAbs.c_str());
	
	// We'll add to this array during iteration
	const StringVector& makeFiles = mbGetMakeFiles();
	mbPushDir("");
			
	appState->isProcessingPrimaryMakefile = true;
	for (size_t i = 0; i < makeFiles.size(); ++i)
	{
		MetaBuilderContext* ctx = mbCreateContext();
		mbPushActiveContext(ctx);		
		{
			{
				std::string makedir = mbPathGetDir(makeFiles[i]);
				ctx->currentMetaMakeDirAbs = Platform::FileGetAbsPath(makedir);
				mbNormaliseFilePath(&ctx->currentMetaMakeDirAbs, mbGetAppState()->makeGlobal->GetTargetDirSep());
			}
		
			lua_State *l;
			l = lua_newstate(mbLuaAllocator, NULL);
			luaL_checkstack(l, MB_LUA_STACK_MAX, "Out of stack!");
			luaL_openlibs(l);

			LuaModuleFunctions luaGlobalFunctions;

//			mbMakeSetupLuaRegister(l);
			mbMakeGlobalLuaRegister(l);
			mbCommonLuaRegister(l, &luaGlobalFunctions);
			mbBlockLuaRegister(l);
			mbMetabaseLuaRegister(l);
			mbSolutionLuaRegister(l);
			mbTargetLuaRegister(l);
			mbConfigParamLuaRegister(l);
			mbPlatformParamLuaRegister(l);

			luaGlobalFunctions.RegisterLuaGlobal(l);
			
			if (!mbGetAppState()->isProcessingPrimaryMakefile)
			{
				std::string metabase = appState->cmdSetup._generator + ".lua";
				//Process metabase
				mbLuaDoFile(l, metabase.c_str(), NULL);
			}
			mbPushDir(ctx->currentMetaMakeDirAbs);
			//Process makefile
			mbLuaDoFile(l, makeFiles[i], NULL);
			mbPopDir();
			
			lua_close(l);
		}
		mbPopActiveContext();
		
		ctx->isMainMakefile = appState->isProcessingPrimaryMakefile;
		appState->isProcessingPrimaryMakefile = false;
	}
	
	const std::list<MetaBuilderContext*>& contexts = mbGetContexts();
	for (std::list<MetaBuilderContext*>::const_reverse_iterator it = contexts.rbegin(); it != contexts.rend(); ++it)
	{
		MetaBuilderContext* ctx = *it;
		mbWriterDo(ctx);
    }

	for (std::list<MetaBuilderContext*>::const_iterator it = contexts.begin(); it != contexts.end(); it = contexts.begin())
	{
		MetaBuilderContext* ctx = *it;
		mbDestroyContext(ctx);
	}
	
	mbPopDir();
	
    MB_LOGINFO("Project generation complete.");
    mbCore_Shutdown();
    return 0;
}
























