platform "Android"

	files
	{
        "corelib/platform/android/apkfile_android.cpp",
        "corelib/platform/android/jnihelpers.h",
		"corelib/platform/android/platformfile_android.cpp",
		"corelib/platform/android/platform_android.cpp",
        "corelib/platform/generic/platformfile_generic.cpp",
	}

	if checkplatform("Android") then
		android.settargetapi("android-14")
	end

	if checkplatform("NDK") then
		ndk.setprojectdir("projects/android")
	end

platform_end()
