/*
  sbv.h - A simple stb-style string builder and string view library. This library is in the public domain.
*/

#ifndef SBV_H
#define SBV_H

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

#ifndef SBVDEF
#define SBVDEF static inline
#endif // SBVDEF

#ifndef SBV_MALLOC
#include <stdlib.h>
#define SBV_MALLOC malloc
#endif // SBV_MALLOC

#ifndef SBV_REALLOC
#include <stdlib.h>
#define SBV_REALLOC realloc
#endif // SBV_REALLOC

#ifndef SBV_FREE
#include <stdlib.h>
#define SBV_FREE free
#endif // SBV_FREE

#ifndef SB_INIT_CAPACITY
#define SB_INIT_CAPACITY 64
#endif // SB_INIT_CAPACITY

#ifdef _WIN32
#define strncasecmp _strnicmp
#endif // _WIN32

#define SBV_MIN(a, b) ((a) < (b)? (a) : (b))
#define SBV_WHITESPACE " \t\n\r\f\v"
#define SV_TRIM_ALL 0

#define SV_FOREACH_SPLIT(it, sv, del) \
    for (sv_t _rest = (sv), it = sv_split(_rest, del, &_rest); \
         it.items != NULL; \
         it = sv_split(_rest, del, &_rest))

#define SV_FOREACH_SPLIT_CHAR(it, sv, del) \
    for (sv_t _rest = (sv), it = sv_split_char(_rest, del, &_rest); \
         it.items != NULL; \
         it = sv_split_char(_rest, del, &_rest))


#if defined(__GNUC__) || defined(__clang__)
#    ifdef __MINGW_PRINTF_FORMAT
#        define SBV_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) __attribute__ ((format (__MINGW_PRINTF_FORMAT, STRING_INDEX, FIRST_TO_CHECK)))
#    else
#        define SBV_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) __attribute__ ((format (printf, STRING_INDEX, FIRST_TO_CHECK)))
#    endif // __MINGW_PRINTF_FORMAT
#else
#    define SBV_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK)
#endif

typedef struct {
    char *items;       // pointer to the buffer, (owned)
    size_t count;      // number of bytes used
    size_t capacity;   // total allocated capacity
} sb_t;

typedef struct{
    const char *items; // pointer to the buffer, (not owned)
    size_t len;        // number of bytes
} sv_t;

