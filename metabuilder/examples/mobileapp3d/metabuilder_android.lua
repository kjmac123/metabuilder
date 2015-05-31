platform "Android"

	files
	{
		"projects/android/jni/*",
		"projects/android/res/*",
		"projects/android/src/*",
		"projects/android/AndroidManifest.xml",
	}

	resources
	{
		"data",
	}

	libs
	{
	    "-llog",
	    "-lGLESv2",
	    "-landroid",
	}

	if checkplatform("TADP") then
		tadp.settargetapi("android-21")

		config "Debug"
			tadp.setproguardenabled(true)
		config_end()

		config "Release"
			tadp.setproguardenabled(true)
		config_end()

		config "Master"
			tadp.setproguardenabled(true)
		config_end()		
	end

	if checkplatform("NDK") then
		ndk.setprojectdir("projects/android")
	end	

platform_end()
