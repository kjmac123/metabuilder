solution "hellolib"

	target "hellolib"
		target_type "staticlib"

		includedirs 
		{ 
			".", 
		}

		files
		{
			"mylib.cpp"
		}

		config "Debug"
		config_end()

		config "Release"
		config_end()

		config "Profile"
		config_end()

		config "MasterSymbols"
		config_end()

		config "Master"
		config_end()

 	target_end()

solution_end()
