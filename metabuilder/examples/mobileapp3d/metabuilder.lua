solution "mobileapp3d"

	target "mobileapp3d"
		target_type "app"

		depends("corelib",	"../corelib/metabuilder.lua")

		includedirs
		{
			"../corelib",
			".",
		}

		files
		{
			"metabuilder.lua",
			"metabuilder_android.lua",
			"metabuilder_ios.lua",

			"app/mainview.cpp",
			"app/app.cpp",
			"app/maths.cpp",
		}

		config "Debug"
			tadp.setproguardenabled(true)
		config_end()

		config "Release"
			tadp.setproguardenabled(true)
		config_end()

		config "Master"
			tadp.setproguardenabled(true)
		config_end()

		import "metabuilder_android.lua"
		import "metabuilder_ios.lua"
		
	target_end()

solution_end()
