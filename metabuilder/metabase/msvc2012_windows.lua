metabase "msvc2012_windows"
	option("msvc", "version", "2012")
	option("msvconfiguration", "PlatformToolset", "v110")
	import "msvc_common_windows.lua"
	import "msvc_common_windows_x86.lua"
metabase_end()
