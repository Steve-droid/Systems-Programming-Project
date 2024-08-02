
#include "lexer.h"
#include <string.h>
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
#define MAX_CMD_ARG_AMOUNT 3
#define UNSET -1

static status generate_tokens(syntax_state *state, keyword *keyword_table, label_table *_label_table);
static status assign_data(syntax_state *state, label_table *_label_table, keyword *keyword_table);
static status assign_args(syntax_state *state, label_table *_label_table, keyword *keyword_table);
static validation_state process_data_command(syntax_state *state, label_table *_label_table);
static validation_state process_string_command(syntax_state *state, label_table *_label_table);
static validation_state process_entry_extern_command(syntax_state *state);
static validation_state assign_addressing_method(syntax_state *state, char *argument, label_table *_label_table, keyword *keyword_table);
static validation_state validate_data_members(syntax_state *state);
static validation_state validate_label_name(syntax_state *state, label_table *_label_table, keyword *keyword_table);
static status assign_addresses(inst_table *_inst_table, label_table *_label_table, keyword *_keyword_table);


inst_table *lex(char *am_filename, label_table *_label_table, keyword *keyword_table) {
	syntax_state *state = NULL;
	inst_table *_inst_table = NULL;
	FILE *file = NULL;


	file = fopen(am_filename, "r");
	if (file == NULL) {
		printf("Error while opening .am file before lexing. Exiting...\n");
		return NULL;
	}

	state = create_syntax_state();

	if (state == NULL) {
		printf("Failed to allocate memory for syntax state. Exiting...");
		fclose(file);
		return NULL;
	}

	/* Create an instance of an instruction table */
	if (create_instruction_table(&_inst_table) != STATUS_OK) {
		printf("ERROR- Failed to create an instance of the instruction table\n");
		fclose(file);
		destroy_instruction_table(&_inst_table);
		destroy_syntax_state(&state);
		return NULL;
	}

	reset_syntax_state(state);

	while (fgets(state->buffer, MAX_LINE_LENGTH, file)) { /* Read every line */
		if (create_instruction(&state->_inst) != STATUS_OK) {
			printf("ERROR- Failed to create an instance of the instruction\n");
			destroy_instruction(&state->_inst);
			destroy_instruction_table(&_inst_table);
			destroy_syntax_state(&state);
			free(state);
			fclose(file);
			return NULL;
		}

		state->buffer_without_offset = state->buffer;

		/* Skip empty lines */
		if (is_empty_line(state->buffer)) {
			destroy_instruction(&state->_inst);
			reset_syntax_state(state);
			continue;
		}

		/* Remove leading spaces */
		state->buffer = trim_whitespace(state->buffer);

		/* Skip comment lines */
		if (state->buffer[0] == ';') {
			destroy_instruction(&state->_inst);
			reset_syntax_state(state);
			continue;
		}

		/* If the line is not a comment, count it as an assembly line */
		state->line_number++;

		/* Insert the line number to the instruction */
		state->_inst->line_number = state->line_number;

		/* If the instruction starts with a label definition, skip it */
		skip_label_name(state, _label_table);


		/* Skip unnecessary spaces */
		state->buffer = trim_whitespace(state->buffer);

		/* Identify the command key from the keyword table */
		state->cmd_key = identify_command(state, _label_table, keyword_table);

		/* If the command does not exist, exit */
		if (state->cmd_key == UNDEFINED) {
			printf("\nError on line %d: '%s'- Undefined command name. Aborting lexer...\n", state->line_number, state->buffer_without_offset);
			fclose(file);
			destroy_instruction(&(state->_inst));
			state->buffer = state->buffer_without_offset;
			state->buffer_without_offset = NULL;
			reset_syntax_state(state);
			continue;
		}
		/* If the command key is valid, assign the command key to the instruction */
		state->_inst->cmd_key = state->cmd_key;

		if (state->label_name)
			state->_inst->label_key = state->label_key;

		if (generate_tokens(state, keyword_table, _label_table) != STATUS_OK) {
			destroy_instruction(&state->_inst);
			state->buffer = state->buffer_without_offset;
			state->buffer_without_offset = NULL;
			reset_syntax_state(state);
			continue;
		}

		/* Insert the instruction to the instruction table */
		if (insert_inst_to_table(_inst_table, state->_inst) != STATUS_OK) {
			printf("ERROR- Failed to insert instruction to table\n");
			fclose(file);
			destroy_instruction(&state->_inst);
			destroy_instruction_table(&_inst_table);
			destroy_syntax_state(&state);
			return NULL;
		}

		state->buffer = state->buffer_without_offset;

		reset_syntax_state(state);
	}

	fclose(file);



	destroy_syntax_state(&state);

	_inst_table->IC = IC("get", 0);
	_inst_table->DC = DC("get", 0);

	printf("\nIC: %lu\n", _inst_table->IC);
	printf("\nDC: %lu\n ", _inst_table->DC);

	if (assign_addresses(_inst_table, _label_table, keyword_table) != STATUS_OK) {
		printf("ERROR- Failed to insert instruction to table\n");
		destroy_instruction_table(&_inst_table);
		destroy_syntax_state(&state);
		free(state);
		return NULL;
	}
	return _inst_table;
}

