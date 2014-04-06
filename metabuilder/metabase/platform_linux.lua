supportedplatforms 
{
	"Linux",
	"POSIX",
}

platform "linux"
	defines { "EXAMPLE_COMMON_MACRO1", "EXAMPLE_COMMON_MACRO2" }

	config "Debug"
		defines { "CFG_DEBUG" }
	config_end()

	config "Release"
		defines { "EXAMPLE_RELEASE_OPTION" }
	config_end()

	config "Master"
		defines { "EXAMPLE_MASTER_OPTION" }
	config_end()

	--an option that is available throughout this platform.
	--platform_setoption("myplatformoption", "helloworld")
platform_end()
