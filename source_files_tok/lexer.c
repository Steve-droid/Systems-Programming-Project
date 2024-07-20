
#include "lexer.h"
#define MIN_ARGS 2
#define FIRST_WORD 0
#define SECOND_WORD 1
#define THIRD_WORD 2
#define FIRST 0
#define SECOND 1
#define THIRD 2
#define LAST_OPCODE_BIT 11
#define LAST_SRC_METHOD_BIT 7
#define LAST_DEST_METHOD_BIT 3
#define ARE_A_BIT 2
#define ARE_R_BIT 1
#define ARE_E_BIT 0
#define LAST_BIT 14
#define FIRST_BIT 0
#define CMD_NAME 0

static status generate_tokens(buffer_data *_buffer_data, inst *_inst, keyword *keyword_table, label_table *_label_table);
static status process_args_unknown_amount(buffer_data *_buffer_data, inst *_inst, keyword *keyword_table, label_table *_label_table);
static validation_state process_data_command(syntax_state *state, buffer_data *_buffer_data, inst *_inst, label_table *_label_table);
static validation_state process_string_command(syntax_state *state, buffer_data *_buffer_data, inst *_inst, label_table *_label_table);
static validation_state process_entry_extern_command(syntax_state *state, buffer_data *_buffer_data, inst *_inst);
static validation_state update_label_table(syntax_state *state, buffer_data *_buffer_data, inst *_inst, label_table *label_table, keyword *keyword_table);
static status process_args_known_amount(buffer_data *_buffer_data, inst *_inst, label_table *_label_table);
static validation_state validate_data_members(char *data_buffer);
static bool continue_reading(char *instruction_buffer, syntax_state *state);
static void update_command(syntax_state *state, keyword *keyword_table, int command_key);
static validation_state validate_known_argument_data(char *str, label_table *_label_table);
static validation_state validate_addressing_methods(bin_word *binary_command, keyword *keyword_table, int cmd_key);
static validation_state validate_source_addressing(bin_word *first_bin_word, keyword *keyword_table, int cmd_key);
static validation_state validate_destination_addressing(bin_word *binary_command, keyword *keyword_table, int cmd_key);


static void handle_undef_cmd_name(buffer_data *data) {
    destroy_buffer_data(data);
}

syntax_state *initialize_state(syntax_state *data, char *_instruction) {
    data->instruction = _instruction;
    data->instruction_args = NULL;
    data->comma = false;
    data->whitespace = false;
    data->null_terminator = false;
    data->new_line = false;
    data->minus_sign = false;
    data->plus_sign = false;
    data->end_of_argument_by_space = false;
    data->end_of_argument = false;
    data->end_of_string = false;
    data->first_quatiotion_mark = false;
    data->last_quatiotion_mark = false;
    data->digit = false;
    data->is_data = false;
    data->is_string = false;
    data->is_entry = false;
    data->is_extern = false;
    return data;
}

void initialize_command(syntax_state *data) {
    data->is_data = false;
    data->is_string = false;
    data->is_entry = false;
    data->is_extern = false;
}






/**
 * @brief Performs the first pass of the assembly process.
 *
 * This function reads an assembly-like file, processes each line to identify commands and arguments,
 * and converts them into a pre-decoded and then a binary format. The results are stored in a 2D array.
 *
 * @param am_filename The name of the assembly file to process.
 * @param _label_table The table containing label information.
 * @param keyword_table The table containing keyword information.
 * @return A 2D array containing the binary representation of each command and its arguments.
 */
 /*Decode without label addresses*/
