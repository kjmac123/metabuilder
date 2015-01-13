makesetup ""
	--subdir for intermediate files
--	intdir "int"
	--subdir for final output files
--	outdir "out"	
makesetup_end()

solution "mobileapp"

	target "mobileapp"
		target_type "app"
		
		depends("hellolib",	"../hellolib/metabuilder.lua")
		
		includedirs 
		{ 
			"../hellolib",
			".",
		}
		
		files
		{
			"metabuilder.lua",
			"metabuilder_android.lua",
			"metabuilder_ios.lua",
			"src/common.cpp",
		}

		config "Debug"
		config_end()

		config "Release"
		config_end()
			
		import "metabuilder_android.lua"
		import "metabuilder_ios.lua"

	target_end()

solution_end()