#ifdef _cplusplus
extern "C" {
#endif // _cplusplus

SBVDEF sb_t sb_null();

SBVDEF int sb_appendf(sb_t *sb, const char *fmt, ...) SBV_PRINTF_FORMAT(2, 3);
SBVDEF int sb_vappendf(sb_t *sb, const char *fmt, va_list args);
SBVDEF int sb_append_cstr(sb_t *sb, const char *cstr);
SBVDEF int sb_append_slice(sb_t *sb, const char *buff, size_t n);
SBVDEF int sb_append_sv(sb_t *sb, sv_t sv);
SBVDEF int sb_append_char(sb_t *sb, char c);
SBVDEF int sb_append_null(sb_t *sb);

SBVDEF int sb_extract(const sb_t *sb, char *buff, size_t buff_size);
SBVDEF int sb_extract_slice(const sb_t *sb, size_t n, char *buff, size_t buff_size);
SBVDEF char* sb_to_cstr(const sb_t *sb);
SBVDEF char* sb_detach(sb_t *sb);

SBVDEF bool sb_reserve(sb_t *sb, size_t bytes);
SBVDEF void sb_clear(sb_t *sb);
SBVDEF void sb_free(sb_t *sb);

SBVDEF sv_t sv_null();
SBVDEF sv_t sv_from_slice(const char *buff, size_t n);
SBVDEF sv_t sv_from_cstr(const char *cstr);
SBVDEF sv_t sv_from_sb(const sb_t *sb);

SBVDEF bool sv_empty(sv_t sv);
SBVDEF bool sv_isnull(sv_t sv);
SBVDEF bool sv_equals(sv_t a, sv_t b);
SBVDEF bool sv_equals_case(sv_t a, sv_t b);
SBVDEF bool sv_starts_with(sv_t sv, sv_t prefix);
SBVDEF bool sv_ends_with(sv_t sv, sv_t suffix);

SBVDEF size_t sv_find(sv_t sv, sv_t query);
SBVDEF size_t sv_find_char(sv_t sv, char query);
SBVDEF size_t sv_count(sv_t sv, sv_t query);
SBVDEF size_t sv_count_char(sv_t sv, char query);
SBVDEF bool sv_contains(sv_t sv, sv_t query);
SBVDEF bool sv_contains_char(sv_t sv, char query);

SBVDEF sv_t sv_slice(sv_t sv, size_t from, size_t to);
SBVDEF sv_t sv_chop_left(sv_t sv, size_t n);
SBVDEF sv_t sv_chop_right(sv_t sv, size_t n);

// you can call these in a loop until the returned sv is sv_null
// or use the SV_FOREACH_SPLIT, SV_FOREACH_SPLIT_CHAR macros
SBVDEF sv_t sv_split(sv_t sv, sv_t del, sv_t *rest);
SBVDEF sv_t sv_split_char(sv_t sv, char del, sv_t *rest);
SBVDEF size_t sv_split_count(sv_t sv, sv_t del);
SBVDEF size_t sv_split_char_count(sv_t sv, char del);

SBVDEF sv_t sv_trim(sv_t sv);
SBVDEF sv_t sv_trim_chars(sv_t sv, const char *chars);
SBVDEF sv_t sv_trim_seq(sv_t sv, sv_t seq, size_t iterations);       // use SV_SPLIT_ALL to trim as often as possible

SBVDEF sv_t sv_trim_left(sv_t sv);
SBVDEF sv_t sv_trim_left_chars(sv_t sv, const char *chars);
SBVDEF sv_t sv_trim_left_seq(sv_t sv, sv_t seq, size_t iterations);  // use SV_SPLIT_ALL to trim as often as possible

SBVDEF sv_t sv_trim_right(sv_t sv);
SBVDEF sv_t sv_trim_right_chars(sv_t sv, const char *chars);
SBVDEF sv_t sv_trim_right_seq(sv_t sv, sv_t seq, size_t iterations); // use SV_SPLIT_ALL to trim as often as possible

SBVDEF int sv_extract(sv_t sv, char *buff, size_t buff_size);
SBVDEF size_t sv_cstr_size(sv_t sv);
SBVDEF char* sv_to_cstr(sv_t sv);

#ifdef _cplusplus
}
#endif // _cplusplus

#endif // SBV_H

#ifdef SBV_IMPLEMENTATION

SBVDEF sb_t sb_null()
{
    return (sb_t){
        .items = NULL,
        .count = 0,
        .capacity = 0
    };
}

SBVDEF bool sb_reserve(sb_t *sb, size_t bytes)
{
    if (sb == NULL) return false;
    size_t required = sb->count + bytes + 1;
    size_t capacity = sb->capacity ? sb->capacity : SB_INIT_CAPACITY;
    while (capacity < required){
        if (capacity > SIZE_MAX / 2) return false;
        capacity *= 2;
    }
    if (capacity != sb->capacity){
        char *new_items = SBV_REALLOC(sb->items, sizeof(*sb->items) * capacity);
        if (new_items == NULL) return false;
        
        sb->items = new_items;
        sb->capacity = capacity;
    }
    return true;
}

SBVDEF int sb_appendf(sb_t *sb, const char *fmt, ...)
{
    if (sb == NULL) return -1;

    va_list args;
    va_start(args, fmt);

    int n = sb_vappendf(sb, fmt, args);
    
    va_end(args);
    return n;
}

SBVDEF int sb_vappendf(sb_t *sb, const char *fmt, va_list args)
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

SBVDEF int sb_append_sv(sb_t *sb, sv_t sv)
{
    return sb_append_slice(sb, sv.items, sv.len);
}

