supportedplatforms 
{
	"NT",
	"Windows",
}

defines 
{ 
	"PLATFORM_WINDOWS", 
	"PLATFORM_PC", --legacy
	"DVPLATFORM_WINDOWS",
	"WIN32",
	"_CRT_SECURE_NO_WARNINGS",
	"_WINDOWS",
}

exedirs
{
	"!$(DXSDK_DIR)Utilities\\bin\\x86",
}

includedirs
{
	"!$(DXSDK_DIR)Include",
}

libdirs
{
	"!$(DXSDK_DIR)Lib\\x86",
}

config "Debug"
	defines 
	{
	}

config_end()

config "Release"
	defines 
	{
		"NDEBUG",
	}
config_end()

config "Master"
	defines 
	{
		"NDEBUG",
	}
config_end()
