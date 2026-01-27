#include <stdio.h>

#define SB_IMPLEMENTATION
#include "sb.h"

int main(void)
{
    sb_t sb = {0};

    // Append formatted text
    sb_appendf(&sb, "Hello %s!", "world");
    sb_append_char(&sb, '\n');

    // Append raw bytes (does not require null-termination)
    const char raw[] = { 'A', 'B', 'C' };
    sb_append_slice(&sb, raw, sizeof(raw));

    sb_append_char(&sb, '\n');

    // Append more formatted data
    for (int i = 0; i < 3; i++) {
        sb_appendf(&sb, "value[%d] = %d\n", i, i * 10);
    }

    // Option 1: extract into an existing buffer
    char buffer[128];
    sb_extract(&sb, buffer, sizeof(buffer));
    printf("Extracted:\n%s\n", buffer);

    // Option 2: duplicate as a heap string
    char *copy = sb_to_cstr(&sb);
    if (copy) {
        printf("Duplicated:\n%s\n", copy);
        free(copy);
    }

    // Option 3: detach ownership (no copy)
    char *owned = sb_detach(&sb);
    if (owned) {
        printf("Detached:\n%s\n", owned);
        free(owned);
    }

    // Builder is now empty and reusable
    // Option 4: use in-place
    sb_appendf(&sb, "Builder reused.\n");
    sb_append_null(&sb);
    printf("After reset:\n%s\n", sb.items);

    // free allocated memory and reset
    sb_free(&sb);

    return 0;
}
