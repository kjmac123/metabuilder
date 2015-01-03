import "metabase_common.lua"

writer "writer_gnumakegcccompatible.lua"

local cppFlagsCommon = "-Wall"
option("cc", "gcc")
option("cxx", "g++")
option("ar", "ar")
option("ld", "g++")

option("cxxflags",	"-Wall -std=c++11")
option("arflags",	"crsT")
option("ldflags",	"")

config "Debug"
	option("cppflags", cppFlagsCommon .. " -g -O0")
config_end()

config "Release"
	option("cppflags", cppFlagsCommon .. " -g -O2")
config_end()

config "Master"
	option("cppflags", cppFlagsCommon .. " -O2")
config_end()


config "Debug"
config_end()

config "Release"
config_end()

config "Master"
config_end()
