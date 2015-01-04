makeglobal()
	dirsep "\\"
makeglobal_end()

supportedplatforms 
{
	"NT",
	"Windows",
}

defines 
{ 
	"PLATFORM_WINDOWS", 
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

libs 
{
	"kernel32.lib",
	"user32.lib",
	"gdi32.lib",
	"winspool.lib",
	"comdlg32.lib",
	"advapi32.lib",
	"shell32.lib",
	"ole32.lib",
	"oleaut32.lib",
	"uuid.lib",
	"odbc32.lib",
	"odbccp32.lib",
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

config "Profile"
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
