supportedplatforms
{
	"GLES",	
	"iOS",
	"POSIX",
}

defines 
{ 
	"PLATFORM_GLES",
	"PLATFORM_IOS", 
	"PLATFORM_POSIX", 
}

config "Debug"
	defines 
	{ 
		"DEBUG=1",
		"CFG_DEBUG" 
	}
config_end()

config "Release"
	defines 
	{ 
	}
config_end()

config "Profile"
	defines 
	{ 
	}
config_end()

config "Master"
	defines 
	{ 
	}
config_end()
