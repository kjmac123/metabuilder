import "metabase_common.lua"
import "platform_windows.lua"

writer "writer_msvc.lua"

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

option("msvc", "customwriter", "writer_msvc_windows.lua")

option("msvc", "platform", "Win32")

option("msvcglobals", "Keyword", "Win32Proj")

option("msvconfiguration", "CharacterSet", "Unicode")

option("msvccompile", "MultiProcessorCompilation", "true")
option("msvccompile", "MinimalRebuild", "false")
option("msvccompile", "WarningLevel", "Level3")
option("msvccompile", "TreatWarningAsError", "false")
option("msvccompile", "IntrinsicFunctions", "true")

config "Debug"
	option("msvccompile", "Optimization", "Disabled")
	option("msvclink", "GenerateDebugInformation", "true")
config_end()

config "Release"
	option("msvccompile", "Optimization", "MaxSpeed")
	option("msvccompile", "FunctionLevelLinking", "true")
	option("msvccompile", "IntrinsicFunctions", "true")

	option("msvclink", "GenerateDebugInformation", "true")
	option("msvclink", "EnableCOMDATFolding", "true")
	option("msvclink", "OptimizeReferences", "true")
config_end()

config "Profile"
	option("msvccompile", "Optimization", "MaxSpeed")
	option("msvccompile", "FunctionLevelLinking", "true")
	option("msvccompile", "IntrinsicFunctions", "true")

	option("msvclink", "GenerateDebugInformation", "true")
	option("msvclink", "EnableCOMDATFolding", "true")
	option("msvclink", "OptimizeReferences", "true")
config_end()

config "Master"
	option("msvccompile", "Optimization", "MaxSpeed")
	option("msvccompile", "FunctionLevelLinking", "true")
	option("msvccompile", "IntrinsicFunctions", "true")

	option("msvclink", "GenerateDebugInformation", "false")
	option("msvclink", "EnableCOMDATFolding", "true")
	option("msvclink", "OptimizeReferences", "true")
config_end()
