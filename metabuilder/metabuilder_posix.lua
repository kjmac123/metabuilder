platform "POSIX"
	addoption("cxxflags",	"-Wunused-function")

	files
	{
		"../external/freebsd/realpath.cpp",
	
		"src/platform/posix/platform_posix.cpp",
	}
platform_end()
