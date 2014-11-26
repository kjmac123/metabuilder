makesetup ""
	intdir "int"
	outdir "out"	
makesetup_end()

solution "helloworld"

	target "helloworld"
		target_type "app"

		--Additional makefiles for libraries we need to build and link against
		
		depends("hellolib",	"../hellolib/metabuilder.lua")

		includedirs 
		{ 
			"../hellolib",
		}

		files
		{
			"main.cpp"
		}

		config "Debug"
		config_end()

		config "Release"
		config_end()

		config "Profile"
		config_end()

		config "Master"
		config_end()
		
 	target_end()

solution_end()