static status generate_tokens(syntax_state *state, keyword *_keyword_table, label_table *_label_table) {
	int cmd_index = 0;
	int command_length = 0;
	size_t i = 0;
	int _tok_amount_to_allocate = 0;

	if (state == NULL || state->buffer == NULL || state->_inst == NULL || _keyword_table == NULL || _label_table == NULL) {
		return STATUS_OK; /* No arguments to process */
	}

	/* Get the number representing the command in the instruction */
	cmd_index = command_number_by_key(_keyword_table, state->cmd_key);

	/* Check if the command exists */
	if (cmd_index == UNDEFINED_KEYWORD) {
		printf("Error- Command not found\n");
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

		state->_inst->opcode = get_command_opcode(_keyword_table, state->cmd_key);
		if (state->_inst->opcode < 0 || state->_inst->opcode>15) {
			printf("Error- Command opcode not found\n");
			return UNDEFINED;
		}

		state->_inst->bin_opcode = state->_inst->opcode;

	}
	else {
		for (i = 0;i < MIN_ARGS;i++) {
			if (create_empty_token(state->_inst) != STATUS_OK) {
				printf("ERROR- Failed to allocate memory for tokens\n");
				return STATUS_ERROR;
			}
		}

	}

	/*Initiallize both addressing methods*/
	state->_inst->dest_addressing_method = NO_ADDRESSING_METHOD;
	state->_inst->src_addressing_method = NO_ADDRESSING_METHOD;

	state->_inst->num_words_to_generate = state->_inst->num_tokens;

	/* Get the length of the command */
	command_length = _keyword_table[cmd_index].length;

	/* Copy the command name to the first field of the instruction */
	strcpy(state->_inst->tokens[CMD_NAME], _keyword_table[cmd_index].name);

	/* Null terminate the command name */
	state->_inst->tokens[CMD_NAME][command_length] = '\0';

	/* Skip the command name in the buffer- point to the argument section */
	state->buffer += command_length;

	/* Skip any leading/trailing whitepace */
	state->buffer = trim_whitespace(state->buffer);


	/* Check if there is a comma between the command and the arguments */
	if (state->buffer && state->buffer[FIRST_ARG] == ',') {
		printf("ERROR- on line: %d: '%s' unnecessary comma between the command and the arguments\n", state->line_number, state->buffer_without_offset);
		return STATUS_ERROR;
	}

	/* Check if the command requires an unknown number of arguments */
	if (_tok_amount_to_allocate == UNKNOWN_NUMBER_OF_ARGUMENTS) { /* keep all the arguments in array[1] */

		return assign_data(state, _label_table, _keyword_table);
	}

	else return assign_args(state, _label_table, _keyword_table);

}

