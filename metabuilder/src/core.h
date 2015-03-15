#ifndef MB_CORE_H
#define MB_CORE_H

//#define MB_ENABLE_PROFILING

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
#include <errno.h>

#include <vector>
#include <string>

//Maximum path length is a complete mess. Let's go with 512bytes for now.
//stackoverflow.com/questions/833291/is-there-an-equivalent-to-winapis-max-path-under-linux-unix
#define MB_MAX_PATH 512

#define MB_LOGSETTIMEENABLED(b)			_mbLogSetTimeEnabled(b)
#define MB_LOGERROR(...)				_mbLogErrorfLF(__VA_ARGS__)
#define MB_LOGINFO(...)					_mbLogInfofLF(__VA_ARGS__)
#define MB_LOGDEBUG(...)				_mbLogDebugfLF(__VA_ARGS__)

#define MB_CHECKERROR(test, ...)		{ if (!test) { _LogInfofLF(__VA_ARGS__); } }
#define MB_CHECKEXPECTEDBLOCK(blockExpected, cmdName) mbCheckExpectedBlock(blockExpected, cmdName);

#define MB_ABORT(...)

void _mbLogSetTimeEnabled(bool b);
void _mbLogErrorf(const char* fmt, ...);
void _mbLogErrorfLF(const char* fmt, ...);
void _mbLogInfof(const char* fmt, ...);
void _mbLogInfofLF(const char* fmt, ...);
void _mbLogDebugf(const char* fmt, ...);
void _mbLogDebugfLF(const char* fmt, ...);

#define ARRAY_LENGTH(a) (sizeof((a))/sizeof((a)[0]))

#define MB_SECONDS_TO_MILLISECONDS          1000
#define MB_SECONDS_TO_NANOSECONDS           1000000000

#define MB_MAX_STRING_BUFFER_SIZE			(16*1024)
#define MB_LUAMODULE_MAX_FUNCTIONS			100
#define MB_ASSERT							assert

void ToUpperStr(char* str);
void ToLowerStr(char* str);

void mbCore_Init();
void mbCore_Shutdown();

#include "platform/platform.h"

#endif
