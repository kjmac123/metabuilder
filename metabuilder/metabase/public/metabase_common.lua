defines 
{
	"METABUILDER",	-- Used in cases where we need to know if we are building with the metabuilder system
	"LUA_ANSI",		-- We'll declare this here given how widely we use Lua
}

config "Debug"
	defines 
	{ 
		"DEBUG=1",
--		"CFG_DEBUG",
		"BUILD_CONFIG_DEBUG",
		"DV_DEBUGVAR_ENABLED",
	}
config_end()

config "Release"
	defines 
	{ 
		"BUILD_CONFIG_RELEASE",
		"DV_DEBUGVAR_ENABLED"
	}
config_end()

config "Master"
	defines 
	{ 
		"BUILD_CONFIG_MASTER"
	}
config_end()
