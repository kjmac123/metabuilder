makesetup ""
	intdir "int"
	outdir "out"	
makesetup_end()

solution "metabuilder"

	target "metabuilder"
		target_type "app"

		depends("lua",	"../external/lua-5.2.2/metabuilder.lua")

		defines
		{
			--"LUA_AS_CPP",
		}
		
		includedirs 
		{ 
			"src",
			"../external/lua-5.2.2/src",
			"../external/ezOptionParser-0.2.1",
		}

		files
		{
			"src/platformparam.cpp",
			"src/writer.cpp",
			"src/main.cpp",
			"src/makesetup.cpp",
			"src/block.cpp",
			"src/writer_msvc.cpp",
			"src/configparam.cpp",
			"src/solution.cpp",
			"src/metabase.cpp",
			"src/platform/platform.cpp",
			"src/writer_xcode.cpp",
			"src/core.cpp",
			"src/common.cpp",
			"src/platform/POSIX/platform_posix.cpp",
			"src/target.cpp",
		}

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

		config "Master"
			defines 
			{ 
				--Add custom defines here
			}
		config_end()
		
 	target_end()

 solution_end()
