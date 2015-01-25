platform "Android"
		
	files
	{
		"projects/android/jni/*",
		"projects/android/res/*",
		"projects/android/src/*",
		"projects/android/AndroidManifest.xml",
	}

	files
	{
		"src/platform/android/platform_android.cpp",
	}

	if checkplatform("TADP") then
		tadp.settargetapi("android-15")
	end

platform_end()
