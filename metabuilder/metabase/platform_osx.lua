platform "osx_64"
	defines { "EXAMPLE_COMMON_MACRO1", "EXAMPLE_COMMON_MACRO2" }

	config "Debug"
		defines { "CFG_DEBUG" }
	config_end()

	config "Release"
	config_end()

	config "Profile"
	config_end()

	config "Master"
	config_end()

	--an option that is available throughout this platform.
	--platform_setoption("myplatformoption", "helloworld")
platform_end()
