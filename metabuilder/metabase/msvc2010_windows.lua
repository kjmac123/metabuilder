metabase "msvc2010_windows"
	option("msvc", "version", "2010")
	option("msvconfiguration", "PlatformToolset", "v100")
	import "msvc_common_windows.lua"
	import "msvc_common_windows_x86.lua"
metabase_end()
