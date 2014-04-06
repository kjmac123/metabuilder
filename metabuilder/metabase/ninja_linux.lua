metabase "ninja_linux"
	import "metabase_common.lua"
	import "platform_linux.lua"

	writer "writer_ninja.lua"	

	config "Debug"
	config_end()

	config "Release"
	config_end()

	config "Master"
	config_end()
metabase_end()
