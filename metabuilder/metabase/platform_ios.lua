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

--------------------------------------------------------------------------------

xcode = {}

function xcode.setinfoplist(plistFile)
	option("_xcode", "INFOPLIST_FILE", plistFile)
end

function xcode.setentitlements(entitlementsFile)
	option("_xcode", "CODE_SIGN_ENTITLEMENTS", entitlementsFile)
end

function xcode.addcapability(capability)
	addoption("_xcode", "_capabilities", capability)
end
