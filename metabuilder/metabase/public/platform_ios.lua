platform "iOS"

defines { 
	"PLATFORM_IOS", 
	"EXAMPLE_COMMON_MACRO1", 
	"EXAMPLE_COMMON_MACRO2" }

config "Debug"
	defines { 
		"DEBUG=1",
		"CFG_DEBUG" 
	}
config_end()

config "Release"
	defines { 
		"EXAMPLE_RELEASE_OPTION" 
	}
config_end()

config "Master"
	defines { 
		"EXAMPLE_MASTER_OPTION" 
	}
config_end()
