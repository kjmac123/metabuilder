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
	"DVPLATFORM_IOS", 
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

config "Master"
	defines 
	{ 
	}
config_end()
