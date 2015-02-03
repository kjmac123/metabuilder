platform "Android"
		
	files
	{
		"projects/android/jni/*",
		"projects/android/res/*",
		"projects/android/src/*",
		"projects/android/AndroidManifest.xml",
	}

	if checkplatform("TADP") then
		tadp.settargetapi("android-15")
	end

platform_end()
