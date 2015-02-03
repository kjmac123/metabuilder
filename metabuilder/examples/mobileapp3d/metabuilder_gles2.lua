platform "GLES2"

	files
	{
		"app/platform/gles2/mainview_gles2.cpp",
	}

	if checkplatform("TADP") then
		tadp.settargetapi("android-15")
	end

platform_end()
