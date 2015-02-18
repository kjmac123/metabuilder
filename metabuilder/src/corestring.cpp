#include "core.h"
#include <cstdarg>

#ifdef _MSC_VER

int core_snprintf(char * out, size_t size, const char * format, ...)
{
    va_list ap;
    va_start(ap, format);
    int result = vsprintf_s(out, size, format, ap);
    va_end(ap);
    
    return result;

}

#endif

char* core_strcpy(char* out, size_t size, const char* src)
{
    size_t len = strlen(src);
    if (len < size)
    {
        memmove(out, src, size);
        return out;
    }
    
    MB_ABORT();
    return NULL;
}