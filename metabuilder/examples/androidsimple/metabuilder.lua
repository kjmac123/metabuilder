makesetup ""
	--subdir for intermediate files
--	intdir "int"
	--subdir for final output files
--	outdir "out"	
makesetup_end()

solution "androidsimple"

	target "androidsimple"
		target_type "app"
		
		androidsettargetapi("android-15")

		includedirs 
		{ 
			"../hellolib",
		}

		files
		{
			"jni/*",
			"res/*",
			"src/*",
			"AndroidManifest.xml",
		}

		config "Debug"
		config_end()

		config "Release"
		config_end()
--[[		
		config "Profile"
		config_end()

		config "Master"
		config_end()
	]]					
 	target_end()

solution_end()
