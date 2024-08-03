
#include "pre_assembler.h"

static bool is_macro_definition(char *line) {
    return (strncmp(line, "macr ", DEFINE_SEQUENCE_LEN) == 0);
}

static bool is_macro_call(char *line, macro_table *table) {
    char macro_name[MAX_MACRO_NAME_LENGTH] = { '\0' };

    /*
    * We use 'sscanf' to read input from a specific string
    * 'sscanf' will read the first word from 'line' into 'macro_name'
    */
    sscanf(line, "%s", macro_name);
    return (get_macro(table, macro_name) != NULL);
}

static status add_macro_to_table(char *macro_name, FILE *as_file, macro_table *table) {
    macro *new_macro;
    char line[BUFSIZ];
    status result = create_macro(macro_name, &new_macro);

    if (result != STATUS_OK) return result;

    while (fgets(line, sizeof(line), as_file)) {
        if (strncmp(line, "endmacr", 7) == 0) break;
        result = insert_line_to_macro(new_macro, line);
        if (result != STATUS_OK) return result;
    }

    if (insert_macro_to_table(table, new_macro) != STATUS_OK) {
        macro_destructor(&new_macro);
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

static status expand_macro(char *macro_name, FILE *am_file, macro_table *table) {
    int i;
    macro *m = get_macro(table, macro_name);
    if (m == NULL) return STATUS_ERROR_MACRO_NOT_FOUND;
    for (i = 0;i < m->line_count;i++) fprintf(am_file, "%s", m->lines[i]);
    return STATUS_OK;
}

static status pre_assemble(char *as_filename, char *am_filename, macro_table *m_table, keyword *keyword_table) {
    FILE *as_file = NULL, *am_file = NULL;
    char first_word[MAX_LINE_LENGTH] = { '\0' };
    char macro_name[MAX_LINE_LENGTH] = { '\0' };
    macro *macroname_found_flag = NULL;
    status result = STATUS_ERROR;
    syntax_state *state = NULL;

    state = create_syntax_state();

    if (state == NULL) {
        return STATUS_ERROR_MEMORY_ALLOCATION;
    }

    state->line_number++;

    if (remove_whitespace(as_filename) != STATUS_OK) {
<<<<<<< HEAD
        printf("*** ERROR ***\nError while removing whitespace from %s\nExiting...\n", as_filename);
        free(as_filename);
        free(am_filename);
=======
        printf("Error while removing whitespace from %s\nExiting...\n", as_filename);
        destroy_syntax_state(&state);
>>>>>>> refs/remotes/origin/master
        return STATUS_ERROR;
    }

    as_file = fopen(as_filename, "r");
    if (as_file == NULL) {
<<<<<<< HEAD
        printf("*** ERROR ***\nCould not open file called: %s\nExiting...", as_filename);
        free(as_filename);
        free(am_filename);
=======
        printf("Error: Could not open file called: %s\nExiting...", as_filename);
        destroy_syntax_state(&state);
>>>>>>> refs/remotes/origin/master
        return STATUS_ERROR_OPEN_SRC;
    }

    am_file = fopen(am_filename, "w");
    if (am_file == NULL) {
        printf("*** ERROR ***\nCould not open file called: %s\nExiting...", am_filename);
        fclose(as_file);
        destroy_syntax_state(&state);
        return STATUS_ERROR_OPEN_DEST;
    }

    while (fgets(state->buffer, MAX_LINE_LENGTH, as_file)) {/* Primary parsing loop */

        state->line_number++;

        state->buffer_without_offset = state->buffer;

        state->buffer[strcspn(state->buffer, "\n")] = '\0'; /* Null terminate line */
        sscanf(state->buffer, "%s", first_word); /* Extract first word in the line */

        /* Check if the current line is a macro call. If so, copy the expanded macro lines to the .am file*/
        if (is_macro_call(state->buffer, m_table)) {
            if (expand_macro(first_word, am_file, m_table) != STATUS_OK) {
<<<<<<< HEAD
                printf("*** ERROR ***\nError while expanding macro %s. Exiting...\n", first_word);
=======
                printf("Error on line %d: '%s':", state->line_number, state->buffer_without_offset);
                printf("Could not expand the macro '%s' \n", first_word);
>>>>>>> refs/remotes/origin/master
                fclose(as_file);
                fclose(am_file);
                macro_table_destructor(&m_table);
                state->buffer = state->buffer_without_offset;
                state->buffer_without_offset = NULL;
                destroy_syntax_state(&state);
                return STATUS_ERROR;
            }
        }

        /* Check if the current line is a macro definition. If so, add macro to the macro table */
        else if (is_macro_definition(state->buffer)) {

            sscanf(state->buffer, "macr %s", macro_name); /* Extract 2nd word wich is the macro name */

            /*
             Check if definition is valid.
             If the macro name is found in the macro table, exit on error- redifinition is not allowed.
             */
            macroname_found_flag = get_macro(m_table, macro_name);
            if (macroname_found_flag != NULL) {
<<<<<<< HEAD
                printf("*** ERROR ***\nMacro with the name '%s' is already defined. Exiting...\n", macro_name);
=======
                printf("Error in file '%s' on line %d: '%s': ", as_filename, state->line_number, state->buffer_without_offset);
                printf("Macro with the name '%s' is already defined\n", macro_name);
>>>>>>> refs/remotes/origin/master
                macro_table_destructor(&m_table);
                state->buffer = state->buffer_without_offset;
                state->buffer_without_offset = NULL;
                destroy_syntax_state(&state);
                fclose(as_file);
                fclose(am_file);
                return STATUS_ERROR_MACRO_REDEFINITION;
            }

            if (get_keyword_by_name(keyword_table, macro_name) != NULL) {
                printf("Error in file '%s' on line %d: '%s': ", as_filename, state->line_number, state->buffer_without_offset);
                printf("Trying to define a macro with the same name as the keyword '%s'\n", macro_name);
                macro_table_destructor(&m_table);
                state->buffer = state->buffer_without_offset;
                state->buffer_without_offset = NULL;
                destroy_syntax_state(&state);
                fclose(as_file);
                fclose(am_file);
                return STATUS_ERROR_MACRO_NAMED_AS_KEYWORD;
            }

            /* Is a valid definition of a new macro */
            result = add_macro_to_table(macro_name, as_file, m_table);

            if (result != STATUS_OK) {
<<<<<<< HEAD
                printf("*** ERROR ***\nError while adding macro: %s to macro table. Exiting...\n", macro_name);
=======
                printf("Error in file '%s' on line %d: '%s': ", as_filename, state->line_number, state->buffer_without_offset);
                printf("Could not add macro '%s' to macro table\n", macro_name);
>>>>>>> refs/remotes/origin/master
                macro_table_destructor(&m_table);
                state->buffer = state->buffer_without_offset;
                state->buffer_without_offset = NULL;
                destroy_syntax_state(&state);
                fclose(as_file);
                fclose(am_file);
                return result;
            }
        }
        /* Neither a macro call or definition */
        else fprintf(am_file, "%s\n", state->buffer); /* Write the line to the .am file */

        reset_syntax_state(state);
    }

    /*Clean up allocated memory*/
    destroy_syntax_state(&state);
    fclose(as_file);
    fclose(am_file);

    return STATUS_OK;

}

macro_table *fill_macro_table(int argc, char *argv[], char ***am_filenames, keyword *keyword_table) {
    macro_table *m_table = NULL;
    status result = STATUS_ERROR;
    size_t i = 0;
    size_t file_amount = argc - 1;
    char **as_filenames = NULL;
    char **backup_filenames = NULL;
    char **generic_filenames = argv + 1;


    if (file_amount < 1) {
        printf("Usage: %s <filename>\n", argv[0]);
        return NULL;
    }

    as_filenames = (char **)calloc(file_amount, sizeof(char *));
    if (as_filenames == NULL) {
        generic_filenames = NULL;
        return NULL;
    }

    backup_filenames = (char **)calloc(file_amount, sizeof(char *));
    if (backup_filenames == NULL) {
        delete_filenames(file_amount, &as_filenames);
        as_filenames = NULL;
        generic_filenames = NULL;
        return NULL;
    }

    *(am_filenames) = (char **)calloc(file_amount, sizeof(char *));
    if (*(am_filenames) == NULL) {
        delete_filenames(file_amount, &as_filenames);
        delete_filenames(file_amount, &backup_filenames);
        delete_filenames(file_amount, am_filenames);
        generic_filenames = NULL;
        as_filenames = NULL;
        backup_filenames = NULL;
        return NULL;
    }

    for (i = 0;i < file_amount;i++) {

        as_filenames[i] = create_file_name(generic_filenames[i], ".as");

        if (as_filenames[i] == NULL) {
            printf("*** ERROR ***\n .as file creation for %s did not execute properly.\nExiting...\n", generic_filenames[i]);
            delete_filenames(file_amount, &as_filenames);
            delete_filenames(file_amount, &backup_filenames);
            delete_filenames(file_amount, am_filenames);
            generic_filenames = NULL;
            as_filenames = NULL;
            backup_filenames = NULL;
            return NULL;
        }
    }

    if (duplicate_files(&backup_filenames, file_amount, as_filenames, ".bk") != STATUS_OK) {
        printf("*** ERROR ***\nFile backup did not execute properly. Exiting..");
        delete_filenames(file_amount, &as_filenames);
        delete_filenames(file_amount, &backup_filenames);
        delete_filenames(file_amount, am_filenames);
        generic_filenames = NULL;
        as_filenames = NULL;
        backup_filenames = NULL;
        return NULL;
    }

    for (i = 0;i < file_amount;i++) {
        *(am_filenames)[i] = create_file_name(generic_filenames[i], ".am");

        if (*(am_filenames)[i] == NULL) {
            printf("*** ERROR ***\n.am file creation for the file '%s.as' did not execute properly.\nExiting...\n", generic_filenames[i]);
            delete_filenames(file_amount, &as_filenames);
            delete_filenames(file_amount, &backup_filenames);
            delete_filenames(file_amount, am_filenames);
            generic_filenames = NULL;
            as_filenames = NULL;
            backup_filenames = NULL;
            return NULL;
        }
    }



    m_table = create_macro_table();
    if (m_table == NULL) {
        printf("*** ERROR ***\nCould not create macro table. Exiting...\n");
        delete_filenames(file_amount, &as_filenames);
        delete_filenames(file_amount, &backup_filenames);
        delete_filenames(file_amount, am_filenames);
        generic_filenames = NULL;
        as_filenames = NULL;
        backup_filenames = NULL;
        return NULL;
    }
    for (i = 0;i < file_amount;i++) {

        result = pre_assemble(as_filenames[i], (*am_filenames)[i], m_table, keyword_table);


<<<<<<< HEAD
        switch (result) {
        case STATUS_OK:
            printf("Pre-assembly of '%s' completed successfully.\n", as_filenames[i]);
            break;
        case STATUS_ERROR_OPEN_SRC:
            printf("*** ERROR ***\nCould not open source file.\n");
            break;
        case STATUS_ERROR_OPEN_DEST:
            printf("*** ERROR ***\nCould not open destination file.\n");
            break;
        case STATUS_ERROR_READ:
            printf("*** ERROR ***\nCould not read from source file.\n");
            break;
        case STATUS_ERROR_WRITE:
            printf("*** ERROR ***\nCould not write to destination file.\n");
            break;
        case STATUS_ERROR_MACRO_REDEFINITION:
            printf("*** ERROR ***\nMacro redefinition detected.\n");
            break;
        case STATUS_ERROR_MEMORY_ALLOCATION:
            printf("*** ERROR ***\nMemory allocation failed.\n");
            break;
        case STATUS_ERROR_MACRO_NOT_FOUND:
            printf("*** ERROR ***\nMacro not found.\n");
            break;
        default:
            printf("*** ERROR ***\nUnknown error.\n");
=======
        if (result != STATUS_OK) {
>>>>>>> refs/remotes/origin/master
            delete_filenames(file_amount, &as_filenames);
            delete_filenames(file_amount, &backup_filenames);
            delete_filenames(file_amount, am_filenames);
            generic_filenames = NULL;
            as_filenames = NULL;
            backup_filenames = NULL;
            return NULL;
        }
    }

    for (i = 0;i < file_amount;i++) {
        remove(as_filenames[i]);
        rename(backup_filenames[i], strcat(generic_filenames[i], ".as"));
    }

    delete_filenames(file_amount, &as_filenames);
    delete_filenames(file_amount, &backup_filenames);
    generic_filenames = NULL;
    as_filenames = NULL;
    backup_filenames = NULL;
    return m_table;
}

