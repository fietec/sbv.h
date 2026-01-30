#include <stdio.h>

#define SBV_IMPLEMENTATION
#include "../sbv.h"

int main(void) {
    // --- Build a dynamic string ---
    sb_t sb = sb_null();
    sb_appendf(&sb, "  Hello, world! Welcome to sbv library demo.  ");
    
    printf("Original string: '%s'\n", sb_to_cstr(&sb));

    // --- Convert sb_t to sv_t ---
    sv_t sv = sv_from_sb(&sb);
    
    // --- Trim whitespace ---
    sv_t trimmed = sv_trim(sv);
    char *trimmed_cstr = sv_to_cstr(trimmed);
    printf("Trimmed string: '%s'\n", trimmed_cstr);

    // --- Split words by space ---
    printf("Words:\n");
    SV_FOREACH_SPLIT_CHAR(word, trimmed, ' ') {
        sv_t w = sv_trim(word); // trim individual word
        char *w_cstr = sv_to_cstr(w);
        if (w_cstr[0] != '\0') { // ignore empty splits
            printf(" - %s\n", w_cstr);
        }
        SBV_FREE(w_cstr);
    }

    // --- Count occurrences of 'o' ---
    size_t o_count = sv_count_char(trimmed, 'o');
    printf("Number of 'o' characters: %zu\n", o_count);

    // --- Free resources ---
    sb_free(&sb);
    SBV_FREE(trimmed_cstr);

    return 0;
}
