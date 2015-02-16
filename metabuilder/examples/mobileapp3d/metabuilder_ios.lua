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

	resources
	{
		"projects/ios/Base.lproj/LaunchScreen.xib",
		"projects/ios/Base.lproj/Main.storyboard",
		"projects/ios/Images.xcassets",

		"data",
	}

	frameworks
	{
		"CoreAudio.framework",
		"Foundation.framework",
		"CoreGraphics.framework",
		"UIKit.framework",
		"GLKit.framework",
		"OpenGLES.framework",
	}

	if checkplatform("iOS") then
		xcode.setinfoplist(plistFile)
	end

platform_end()
