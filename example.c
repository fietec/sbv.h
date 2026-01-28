#include <stdio.h>

#define SBV_IMPLEMENTATION
#include "sbv.h"

int main(void) {
    /* --- String builder basics --- */
    sb_t sb = sb_null();

    sb_appendf(&sb, "Hello %s", "world");
    sb_append_char(&sb, '!');
    sb_append_char(&sb, '\n');

    const char raw[] = "Raw bytes";
    sb_append_slice(&sb, raw, sizeof(raw) - 1);

    /* Convert to C string (copy) */
    char *cstr = sb_to_cstr(&sb);
    printf("sb_to_cstr:\n%s\n\n", cstr);
    SBV_FREE(cstr);

    /* Detach buffer (takes ownership) */
    char *detached = sb_detach(&sb);
    printf("sb_detach:\n%s\n\n", detached);
    SBV_FREE(detached);

    /* --- String view usage --- */
    sv_t text   = sv_from_cstr("one,two,three");
    sv_t comma  = sv_from_cstr(",");
    size_t pos  = sv_find(text, comma);

    sv_t left  = sv_slice(text, 0, pos);
    sv_t right = sv_chop_left(text, pos + 1);

    printf("sv_find: %zu\n", pos);
    printf("left:  '%.*s'\n", (int)left.len, left.items);
    printf("right: '%.*s'\n\n", (int)right.len, right.items);

    /* Prefix / suffix checks */
    printf("starts with 'one'? %d\n",
           sv_starts_with(text, sv_from_cstr("one")));
    printf("ends with 'three'? %d\n",
           sv_ends_with(text, sv_from_cstr("three")));

    return 0;
}
