platform "Windows"
	option("msvconfiguration", "CharacterSet", "NotSet")
	--4996 'stricmp': The POSIX name for this item is deprecated. TODO - fix this
	option("msvccompile", "DisableSpecificWarnings", "4996")

	files
	{
		"src/platform/windows/platform_windows.cpp",
	}
platform_end()
