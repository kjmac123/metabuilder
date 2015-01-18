#ifndef MB_CORE_H
#define MB_CORE_H

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

#include <stdio.h>

#if defined(PLATFORM_OSX) || defined(PLATFORM_IOS)
#include <unistd.h>
#elif defined(PLATFORM_POSIX)
#include <strings.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <errno.h>

#if defined(PLATFORM_OSX) || defined(PLATFORM_LINUX)
#define stricmp strcasecmp
#endif

#define MB_LOGSETTIMEENABLED(b)			_mbLogSetTimeEnabled(b)
#define MB_LOGERROR(...)				_mbLogErrorfLF(__VA_ARGS__)
#define MB_LOGINFO(...)					_mbLogInfofLF(__VA_ARGS__)
#define MB_LOGDEBUG(...)				_mbLogDebugfLF(__VA_ARGS__)

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

#include "platform/platform.h"

#endif
