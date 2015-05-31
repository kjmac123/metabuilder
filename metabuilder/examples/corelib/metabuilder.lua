solution "corelib"

	target "corelib"
		target_type "module"

		includedirs
		{
			".",
		}

		files
		{
			"metabuilder.lua",
			"metabuilder_android.lua",
			"metabuilder_ios.lua",

			"corelib/platform/platform.cpp",
			"corelib/platform/platformfile.cpp",

			"corelib/core.cpp",
			"corelib/corestring.cpp",
			"corelib/logicalfs.cpp",
			"corelib/timeutil.cpp",

            "corelib/appdelegate.cpp",
        }

		config "Debug"
		config_end()

		config "Release"
		config_end()

		config "Master"
		config_end()

		import "metabuilder_android.lua"
		import "metabuilder_ios.lua"

 	target_end()

solution_end()
