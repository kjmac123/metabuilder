platform "Windows"
	option("msvconfiguration", "CharacterSet", "NotSet")
	--4996 'stricmp': The POSIX name for this item is deprecated. TODO - fix this
	option("msvccompile", "DisableSpecificWarnings", "4996")

	defines
	{
		MB_DLMALLOC,
	}

	files
	{
		"../external/dlmalloc-2.8.6/dlmalloc.cpp",	

		"src/platform/windows/platform_windows.cpp",
	}
	
config "Debug"
	option("msvccompile", "RuntimeLibrary", "MultiThreadedDebug")
config_end()

config "Release"
	option("msvccompile", "RuntimeLibrary", "MultiThreaded")
config_end()

platform_end()