inst_table *lex(char *am_filename, label_table *_label_table, keyword *keyword_table) {
    buffer_data *data = NULL;
    char *buffer = NULL;
    inst *_inst = NULL;
    inst_table *_inst_table = NULL;
    FILE *file = NULL;
    size_t i;
    status _status = STATUS_ERROR;
    size_t tk_amount_to_allocate = 0;


    file = fopen(am_filename, "r");
    if (file == NULL) {
        printf("Error while opening .am file before lexing. Exiting...\n");
        return NULL;
    }

    buffer = (char *)malloc(sizeof(char) * MAX_LINE_LENGTH);
    if (buffer == NULL) {
        printf("Error while allocating memory for buffer. Exiting...\n");
        fclose(file);
        return NULL;
    }

    if (create_buffer_data(&data) != STATUS_OK) {
        printf("ERROR- Failed to create buffer data\n");
        return NULL;
    }

    data->buffer = buffer;



    /* Create an instance of an instruction table */
    if (create_instruction_table(&_inst_table) != STATUS_OK) {
        printf("ERROR- Failed to create an instance of the instruction table\n");
        return NULL;
    }



    printf("ASSEMBLY INSTRUCTIONS:\n");

    while (fgets(data->buffer, MAX_LINE_LENGTH, file)) { /* Read every line */
        if (create_instruction(&_inst) != STATUS_OK) {
            printf("ERROR- Failed to create an instance of the instruction\n");
            return NULL;
        }

        /* Skip empty lines */
        if (is_empty_line(data->buffer)) {
            continue;
        }

        /* Remove leading spaces */
        data->buffer = trim_whitespace(data->buffer);

        /* Skip comment lines */
        if (data->buffer[0] == ';') {
            continue;
        }

        /* If the line is not a comment, count it as an assembly line */
        data->line_counter++;

        /* Insert the line number to the instruction */
        _inst->line_number = data->line_counter;

        /* If the instruction starts with a label definition, skip it */
        data->buffer = skip_label_name(data, _label_table);

        /* Skip unnecessary spaces */
        data->buffer = trim_whitespace(data->buffer);

        /* Identify the command key from the keyword table */
        data->command_key = identify_command(data, _label_table, keyword_table);

        /* If the command does not exist, exit */
        if (data->command_key == UNDEFINED || data->command_key == UNDEFINED) {
            printf("ERROR- undefined command name\n");
            handle_undef_cmd_name(data);
            return NULL;
        }
        /* If the command key is valid, assign the command key to the instruction */
        _inst->cmd_key = data->command_key;




        if (generate_tokens(data, _inst, keyword_table, _label_table) != STATUS_OK) {
            printf("ERROR- Failed to generate tokens\n");
            free(_inst);
            free(data);
            free(_inst_table);
            return NULL;
        }

        print_instruction(_inst);

        /* Insert the instruction to the instruction table */
        if (insert_inst_to_table(_inst_table, _inst) != STATUS_OK) {
            printf("ERROR- Failed to insert instruction to table\n");
            free(_inst);
            free(data);
            free(_inst_table);
            return NULL;
        }

        create_instruction(&_inst);

        IC("increment");

        /* Reset the buffer */
        reset_buffer(buffer);

        data->buffer = buffer;
    }


    printf("line counter is: %d\n", data->line_counter);
    fclose(file); /* Close the file */

    return _inst_table;
}


static status generate_tokens(buffer_data *_buffer_data, inst *_inst, keyword *keyword_table, label_table *_label_table) {
    int cmd_index = 0;
    int command_length = 0;
    int opcode = UNDEFINED;
    size_t i;
    int _tok_amount_to_allocate = 0;
    if (_buffer_data == NULL || _buffer_data->buffer == NULL || _inst == NULL || keyword_table == NULL || _label_table == NULL) {
        return STATUS_OK; /* No arguments to process */
    }

    /* Get the number representing the command in the instruction */
    cmd_index = command_number_by_key(keyword_table, _inst->cmd_key);

    /* Check if the command exists */
    if (cmd_index == UNDEFINED) {
        printf("ERROR- Command not found\n");
        return UNDEFINED;
    }

    /* Get the number of arguments required for the command */
    _tok_amount_to_allocate = get_command_argument_count(_buffer_data->command_key);

    if (_tok_amount_to_allocate != UNKNOWN_NUMBER_OF_ARGUMENTS) {
        /* Allocate memory for the string tokens and for the binary words */
        for (i = 0;i < _tok_amount_to_allocate;i++) {
            if (create_empty_token(_inst) != STATUS_OK) {
                printf("ERROR- Failed to allocate memory for string tokens\n");
                return STATUS_ERROR;
            }

        }
    }
    else {
        for (i = 0;i < MIN_ARGS;i++) {
            if (create_empty_token(_inst) != STATUS_OK) {
                printf("ERROR- Failed to allocate memory for string tokens\n");
                return STATUS_ERROR;
            }
        }
    }

    /* Get the length of the command */
    command_length = keyword_table[cmd_index].length;

    /* Copy the command name to the first field of the instruction */
    strcpy(_inst->tokens[CMD_NAME], keyword_table[cmd_index].name);

    /* Null terminate the command name */
    _inst->tokens[CMD_NAME][command_length] = '\0';

    /* Skip the command name in the buffer- point to the argument section */
    _buffer_data->buffer += command_length;

    /* Skip any leading/trailing whitepace */
    _buffer_data->buffer = trim_whitespace(_buffer_data->buffer);


    /* Check if there is a comma between the command and the arguments */
    if (_buffer_data->buffer && _buffer_data->buffer[FIRST_ARG] == ',') {
        printf("ERROR- An unnecessary comma between command and arguments\n");
        return STATUS_ERROR;
    }

    /* Check if the command requires an unknown number of arguments */
    if (_tok_amount_to_allocate == UNKNOWN_NUMBER_OF_ARGUMENTS) { /* keep all the arguments in array[1] */
        return process_args_unknown_amount(_buffer_data, _inst, keyword_table, _label_table);
    }

    else return process_args_known_amount(_buffer_data, _inst, _label_table);

}

