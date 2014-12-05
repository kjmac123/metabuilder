makesetup ""
	--subdir for intermediate files
	intdir "int"
	--subdir for final output files
	outdir "out"	
makesetup_end()

solution "helloworld"

	--Currently metabuilder supports only a single target per solution
	target "helloworld"
		--this is an application (as opposed to a module for example)
		target_type "app"

		--Additional makefiles for libraries we need to build and link against
		depends("hellolib",	"../hellolib/metabuilder.lua")

		includedirs 
		{ 
			"../hellolib",
		}

		--This is the list of files to build.
		--Note that for each source file added metabuilder will automatically
		--add any .h or .inl files found with a matching name
		files
		{
			"main.cpp"
		}

		--A current metabuilder quirk is that any build configurations you
		--wish to have must be at least stubbed out like this in all your
		--make files.
		--This is because there's no concept of a mapping table to use if
		--one of your libraries has a 'missing' build configuration.
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
