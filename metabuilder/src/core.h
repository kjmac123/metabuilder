#ifndef MB_CORE_H
#define MB_CORE_H

#if defined(__APPLE__) && defined(__MACH__)
	#include <TargetConditionals.h>
	#if TARGET_OS_MAC == 1
		#define PLATFORM_OSX
		#define PLATFORM_POSIX
	#endif
#endif

#if defined(__linux__)
	#define PLATFORM_LINUX
	#define PLATFORM_POSIX
#endif

typedef unsigned char U8;
typedef char S8;
typedef unsigned short U16;
typedef short S16;
typedef unsigned int U32;
typedef int S32;
typedef unsigned long long U64;
typedef long long S64;
typedef float F32;
typedef double F64;

#if defined(PLATFORM_OSX) || defined(PLATFORM_POSIX)
#include <unistd.h>
#elif defined(PLATFORM_POSIX)
#include <strings.h>
#endif

#define MB_LUA_STACK_MAX 900000

#include <string.h>
#include <stdlib.h>

#include <vector>
#include <string>

#if defined(PLATFORM_OSX) || defined(PLATFORM_LINUX)
#define stricmp strcasecmp
#endif

#define MB_LOGERROR(...)				_mbLogErrorfLF(__VA_ARGS__)
#define MB_LOGINFO(...)					_mbLogInfofLF(__VA_ARGS__)
#define MB_LOGDEBUG(...)				_mbLogDebugfLF(__VA_ARGS__)

#define MB_CHECKERROR(test, ...)		{ if (!test) { _LogInfofLF(__VA_ARGS__); } }
#define MB_CHECKEXPECTEDBLOCK(blockExpected, cmdName) mbCheckExpectedBlock(blockExpected, cmdName);

void _mbLogErrorf(const char* fmt, ...);
void _mbLogErrorfLF(const char* fmt, ...);
void _mbLogInfof(const char* fmt, ...);
void _mbLogInfofLF(const char* fmt, ...);
void _mbLogDebugf(const char* fmt, ...);
void _mbLogDebugfLF(const char* fmt, ...);

#include "platform/platform.h"

#endif