/* Process commands that declare an unknown number of arguments: .data, .string, .entry, .extern */
static status process_args_unknown_amount(buffer_data *_buffer_data, inst *_inst, keyword *keyword_table, label_table *_label_table) {
    syntax_state state;
    int arg_index = 0;
    int command_key = _inst->cmd_key;
    char *line = _buffer_data->buffer;

    if (_buffer_data == NULL || _inst == NULL || keyword_table == NULL || _label_table == NULL) {
        printf("ERROR- Tried to process unknown amount of arguments with NULL arguments\n");
        return invalid;
    }

    /* Set all state flags to false */
    initialize_state(&state, line);

    /* Check if the command is .data, .string, .entry, or .extern */
    update_command(&state, keyword_table, command_key);

    /* If the command is neither .data, .string, .entry, nor .extern, return an error */
    if (!state.is_data && !state.is_string && !state.is_entry && !state.is_extern) {
        printf("ERROR- Trying to process an unknown command\n");
        printf("Instruction: %s\nExiting...", line);
        return STATUS_ERROR;
    }

    state.index = 0;

    /* Parse the instruction and call the appropriate function based on the command */
    do {

        if (state.is_data) {  /* Process .data command */
            _inst->is_dot_data = true;
            if (process_data_command(&state, _buffer_data, _inst, _label_table) == invalid)
                return STATUS_ERROR;
        }

        else if (state.is_string) { /* Process .string command */
            _inst->is_dot_string = true;
            if (process_string_command(&state, _buffer_data, _inst, _label_table) == invalid)
                return STATUS_ERROR;
        }

        /* Process .entry and .extern commands */
        else if (state.is_entry || state.is_extern) {
            if (state.is_entry) _inst->is_entry = true;
            else _inst->is_extern = true;
            if (process_entry_extern_command(&state, _buffer_data, _inst) == invalid) {
                return STATUS_ERROR;
            }
        }
        /* Move the index to the next character */
        state.index++;
    } while (continue_reading(line, &state));


    if (state.is_entry || state.is_extern) {
        return update_label_table(&state, _buffer_data, _inst, _label_table, keyword_table);
    }

    return STATUS_OK;

}

static bool continue_reading(char *instruction_buffer, syntax_state *state) {
    size_t index = state->index;
    size_t instruction_length = strlen(instruction_buffer);
    if (index >= instruction_length) {
        return false;
    }

    /* If we reached a null terminator or a new line, break out of the loop */
    if (instruction_buffer[index] == '\0' || instruction_buffer[index] == '\n') {
        return false;
    }

    /* Otherwise, continue reading */
    return true;
}

/* Process .data command */
static validation_state process_data_command(syntax_state *state, buffer_data *_buffer_data, inst *_inst, label_table *_label_table) {
    int i = state->index;
    char *line = _buffer_data->buffer;
    int arg_index = 0;
    size_t next = _inst->num_tokens - 1;

    if (isspace(line[i]) && !(state->comma)) {
        state->end_of_argument_by_space = true;
        i++;
    }

    trim_whitespace(&line[i]);
    if (line[i] == ',' && state->comma) {
        printf("ERROR- Additional unnecessary comma before '.data' data\n");
        return invalid;
    }
    if (!(isdigit(line[i]) || line[i] == '-' || line[i] == '+' || line[i] == ',' || line[i] == '\0' || line[i] == '\n' || isspace(line[i]))) {
        printf("ERROR- Invalid symbol in '.data' data\n");
        return invalid;
    }
    if (state->end_of_argument_by_space && line[i] != ',' && line[i] != '\0' && line[i] != '\n') {
        printf("ERROR- NO COMMA BETWEEN INTEGERS\n");
        return invalid;
    }
    if (state->comma && line[i] == ',') {
        printf("ERROR- A series of commas without a number between them\n");
        return invalid;
    }

    _inst->tokens[next][i] = line[i];
    _inst->tokens[next][i + 1] = '\0';

    if (line[i] == ',') {
        state->comma = true;
    }
    else {
        state->end_of_argument_by_space = state->comma = false;
    }
    if (line[i] == '\n' || line[i + 1] == '\n' || line[i] == '\0' || line[i + 1] == '\0') {
        if (state->comma) {
            printf("ERROR- An unnecessary comma in the end of '.data' data\n");
            return invalid;
        }
        if (validate_data_members(_inst->tokens[1]) == invalid) {
            printf("ERROR- not a valid integer\n");
            return invalid;
        }
    }
    return valid;
}

