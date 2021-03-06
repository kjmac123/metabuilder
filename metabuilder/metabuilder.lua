solution "metabuilder"

	import "solution_windows.lua"

	target "metabuilder"
		target_type "app"
		target_subsystem "console"
		
		--build against Lua library
		depends("lua",	"../external/lua-5.2.2/metabuilder.lua")

		defines
		{
		}
		
		includedirs 
		{ 
			"src",
			"../external/dlmalloc-2.8.6",
			"../external/ezOptionParser-0.2.1",
			"../external/lua-5.2.2/src",
			"../external",
		}

		files
		{
			"src/platform/platform.cpp",
			"src/block.cpp",
			"src/common.cpp",
			"src/configparam.cpp",
			"src/core.cpp",
			"src/corestring.cpp",
			"src/filepath.cpp",
			"src/luafile.cpp",
			"src/main.cpp",
			"src/makeglobal.cpp",
			"src/mbstring.cpp",
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
			"metabuilder.lua",
			"metabuilder_posix.lua",
			"metabuilder_windows.lua",
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
		
		import "metabuilder_posix.lua"
		import "metabuilder_windows.lua"
	
 	target_end()
	
 solution_end()
