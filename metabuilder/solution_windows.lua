platform "Windows"
	option("msvconfiguration", "CharacterSet", "NotSet")

	config "Debug"
		option("msvccompile", "RuntimeLibrary", "MultiThreadedDebug")
	config_end()

	config "Release"
		option("msvccompile", "RuntimeLibrary", "MultiThreaded")
	config_end()

platform_end()
