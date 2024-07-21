
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

static status generate_tokens(syntax_state *state, keyword *keyword_table, label_table *_label_table);
static status assign_data(syntax_state *state, label_table *_label_table, keyword *keyword_table);
static status assign_args(syntax_state *state, label_table *_label_table, keyword *keyword_table);
static validation_state process_data_command(syntax_state *state, label_table *_label_table);
static validation_state process_string_command(syntax_state *state, label_table *_label_table);
static validation_state process_entry_extern_command(syntax_state *state);
static validation_state assign_addressing_method(syntax_state *state, int arg_count, char *argument, label_table *_label_table);
static validation_state validate_data_members(syntax_state *state);
static validation_state validate_label_name(syntax_state *state, label_table *_label_table, keyword *keyword_table);
static status post_process_addresing_methods(syntax_state *state, label_table *_label_table, keyword *_keyword_table);

data_image *create_data_image(inst_table *_inst_table) {
    size_t _inst_indx;
    size_t _token_index;

    data_image *_data_image = (data_image *)calloc(1, sizeof(data_image));
    if (_data_image == NULL) {
        printf("ERROR- Failed to allocate memory for data image\n");
        return NULL;
    }
    _data_image->num_dot_data = _data_image->num_words;

    _data_image->data = (char **)calloc(_data_image->num_dot_data, sizeof(char *));

    _inst_indx = 0;
    while (_inst_indx < _inst_table->num_instructions) {
        if (_inst_table->inst_vec[_inst_indx]->is_dot_data) {
            _data_image->data[_data_image->num_dot_data] = (char *)calloc(MAX_LINE_LENGTH, sizeof(char));
            strcpy(_data_image->data[_data_image->num_dot_data], _inst_table->inst_vec[_inst_indx]->tokens[1]);
            _data_image->num_dot_data++;
        }

        if (_inst_table->inst_vec[_inst_indx]->is_dot_string) {
            _data_image->data[_data_image->num_dot_data] = (char *)calloc(MAX_LINE_LENGTH, sizeof(char));
            strcpy(_data_image->data[_data_image->num_dot_data], _inst_table->inst_vec[_inst_indx]->tokens[1]);
            _data_image->num_dot_data++;
        }

        _inst_indx++;
    }

    printf("\n#################### Data Image #########################\n");
    printf("\n\tPrinting data image:\n");
    for (_inst_indx = 0; _inst_indx < _data_image->num_dot_data; _inst_indx++) {
        printf("Data #%lu: %s\n", _inst_indx, _data_image->data[_inst_indx]);
    }
    printf("\n################# End Of Data Image #####################\n");

    return _data_image;

}

inst_table *lex(char *am_filename, label_table *_label_table, keyword *keyword_table) {
    syntax_state *state = NULL;
    data_image *_data_image = NULL;
    char *buffer = NULL;
    inst *_inst = NULL;
    inst_table *_inst_table = NULL;
    FILE *file = NULL;
    size_t i;
    status _status = STATUS_ERROR;
    size_t tk_amount_to_allocate = 0;
    status _entry_or_external = NEITHER_EXTERN_NOR_ENTRY;


    file = fopen(am_filename, "r");
    if (file == NULL) {
        printf("Error while opening .am file before lexing. Exiting...\n");
        return NULL;
    }

    state = create_syntax_state();

    if (state == NULL) {
        printf("Failed to allocate memory for syntax state. Exiting...");
        fclose(file);
        free(buffer);
        return NULL;
    }

    buffer = (char *)calloc(MAX_LINE_LENGTH, sizeof(char));
    if (buffer == NULL) {
        printf("Error while allocating memory for buffer. Exiting...\n");
        fclose(file);
        return NULL;
    }

    state->buffer = buffer;

    /* Create an instance of an instruction table */
    if (create_instruction_table(&_inst_table) != STATUS_OK) {
        printf("ERROR- Failed to create an instance of the instruction table\n");
        return NULL;
    }

    while (fgets(state->buffer, MAX_LINE_LENGTH, file)) { /* Read every line */
        if (create_instruction(&_inst) != STATUS_OK) {
            printf("ERROR- Failed to create an instance of the instruction\n");
            return NULL;
        }

        /* Skip empty lines */
        if (is_empty_line(state->buffer)) {
            continue;
        }

        /* Remove leading spaces */
        state->buffer = trim_whitespace(state->buffer);

        /* Skip comment lines */
        if (state->buffer[0] == ';') {
            continue;
        }

        /* If the line is not a comment, count it as an assembly line */
        state->line_number++;

        /* Insert the line number to the instruction */
        _inst->line_number = state->line_number;

        /* If the instruction starts with a label definition, skip it */
        skip_label_name(state, _label_table);

        if (state->extern_or_entry == CONTAINS_ENTRY || state->extern_or_entry == CONTAINS_EXTERN) {
            continue;
        }

        /* Skip unnecessary spaces */
        state->buffer = trim_whitespace(state->buffer);

        /* Identify the command key from the keyword table */
        state->cmd_key = identify_command(state, _label_table, keyword_table);

        /* If the command does not exist, exit */
        if (state->cmd_key == UNDEFINED) {
            printf("ERROR- undefined command name\n");
            free(_inst);
            free(buffer);
            free(state);
            destroy_instruction_table(_inst_table);
            return NULL;
        }
        /* If the command key is valid, assign the command key to the instruction */
        _inst->cmd_key = state->cmd_key;

        state->_inst = _inst;


        if (generate_tokens(state, keyword_table, _label_table) != STATUS_OK) {
            printf("ERROR- Failed to generate tokens\n");
            free(_inst);
            free(_inst_table);
            free(buffer);
            free(state);
            return NULL;
        }

        /* Insert the instruction to the instruction table */
        if (insert_inst_to_table(_inst_table, _inst) != STATUS_OK) {
            printf("ERROR- Failed to insert instruction to table\n");
            free(_inst);
            free(_inst_table);
            free(buffer);
            free(state);
            return NULL;
        }

        IC("increment");

        state->buffer = buffer;

        reset_syntax_state(state);
    }


    _inst_table->IC = IC("get");
    _inst_table->DC = DC("get");

    _data_image = create_data_image(_inst_table);

    print_instruction_table(_inst_table);


    return _inst_table;
}

