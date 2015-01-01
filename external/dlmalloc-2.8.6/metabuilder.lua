solution "dlmalloc"

	target "dlmalloc"
		target_type "module"

		includedirs 
		{ 
			"src", 
		}

		files
		{	
			"metabuilder.lua",
			
			"dlmalloc.cpp",
		}

		config "Debug"
		config_end()

		config "Release"
		config_end()

		config "Master"
		config_end()

 	target_end()

 solution_end()
 