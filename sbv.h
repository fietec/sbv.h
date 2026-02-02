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
#define SV_PRINT_FORMAT "%.*s"
#define SV_PRINT_ARGS(sv) (int)(sv).len, (sv).items

// convenience macros to iterate over substrings of a string view split by a delimiter
// these macros repeatedly call sv_split*, yielding each left-hand substring in `it`
#define SV_FOREACH_SPLIT(it, sv, del) \
    for (sv_t _rest = (sv), it = sv_split(_rest, del, &_rest); \
         it.items != NULL; \
         it = sv_split(_rest, del, &_rest))

#define SV_FOREACH_SPLIT_CASE(it, sv, del) \
    for (sv_t _rest = (sv), it = sv_split_case(_rest, del, &_rest); \
         it.items != NULL; \
         it = sv_split_case(_rest, del, &_rest))

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

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/* String Builder Functions */

// create an empty string builder, same as `sb_t sb = {0}`
SBVDEF sb_t sb_null();

// append data to the string builder
// return the number of bytes appended on success, or a negative value on error.
SBVDEF int sb_appendf(sb_t *sb, const char *fmt, ...) SBV_PRINTF_FORMAT(2, 3);
SBVDEF int sb_vappendf(sb_t *sb, const char *fmt, va_list args);
SBVDEF int sb_append_cstr(sb_t *sb, const char *cstr);
SBVDEF int sb_append_slice(sb_t *sb, const char *buff, size_t n);
SBVDEF int sb_append_sv(sb_t *sb, sv_t sv);
SBVDEF int sb_append_char(sb_t *sb, char c);
SBVDEF int sb_append_null(sb_t *sb);
SBVDEF int sb_append_file(sb_t *sb, const char *filename);

// pop the last n-bytes of the string builder
// return the number of bytes popped on success, or a negative value on error
SBVDEF int sb_pop(sb_t *sb, size_t n);

// write a string builder's content as a null-terminated string into a buffer
// return the bytes written (including the null-terminator) on success, or a negative value on error
SBVDEF int sb_extract(const sb_t *sb, char *buff, size_t buff_size);
SBVDEF int sb_extract_slice(const sb_t *sb, size_t n, char *buff, size_t buff_size);

// allocate a null-terminated string storing a copy of the string builder's current content
// return the allocated string
SBVDEF char* sb_to_cstr(const sb_t *sb);

// null terminate the string builder's content and hand off owning of the content
// return the string builder's content
SBVDEF char* sb_detach(sb_t *sb);

// reserve space for additional bytes
// return success
SBVDEF bool sb_reserve(sb_t *sb, size_t bytes);

// reset the string builder
SBVDEF void sb_clear(sb_t *sb);
// reset string builder and free allocated memory
SBVDEF void sb_free(sb_t *sb);

/* String View Functions */

// create a string view
SBVDEF sv_t sv_null();
SBVDEF sv_t sv_from_slice(const char *buff, size_t n);
SBVDEF sv_t sv_from_cstr(const char *cstr);
SBVDEF sv_t sv_from_sb(const sb_t *sb);
SBVDEF sv_t sv_from_format(char *buff, size_t buff_size, const char *fmt, ...) SBV_PRINTF_FORMAT(3, 4);
SBVDEF sv_t sv_from_vformat(char *buff, size_t buff_size, const char *fmt, va_list args);

// comparison functions
SBVDEF bool sv_empty(sv_t sv);
SBVDEF bool sv_isnull(sv_t sv); // check whether the string view's items are null
SBVDEF bool sv_equals(sv_t a, sv_t b);
SBVDEF bool sv_equals_case(sv_t a, sv_t b);
SBVDEF int  sv_compare(sv_t a, sv_t b);
SBVDEF int  sv_compare_case(sv_t a, sv_t b);
SBVDEF bool sv_starts_with(sv_t sv, sv_t prefix);
SBVDEF bool sv_starts_with_case(sv_t sv, sv_t prefix);
SBVDEF bool sv_ends_with(sv_t sv, sv_t suffix);
SBVDEF bool sv_ends_with_case(sv_t sv, sv_t suffix);

