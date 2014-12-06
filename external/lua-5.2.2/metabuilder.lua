solution "lua"

	target "lua"
		--target_type "staticlib"
		target_type "module"

		--pch "luapch"

		includedirs 
		{ 
			"src", 
		}

		files
		{	
			"metabuilder.lua",
			
			"src/lapi.c",
			"src/lauxlib.c",
			"src/lbaselib.c",
			"src/lbitlib.c",
			"src/lcode.c",
			"src/lcorolib.c",
			"src/lctype.c",
			"src/ldblib.c",
			"src/ldebug.c",
			"src/ldo.c",
			"src/ldump.c",
			"src/lfunc.c",
			"src/lgc.c",
			"src/linit.c",
			"src/liolib.c",
			"src/llex.c",
			"src/lmathlib.c",
			"src/lmem.c",
			"src/loadlib.c",
			"src/lobject.c",
			"src/lopcodes.c",
			"src/loslib.c",
			"src/lparser.c",
			"src/lstate.c",
			"src/lstring.c",
			"src/lstrlib.c",
			"src/ltable.c",
			"src/ltablib.c",
			"src/ltm.c",
			"src/lundump.c",
			"src/lvm.c",
			"src/lzio.c",
		}

		config "Debug"
		config_end()

		config "Release"
		config_end()

		config "Master"
		config_end()

 	target_end()

 solution_end()
 