static validation_state process_string_command(syntax_state *state, buffer_data *_buffer_data, inst *_inst, label_table *_label_table) {
    int i = state->index;
    int token_index = 1;
    size_t line_len = strlen(_buffer_data->buffer);
    char *line = _buffer_data->buffer;
    if (i == 0) {
        if (line[0] == '\"') {
            state->first_quatiotion_mark = true;
            _inst->tokens[token_index][i] = line[i];
        }
        return valid;
    }
    if (line[i] == '\"') {
        state->last_quatiotion_mark = true;
    }
    else {
        state->last_quatiotion_mark = false;
    }


    _inst->tokens[token_index][i] = line[i];

    if (line[i + 1] == '\n' || line[i + 1] == '\0') {
        if (!state->first_quatiotion_mark && !state->last_quatiotion_mark) {
            printf("ERROR- There are no quotation marks at the data in .string command\n");
            fflush(stdout);
            return invalid;
        }
        else if (!state->first_quatiotion_mark && state->last_quatiotion_mark) {
            printf("ERROR- There is no \" at the beginning of the data in .string command\n");
            fflush(stdout);
            return invalid;
        }
        else if (state->first_quatiotion_mark && !state->last_quatiotion_mark) {
            printf("ERROR- There is no \" at the end of the data in .string command\n");
            fflush(stdout);
            return invalid;
        }
        else {
            while (i > 0 && _inst->tokens[token_index][i] != '\"') {
                i--;
            }
        }
    }
    return valid;
}

static validation_state process_entry_extern_command(syntax_state *state, buffer_data *_buffer_data, inst *_inst) {
    int i = state->index;
    char *line = _buffer_data->buffer;
    int next = _inst->num_tokens;

    if (create_empty_token(_inst) != STATUS_OK) {
        printf("ERROR- Failed to allocate memory for string tokens\n");
        return invalid;
    }

    if (!(isalpha(line[i]) || isdigit(line[i]))) {
        printf("ERROR- Invalid character for label, no label with this name\n");
        return invalid;
    }
    if (state->end_of_argument && (!isspace(line[i]))) {
        printf("ERROR- Too many arguments\n");
        return invalid;
    }
    if (isspace(line[i]) || line[i + 1] == '\0' || line[i + 1] == '\n') {
        state->end_of_argument = true;
    }
    if (!isspace(line[i])) {
        _inst->tokens[next][i] = line[i];
        _inst->tokens[next][i + 1] = '\0';
    }
    return valid;
}

static validation_state update_label_table(syntax_state *state, buffer_data *_buffer_data, inst *_inst, label_table *label_table, keyword *keyword_table) {
    int i;
    bool label_found = false;
    int next = _inst->num_tokens;
    int cmd_key = _inst->cmd_key;
    if (!strcmp(keyword_table[cmd_key].name, ".entry") || !strcmp(keyword_table[cmd_key].name, ".extern")) {
        for (i = 0; i < label_table->size; i++) {
            if (!strcmp(_inst->tokens[next - 1], label_table->labels[i]->name)) {
                if (!strcmp(keyword_table[cmd_key].name, ".entry")) {
                    if (label_table->labels[i]->is_extern == true) {
                        printf("ERROR - DEFINED A LABEL BY ENTRY AND EXTERN\n");
                        return invalid;
                    }
                }
                else { /* extern */
                    if (label_table->labels[i]->is_entry == true) {
                        printf("ERROR - DEFINED A LABEL BY ENTRY AND EXTERN\n");
                        return invalid;
                    }
                    label_table->labels[i]->is_extern = true;
                    label_table->labels[i]->is_entry = false;
                }
                label_found = true;
            }
        }
        if (label_found == false) {
            printf("ERROR- No such label name\n");
            return invalid;
        }
    }
    return valid;
}