static status generate_tokens(syntax_state *state, keyword *keyword_table, label_table *_label_table) {
    int cmd_index = 0;
    int command_length = 0;
    int opcode = UNDEFINED;
    size_t i;
    int _tok_amount_to_allocate = 0;
    status assignment_status = STATUS_ERROR;
    bool need_to_assign_data = false;
    bool need_to_assign_args = false;

    if (state == NULL || state->buffer == NULL || state->_inst == NULL || keyword_table == NULL || _label_table == NULL) {
        return STATUS_OK; /* No arguments to process */
    }

    /* Get the number representing the command in the instruction */
    cmd_index = command_number_by_key(keyword_table, state->cmd_key);

    /* Check if the command exists */
    if (cmd_index == UNDEFINED) {
        printf("ERROR- Command not found\n");
        return UNDEFINED;
    }

    /* Get the number of arguments required for the command */
    _tok_amount_to_allocate = get_command_argument_count(state->cmd_key);

    if (_tok_amount_to_allocate != UNKNOWN_NUMBER_OF_ARGUMENTS) {
        /* Allocate memory for the string tokens and for the binary words */
        for (i = 0;i < _tok_amount_to_allocate;i++) {
            if (create_empty_token(state->_inst) != STATUS_OK) {
                printf("ERROR- Failed to allocate memory for string tokens\n");
                return STATUS_ERROR;
            }
        }

        /* Indicate that the instruction needs to handle a fixed number of arguments */
        need_to_assign_args = true;

    }
    else {
        for (i = 0;i < MIN_ARGS;i++) {
            if (create_empty_token(state->_inst) != STATUS_OK) {
                printf("ERROR- Failed to allocate memory for string tokens\n");
                return STATUS_ERROR;
            }
        }

        /* Indicate that the instruction needs to handle  .data or .string  */
        need_to_assign_data = true;

    }

    /* Get the length of the command */
    command_length = keyword_table[cmd_index].length;

    /* Copy the command name to the first field of the instruction */
    strcpy(state->_inst->tokens[CMD_NAME], keyword_table[cmd_index].name);

    /* Null terminate the command name */
    state->_inst->tokens[CMD_NAME][command_length] = '\0';

    /* Skip the command name in the buffer- point to the argument section */
    state->buffer += command_length;

    /* Skip any leading/trailing whitepace */
    state->buffer = trim_whitespace(state->buffer);


    /* Check if there is a comma between the command and the arguments */
    if (state->buffer && state->buffer[FIRST_ARG] == ',') {
        printf("ERROR- An unnecessary comma between command and arguments\n");
        return STATUS_ERROR;
    }

    /* Check if the command requires an unknown number of arguments */
    if (need_to_assign_data) { /* keep all the arguments in array[1] */

        return assign_data(state, _label_table, keyword_table);
    }

    else return assign_args(state, _label_table, keyword_table);

}

