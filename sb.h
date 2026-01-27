/*
  sb.h - A simple stb-style string builder library. This library is in the public domain.
*/

#ifndef SB_H
#define SB_H

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#ifndef SBDEF
#define SBDEF static inline
#endif // SBDEF

#ifndef SB_MALLOC
#include <stdlib.h>
#define SB_MALLOC malloc
#endif // SB_MALLOC

#ifndef SB_REALLOC
#include <stdlib.h>
#define SB_REALLOC realloc
#endif // SB_REALLOC

#ifndef SB_FREE
#include <stdlib.h>
#define SB_FREE free
#endif // SB_FREE

#ifndef SB_INIT_CAPACITY
#define SB_INIT_CAPACITY 256
#endif // SB_INIT_CAPACITY

#define SB_MIN(a, b) ((a) < (b)? (a) : (b))

#if defined(__GNUC__) || defined(__clang__)
#    ifdef __MINGW_PRINTF_FORMAT
#        define SB_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) __attribute__ ((format (__MINGW_PRINTF_FORMAT, STRING_INDEX, FIRST_TO_CHECK)))
#    else
#        define SB_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) __attribute__ ((format (printf, STRING_INDEX, FIRST_TO_CHECK)))
#    endif // __MINGW_PRINTF_FORMAT
#else
#    define SB_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK)
#endif

typedef struct {
    char *items;      // pointer to the buffer
    size_t count;     // number of bytes used
    size_t capacity;  // total allocated capacity
} sb_t;

/* Append formatted text (like printf) */
SBDEF int   sb_appendf(sb_t *sb, const char *fmt, ...) SB_PRINTF_FORMAT(2, 3);
SBDEF int   sb_vappendf(sb_t *sb, const char *fmt, va_list args);

/* Append a raw slice of memory or a single character */
SBDEF int   sb_append_slice(sb_t *sb, const char *buff, size_t n);
SBDEF int   sb_append_char(sb_t *sb, char c);

/* Append a null terminator (count is incremented) */
SBDEF int   sb_append_null(sb_t *sb);

/* Extract content into a buffer (always null-terminated if buff_size > 0) */
SBDEF int   sb_extract(const sb_t *sb, char *buff, size_t buff_size);
SBDEF int   sb_extract_slice(const sb_t *sb, size_t n, char *buff, size_t buff_size);

/* Allocate a new null-terminated copy of the string */
SBDEF char* sb_to_cstr(const sb_t *sb);

/* Detach buffer ownership from builder (caller must free) */
SBDEF char* sb_detach(sb_t *sb);

/* Ensure there is enough space for additional bytes */
SBDEF bool  sb_reserve(sb_t *sb, size_t bytes);

/* Free the builder's memory and reset to empty */
SBDEF void  sb_free(sb_t *sb);

#endif // SB_H

#ifdef SB_IMPLEMENTATION

SBDEF bool sb_reserve(sb_t *sb, size_t bytes)
{
    if (sb == NULL) return false;
    size_t required = sb->count + bytes + 1;
    size_t capacity = sb->capacity ? sb->capacity : SB_INIT_CAPACITY;
    while (capacity < required){
        if (capacity > SIZE_MAX / 2) return false;
        capacity *= 2;
    }
    if (capacity != sb->capacity){
        char *new_items = SB_REALLOC(sb->items, sizeof(*sb->items) * capacity);
        if (new_items == NULL) return false;
        
        sb->items = new_items;
        sb->capacity = capacity;
    }
    return true;
}

SBDEF int sb_appendf(sb_t *sb, const char *fmt, ...)
{
    if (sb == NULL) return -1;

    va_list args;
    va_start(args, fmt);

    int n = sb_vappendf(sb, fmt, args);
    
    va_end(args);
    return n;
}

SBDEF int sb_vappendf(sb_t *sb, const char *fmt, va_list args)
{
    if (sb == NULL) return -1;
    
    va_list args_copy;
    va_copy(args_copy, args);

    int n = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);
    if (n < 0) return n;

    if (!sb_reserve(sb, n)) return -1;
    
    int w = vsnprintf(&sb->items[sb->count], n+1, fmt, args);
    if (w != n) return -1;

    sb->count += n;
    return n;
}

SBDEF int sb_append_slice(sb_t *sb, const char *buff, size_t n)
{
    if (sb == NULL || buff == NULL) return -1;

    if (!sb_reserve(sb, n)) return -1;

    (void) memcpy(&sb->items[sb->count], buff, n);
    
    sb->count += n;
    return n;
}

SBDEF int sb_append_char(sb_t *sb, char c)
{
    if (sb == NULL) return -1;
    if (!sb_reserve(sb, 1)) return -1;

    sb->items[sb->count++] = c;
    return 1;
}

SBDEF int sb_append_null(sb_t *sb)
{
    if (sb == NULL) return -1;
    if (!sb_reserve(sb, 0)) return -1;
    
    sb->items[sb->count++] = '\0';
    return 1;
}

SBDEF int sb_extract(const sb_t *sb, char *buff, size_t buff_size)
{
    return sb_extract_slice(sb, sb->count, buff, buff_size);
}

SBDEF int sb_extract_slice(const sb_t *sb, size_t n, char *buff, size_t buff_size)
{
    if (sb == NULL) return -1;
    if (buff_size == 0) return 0;

    size_t bytes_to_write = SB_MIN(SB_MIN(sb->count, n), buff_size-1);
    (void) memcpy(buff, sb->items, bytes_to_write);
    (void) memset(&buff[bytes_to_write], 0, buff_size-bytes_to_write);
    return bytes_to_write;
}

SBDEF char* sb_to_cstr(const sb_t *sb)
{
    if (sb == NULL) return NULL;

    char *string = SB_MALLOC(sb->count+1);
    if (string == NULL) return NULL;

    (void) memcpy(string, sb->items, sb->count);
    string[sb->count] = '\0';
    
    return string;
}

SBDEF char* sb_detach(sb_t *sb)
{
    if (sb == NULL) return NULL;
    
    if (sb_append_null(sb) == -1){
        sb->items[sb->count] = '\0';
    }
    char *content = sb->items;
    
    sb->items = NULL;
    sb->count = sb->capacity = 0;
        
    return content;
}

SBDEF void sb_free(sb_t *sb)
{
    if (sb == NULL) return;
    SB_FREE(sb->items);
    sb->items = NULL;
    sb->count = sb->capacity = 0;
}

#endif // SB_IMPLEMENTATION
