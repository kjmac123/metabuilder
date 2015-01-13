solution "hellococoa"

	target "hellococoa"
		target_type "app"

		depends("hellolib",	"../hellolib/metabuilder.lua")
		
		includedirs 
		{ 
			"../hellolib",
		}

		files
		{
			"AppDelegate.h",
			"AppDelegate.m",
			"cocoaapp-Info.plist",
			"cocoaapp-Prefix.pch",
			"main.m",
			"metabuilder.lua",
		}

		resources
		{
			"Base.lproj/MainMenu.xib",
			"en.lproj/Credits.rtf",
			"en.lproj/InfoPlist.strings",
			"Images.xcassets/*",
		}

		frameworks
		{
			"AppKit.framework",
			"Cocoa.framework",
			"CoreData.framework",
			"Foundation.framework"
		}

		config "Debug"
			defines 
			{ 
			}
		config_end()

		config "Release"
			defines 
			{ 
			}
		config_end()

		config "Master"
			defines 
			{ 
				--Add custom defines here
			}
		config_end()
		
 	target_end()

	option("compiler", "INFOPLIST_FILE", "../../../cocoaapp-Info.plist")

 solution_end()
