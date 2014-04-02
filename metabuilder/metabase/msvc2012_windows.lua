metabase "msvc2012_windows"
	option("msvc", "version", "2012")
	option("msvcgeneral", "PlatformToolset", "v110")
	option("msvclink", "AdditionalOptions", "/SAFESEH:NO %(AdditionalOptions)")
	import "msvc_common_windows.lua"
metabase_end()
