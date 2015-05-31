import "metabase_common.lua"
import "platform_android.lua"

supportedplatforms
{
	"TADP",
}

writer "writer_msvc.lua"

option("msvc",				"customwriter",					"/writer_msvctadp_android.lua")
option("msvc",				"platform",						"Tegra-Android")

option("msvconfiguration",	"AndroidArch",					"armv7-a")

option("msvccompile",		"MultiProcessorCompilation",	"true")
option("msvccompile",		"CppLanguageStandard",			"gnu++11")

config "Debug"
	--C/C++ 
		--General
		option("msvclink",		"GenerateDebugInformation",	"true")
		--Optimization
		option("msvccompile",	"OptimizationLevel",		"O0")
		--Code generation
		option("msvccompile",	 "StackProtector",			"false")
	--Ant Build
		option("antbuild",		"AntBuildType", 			"Debug")
		option("antbuild",		"Debuggable",				"true")
config_end()

config "Release"
	--C/C++ 
		--General
		option("msvclink",		"GenerateDebugInformation",	"true")
		--Optimization
		option("msvccompile",	"OptimizationLevel",		"O3")
		option("msvccompile",	"OmitFramePointer",			"true")
		--Code generation
		option("msvccompile",	"StackProtector",			"false")
		--Command Line
		option("msvccompile",	"AdditionalOptions",		"-funsafe-math-optimizations -ffast-math -ftree-vectorize %(AdditionalOptions)")
	--Ant Build
		option("antbuild",		"AntBuildType",				"Release")
		option("antbuild",		"Debuggable",				"true")
config_end()

config "Profile"
	--C/C++
		--General
		option("msvclink",		"GenerateDebugInformation", "true")
		--Optimization
		option("msvccompile",	"OptimizationLevel",		"O3")
		option("msvccompile",	"OmitFramePointer",			"false")
		--Code generation
		option("msvccompile",	"StackProtector",			"false")
		--Command Line
		option("msvccompile",	"AdditionalOptions",		"-funsafe-math-optimizations -ffast-math -ftree-vectorize %(AdditionalOptions)")
	--Ant Build
		option("antbuild",		"AntBuildType",				"Release")
		option("antbuild",		"Debuggable",				"true")
config_end()

config "Master"
	--C/C++
		--General
		--option("msvclink",		"GenerateDebugInformation",	"false")
		option("msvclink",		"GenerateDebugInformation",	"true")
		--Optimization
		option("msvccompile",	"OptimizationLevel",		"O3")
		--option("msvccompile",	"OmitFramePointer",			"true")
		--Code generation
		option("msvccompile",	"StackProtector",			"false")
		--Command Line
		option("msvccompile",	"AdditionalOptions",		"-funsafe-math-optimizations -ffast-math -ftree-vectorize %(AdditionalOptions)")
	--Ant Build
		option("antbuild",		"AntBuildType",				"Release")
		option("antbuild",		"Debuggable",				"false")
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