static status process_args_known_amount(buffer_data *_buffer_data, inst *_inst, label_table *_label_table) {

    int arg_count;
    int arg_index = 0;
    int arg_section_len = 0;
    int arg_len = 0;
    char *_instruction_args = NULL;

    if (_buffer_data == NULL || _buffer_data->buffer == NULL || _inst == NULL || _label_table == NULL) {
        return STATUS_OK; /* No arguments to process */
    }

    arg_section_len = strlen(_buffer_data->buffer);
    _instruction_args = _buffer_data->buffer;

    bool comma = false;
    bool null_terminator = false;
    bool new_line = false;
    bool whitespace = false;
    bool continue_reading = false;
    arg_index = 0;
    /*loop for every argument*/
    for (arg_count = 1; arg_count < _inst->num_tokens; arg_count++) {
        arg_index = 0;
        arg_len = 0;

        /* Check if the intruction terminated before any arguments arguments are found */
        if (_instruction_args && (_instruction_args[arg_index] == '\0' || _instruction_args[arg_index] == '\n')) {
            printf("ERROR- There are less arguments than expected for the specific command\n");
            return STATUS_ERROR;
        }

        comma = false;
        null_terminator = false;
        new_line = false;
        whitespace = false;


        do {

            if (_instruction_args) {
                comma = _instruction_args[arg_index] == ',';
                null_terminator = _instruction_args[arg_index] == '\0';
                new_line = _instruction_args[arg_index] == '\n';
                whitespace = isspace(_instruction_args[arg_index]);

            }
            continue_reading = !(comma || null_terminator || new_line || whitespace);

            if (continue_reading == false) {
                break;
            }

            _inst->tokens[arg_count][arg_len] = _instruction_args[arg_index];

            arg_len++;
            arg_index++;

        } while (_instruction_args && continue_reading && (arg_len < arg_section_len));

        /* Null terminate the copied arg field */
        _inst->tokens[arg_count][arg_len] = '\0';

        /* Point to the first character after the argument we just read */
        _instruction_args += arg_len;
        _instruction_args = trim_whitespace(_instruction_args);

        /* If we reached the end of the argument section, break out of the loop */
        if (null_terminator || new_line) {
            if (arg_count != (_inst->num_tokens - 1)) {
                printf("ERROR- There are less arguments than expected for the specific command\n");
                return STATUS_ERROR;
            }

            break;
        }

        /* Check if there is a comma after the last argument */
        if ((null_terminator == false) && _instruction_args && *(_instruction_args) == ',') {
            comma = true;

            /* Skip the comma */
            _instruction_args++;

            /* Skip any leading/trailing whitespace */
            _instruction_args = trim_whitespace(_instruction_args);
        }

        /*Check if there are multiple consecutive commas*/
        if ((comma == true) && _instruction_args && *(_instruction_args) == ',') {
            printf("ERROR- A series of commas without an argument between them\n");
            return STATUS_ERROR;
        }

        /* If there is no comma after the last argument check if there are any more arguments */
        if ((null_terminator || new_line) && arg_count != (_inst->num_tokens - 1)) {
            /* If the argument count is less than the number of expected arguments, there is a missing comma */
            printf("ERROR- Missing ',' between arguments of the instruction\n");
            return STATUS_ERROR;
        }


        if (validate_known_argument_data(_inst->tokens[arg_count], _label_table) != valid) {
            printf("ERROR- The argument data is not valid");
            return STATUS_ERROR;
        }
    }

    /*
    If we reached this point, all arguments have been successfully processed
    Check if there are any additional arguments after the last one
    */

    arg_count = 0;
    arg_len = 0;
    _instruction_args = trim_whitespace(_instruction_args);

    if (_instruction_args == NULL) {
        return STATUS_OK;
    }

    if (_instruction_args[0] != '\0' && _instruction_args[0] != '\n') {
        printf("ERROR- Too many arguments for the specific command\n");
        return STATUS_ERROR;
    }

    comma = *(_instruction_args) == ',';
    if (comma == true) {
        printf("ERROR- There is a comma after the last argument\n");
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

static validation_state validate_data_members(char *data_buffer) {
    size_t i;
    bool minus_or_plus = false;
    bool comma = false;
    bool number = false;


    for (i = 0; data_buffer[i] != '\0' && data_buffer[i] != '\n'; i++) {
        trim_whitespace(&data_buffer[i]);
        if (data_buffer[i] == '-' || data_buffer[i] == '+') {
            if (number == true && minus_or_plus == true) { /*in case of 1,2,3-,4 or 1,+-2,3,4*/
                return invalid;
            }
            minus_or_plus = true;
            comma = number = false;
        }
        else if (data_buffer[i] == ',') {
            if (minus_or_plus == true) { /*in case of 1,-,2,3 */
                return invalid;
            }
            comma = true;
            number = false;
            minus_or_plus = false;
        }
        else {   /*data_buffer[i] == number*/
            number = true;
            comma = false;
            minus_or_plus = false;
        }
    }

    return valid;
}

void update_command(syntax_state *state, keyword *keyword_table, int command_key) {
    if (!strcmp(keyword_table[command_key].name, ".data")) {
        state->is_data = true;
    }
    else if (!strcmp(keyword_table[command_key].name, ".string")) {
        state->is_string = true;
    }
    else if (!strcmp(keyword_table[command_key].name, ".entry")) {
        state->is_entry = true;
    }
    else if (!strcmp(keyword_table[command_key].name, ".extern")) {
        state->is_extern = true;
    }
}

validation_state validate_known_argument_data(char *str, label_table *_label_table) {
    addressing_method tmp = UNDEFINED_METHOD;

    tmp = get_addressing_method(str, _label_table);

    if (tmp == IMMEDIATE || tmp == DIRECT || tmp == INDIRECT_REGISTER || tmp == DIRECT_REGISTER) {
        return valid;
    }

    return invalid;
}

static validation_state validate_addressing_methods(bin_word *binary_command, keyword *keyword_table, int cmd_key) {
    validation_state decision_source = invalid;
    validation_state decision_destination = invalid;

    decision_source = validate_source_addressing(binary_command, keyword_table, cmd_key);
    decision_destination = validate_destination_addressing(binary_command, keyword_table, cmd_key);

    return decision_source && decision_destination;
}

static validation_state validate_source_addressing(bin_word *first_bin_word, keyword *keyword_table, int cmd_key) {
    validation_state decision = invalid;
    size_t i;
    switch (command_number_by_key(keyword_table, cmd_key)) {
        /* case 0 , 1 , 2 , 3 */
    case MOV:
    case CMP:
    case ADD:
    case SUB:
        for (i = OPCODE_LEN; i < (OPCODE_LEN + ADDRESSING_METHOD_LEN); i++) {
            if (first_bin_word->bits_vec[i] == 1) {
                decision = valid;
            }
        }
        break;
        /* case 1 */
    case LEA:
        if (first_bin_word->bits_vec[OPCODE_LEN + 2] != 1) { /*addresing method 1*/
            return invalid;
        }

        decision = valid;

        for (i = OPCODE_LEN; i < OPCODE_LEN + ADDRESSING_METHOD_LEN; i++) {
            if ((i != OPCODE_LEN + 2) && (first_bin_word->bits_vec[i] == 1)) {
                decision = invalid;
            }
        }
        break;
        /* case - no source */
    default: /* clr, not, inc, dec, jmp, bne, red, prn, jsr, rts, stop */
        decision = valid;
        for (i = OPCODE_LEN; i < OPCODE_LEN + ADDRESSING_METHOD_LEN; i++) {
            if (first_bin_word->bits_vec[i] == 1) {
                decision = invalid;
            }
        }
        break;
    }
    return decision;
}

static validation_state validate_destination_addressing(bin_word *binary_command, keyword *keyword_table, int cmd_key) {
    validation_state decision = invalid;
    size_t i;

    switch (command_number_by_key(keyword_table, cmd_key)) {
        /* case 0 , 1 , 2 , 3 */
    case CMP:
    case PRN:
        decision = invalid;
        for (i = OPCODE_LEN + ADDRESSING_METHOD_LEN; i < OPCODE_LEN + 2 * ADDRESSING_METHOD_LEN; i++) {
            if (binary_command->bits_vec[i] == 1) {
                decision = invalid;
            }
        }
        break;
        /* case 1 , 2 , 3*/
    case MOV:
    case ADD:
    case SUB:
    case LEA:
    case CLR:
    case NOT:
    case INC:
    case DEC:
    case RED:
        if (binary_command->bits_vec[OPCODE_LEN + ADDRESSING_METHOD_LEN + (3 - 0)] == 1) { /*in case of 0 is on*/
            return invalid;
        }
        decision = invalid;
        for (i = OPCODE_LEN + ADDRESSING_METHOD_LEN; i < OPCODE_LEN + 2 * ADDRESSING_METHOD_LEN; i++) {
            if (i != OPCODE_LEN + ADDRESSING_METHOD_LEN + (3 - 0) && binary_command->bits_vec[i] == 1) {
                decision = valid;
            }
        }
        break;
        /* case 1 , 2 */
    case JMP:
    case BNE:
    case JSR:
        /*in case of 1 and 2 is off*/
        if (binary_command->bits_vec[OPCODE_LEN + ADDRESSING_METHOD_LEN + (3 - 1)] != 1 &&
            binary_command->bits_vec[OPCODE_LEN + ADDRESSING_METHOD_LEN + (3 - 2)] != 1) {
            return invalid;
        }
        decision = valid;
        /*in case of 0 or 3 is on*/
        if (binary_command->bits_vec[OPCODE_LEN + ADDRESSING_METHOD_LEN + (3 - 0)] == 1 ||
            binary_command->bits_vec[OPCODE_LEN + ADDRESSING_METHOD_LEN + (3 - 3)] == 1) {
            decision = invalid;
        }

        break;
        /* case no destination */
    default: /* rts , stop */
        decision = valid;
        for (i = OPCODE_LEN + ADDRESSING_METHOD_LEN; i < OPCODE_LEN + 2 * ADDRESSING_METHOD_LEN; i++) {
            if (binary_command->bits_vec[i] == 1) {
                decision = invalid;
            }
        }
        break;
    }
    return decision;
}

register_name get_register_number(char *register_as_string) {
    size_t i;
    size_t length = strlen(register_as_string);
    int reg = UNDEFINED_REGISTER;

    /* Find the 'r' character in the string */
    for (i = 0; i < length && register_as_string[i] != 'r' && register_as_string[i] != '\0'; i++);

    /* If the 'r' character is not found, return an error */
    if (register_as_string[i] == '\0') return UNDEFINED_REGISTER;

    /* Extract the register number from the string */
    register_as_string += i + 1;
    reg = atoi(register_as_string);

    /* Check if the register number is valid */
    if (reg < R0 || reg > R7) return UNDEFINED_REGISTER;

    return reg;
}


addressing_method get_addressing_method(char *sub_inst, label_table *_label_table) {
    int i;

    /* case 0 */
    if (sub_inst[0] == '#') {
        if (sub_inst[1] == '\0') {
            return UNDEFINED;
        }
        if (sub_inst[1] != '-' && sub_inst[1] != '+' && !isdigit(sub_inst[1])) {
            return UNDEFINED;
        }
        if (sub_inst[1] == '-' || sub_inst[1] == '+') {
            if (!isdigit(sub_inst[2])) {
                return UNDEFINED;
            }
        }
        for (i = 2; i < (int)strlen(sub_inst); i++) {
            if (!isdigit(sub_inst[i])) {
                return UNDEFINED;
            }
        }
        return IMMEDIATE;
    }

    /* case 1 */
    for (i = 0; i < _label_table->size; i++) {
        if (!strcmp(_label_table->labels[i]->name, sub_inst)) {
            return DIRECT;
        }
    }

    /* case 2 */
    if (sub_inst[0] == '*') {
        if (sub_inst[1] == '\0' || sub_inst[1] != 'r') {
            return UNDEFINED_METHOD;
        }
        if (sub_inst[2] == '\0' || sub_inst[2] < '0' || sub_inst[2] > '7') {
            return UNDEFINED_METHOD;
        }
        if (sub_inst[3] != '\0') {
            return UNDEFINED_METHOD;
        }
        return INDIRECT_REGISTER;
    }

    /* case 3 */

    if (sub_inst[0] == 'r') {
        if (sub_inst[1] == '\0' || sub_inst[1] < '0' || sub_inst[1] > '7') {
            return UNDEFINED_METHOD;
        }
        if (sub_inst[2] != '\0') {
            return UNDEFINED_METHOD;
        }
        return DIRECT_REGISTER;
    }

    /* else */
    return UNDEFINED_METHOD;
}