SBVDEF int sb_append_slice(sb_t *sb, const char *buff, size_t n)
{
    if (sb == NULL || buff == NULL) return -1;

    if (!sb_reserve(sb, n)) return -1;

    (void) memcpy(&sb->items[sb->count], buff, n);
    
    sb->count += n;
    return n;
}

SBVDEF int sb_append_cstr(sb_t *sb, const char *cstr)
{
    return sb_append_slice(sb, cstr, strlen(cstr));
}

SBVDEF int sb_append_char(sb_t *sb, char c)
{
    if (sb == NULL) return -1;
    if (!sb_reserve(sb, 1)) return -1;

    sb->items[sb->count++] = c;
    return 1;
}

SBVDEF int sb_append_null(sb_t *sb)
{
    if (sb == NULL) return -1;
    if (!sb_reserve(sb, 0)) return -1;
    
    sb->items[sb->count] = '\0';
    return 0;
}

SBVDEF int sb_extract(const sb_t *sb, char *buff, size_t buff_size)
{
    return sb_extract_slice(sb, sb->count, buff, buff_size);
}

SBVDEF int sb_extract_slice(const sb_t *sb, size_t n, char *buff, size_t buff_size)
{
    if (sb == NULL) return -1;
    if (buff_size == 0) return 0;

    size_t bytes_to_write = SBV_MIN(SBV_MIN(sb->count, n), buff_size-1);
    (void) memcpy(buff, sb->items, bytes_to_write);
    buff[bytes_to_write] = '\0';
    return bytes_to_write;
}

SBVDEF char* sb_to_cstr(const sb_t *sb)
{
    if (sb == NULL) return NULL;

    char *string = SBV_MALLOC(sb->count+1);
    if (string == NULL) return NULL;

    (void) memcpy(string, sb->items, sb->count);
    string[sb->count] = '\0';
    
    return string;
}

SBVDEF char* sb_detach(sb_t *sb)
{
    if (sb == NULL) return NULL;
    
    if (sb_append_null(sb) == -1 && sb->items != NULL){
        sb->items[sb->count] = '\0';
    }
    char *content = sb->items;
    
    sb->items = NULL;
    sb->count = sb->capacity = 0;
        
    return content;
}

SBVDEF void sb_clear(sb_t *sb)
{
    if (sb == NULL) return;
    sb->count = 0;
}

SBVDEF void sb_free(sb_t *sb)
{
    if (sb == NULL) return;
    SBV_FREE(sb->items);
    sb->items = NULL;
    sb->count = sb->capacity = 0;
}

SBVDEF sv_t sv_from_slice(const char *buff, size_t n)
{
    return (sv_t){
        .items = buff,
        .len = n
    };
}

SBVDEF sv_t sv_null()
{
    return sv_from_slice(NULL, 0);
}

SBVDEF sv_t sv_from_cstr(const char *cstr)
{
    return sv_from_slice(cstr, strlen(cstr));
}

SBVDEF sv_t sv_from_sb(const sb_t *sb)
{
    return sv_from_slice(sb->items, sb->count);
}

SBVDEF bool sv_isnull(sv_t sv)
{
    return sv.items == NULL;
}

SBVDEF bool sv_empty(sv_t sv)
{
    return sv.len == 0;
}

SBVDEF bool sv_equals(sv_t a, sv_t b)
{
    if (a.len != b.len) return false;
    if (a.len == 0) return true;
    return memcmp(a.items, b.items, a.len) == 0;
}

SBVDEF bool sv_equals_case(sv_t a, sv_t b)
{
    if (a.len != b.len) return false;
    for (size_t i=0; i<a.len; ++i){
        if (tolower((unsigned char) a.items[i]) != tolower((unsigned char) b.items[i])){
            return false;
        }
    }
    return true;
}

SBVDEF bool sv_starts_with(sv_t sv, sv_t prefix)
{
    if (prefix.len == 0) return true;
    if (sv.len < prefix.len) return false;
    return memcmp(sv.items, prefix.items, prefix.len) == 0;
}

