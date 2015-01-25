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

	libs
	{
		"-llog",
	}

	if checkplatform("Android") then
		android.settargetapi("android-15")
	end

	if checkplatform("NDK") then
		ndk.setprojectdir("projects/android")
	end

platform_end()
