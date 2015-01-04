platform "POSIX"
	addoption("cxxflags",	"-Wno-unused-function")

	files
	{
		"../external/freebsd/realpath.cpp",
	
		"src/platform/posix/platform_posix.cpp",
	}
	
	if checkplatform("OSX") then
		--Nothing extra for now
	else
		libs 
		{ 
			"-lrt",
		}
	end
platform_end()
