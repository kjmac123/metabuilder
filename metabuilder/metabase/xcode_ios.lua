metabase "xcode_ios"
	import "metabase_common.lua"
	import "platform_ios.lua"
	import "xcode_common.lua"

	writer "writer_xcode.lua"	

	--option("compiler", "ARCHS", "\"$(ARCHS_STANDARD)\"")
	option("compiler", "CLANG_CXX_LANGUAGE_STANDARD", "\"gnu++0x\"")
	option("compiler", "CLANG_CXX_LIBRARY", "\"libc++\"")
	option("compiler", "CODE_SIGN_IDENTITY", "\"iPhone Developer\"")	
	option("compiler", "GCC_C_LANGUAGE_STANDARD", "gnu99")
	option("compiler", "IPHONEOS_DEPLOYMENT_TARGET", "7.0")
	option("compiler", "SDKROOT", "iphoneos")
	option("compiler", "TARGETED_DEVICE_FAMILY", "\"1,2\"");
	option("compiler", "USER_HEADER_SEARCH_PATHS", "\"\"");
	option("compiler", "VALIDATE_PRODUCT", "YES");
	--option("compiler", "VALID_ARCHS", "armv7");
	option("compiler", "GCC_FAST_MATH","YES")

    option("compiler", "CLANG_WARN_BOOL_CONVERSION", "YES")
    option("compiler", "CLANG_WARN_CONSTANT_CONVERSION", "YES")
    option("compiler", "CLANG_WARN_DIRECT_OBJC_ISA_USAGE", "YES_ERROR")
    option("compiler", "CLANG_WARN_EMPTY_BODY", "NO")
    option("compiler", "CLANG_WARN_ENUM_CONVERSION", "YES")
    option("compiler", "CLANG_WARN_INT_CONVERSION", "YES")
    option("compiler", "CLANG_WARN_OBJC_ROOT_CLASS", "YES_ERROR")
    option("compiler", "CLANG_WARN__DUPLICATE_METHOD_MATCH", "YES")
    option("compiler", "GCC_WARN_64_TO_32_BIT_CONVERSION", "YES")
    option("compiler", "GCC_WARN_ABOUT_RETURN_TYPE", "YES_ERROR")
    option("compiler", "GCC_WARN_UNDECLARED_SELECTOR", "YES")
    option("compiler", "GCC_WARN_UNINITIALIZED_AUTOS", "YES")
    option("compiler", "GCC_WARN_UNUSED_FUNCTION", "NO")
    option("compiler", "GCC_WARN_UNUSED_VARIABLE", "NO")
    option("compiler", "GCC_WARN_UNUSED_VALUE", "NO")
    option("compiler", "GCC_WARN_CHECK_SWITCH_STATEMENTS", "NO")
    option("compiler", "GCC_WARN_ABOUT_DEPRECATED_FUNCTIONS", "NO")

	option("compiler", "GCC_INLINES_ARE_PRIVATE_EXTERN", "NO")
	option("compiler", "GCC_SYMBOLS_PRIVATE_EXTERN", "NO")

    option("compiler", "GCC_TREAT_WARNINGS_AS_ERRORS", "YES")

	config "Debug"
		option("compiler", "ALWAYS_SEARCH_USER_PATHS", "NO")
		option("compiler", "CLANG_ENABLE_MODULES", "YES")
		option("compiler", "CLANG_ENABLE_OBJC_ARC", "YES")
		option("compiler", "COPY_PHASE_STRIP", "NO")
		option("compiler", "GCC_DYNAMIC_NO_PIC", "NO")
		option("compiler", "GCC_OPTIMIZATION_LEVEL", "0")
		option("compiler", "ONLY_ACTIVE_ARCH", "YES")	
	config_end()

	config "Release"
		option("compiler", "ALWAYS_SEARCH_USER_PATHS", "NO")
		option("compiler", "CLANG_ENABLE_MODULES", "YES")
		option("compiler", "CLANG_ENABLE_OBJC_ARC", "YES")
		option("compiler", "COPY_PHASE_STRIP", "YES")
		option("compiler", "ENABLE_NS_ASSERTIONS", "NO")
		option("compiler", "VALIDATE_PRODUCT",  "YES")
		option("compiler", "GCC_OPTIMIZATION_LEVEL", "fast")
		option("compiler", "ONLY_ACTIVE_ARCH", "YES")
	config_end()

	config "Profile"
		option("compiler", "ALWAYS_SEARCH_USER_PATHS", "NO")
		option("compiler", "CLANG_ENABLE_MODULES", "YES")
		option("compiler", "CLANG_ENABLE_OBJC_ARC", "YES")
		option("compiler", "COPY_PHASE_STRIP", "YES")
		option("compiler", "ENABLE_NS_ASSERTIONS", "NO")
		option("compiler", "VALIDATE_PRODUCT",  "YES")
		option("compiler", "GCC_OPTIMIZATION_LEVEL", "fast")
		option("compiler", "ONLY_ACTIVE_ARCH", "YES")
	config_end()

	config "Master"
		option("compiler", "COPY_PHASE_STRIP", "YES")
		option("compiler", "ENABLE_NS_ASSERTIONS", "NO")
		option("compiler", "ALWAYS_SEARCH_USER_PATHS", "NO")
		option("compiler", "CLANG_ENABLE_MODULES", "YES")
		option("compiler", "CLANG_ENABLE_OBJC_ARC", "YES")
		option("compiler", "VALIDATE_PRODUCT",  "YES")
		option("compiler", "GCC_OPTIMIZATION_LEVEL", "fast")
		option("compiler", "GCC_UNROLL_LOOPS", "YES")
		option("compiler", "LLVM_LTO", "YES")

		option("compiler", "CODE_SIGN_IDENTITY", "\"iPhone Distribution\"")
	config_end()

metabase_end()
