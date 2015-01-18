platform "iOS"

	files
	{
		"projects/iOS/AppDelegate.h",
		"projects/iOS/AppDelegate.m",
		"projects/iOS/AppViewController.h",
		"projects/iOS/AppViewController.m",
		"projects/iOS/Info.plist",
		"projects/iOS/main.m",
	}

	files
	{
		"src/platform/ios/platform_ios.cpp",
	}

	resources
	{
		"projects/iOS/Base.lproj/LaunchScreen.xib",
		"projects/iOS/Base.lproj/Main.storyboard",
		"projects/iOS/Images.xcassets/*",
		"projects/iOS/Shaders/*",
	}

	frameworks
	{
	}

	option("compiler", "INFOPLIST_FILE", "projects/iOS/Info.plist")	

platform_end()
