solution "game_ios"

	target "game_ios"
		target_type "app"

		files
		{
			"AppDelegate.h",
			"AppDelegate.m",
			"GameViewController.h",
			"GameViewController.m",
			"Info.plist",
			"main.m",
			"metabuilder.lua",
		}

		resources
		{
			"Base.lproj/LaunchScreen.xib",
			"Base.lproj/Main.storyboard",
			"Images.xcassets/*",
			"Shaders/*",
		}

		frameworks
		{
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

	option("compiler", "INFOPLIST_FILE", "../../../Info.plist")

 solution_end()
