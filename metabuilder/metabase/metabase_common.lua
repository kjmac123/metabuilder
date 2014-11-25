defines 
{
	"METABUILDER",	-- Used in cases where we need to know if we are building with the metabuilder system
}

config "Debug"
	defines 
	{ 
		"DEBUG=1",
		"BUILD_CONFIG_DEBUG",
	}
config_end()

config "Release"
	defines 
	{ 
		"BUILD_CONFIG_RELEASE",
	}
config_end()

config "Profile"
	defines 
	{ 
		"BUILD_CONFIG_PROFILE",
	}
config_end()

config "Master"
	defines 
	{ 
		"BUILD_CONFIG_MASTER"
	}
config_end()
