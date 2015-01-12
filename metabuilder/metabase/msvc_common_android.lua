import "metabase_common.lua"
import "platform_android.lua"

writer "writer_msvc.lua"

option("msvc", "customwriter", "/writer_msvc_android.lua")
option("msvc", "platform", "Tegra-Android")
option("msvccompile", "MultiProcessorCompilation", "true")
option("msvccompile", "CppLanguageStandard", "gnu++11")

function androidsettargetapi(targetapi)
	option("msvconfiguration", "AndroidTargetAPI", targetapi)
end

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
