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

#ifdef PLATFORM_OSX
#include <unistd.h>
#endif

#define MB_LUA_STACK_MAX 900000

#include <string>
#include <vector>

#ifdef PLATFORM_OSX
#define stricmp strcasecmp
#endif

#define MB_LOGERROR(...)				_mbLogErrorfLF(__VA_ARGS__)
#define MB_LOGINFO(...)					_mbLogInfofLF(__VA_ARGS__)

#define MB_CHECKERROR(test, ...)		{ if (!test) { _LogInfofLF(__VA_ARGS__); } }
#define MB_CHECKEXPECTEDBLOCK(blockExpected, cmdName) mbCheckExpectedBlock(blockExpected, cmdName);

void _mbLogErrorf(const char* fmt, ...);
void _mbLogInfof(const char* fmt, ...);
void _mbLogErrorfLF(const char* fmt, ...);
void _mbLogInfofLF(const char* fmt, ...);

#include "platform/platform.h"

#endif
