metabase "msvc2013_windows"
	option("msvc", "version", "2013")
	option("msvconfiguration", "PlatformToolset", "v120")
	import "msvc_common_windows.lua"
	import "msvc_common_windows_x86.lua"
metabase_end()
