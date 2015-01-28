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
		android.setminapi("android-14")
		android.settargetapi("android-21")
	end

	if checkplatform("NDK") then
		ndk.setprojectdir("projects/android")
	end

platform_end()
