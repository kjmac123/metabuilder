metabase "msvc2015_windows"
	option("msvc", "version", "2015")
	option("msvconfiguration", "PlatformToolset", "v140")
	import "msvc_common_windows.lua"
	import "msvc_common_windows_x86.lua"
metabase_end()
