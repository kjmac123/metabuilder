metabase "ninja_linux"
	import "metabase_common.lua"
	import "platform_linux.lua"

	writer "writer_ninja.lua"

	option("cflags", "-Wall")
	option("arflags", "crsT")
--	option("ldflags", "-fno-exceptions")

	config "Debug"
		option("cflags", "-O0")
	config_end()

	config "Release"
		option("cflags", "-O2")
	config_end()

	config "Master"
		option("cflags", "-O2")
	config_end()


	config "Debug"
	config_end()

	config "Release"
	config_end()

	config "Master"
	config_end()
metabase_end()
