
#include "pre_assembler.h"
#define DEFINE_SEQUENCE_LEN 5

static bool is_macro_definition(const char *line) {
    return strncmp(line, "macr ", DEFINE_SEQUENCE_LEN);
}

static bool is_macro_call(const char *line, macro_table *table) {
    char macro_name[MAX_MACRO_NAME_LENGTH] = { '\0' };

    /*
    * We use 'sscanf' to read input from a specific string
    * 'sscanf' will read the first word from 'line' into 'macro_name'
    */
    sscanf(line, "%s", macro_name);
    return (find_macro_in_table(table, macro_name));
}

static status add_macro_to_table(const char *macro_name, FILE *as_file, macro_table *table) {
    macro *new_macro = create_macro(macro_name);
    char line[BUFSIZ];

    while (fgets(line, sizeof(line), as_file)) {
        if (strncmp(line, "endmacr", 7) == 0) break;
        insert_line_to_macro(new_macro, line);
    }

    insert_macro_to_table(table, new_macro);
}

void pre_assemble(char *filename) {

}
