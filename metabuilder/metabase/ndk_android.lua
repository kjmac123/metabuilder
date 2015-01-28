metabase "ndk_android"
	import "metabase_common.lua"
	import "platform_android.lua"

	supportedplatforms
	{
		"NDK",
	}

	writer "writer_ndk.lua"

	option("ndkoptions", "APP_ABI",								"armeabi-v7a")
	option("ndkoptions", "APP_PLATFORM",						"android-14")
	option("ndkoptions", "NDK_TOOLCHAIN_VERSION",				"clang")
	option("ndkoptions", "APP_STL", 							"stlport_static")

	local appCFLAGSCommon = "-Wno-multichar -fno-rtti -fno-exceptions -marm -fpic -Wno-unused-variable -Wno-unused-value "

	config "Debug"
		option(		"ndkoptions", "APP_OPTIM",			"debug")
		option(		"ndkoptions", "APP_CFLAGS",			appCFLAGSCommon .. " -O0")
	config_end()

	config "Release"
		option(		"ndkoptions", "APP_OPTIM",			"release")
		option(		"ndkoptions", "APP_CFLAGS",			appCFLAGSCommon .. " -O3")
	config_end()

	config "Profile"
		option(		"ndkoptions", "APP_OPTIM",			"release")
		option(		"ndkoptions", "APP_CFLAGS",			appCFLAGSCommon .. " -O3")
	config_end()

	config "Master"
		option(		"ndkoptions", "APP_ABI",			"all") -- Build for all supported CPU types
		option(		"ndkoptions", "APP_OPTIM",			"release")
		option(		"ndkoptions", "APP_CFLAGS",			appCFLAGSCommon .." -O3")
	config_end()

metabase_end()

android = {}

function android.setminapi(minapi)
end

function android.settargetapi(targetapi)
	--option("ndkoptions", "APP_PLATFORM", targetapi)
end

function android.setproguardenabled(proguardEnabled)
end

ndk = {}

function ndk.setprojectdir(dir)
	option("_ndk", "ProjectDir", dir)
end
