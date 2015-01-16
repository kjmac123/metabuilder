import "metabase_common.lua"
import "platform_windows.lua"

writer "writer_msvc.lua"

option("msvc", "customwriter", "writer_msvc_windows.lua")

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

	option("msvclink", "GenerateDebugInformation", "true")
	option("msvclink", "EnableCOMDATFolding", "true")
	option("msvclink", "OptimizeReferences", "true")
config_end()

config "Profile"
	option("msvccompile", "Optimization", "MaxSpeed")
	option("msvccompile", "FunctionLevelLinking", "true")

	option("msvclink", "GenerateDebugInformation", "true")
	option("msvclink", "EnableCOMDATFolding", "true")
	option("msvclink", "OptimizeReferences", "true")
config_end()

config "Master"
	option("msvccompile", "Optimization", "MaxSpeed")
	option("msvccompile", "FunctionLevelLinking", "true")

	option("msvclink", "GenerateDebugInformation", "false")
	option("msvclink", "EnableCOMDATFolding", "true")
	option("msvclink", "OptimizeReferences", "true")
config_end()
