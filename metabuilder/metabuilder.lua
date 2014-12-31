--setup output directories
makesetup ""
	intdir "int"
	outdir "out"	
makesetup_end()

solution "metabuilder"

	target "metabuilder"
		target_type "app"
		target_subsystem "console"
		
		--build against Lua library
		depends("dlmalloc",	"../external/dlmalloc-2.8.6/metabuilder.lua")
		depends("lua",	"../external/lua-5.2.2/metabuilder.lua")

		defines
		{
		}
		
		includedirs 
		{ 
			"src",
			"../external/ezOptionParser-0.2.1",
			"../external/dlmalloc-2.8.6",
			"../external/lua-5.2.2/src",
		}

		files
		{
			"src/platform/platform.cpp",
			"src/block.cpp",
			"src/common.cpp",
			"src/configparam.cpp",
			"src/core.cpp",
			"src/main.cpp",
			"src/makeglobal.cpp",
			"src/makesetup.cpp",
			"src/metabase.cpp",
			"src/metabuilder_pch.cpp",
			"src/platformparam.cpp",
			"src/solution.cpp",
			"src/target.cpp",
			"src/timeutil.cpp",
			"src/writer.cpp",
			"src/writer_msvc.cpp",
			"src/writer_utility.cpp",
			"src/writer_xcode.cpp",
		}

		--build configurations
		config "Debug"
			defines 
			{ 
			}
		config_end()

		config "Release"
			defines 
			{ 
			}
		config_end()
		
		if checkplatform("Windows") == true then
			pch "metabuilder_pch"
		end

		import "metabuilder_posix.lua"
		import "metabuilder_windows.lua"
		
 	target_end()

 solution_end()
