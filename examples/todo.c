#include <stdio.h>

#define SBV_IMPLEMENTATION
#include "../sbv.h"

int main(int argc, const char *argv[])
{
    if (argc < 2){
        fprintf(stderr, "[ERROR] missing input file!\n");
        return 1;
    }

    const char *filename = argv[1];

    sb_t sb = {0};
    int content_length = sb_append_file(&sb, filename);
    if (content_length == -1){
        fprintf(stderr, "[ERROR] could not read file '%s'!\n", filename);
        return 1;
    }

    sv_t sv = sv_from_sb(&sb);
    sv_t todo = sv_from_cstr("TODO:");
    size_t line_number = 0;
    
    SV_FOREACH_SPLIT_CHAR(line, sv, '\n'){
        line_number += 1;
        sv_t body;
        sv_split(line, todo, &body);

        if (sv_empty(body)) continue;
        body = sv_trim(body);

        size_t column = body.items - line.items + 1;
        
        printf("%s:%zu:%zu: '"SV_PRINT_FORMAT"'\n", filename, line_number, column, SV_PRINT_ARGS(sv_trim(line)));
    }
    
    sb_free(&sb);
    return 0;
}