SBVDEF bool sv_ends_with(sv_t sv, sv_t suffix)
{
    if (suffix.len == 0) return true;
    if (sv.len < suffix.len) return false;
    return memcmp(sv.items + sv.len - suffix.len, suffix.items, suffix.len) == 0;
}

SBVDEF size_t sv_find(sv_t sv, sv_t query)
{
    if (sv.items == NULL) return SIZE_MAX;
    if (query.len > sv.len) return SIZE_MAX;
    if (sv_empty(query)) return 0;

    for (size_t i = 0; i + query.len <= sv.len; ++i) {
        if (memcmp(sv.items + i, query.items, query.len) == 0) {
            return i;
        }
    }
    return SIZE_MAX;
}

SBVDEF size_t sv_find_char(sv_t sv, char query)
{
    if (sv_empty(sv)) return SIZE_MAX;

    for (size_t i = 0; i + 1 <= sv.len; ++i) {
        if (sv.items[i] == query) return i;
    }
    return SIZE_MAX;
}

SBVDEF bool sv_contains(sv_t sv, sv_t query)
{
    return sv_find(sv, query) != SIZE_MAX;
}

SBVDEF bool sv_contains_char(sv_t sv, char query)
{
    return sv_find_char(sv, query) != SIZE_MAX;
}

SBVDEF size_t sv_cstr_size(sv_t sv)
{
    return sv.len + 1;
}

SBVDEF size_t sv_count(sv_t sv, sv_t query)
{
    if (query.len == 0 || sv.len < query.len) return 0;
    size_t count = 0;
    for (size_t i=0; i <= sv.len - query.len;){
        if (memcmp(sv.items + i, query.items, query.len) == 0){
            count += 1;
            i += query.len;
        } else{
            i += 1;
        }
    }
    return count;
}

SBVDEF size_t sv_count_char(sv_t sv, char query)
{
    size_t count = 0;
    for (size_t i=0; i<sv.len; ++i){
        if (sv.items[i] == query) count += 1;
    }
    return count;
}

SBVDEF sv_t sv_slice(sv_t sv, size_t from, size_t to)
{
    if (from > sv.len) return sv_null();
    if (to > sv.len) to = sv.len;
    if (to < from) return sv_null();
    return (sv_t){
        .items = sv.items + from,
        .len = to - from
    };
}

SBVDEF sv_t sv_chop_left(sv_t sv, size_t n)
{
    if (n >= sv.len) n = sv.len;
    return sv_from_slice(sv.items + n, sv.len - n);
}

SBVDEF sv_t sv_chop_right(sv_t sv, size_t n)
{
    if (n >= sv.len) n = sv.len;
    return sv_from_slice(sv.items, sv.len - n);
}

SBVDEF sv_t sv_split(sv_t sv, sv_t del, sv_t *rest)
{
    if (sv.items == NULL) {
        if (rest) *rest = sv_null();
        return sv_null();
    }

    if (del.len == 0 || del.items == NULL) {
        if (rest) *rest = sv_null();
        return sv;
    }

    for (size_t i = 0; i + del.len <= sv.len; ++i) {
        if (memcmp(sv.items + i, del.items, del.len) == 0) {
            if (rest) *rest = sv_from_slice(sv.items + i + del.len, sv.len - i - del.len);
            return sv_from_slice(sv.items, i);
        }
    }

    if (rest) *rest = sv_null();
    return sv;
}

SBVDEF sv_t sv_split_char(sv_t sv, char del, sv_t *rest)
{
    if (sv.items == NULL) {
        if (rest) *rest = sv_null();
        return sv_null();
    }
    for (size_t i = 0; i + 1 <= sv.len; ++i) {
        if (sv.items[i] == del) {
            if (rest) *rest = sv_from_slice(sv.items + i + 1, sv.len - i - 1);
            return sv_from_slice(sv.items, i);
        }
    }

    if (rest) *rest = sv_null();
    return sv;
}