// find a query within a string view
// return the index of the found query, or SIZE_MAX if not found
SBVDEF size_t sv_find(sv_t sv, sv_t query);
SBVDEF size_t sv_find_case(sv_t sv, sv_t query);
SBVDEF size_t sv_find_char(sv_t sv, char query);

// count the number of occurrences of a query within a string view
SBVDEF size_t sv_count(sv_t sv, sv_t query);
SBVDEF size_t sv_count_case(sv_t sv, sv_t query);
SBVDEF size_t sv_count_char(sv_t sv, char query);

// check for the occurrence of a query within a string view
SBVDEF bool sv_contains(sv_t sv, sv_t query);
SBVDEF bool sv_contains_case(sv_t sv, sv_t query);
SBVDEF bool sv_contains_char(sv_t sv, char query);

// return resized string view
SBVDEF sv_t sv_slice(sv_t sv, size_t from, size_t to);
SBVDEF sv_t sv_chop_left(sv_t sv, size_t n);
SBVDEF sv_t sv_chop_right(sv_t sv, size_t n);

// split a string view once by a delimiter
// return the lhs of the delimiter (or the full string view if the delimiter was not found)
// assign the rhs of the delimiter to an other string view
// you can call these in a loop until the returned sv is sv_null
// or use the SV_FOREACH_SPLIT, SV_FOREACH_SPLIT_CASE and SV_FOREACH_SPLIT_CHAR macros
SBVDEF sv_t sv_split(sv_t sv, sv_t del, sv_t *rest);
SBVDEF sv_t sv_split_case(sv_t sv, sv_t del, sv_t *rest);
SBVDEF sv_t sv_split_char(sv_t sv, char del, sv_t *rest);
SBVDEF size_t sv_split_count(sv_t sv, sv_t del);
SBVDEF size_t sv_split_case_count(sv_t sv, sv_t del);
SBVDEF size_t sv_split_char_count(sv_t sv, char del);

// trim whitespaces from a string view
SBVDEF sv_t sv_trim(sv_t sv);
// trim a set of characters from a string view
SBVDEF sv_t sv_trim_chars(sv_t sv, const char *chars);
// trim sequences from a string view, iterations-times
SBVDEF sv_t sv_trim_seq(sv_t sv, sv_t seq, size_t iterations);       // use SV_TRIM_ALL to trim as often as possible

// only trim from the left
SBVDEF sv_t sv_trim_left(sv_t sv);
SBVDEF sv_t sv_trim_left_chars(sv_t sv, const char *chars);
SBVDEF sv_t sv_trim_left_seq(sv_t sv, sv_t seq, size_t iterations);  // use SV_TRIM_ALL to trim as often as possible

// only trim from the right
SBVDEF sv_t sv_trim_right(sv_t sv);
SBVDEF sv_t sv_trim_right_chars(sv_t sv, const char *chars);
SBVDEF sv_t sv_trim_right_seq(sv_t sv, sv_t seq, size_t iterations); // use SV_TRIM_ALL to trim as often as possible

// append two string views into a buffer
// return the resulting string view, truncated if necessary, always null-terminated if buff_size > 0
SBVDEF sv_t sv_append(sv_t a, sv_t b, char* buff, size_t buff_size);
// append multiple string views into a buffer
// return the resulting string view, truncated if necessary, always null-terminated if buff_size > 0
SBVDEF sv_t sv_append_many(const sv_t *svs, size_t count, char *buff, size_t buff_size);
// compute the total length of multiple string views concatenated
SBVDEF size_t sv_append_many_len(const sv_t *svs, size_t count);

// join multiple string views with a separator into a buffer
// return the resulting string view, truncated if necessary, always null-terminated if buff_size > 0
SBVDEF sv_t sv_join(const sv_t *svs, size_t count, sv_t sep, char *buff, size_t buff_size);
// compute the total length of a join operation with a separator
SBVDEF size_t sv_join_len(const sv_t *svs, size_t count, sv_t sep);

// replace all occurrences of a query in a string view and write into a buffer
// return the resulting string view, truncated if necessary, always null-terminated if buff_size > 0
SBVDEF sv_t sv_replace(sv_t sv, sv_t query, sv_t replace, char *buff, size_t buff_size);
// compute the total length of a replacement operation
SBVDEF size_t sv_replace_len(sv_t sv, sv_t query, sv_t replace);

