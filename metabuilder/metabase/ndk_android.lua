metabase "ndk_android"
	import "metabase_common.lua"
	import "platform_android.lua"

	supportedplatforms
	{
		"NDK",
	}

	writer "writer_ndk.lua"

	option("ndkoptions", "APP_ABI",								"all")
	option("ndkoptions", "APP_PLATFORM",					"android-14")
	option("ndkoptions", "NDK_TOOLCHAIN_VERSION",	"clang")
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

ndk = {}

function ndk.setminapi(minapi)
end

function ndk.settargetapi(targetapi)
	--option("ndkoptions", "APP_PLATFORM", targetapi)
end

function ndk.setproguardenabled(value)
end

function ndk.setdebuggable(value)
end

ndk = {}

function ndk.setprojectdir(dir)
	option("_ndk", "ProjectDir", dir)
end
