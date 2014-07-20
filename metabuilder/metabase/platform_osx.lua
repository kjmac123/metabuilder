supportedplatforms
{
	"OSX",
	"POSIX",
}

defines 
{ 
	"PLATFORM_IOS", 
	"PLATFORM_POSIX", 
	"DVPLATFORM_OSX", 
}

config "Debug"
	defines 
	{ 
		"DEBUG=1",
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