/* Process commands that declare an unknown number of arguments: .data, .string, .entry, .extern */
static status assign_data(syntax_state *state, label_table *_label_table, keyword *keyword_table) {
	int command_key = state->cmd_key;
	char *line = state->buffer;
	validation_state valid_ent_ext = invalid;
	char *temp = NULL;
	char *token = NULL;
	int char_indx = 0;
	int copy_indx = 0;

	/* Check if the command is .data, .string, .entry, or .extern */
	update_command(state, keyword_table, command_key);

	/* If the command is neither .data, .string, .entry, nor .extern, return an error */
	if (!state->is_data && !state->is_string && !state->is_entry && !state->is_extern) {
		printf("ERROR- Trying to process an unknown command\n");
		printf("Instruction: %s\nExiting...", line);
		return STATUS_ERROR;
	}

	state->index = 0;
	state->_inst->num_words_to_generate = 0;

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

	if (state->is_data) {
		temp = my_strdup(state->_inst->tokens[1]);
		token = strtok(temp, ",");

		while (token != NULL) {
			state->_inst->num_dot_data_members++;
			state->_inst->num_words_to_generate++;
			DC("increment", 1);
			IC("increment", 1);
			token = strtok(NULL, ", ");
		}

		free(temp);
		temp = NULL;
	}

	if (state->is_string) {
		temp = NULL;
		temp = my_strdup(state->_inst->tokens[1]);
		char_indx = 0;
		copy_indx = 1;
		while (temp != NULL && temp[copy_indx] != '\"' && temp[copy_indx] != '\0') {
			state->_inst->tokens[1][char_indx] = temp[copy_indx];
			state->_inst->num_dot_string_members++;
			state->_inst->num_words_to_generate++;
			DC("increment", 1);
			IC("increment", 1);
			copy_indx++;
			char_indx++;
		}

		state->_inst->tokens[1][char_indx] = '\0';
		state->_inst->num_dot_string_members++;
		state->_inst->num_words_to_generate++;
		DC("increment", 1);
		IC("increment", 1);
		free(temp);
		temp = NULL;
	}

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
	addressing_method src_reg = NO_ADDRESSING_METHOD;
	addressing_method dest_reg = NO_ADDRESSING_METHOD;

	if (state == NULL || state->buffer == NULL || state->_inst == NULL || _label_table == NULL) {
		return STATUS_OK; /* No arguments to process */
	}

	arg_section_len = strlen(state->buffer);
	_instruction_args = state->buffer;


	arg_index = 0;

	/* Check each argument- look for syntax errors */
	for (arg_count = 1; arg_count < state->_inst->num_tokens; arg_count++) {
		arg_index = 0;
		arg_len = 0;

		/* Check if the intruction terminated before any arguments arguments are found */
		if (_instruction_args && (_instruction_args[arg_index] == '\0' || _instruction_args[arg_index] == '\n')) {
			printf("Error on line: %d: ", state->line_number + 1);
			printf("There are less arguments than expected for the specific command\n");
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
			if (assign_addressing_method(state, state->_inst->tokens[arg_count], _label_table, keyword_table) != valid) {
				printf("ERROR- The argument '%s' on line %d has an invalid addressing method\n", state->_inst->tokens[arg_count], state->line_number);
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


		if (assign_addressing_method(state, state->_inst->tokens[arg_count], _label_table, keyword_table) != valid) {
			printf("ERROR- The argument '%s' on line %d has an invalid addressing method\n", state->_inst->tokens[arg_count], state->line_number);
			return STATUS_ERROR;
		}

	}


	src_reg = state->_inst->src_addressing_method;
	dest_reg = state->_inst->dest_addressing_method;
	if (state->_inst->num_tokens == MAX_CMD_ARG_AMOUNT) {
		if (src_reg == DIRECT_REGISTER || src_reg == INDIRECT_REGISTER)
			if (dest_reg == DIRECT_REGISTER || dest_reg == INDIRECT_REGISTER)
				state->_inst->num_words_to_generate = 2;
	}

	IC("increment", state->_inst->num_words_to_generate);



	/*
	If we reached this point, all arguments for the specific command (i.e operation) have been successfully processed.
	Check if there are any additional arguments after the last one
	*/

	arg_count = 0;
	arg_len = 0;
	_instruction_args = trim_whitespace(_instruction_args);

	if (_instruction_args == NULL) {
		return STATUS_OK;
	}

	if (_instruction_args[0] != '\0' && _instruction_args[0] != '\n') {
		printf("\nError on line: %d: ", state->line_number + 1);
		printf("There are too many arguments for the '%s' command\n", state->_inst->tokens[0]);
		return STATUS_ERROR;
	}

	state->comma = *(_instruction_args) == ',';
	if (state->comma == true) {
		printf("\nError on line: %d: ", state->line_number + 1);
		printf("There is a comma after the last argument of the '%s' command\n", state->_inst->tokens[0]);
		return STATUS_ERROR;
	}

	return STATUS_OK;
}

static validation_state process_data_command(syntax_state *state, label_table *_label_table) {
	int i = state->index;
	char *line = state->buffer;
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


	return valid;
}

static validation_state process_string_command(syntax_state *state, label_table *_label_table) {
	int i = state->index;
	int token_index = 1;
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
		if (state->first_quatiotion_mark && !state->last_quatiotion_mark) {
			printf("ERROR- There is no \" at the end of the data in .string command\n");
			fflush(stdout);
			return invalid;
		}
		{
			while (i > 0 && state->_inst->tokens[token_index][i] != '\"') {
				i--;
			}
		}
	}

	return valid;
}

static validation_state process_entry_extern_command(syntax_state *state) {
	int i = state->index;
	char *line = state->buffer;
	int next = 0;
	static bool created_token = false;

	if (!created_token && create_empty_token(state->_inst) != STATUS_OK) {
		printf("ERROR- Failed to allocate memory for string tokens\n");
		return invalid;
	}

	created_token = true;
	next = state->_inst->num_tokens - 1;

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

static validation_state assign_addressing_method(syntax_state *state, char *argument, label_table *_label_table, keyword *keyword_table) {
	size_t i;
	addressing_method _addressing_method = UNDEFINED_METHOD;
	keyword_name command = UNDEFINED_KEYWORD;
	bool contains_src_addressing = false;
	bool contains_dest_addressing = false;
	bool found_label_with_matching_name = false;
	label *tmp_label = NULL;
	char *tmp_ptr = NULL;
	int tmp_val = 0;;

	if (state->_inst->src_addressing_method != NO_ADDRESSING_METHOD) contains_src_addressing = true;
	if (state->_inst->dest_addressing_method != NO_ADDRESSING_METHOD) contains_dest_addressing = true;

	if (contains_dest_addressing && contains_src_addressing) {
		printf("Error on line: #%lu: Trying to assign addressing methods to arguments for a 2nd time.\n", state->_inst->line_number);
		return invalid;
	}

	_addressing_method = get_addressing_method(argument, _label_table);

	if (!(_addressing_method == IMMEDIATE || _addressing_method == DIRECT || _addressing_method == INDIRECT_REGISTER || _addressing_method == DIRECT_REGISTER)) {
		printf("Error on line: #%lu: Invalid addressing method.\n", state->_inst->line_number);
		return invalid;
	}

	/*Get the number that represents the command in the current instruction*/
	command = command_number_by_key(keyword_table, state->cmd_key);

	switch (command) {
	case MOV:
	case ADD:
	case SUB:
		if (contains_src_addressing) {/*Check dest addressing */
			if (_addressing_method == IMMEDIATE) {
				printf("Error on line: #%lu: Trying to assign an immediate addressing method to a 'mov'/'add'/'sub' destination argument\n", state->_inst->line_number);
				return invalid;
			}

			/*If the addressing method is valid for the specific command, assign it to the instruction */
			state->_inst->dest_addressing_method = _addressing_method;
			break;
		}
		if (contains_dest_addressing) {
			printf("Error on line: #%lu: Wrong order of addressing method assignment.\n", state->_inst->line_number);
			return invalid;
		}
		state->_inst->src_addressing_method = _addressing_method;
		break;
	case CMP:
		if (contains_src_addressing) {
			state->_inst->dest_addressing_method = _addressing_method;
			break;
		}

		if (contains_dest_addressing) {
			printf("Error on line: #%lu: Wrong order of addressing method assignment.\n", state->_inst->line_number);
			return invalid;
		}

		state->_inst->src_addressing_method = _addressing_method;
		break;
	case LEA:
		if (contains_src_addressing) {
			if (_addressing_method == IMMEDIATE) {
				printf("Error on line: #%lu: Trying to assign an immediate addressing method to a destination argument of 'lea' instruction\n", state->_inst->line_number);
				return invalid;
			}

			/*If the addressing method is valid for the specific command, assign it to the instruction */
			state->_inst->dest_addressing_method = _addressing_method;
			break;
		}

		if (contains_dest_addressing) {
			printf("Error on line: #%lu: Wrong order of addressing method assignment.\n", state->_inst->line_number);
			return invalid;
		}

		if (_addressing_method != DIRECT) {
			printf("Error on line: #%lu: Trying to assign an an invalid addressing method to a source argument of 'lea' instruction\n", state->_inst->line_number);
			return invalid;
		}

		/*If we got here it means that the argument is a source argument with a valid addressing method*/
		state->_inst->src_addressing_method = _addressing_method;
		break;
	case CLR:
	case NOT:
	case INC:
	case DEC:
	case RED:
		if (contains_src_addressing) {
			printf("Error on line: #%lu: ", state->_inst->line_number);
			printf("Trying to assign a source argument addressing method to a 'clr'/'not'/'inc'/'dec'/'red' instruction\n");
			return invalid;
		}

		if (contains_dest_addressing) {
			printf("Error on line: #%lu: ", state->_inst->line_number);
			printf("Multiple assignment attempts for a destination argument addressing method of a 'clr'/'not'/'inc'/'dec'/'red' instruction\n");
			return invalid;
		}

		if (_addressing_method == IMMEDIATE) {
			printf("Error on line: #%lu: ", state->_inst->line_number);
			printf("Trying to assign an immediate addressing method to a destination argument of 'clr'/'not'/'inc'/'dec'/'red'  instruction\n");
			return invalid;
		}

		/*If we got here it means that the argument is a destination argument with a valid addressing method*/
		state->_inst->dest_addressing_method = _addressing_method;
		break;

	case JMP:
	case BNE:
	case JSR:
		if (contains_src_addressing) {
			printf("Error on line: #%lu: ", state->_inst->line_number);
			printf("Trying to assign a source argument addressing method to a 'jmp'/'bne'/'jsr' instruction\n");
			return invalid;
		}

		if (contains_dest_addressing) {
			printf("Error on line: #%lu: ", state->_inst->line_number);
			printf("Multiple assignment attempts for a destination argument addressing method of a 'jmp'/'bne'/'jsr' instruction\n");
			return invalid;
		}

		if (_addressing_method == IMMEDIATE) {
			printf("Error on line: #%lu: ", state->_inst->line_number);
			printf("Trying to assign an immediate addressing method to a destination argument of 'jmp'/'bne'/'jsr'  instruction\n");
			return invalid;
		}

		if (_addressing_method == DIRECT_REGISTER) {
			printf("Error on line: #%lu: ", state->_inst->line_number);
			printf("Trying to assign an indirect register addressing method to a destination argument of 'jmp'/'bne'/'jsr'  instruction\n");
			return invalid;
		}

		state->_inst->dest_addressing_method = _addressing_method;
		break;

	case PRN:
		if (contains_src_addressing) {
			printf("Error on line: #%lu: ", state->_inst->line_number);
			printf("Trying to assign a source argument addressing method to a 'prn' instruction\n");
			return invalid;
		}

		if (contains_dest_addressing) {
			printf("Error on line: #%lu: ", state->_inst->line_number);
			printf("Multiple assignment attempts for a destination argument addressing method of a 'prn' instruction\n");
			return invalid;
		}
		state->_inst->dest_addressing_method = _addressing_method;
		break;
	case RTS:
	case STOP:
		if (contains_src_addressing) {
			printf("Error on line: #%lu: ", state->_inst->line_number);
			printf("Trying to assign a source argument addressing method to a 'rts'/'stop' instruction\n");
			return invalid;
		}

		if (contains_dest_addressing) {
			printf("Error on line: #%lu: ", state->_inst->line_number);
			printf("Trying to assign a destination argument addressing method to a 'rts','stop' instruction\n");
			return invalid;
		}

		break;

	default:
		printf("Could not assign addressing method to arguments on line #%lu\n", state->_inst->line_number);
		return invalid;
	}

	if (_addressing_method == IMMEDIATE) {
		tmp_ptr = strchr(argument, '#');
		tmp_val = atoi(tmp_ptr + 1);

		switch (command) {
		case PRN:
			if (state->_inst->immediate_val_dest != UNDEFINED) {
				break;
			}
			state->_inst->immediate_val_dest = tmp_val;
			break;

		case MOV:
		case ADD:
		case SUB:
			if (state->_inst->immediate_val_src != UNDEFINED) {
				break;
			}
			state->_inst->immediate_val_src = tmp_val;
			break;

		case CMP:
			if (state->_inst->src_addressing_method != IMMEDIATE && state->_inst->dest_addressing_method == IMMEDIATE) {
				if (state->_inst->immediate_val_dest == UNDEFINED) {
					state->_inst->immediate_val_dest = tmp_val;
				}
				break;
			}

			if (state->_inst->src_addressing_method == IMMEDIATE && state->_inst->dest_addressing_method != IMMEDIATE) {
				if (state->_inst->immediate_val_src == UNDEFINED) {
					state->_inst->immediate_val_src = tmp_val;
				}
				break;
			}

			if ((state->_inst->src_addressing_method == IMMEDIATE && state->_inst->dest_addressing_method == IMMEDIATE)) {
				if (state->_inst->immediate_val_src == UNDEFINED) {
					state->_inst->immediate_val_src = tmp_val;
					break;
				}

				if (state->_inst->immediate_val_dest == UNDEFINED) {
					state->_inst->immediate_val_dest = tmp_val;
					break;
				}
			}

		default:
			tmp_ptr = NULL;
			tmp_val = 0;
		}

	}

	if (_addressing_method == DIRECT) {
		if (validate_label_name(state, _label_table, keyword_table) != valid) {
			printf("Error on line: #%lu: ", state->_inst->line_number);
			printf("Trying to use an invalid label name as an argument\n");
			return invalid;
		}

		for (i = 0;i < _label_table->size;i++) {
			tmp_label = get_label_by_name(_label_table, argument);
			if (tmp_label != NULL) {
				found_label_with_matching_name = true;
				break;
			}
		}

		if (found_label_with_matching_name) {
			if (state->_inst->src_addressing_method == DIRECT) {
				if (state->_inst->direct_label_key_src == -1) {
					state->_inst->direct_label_key_src = tmp_label->key;
					strcpy(state->_inst->direct_label_name_src, tmp_label->name);
					if (tmp_label->is_entry) state->_inst->is_src_entry = true;
					else if (tmp_label->is_extern) state->_inst->is_src_extern = true;
				}
			}
			if (state->_inst->dest_addressing_method == DIRECT) {
				if (state->_inst->direct_label_key_dest == -1) {
					state->_inst->direct_label_key_dest = tmp_label->key;
					strcpy(state->_inst->direct_label_name_dest, tmp_label->name);
					if (tmp_label->is_entry) state->_inst->is_dest_entry = true;
					else if (tmp_label->is_extern) state->_inst->is_dest_extern = true;
				}
			}

		}
	}

	if (_addressing_method == INDIRECT_REGISTER) {
		tmp_ptr = strstr(argument, "r") + 1;
		tmp_val = atoi(tmp_ptr);


		switch (command) {
		case MOV:
		case CMP:
		case ADD:
		case SUB:
			if (state->_inst->src_addressing_method != INDIRECT_REGISTER && state->_inst->dest_addressing_method == INDIRECT_REGISTER) {
				if (state->_inst->indirect_reg_num_dest == UNSET) {
					state->_inst->indirect_reg_num_dest = tmp_val;
				}
				break;
			}

			if (state->_inst->src_addressing_method == INDIRECT_REGISTER && state->_inst->dest_addressing_method != INDIRECT_REGISTER) {
				if (state->_inst->indirect_reg_num_src == UNSET) {
					state->_inst->indirect_reg_num_src = tmp_val;
				}
				break;
			}

			if ((state->_inst->src_addressing_method == INDIRECT_REGISTER && state->_inst->dest_addressing_method == INDIRECT_REGISTER)) {
				if (state->_inst->indirect_reg_num_src == UNSET) {
					state->_inst->indirect_reg_num_src = tmp_val;
					break;
				}

				if (state->_inst->indirect_reg_num_dest == UNSET) {
					state->_inst->indirect_reg_num_dest = tmp_val;
					break;
				}
			}


		case LEA:
		case CLR:
		case NOT:
		case INC:
		case DEC:
		case JMP:
		case BNE:
		case RED:
		case PRN:
		case JSR:
			if (state->_inst->indirect_reg_num_dest == UNDEFINED) {
				state->_inst->indirect_reg_num_dest = tmp_val;
			}
			break;
		default:
			break;
		}


	}

	if (_addressing_method == DIRECT_REGISTER) {
		tmp_ptr = NULL;
		tmp_val = UNSET;

		tmp_ptr = strchr(argument, 'r') + 1;
		tmp_val = atoi(tmp_ptr);

		switch (command) {
		case MOV:
		case CMP:
		case ADD:
		case SUB:
			if (state->_inst->src_addressing_method != DIRECT_REGISTER && state->_inst->dest_addressing_method == DIRECT_REGISTER) {
				if (state->_inst->direct_reg_num_dest == UNSET) {
					state->_inst->direct_reg_num_dest = tmp_val;
				}
				break;
			}

			if (state->_inst->src_addressing_method == DIRECT_REGISTER && state->_inst->dest_addressing_method != DIRECT_REGISTER) {
				if (state->_inst->direct_reg_num_src == UNSET) {
					state->_inst->direct_reg_num_src = tmp_val;
				}
				break;
			}

			if ((state->_inst->src_addressing_method == DIRECT_REGISTER && state->_inst->dest_addressing_method == DIRECT_REGISTER)) {
				if (state->_inst->direct_reg_num_src == UNSET) {
					state->_inst->direct_reg_num_src = tmp_val;
					break;
				}

				if (state->_inst->direct_reg_num_dest == UNSET) {
					state->_inst->direct_reg_num_dest = tmp_val;
					break;
				}

				break;
			}

		case LEA:
		case CLR:
		case NOT:
		case INC:
		case DEC:
		case RED:
		case PRN:
			if (state->_inst->direct_reg_num_dest == UNDEFINED) {
				state->_inst->direct_reg_num_dest = tmp_val;
			}
			break;


		default: break;
		}

	}
	return valid;
}

static validation_state validate_label_name(syntax_state *state, label_table *_label_table, keyword *keyword_table) {
	int i;
	bool label_found = false;
	label *_label = NULL;
	int cmd_key = state->_inst->cmd_key;
	if (!strcmp(keyword_table[cmd_key].name, ".entry") || !strcmp(keyword_table[cmd_key].name, ".extern")) {
		for (i = 0; i < _label_table->size; i++) {
			_label = _label_table->labels[i];

			/* For every entry point, check if the label name exists in the label table */
			if (_label->is_entry && state->is_entry) {
				label_found = !strcmp(_label->name, state->buffer);
				if (label_found) break;
			}

			/* For every external point, check if the label name exists in the label table */
			if (_label->is_extern && state->is_extern) {
				label_found = !strcmp(_label->name, state->buffer);
				if (label_found) break;
			}


		}

		_label = get_label_by_name(_label_table, state->buffer);
		if (_label != NULL)
			if (_label->is_entry || _label->is_extern) {
				label_found = true;
			}
		if (label_found == false) {
			printf("Error on line %d: The name '%s' is an invalid label name.\n", state->line_number, state->buffer);
			return invalid;
		}
	}
	return valid;
}

static validation_state validate_data_members(syntax_state *state) {
	size_t i;
	bool minus_or_plus = false;
	bool number = false;
	char *buffer = state->_inst->tokens[1];


	for (i = 0; buffer[i] != '\0' && buffer[i] != '\n'; i++) {
		trim_whitespace(&buffer[i]);
		if (buffer[i] == '-' || buffer[i] == '+') {
			if (number == true && minus_or_plus == true) { /*in case of 1,2,3-,4 or 1,+-2,3,4*/
				return invalid;
			}
			minus_or_plus = true;
			number = false;
		}
		else if (buffer[i] == ',') {
			if (minus_or_plus == true) { /*in case of 1,-,2,3 */
				return invalid;
			}
			number = false;
			minus_or_plus = false;
		}
		else {   /*data_buffer[i] == number*/
			number = true;
			minus_or_plus = false;
		}
	}

	return valid;
}

static status assign_addresses(inst_table *_inst_table, label_table *_label_table, keyword *_keyword_table) {
	int initial_address = 100;
	int inst_index = 0;
	int words_generated = 0;
	int label_key = 0;
	label *tmp_label = NULL;
	inst *tmp_inst = NULL;

	if (_inst_table == NULL || _label_table == NULL || _keyword_table == NULL) {
		return STATUS_ERROR;
	}

	while (inst_index < (_inst_table->num_instructions)) {
		if (_inst_table->inst_vec) {
			tmp_inst = _inst_table->inst_vec[inst_index];
			tmp_inst->address = initial_address + words_generated;
			if (tmp_inst->label_key != 0) {

				label_key = tmp_inst->label_key;
				tmp_label = get_label_by_key(_label_table, label_key);
				if (tmp_label != NULL) {
					tmp_label->address = tmp_inst->address;
				}
			}
			words_generated += tmp_inst->num_words_to_generate;
			tmp_inst->bin_address = tmp_inst->address;
			tmp_inst = NULL;
			tmp_label = NULL;
			inst_index++;
		}
	}


	for (inst_index = 0;inst_index < _inst_table->num_instructions;inst_index++) {
		tmp_inst = _inst_table->inst_vec[inst_index];
		if (tmp_inst->src_addressing_method == DIRECT) {
			label_key = tmp_inst->direct_label_key_src;
			tmp_label = get_label_by_key(_label_table, label_key);
			tmp_inst->direct_label_address_src = tmp_label->address;

			tmp_label = NULL;
			label_key = UNSET;
		}

		if (tmp_inst->dest_addressing_method == DIRECT) {
			label_key = tmp_inst->direct_label_key_dest;
			tmp_label = get_label_by_key(_label_table, label_key);
			tmp_inst->direct_label_address_dest = tmp_label->address;
			tmp_label = NULL;
			label_key = UNSET;
		}

	}


	return STATUS_OK;
}