// write a string view's content as a null-terminated string into a buffer
// return the amount of bytes written (including the null-terminator), or a negative value on error
SBVDEF int sv_extract(sv_t sv, char *buff, size_t buff_size);

// return the size of a buffer that could hold a null-terminated copy of the string view's content
SBVDEF size_t sv_cstr_size(sv_t sv);

// allocate a null-terminated string storing a copy of the string view's current content
// return the allocated string
SBVDEF char* sv_to_cstr(sv_t sv);

/* Helper Functions */

// case-insensitive libc `memcmp`
SBVDEF int sbv_memicmp(const void *a, const void *b, size_t n);
// custom implementation of `strdup`
SBVDEF char* sbv_strdup(const char *string);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SBV_H

#ifdef SBV_IMPLEMENTATION

SBVDEF int sbv_memicmp(const void *a, const void *b, size_t n)
{
    const unsigned char *pa = (const unsigned char *)a;
    const unsigned char *pb = (const unsigned char *)b;

    for (size_t i = 0; i < n; ++i) {
        unsigned char ca = pa[i];
        unsigned char cb = pb[i];

        if (ca >= 'A' && ca <= 'Z') ca += 'a' - 'A';
        if (cb >= 'A' && cb <= 'Z') cb += 'a' - 'A';

        if (ca != cb) return (int)ca - (int)cb;
    }
    return 0;
}