/* Process commands that declare an unknown number of arguments: .data, .string, .entry, .extern */
static status assign_data(syntax_state *state, label_table *_label_table, keyword *keyword_table) {
    int arg_index = 0;
    int command_key = state->cmd_key;
    char *line = state->buffer;
    validation_state valid_ent_ext = invalid;

    if (state == NULL || state->_inst == NULL || keyword_table == NULL || _label_table == NULL) {
        printf("ERROR- Tried to process unknown amount of arguments with NULL arguments\n");
        return STATUS_ERROR;
    }

    /* Check if the command is .data, .string, .entry, or .extern */
    update_command(state, keyword_table, command_key);

    /* If the command is neither .data, .string, .entry, nor .extern, return an error */
    if (!state->is_data && !state->is_string && !state->is_entry && !state->is_extern) {
        printf("ERROR- Trying to process an unknown command\n");
        printf("Instruction: %s\nExiting...", line);
        return STATUS_ERROR;
    }

    state->index = 0;

    /* Parse the instruction and call the appropriate function based on the command */
    do {

        if (state->is_data) {  /* Process .data command */
            state->_inst->is_dot_data = true;
            if (process_data_command(state, _label_table) == invalid)
                return STATUS_ERROR;
        }

        else if (state->is_string) { /* Process .string command */
            state->_inst->is_dot_string = true;
            if (process_string_command(state, _label_table) == invalid)
                return STATUS_ERROR;
        }

        /* Process .entry and .extern commands */
        else if (state->is_entry || state->is_extern) {
            if (state->is_entry) state->_inst->is_entry = true;
            else state->_inst->is_extern = true;
            if (process_entry_extern_command(state) == invalid) {
                return STATUS_ERROR;
            }
        }
        /* Move the index to the next character */
        state->index++;
    } while (continue_reading(line, state));


    if (state->is_entry || state->is_extern) {
        valid_ent_ext = validate_label_name(state, _label_table, keyword_table);
        if (valid_ent_ext == invalid) {
            return STATUS_ERROR;
        }
    }


    return STATUS_OK;
}

