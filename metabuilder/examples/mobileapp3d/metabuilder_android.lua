platform "Android"

	files
	{
		"projects/android/jni/*",
		"projects/android/res/*",
		"projects/android/src/*",
		"projects/android/AndroidManifest.xml",
	}

	if checkplatform("TADP") then
		tadp.settargetapi("android-21")
	end

	if checkplatform("NDK") then
		ndk.setprojectdir("projects/android")
	end

platform_end()
