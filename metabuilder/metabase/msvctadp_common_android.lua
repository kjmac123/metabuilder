import "metabase_common.lua"
import "platform_android.lua"

supportedplatforms
{
	"TADP",
}

writer "writer_msvc.lua"

option("msvc", "customwriter", "/writer_msvctadp_android.lua")
option("msvc", "platform", "Tegra-Android")
option("msvccompile", "MultiProcessorCompilation", "true")

option("msvccompile", "CppLanguageStandard", "gnu++11")
config "Debug"
config_end()

config "Release"
config_end()

config "Profile"
config_end()

config "Master"
config_end()

config "MasterSymbols"
config_end()

tadp = {}

function tadp.setminapi(minapi)
	option("msvconfiguration", "AndroidMinAPI", minapi)
end

function tadp.settargetapi(targetapi)
	option("msvconfiguration", "AndroidTargetAPI", targetapi)
end

function tadp.setproguardenabled(value)
	local tmp = nil
	if value then
		tmp = "true"
	else
		tmp = "false"
	end
	option("_android", "ProguardEnabled", tmp)
end

function tadp.setdebuggable(value)
	local tmp = nil
	if value then
		tmp = "true"
	else
		tmp = "false"
	end
	option("_android", "Debuggable", tmp)
end
