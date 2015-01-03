platform "POSIX"
	addoption("cxxflags",	"-Wno-unused-function")

	files
	{
		"../external/freebsd/realpath.cpp",
	
		"src/platform/posix/platform_posix.cpp",
	}
	
	libs 
	{ 
		"-lrt",
	}

platform_end()
