#ifndef CORELIB_CORE_H
#define CORELIB_CORE_H

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
#include <assert.h>

#if defined(PLATFORM_OSX) || defined(PLATFORM_LINUX)
#define stricmp strcasecmp
#endif

#define MB_LOGSETTIMEENABLED(b)			_mbLogSetTimeEnabled(b)
#define MB_LOGERROR(...)				_mbLogErrorfNewLine(__VA_ARGS__)
#define MB_LOGINFO(...)					_mbLogInfofNewLine(__VA_ARGS__)
#define MB_LOGDEBUG(...)				_mbLogDebugfNewLine(__VA_ARGS__)
#define MB_ABORT                        private_CoreAbort

void _mbLogSetTimeEnabled(bool b);
void _mbLogErrorfNewLine(const char* fmt, ...);
void _mbLogInfofNewLine(const char* fmt, ...);
void _mbLogDebugfNewLine(const char* fmt, ...);
void private_CoreAbort();

#define ARRAY_LENGTH(a) (sizeof((a))/sizeof((a)[0]))

#define MB_SECONDS_TO_MILLISECONDS          1000
#define MB_SECONDS_TO_NANOSECONDS           1000000000

#define MB_MAX_STRING_BUFFER_SIZE			(16*1024)
#define MB_ASSERT							assert

void    Core_Init(void* initData);
void    Core_Shutdown();

double  Core_GetElapsedTimeSeconds();

#include "corestring.h"

#include "platform/platform.h"

#endif
