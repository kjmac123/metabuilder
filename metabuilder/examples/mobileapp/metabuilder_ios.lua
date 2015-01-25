platform "iOS"

	local plistFile = "projects/ios/Info.plist"

	files
	{
		"projects/ios/AppDelegate.h",
		"projects/ios/AppDelegate.mm",
		"projects/ios/AppViewController.h",
		"projects/ios/AppViewController.mm",
		plistFile,
		"projects/ios/main.mm",
	}

	files
	{
		"src/platform/ios/platform_ios.cpp",
	}

	resources
	{
		"projects/ios/Base.lproj/LaunchScreen.xib",
		"projects/ios/Base.lproj/Main.storyboard",
		"projects/ios/Images.xcassets/*",
	}

	frameworks
	{
		"CoreAudio.framework",
		"Foundation.framework",
		"CoreGraphics.framework",
		"UIKit.framework",
	}

	if checkplatform("iOS") then
		xcode.setinfoplist(plistFile)
	end

platform_end()