static status assign_args(syntax_state *state, label_table *_label_table, keyword *keyword_table) {

    int arg_count;
    int arg_index = 0;
    int arg_section_len = 0;
    int arg_len = 0;
    char *_instruction_args = NULL;

    if (state == NULL || state->buffer == NULL || state->_inst == NULL || _label_table == NULL) {
        return STATUS_OK; /* No arguments to process */
    }

    arg_section_len = strlen(state->buffer);
    _instruction_args = state->buffer;


    arg_index = 0;
    /*loop for every argument*/
    for (arg_count = 1; arg_count < state->_inst->num_tokens; arg_count++) {
        arg_index = 0;
        arg_len = 0;

        /* Check if the intruction terminated before any arguments arguments are found */
        if (_instruction_args && (_instruction_args[arg_index] == '\0' || _instruction_args[arg_index] == '\n')) {
            printf("ERROR- There are less arguments than expected for the specific command\n");
            return STATUS_ERROR;
        }

        state->comma = false;
        state->null_terminator = false;
        state->new_line = false;
        state->whitespace = false;


        do {

            if (_instruction_args) {
                state->comma = _instruction_args[arg_index] == ',';
                state->null_terminator = _instruction_args[arg_index] == '\0';
                state->new_line = _instruction_args[arg_index] == '\n';
                state->whitespace = isspace(_instruction_args[arg_index]);

            }

            state->continue_reading = !(state->comma || state->null_terminator || state->new_line || state->whitespace);

            if (state->continue_reading == false) {
                break;
            }

            state->_inst->tokens[arg_count][arg_len] = _instruction_args[arg_index];

            arg_len++;
            arg_index++;

        } while (_instruction_args && state->continue_reading && (arg_len < arg_section_len));

        /* Null terminate the copied arg field */
        state->_inst->tokens[arg_count][arg_len] = '\0';

        /* Point to the first character after the argument we just read */
        _instruction_args += arg_len;
        _instruction_args = trim_whitespace(_instruction_args);

        /* If we reached the end of the argument section, break out of the loop */
        if (state->null_terminator || state->new_line) {
            if (arg_count != (state->_inst->num_tokens - 1)) {
                printf("ERROR- There are less arguments than expected for the specific command\n");
                return STATUS_ERROR;
            }

            break;
        }

        /* Check if there is a comma after the last argument */
        if ((state->null_terminator == false) && _instruction_args && *(_instruction_args) == ',') {
            state->comma = true;

            /* Skip the comma */
            _instruction_args++;

            /* Skip any leading/trailing whitespace */
            _instruction_args = trim_whitespace(_instruction_args);
        }

        /*Check if there are multiple consecutive commas*/
        if ((state->comma == true) && _instruction_args && *(_instruction_args) == ',') {
            printf("ERROR- A series of commas without an argument between them\n");
            return STATUS_ERROR;
        }

        /* If there is no comma after the last argument check if there are any more arguments */
        if ((state->null_terminator || state->new_line) && arg_count != (state->_inst->num_tokens - 1)) {
            /* If the argument count is less than the number of expected arguments, there is a missing comma */
            printf("ERROR- Missing ',' between arguments of the instruction\n");
            return STATUS_ERROR;
        }


        if (assign_addressing_method(state, arg_count, state->_inst->tokens[arg_count], _label_table) != valid) {
            printf("ERROR- The argument '%s' on line %d has an invalid addressing method\n", state->_inst->tokens[arg_count], state->line_number);
            return STATUS_ERROR;
        }
    }

    /*
    If we reached this point, all arguments have been successfully processed, but might not have a valid addressing method
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

    state->comma = *(_instruction_args) == ',';
    if (state->comma == true) {
        printf("ERROR- There is a comma after the last argument\n");
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

static validation_state process_data_command(syntax_state *state, label_table *_label_table) {
    int i = state->index;
    char *line = state->buffer;
    int arg_index = 0;
    size_t next = state->_inst->num_tokens - 1;

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

    state->_inst->tokens[next][i] = line[i];
    state->_inst->tokens[next][i + 1] = '\0';

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
        if (validate_data_members(state) == invalid) {
            printf("ERROR- not a valid integer\n");
            return invalid;
        }
    }
    state->_inst->num_dot_data_members++;
    DC("increment");
    return valid;
}

static validation_state process_string_command(syntax_state *state, label_table *_label_table) {
    int i = state->index;
    int token_index = 1;
    size_t line_len = strlen(state->buffer);
    char *line = state->buffer;
    if (i == 0) {
        if (line[0] == '\"') {
            state->first_quatiotion_mark = true;
            state->_inst->tokens[token_index][i] = line[i];
        }
        return valid;
    }
    if (line[i] == '\"') {
        state->last_quatiotion_mark = true;
    }
    else {
        state->last_quatiotion_mark = false;
    }


    state->_inst->tokens[token_index][i] = line[i];

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
            while (i > 0 && state->_inst->tokens[token_index][i] != '\"') {
                i--;
            }
        }
    }

    state->_inst->num_dot_string_members++;
    DC("increment");
    return valid;
}

static validation_state process_entry_extern_command(syntax_state *state) {
    int i = state->index;
    char *line = state->buffer;
    int next = state->_inst->num_tokens;

    if (create_empty_token(state->_inst) != STATUS_OK) {
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
        state->_inst->tokens[next][i] = line[i];
        state->_inst->tokens[next][i + 1] = '\0';
    }
    return valid;
}

static validation_state assign_addressing_method(syntax_state *state, int arg_count, char *argument, label_table *_label_table) {
    addressing_method tmp = UNDEFINED_METHOD;

    tmp = get_addressing_method(argument, _label_table);

    if (!(tmp == IMMEDIATE || tmp == DIRECT || tmp == INDIRECT_REGISTER || tmp == DIRECT_REGISTER)) {
        return invalid;
    }
    if (arg_count == 1) {
        state->_inst->src_addressing_method = tmp;
        return valid;
    }
    if (arg_count == 2) {
        state->_inst->dest_addressing_method = tmp;
        return valid;
    }

    return invalid;
}

static validation_state validate_label_name(syntax_state *state, label_table *_label_table, keyword *keyword_table) {
    int i;
    bool label_found = false;
    label *_label = NULL;
    int next = state->_inst->num_tokens;
    int cmd_key = state->_inst->cmd_key;
    if (!strcmp(keyword_table[cmd_key].name, ".entry") || !strcmp(keyword_table[cmd_key].name, ".extern")) {
        for (i = 0; i < _label_table->size; i++) {
            _label = _label_table->labels[i];

            /* For every entry point, check if the label name exists in the label table */
            if (_label->is_entry && state->is_entry) {
                if (label_found = !strcmp(_label->name, state->_inst->tokens[next])) break;
            }

            /* For every external point, check if the label name exists in the label table */
            if (_label->is_extern && state->is_extern) {
                if (label_found = !strcmp(_label->name, state->_inst->tokens[next])) break;
            }
        }
        if (label_found == false) {
            printf("ERROR- No such label name\n");
            return invalid;
        }
    }
    return valid;
}

static validation_state validate_data_members(syntax_state *state) {
    size_t i;
    bool minus_or_plus = false;
    bool comma = false;
    bool number = false;
    char *buffer = state->_inst->tokens[1];


    for (i = 0; buffer[i] != '\0' && buffer[i] != '\n'; i++) {
        trim_whitespace(&buffer[i]);
        if (buffer[i] == '-' || buffer[i] == '+') {
            if (number == true && minus_or_plus == true) { /*in case of 1,2,3-,4 or 1,+-2,3,4*/
                return invalid;
            }
            minus_or_plus = true;
            comma = number = false;
        }
        else if (buffer[i] == ',') {
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

static validation_state validate_addresing_methods(syntax_state *_syntax_state, label_table *_label_table, keyword *_keyword_table) {

}



