SBVDEF size_t sv_split_count(sv_t sv, sv_t del)
{
    if (sv.items == NULL) return 0;

    if (sv.len == 0 || del.len == 0 || del.items == NULL) return 1;

    size_t count = 1;
    for (size_t i = 0; i + del.len <= sv.len; ++i) {
        if (memcmp(sv.items + i, del.items, del.len) == 0) {
            count++;
            i += del.len - 1;
        }
    }
    return count;
}

SBVDEF size_t sv_split_char_count(sv_t sv, char del)
{
    if (sv.items == NULL) return 0;
    if (sv.len == 0) return 1;
    size_t count = 1;
    for (size_t i = 0; i + 1 <= sv.len; ++i) {
        if (sv.items[i] == del) {
            count++;
        }
    }
    return count;
}

SBVDEF sv_t sv_trim(sv_t sv)
{
    return sv_trim_right(sv_trim_left(sv));
}

SBVDEF sv_t sv_trim_chars(sv_t sv, const char *chars)
{
    return sv_trim_right_chars(sv_trim_left_chars(sv, chars), chars);
}

SBVDEF sv_t sv_trim_seq(sv_t sv, sv_t seq, size_t iterations)
{
    return sv_trim_right_seq(sv_trim_left_seq(sv, seq, iterations), seq, iterations);
}

SBVDEF sv_t sv_trim_left(sv_t sv)
{
    return sv_trim_left_chars(sv, SBV_WHITESPACE);
}

SBVDEF sv_t sv_trim_right(sv_t sv)
{
    return sv_trim_right_chars(sv, SBV_WHITESPACE);
}

SBVDEF sv_t sv_trim_left_chars(sv_t sv, const char *chars)
{
    if (chars == NULL) return sv;
    size_t start = 0;
    size_t chars_count = strlen(chars);
    while (start < sv.len){
        if (memchr(chars, sv.items[start], chars_count) == NULL) break;
        start += 1;
    }
    return sv_slice(sv, start, sv.len);
}

SBVDEF sv_t sv_trim_right_chars(sv_t sv, const char *chars)
{
    if (chars == NULL) return sv;
    size_t end = sv.len;
    size_t chars_count = strlen(chars);
    while (end > 0){
        if (memchr(chars, sv.items[end-1], chars_count) == NULL) break;
        end -= 1;
    }
    return sv_slice(sv, 0, end);
}

SBVDEF sv_t sv_trim_left_seq(sv_t sv, sv_t seq, size_t iterations)
{
    if (sv.len == 0 || seq.len == 0) return sv;

    bool keep_trimming = iterations == 0;
    while (sv_starts_with(sv, seq) && (keep_trimming || iterations > 0)){
        sv = sv_slice(sv, seq.len, sv.len);
        if (!keep_trimming) iterations -= 1;
    }
    return sv;
}

SBVDEF sv_t sv_trim_right_seq(sv_t sv, sv_t seq, size_t iterations)
{
    if (sv.len == 0 || seq.len == 0) return sv;

    bool keep_trimming = iterations == 0;
    while (sv_ends_with(sv, seq) && (keep_trimming || iterations > 0)){
        sv = sv_slice(sv, 0, sv.len - seq.len);
        if (!keep_trimming) iterations -= 1;
    }
    return sv;
}

SBVDEF int sv_extract(sv_t sv, char *buff, size_t buff_size)
{
    if (buff_size == 0) return 0;
    size_t bytes_to_write = SBV_MIN(sv.len, buff_size-1);
    (void) memcpy(buff, sv.items, bytes_to_write);
    buff[bytes_to_write] = '\0';
    return bytes_to_write;
}

SBVDEF char* sv_to_cstr(sv_t sv)
{
    if (sv.len == 0 || sv.items == NULL){
        char *cstr = SBV_MALLOC(1);
        if (cstr) cstr[0] = '\0';
        return cstr;
    }

    char *cstr = SBV_MALLOC(sv.len + 1);
    if (cstr == NULL) return NULL;

    (void) memcpy(cstr, sv.items, sv.len);
    cstr[sv.len] = '\0';

    return cstr;
}

#endif // SBV_IMPLEMENTATION
