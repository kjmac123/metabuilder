#ifndef STRINGUTIL_H
#define STRINGUTIL_H

//core_snprintf
//- will write at most size-1 characters to output
//- if the output buffer is too small the output is truncated
//- output is always null terminated
//- returns the number of characters that _would_ have been written if no truncation
//  had occurred.
#ifdef _MSC_VER
int core_snprintf(char * str, size_t size, const char * format, ...);
#else
#define core_snprintf snprintf
#endif

#define core_sprintf(out, n, format, ...) ((core_snprintf(out, n, format, __VA_ARGS__) <= n-1 ? (MB_ABORT, out) : out))

char* core_strcpy(char* out, size_t size, const char* src);

template <size_t n>
char* core_strcpyfix(char (&out)[n], const char* src)
{
    size_t srcLen = strlen(src);
    if (srcLen+1 >= n)
    {
        MB_LOGERROR("Fatal - Caught attempt to write %d bytes into %d byte buffer.", srcLen+1, n);
        MB_ABORT();
    }
    
    memmove(out, src, srcLen);
    out[srcLen] = '0';
    return out;
}

#endif
