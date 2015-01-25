metabase "xcode_osx"
	import "metabase_common.lua"
	import "platform_osx.lua"

	writer "writer_xcode.lua"

	option("compiler", "ALWAYS_SEARCH_USER_PATHS", "NO")
	option("compiler", "CLANG_CXX_LANGUAGE_STANDARD", "\"gnu++0x\"")
	option("compiler", "CLANG_CXX_LIBRARY", "\"libc++\"")
	option("compiler", "CLANG_ENABLE_OBJC_ARC", "YES")
	option("compiler", "CLANG_WARN_BOOL_CONVERSION", "YES")
	option("compiler", "CLANG_WARN_CONSTANT_CONVERSION", "YES")
	option("compiler", "CLANG_WARN_DIRECT_OBJC_ISA_USAGE", "YES_ERROR")
	option("compiler", "CLANG_WARN_EMPTY_BODY", "YES")
	option("compiler", "CLANG_WARN_ENUM_CONVERSION", "YES")
	option("compiler", "CLANG_WARN_INT_CONVERSION", "YES")
	option("compiler", "CLANG_WARN_OBJC_ROOT_CLASS", "YES_ERROR")
	option("compiler", "CLANG_WARN__DUPLICATE_METHOD_MATCH", "YES")
	option("compiler", "GCC_C_LANGUAGE_STANDARD", "gnu99")
	option("compiler", "GCC_DYNAMIC_NO_PIC", "NO")
	option("compiler", "GCC_ENABLE_OBJC_EXCEPTIONS", "YES")
	option("compiler", "GCC_SYMBOLS_PRIVATE_EXTERN", "NO")
	option("compiler", "GCC_WARN_64_TO_32_BIT_CONVERSION", "YES")
	option("compiler", "GCC_WARN_ABOUT_RETURN_TYPE", "YES_ERROR")
	option("compiler", "GCC_WARN_UNDECLARED_SELECTOR", "YES")
	option("compiler", "GCC_WARN_UNUSED_FUNCTION", "YES")
	option("compiler", "GCC_WARN_UNUSED_VARIABLE", "YES")
	option("compiler", "MACOSX_DEPLOYMENT_TARGET", "10.9")
	option("compiler", "ONLY_ACTIVE_ARCH", "YES")
	option("compiler", "SDKROOT", "macosx")

	option("compiler", "ASSETCATALOG_COMPILER_APPICON_NAME", "AppIcon")
	option("compiler", "ASSETCATALOG_COMPILER_LAUNCHIMAGE_NAME", "LaunchImage")
	option("compiler", "COMBINE_HIDPI_IMAGES", "YES")
	option("compiler", "GCC_PRECOMPILE_PREFIX_HEADER", "NO")
	option("compiler", "PRODUCT_NAME", "\"$(TARGET_NAME)\"")

	config "Debug"
		option("compiler", "COPY_PHASE_STRIP", "NO")
		option("compiler", "GCC_C_LANGUAGE_STANDARD", "gnu99")
		option("compiler", "GCC_DYNAMIC_NO_PIC", "NO")
		option("compiler", "GCC_OPTIMIZATION_LEVEL", 0)
	config_end()

	config "Release"
		option("compiler", "COPY_PHASE_STRIP", "YES")
		option("compiler", "DEBUG_INFORMATION_FORMAT", "dwarf-with-dsym")
		option("compiler", "ENABLE_NS_ASSERTIONS", "NO")
		option("compiler", "GCC_OPTIMIZATION_LEVEL", 2)
	config_end()

	config "Master"
		option("compiler", "COPY_PHASE_STRIP", "YES")
		option("compiler", "DEBUG_INFORMATION_FORMAT", "dwarf-with-dsym")
		option("compiler", "ENABLE_NS_ASSERTIONS", "NO")
		option("compiler", "GCC_OPTIMIZATION_LEVEL", 2)
	config_end()

metabase_end()