SBVDEF char* sbv_strdup(const char *string)
{
    if (string == NULL) return NULL;

    size_t string_len = strlen(string);

    char *new_string = SBV_MALLOC(string_len + 1);
    if (new_string == NULL) return NULL;

    (void) memcpy(new_string, string, string_len);
    new_string[string_len] = '\0';

    return new_string;
}

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
    if (bytes > SIZE_MAX - sb->count - 1) return false;
    size_t required = sb->count + bytes + 1;
    size_t capacity = sb->capacity ? sb->capacity : SB_INIT_CAPACITY;
    while (capacity < required){
        if (capacity == SIZE_MAX) return false;
        if (capacity > SIZE_MAX / 2){
            capacity = SIZE_MAX;
        } else{
            capacity *= 2;
        }
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
    if (cstr == NULL) return -1;
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

SBVDEF int sb_append_file(sb_t *sb, const char *filename)
{
    if (sb == NULL || filename == NULL) return -1;

    FILE *file = fopen(filename, "rb");
    if (file == NULL) return -1;

    char buffer[4096];
    size_t n;
    size_t bytes_written = 0;

    while ((n = fread(buffer, 1, sizeof(buffer), file)) > 0){
        int bytes = sb_append_slice(sb, buffer, n);
        if (bytes == -1){
            sb_pop(sb, bytes_written);
            fclose(file);
            return -1;
        }
        bytes_written += (size_t) bytes;
    }
    if (ferror(file)){
        sb_pop(sb, bytes_written);
        fclose(file);
        return -1;
    }
    fclose(file);
    return bytes_written;
}

SBVDEF int sb_pop(sb_t *sb, size_t n)
{
    if (sb == NULL) return -1;

    size_t bytes = SBV_MIN(sb->count, n);
    sb->count -= bytes;
    return bytes;
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
    return bytes_to_write + 1;
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

SBVDEF sv_t sv_from_format(char *buff, size_t buff_size, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    sv_t result = sv_from_vformat(buff, buff_size, fmt, args);

    va_end(args);
    return result;
}

SBVDEF sv_t sv_from_vformat(char *buff, size_t buff_size, const char *fmt, va_list args)
{
    if (buff == NULL || fmt == NULL) return sv_null();
    if (buff_size == 0) return sv_from_slice(buff, 0);

    int bytes_written = vsnprintf(buff, buff_size, fmt, args);
    size_t total_bytes = (bytes_written < 0)? 0 : SBV_MIN((size_t) bytes_written, buff_size-1);
    buff[total_bytes] = '\0';
    return sv_from_slice(buff, total_bytes);
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
    if (a.len == 0) return true;
    return sbv_memicmp(a.items, b.items, a.len) == 0;
}

SBVDEF int sv_compare(sv_t a, sv_t b)
{
    if (sv_isnull(a) && sv_isnull(b)) return 0;
    if (sv_isnull(a)) return -1;
    if (sv_isnull(b)) return 1;

    size_t n = SBV_MIN(a.len, b.len);
    int result = memcmp(a.items, b.items, n);
    if (result != 0) return result;
    return (a.len < b.len) ? -1 : (a.len > b.len);
}

SBVDEF int sv_compare_case(sv_t a, sv_t b)
{
    if (sv_isnull(a) && sv_isnull(b)) return 0;
    if (sv_isnull(a)) return -1;
    if (sv_isnull(b)) return 1;

    size_t n = SBV_MIN(a.len, b.len);
    int result = sbv_memicmp(a.items, b.items, n);
    if (result != 0) return result;
    return (a.len < b.len) ? -1 : (a.len > b.len);
}

SBVDEF bool sv_starts_with(sv_t sv, sv_t prefix)
{
    if (prefix.len == 0) return true;
    if (sv.len < prefix.len) return false;
    return memcmp(sv.items, prefix.items, prefix.len) == 0;
}

SBVDEF bool sv_starts_with_case(sv_t sv, sv_t prefix)
{
    if (prefix.len == 0) return true;
    if (sv.len < prefix.len) return false;
    return sbv_memicmp(sv.items, prefix.items, prefix.len) == 0;
}

SBVDEF bool sv_ends_with(sv_t sv, sv_t suffix)
{
    if (suffix.len == 0) return true;
    if (sv.len < suffix.len) return false;
    return memcmp(sv.items + sv.len - suffix.len, suffix.items, suffix.len) == 0;
}

SBVDEF bool sv_ends_with_case(sv_t sv, sv_t suffix)
{
    if (suffix.len == 0) return true;
    if (sv.len < suffix.len) return false;
    return sbv_memicmp(sv.items + sv.len - suffix.len, suffix.items, suffix.len) == 0;
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

SBVDEF size_t sv_find_case(sv_t sv, sv_t query)
{
    if (sv.items == NULL || query.len > sv.len) return SIZE_MAX;
    if (sv_empty(query)) return 0;

    for (size_t i=0; i+query.len <= sv.len; ++i){
        if (sbv_memicmp(sv.items + i, query.items, query.len) == 0) return i;
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

SBVDEF bool sv_contains_case(sv_t sv, sv_t query)
{
    return sv_find_case(sv, query) != SIZE_MAX;
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

SBVDEF size_t sv_count_case(sv_t sv, sv_t query)
{
    if (query.len == 0 || sv.len < query.len) return 0;
    size_t count = 0;
    for (size_t i=0; i <= sv.len - query.len;){
        if (sbv_memicmp(sv.items + i, query.items, query.len) == 0){
            count += 1;
            i += query.len;
        } else {
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

    for (size_t i=0; i + del.len <= sv.len; ++i) {
        if (memcmp(sv.items + i, del.items, del.len) == 0) {
            if (rest) *rest = sv_from_slice(sv.items + i + del.len, sv.len - i - del.len);
            return sv_from_slice(sv.items, i);
        }
    }

    if (rest) *rest = sv_null();
    return sv;
}

SBVDEF sv_t sv_split_case(sv_t sv, sv_t del, sv_t *rest)
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
        if (sbv_memicmp(sv.items + i, del.items, del.len) == 0) {
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
    for (size_t i=0; i < sv.len; ++i) {
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

SBVDEF size_t sv_split_case_count(sv_t sv, sv_t del)
{
    if (sv.items == NULL) return 0;

    if (sv.len == 0 || del.len == 0 || del.items == NULL) return 1;

    size_t count = 1;
    for (size_t i = 0; i + del.len <= sv.len; ++i) {
        if (sbv_memicmp(sv.items + i, del.items, del.len) == 0) {
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

SBVDEF sv_t sv_append(sv_t a, sv_t b, char *buff, size_t buff_size)
{
    if (buff == NULL) return sv_null();
    if (buff_size == 0) return sv_from_slice(buff, 0);
    sv_t svs[] = {a, b};
    return sv_append_many(svs, sizeof(svs)/sizeof(svs[0]), buff, buff_size);
}

SBVDEF sv_t sv_append_many(const sv_t *svs, size_t count, char *buff, size_t buff_size)
{
    if (buff == NULL || svs == NULL) return sv_null();
    if (buff_size == 0) return sv_from_slice(buff, 0);

    size_t bytes_used = 0;

    for (size_t i=0; i<count; ++i){
        int bytes_written = sv_extract(svs[i], buff + bytes_used, buff_size - bytes_used);
        if (bytes_written < 0) break;
        bytes_used += (size_t) bytes_written - 1;
        if (bytes_used >= buff_size - 1) break;
    }
    size_t total_bytes = SBV_MIN(bytes_used, buff_size - 1);
    buff[total_bytes] = '\0';
    return sv_from_slice(buff, total_bytes);
}

SBVDEF size_t sv_append_many_len(const sv_t *svs, size_t count)
{
    if (svs == NULL || count == 0) return 0;
    size_t total_len = 0;
    for (size_t i=0; i<count; ++i){
        total_len += svs[i].len;
    }
    return total_len;
}

SBVDEF sv_t sv_join(const sv_t *svs, size_t count, sv_t sep, char *buff, size_t buff_size)
{
    if (buff == NULL || svs == NULL) return sv_null();
    if (buff_size == 0) return sv_from_slice(buff, 0);

    size_t bytes_used = 0;
    for (size_t i=0; i<count; ++i){
        sv_t it = svs[i];
        int bytes_written = sv_extract(it, buff + bytes_used, buff_size - bytes_used);
        if (bytes_written < 0) break;
        bytes_used += (size_t) bytes_written - 1;
        if (bytes_used >= buff_size - 1) break;

        if (i < count-1){
            bytes_written = sv_extract(sep, buff + bytes_used, buff_size - bytes_used);
            if (bytes_written < 0) break;
            bytes_used += (size_t) bytes_written - 1;
            if (bytes_used >= buff_size - 1) break;
        }
    }
    size_t total_bytes = SBV_MIN(bytes_used, buff_size - 1);
    buff[total_bytes] = '\0';
    return sv_from_slice(buff, total_bytes);
}

SBVDEF size_t sv_join_len(const sv_t *svs, size_t count, sv_t sep)
{
    if (svs == NULL || count == 0) return 0;
    size_t total_len = 0;
    for (size_t i=0; i<count-1; ++i){
        total_len += svs[i].len + sep.len;
    }
    return total_len + svs[count-1].len;
}

SBVDEF sv_t sv_replace(sv_t sv, sv_t query, sv_t replace, char *buff, size_t buff_size)
{
    if (buff == NULL) return sv_null();
    if (buff_size == 0) return sv_from_slice(buff, 0);

    size_t bytes_used = 0;
    for (sv_t rest = sv, part = sv_split(rest, query, &rest); part.items != NULL; part = sv_split(rest, query, &rest)){

        int bytes_written = sv_extract(part, buff + bytes_used, buff_size - bytes_used);
        if (bytes_written < 0) break;
        bytes_used += (size_t) bytes_written - 1;
        if (bytes_used >= buff_size - 1) break;

        if (!sv_isnull(rest)){
            bytes_written = sv_extract(replace, buff + bytes_used, buff_size - bytes_used);
            if (bytes_written < 0) break;
            bytes_used += (size_t) bytes_written - 1;
            if (bytes_used >= buff_size - 1) break;
        }
    }
    size_t total_bytes = SBV_MIN(bytes_used, buff_size - 1);
    buff[total_bytes] = '\0';
    return sv_from_slice(buff, total_bytes);
}

SBVDEF size_t sv_replace_len(sv_t sv, sv_t query, sv_t replace)
{
    size_t total_len = 0;
    for (sv_t rest = sv, part = sv_split(rest, query, &rest); part.items != NULL; part = sv_split(rest, query, &rest)){
        total_len += part.len;
        if (!sv_isnull(rest)){
            total_len += replace.len;
        }
    }
    return total_len;
}

SBVDEF int sv_extract(sv_t sv, char *buff, size_t buff_size)
{
    if (buff == NULL || sv_isnull(sv)) return -1;
    if (buff_size == 0) return 0;
    size_t bytes_to_write = SBV_MIN(sv.len, buff_size-1);
    (void) memcpy(buff, sv.items, bytes_to_write);
    buff[bytes_to_write] = '\0';
    return bytes_to_write + 1;
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
