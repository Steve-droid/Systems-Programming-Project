#include "data_structs.h"
#include "asm_error.h"
#define MAX_TOKEN_SIZE 81
#define INITIAL_CAPACITY 10


macro *get_macro(macro_table *table, char *name) {
   int index;
   if (table == NULL) return NULL;

   for (index = 0;index < table->macro_count;index++)
      if (strcmp(table->macros[index]->name, name) == 0)
         return table->macros[index];
   return NULL;
}

status create_macro(char *macro_name, macro **new_macro) {
   *new_macro = (macro *)malloc(sizeof(macro));
   if (*new_macro == NULL) {
      print_system_error(NULL, NULL, m16_create_macro);
      return failure;
   }
   (*new_macro)->name = my_strdup(macro_name);
   (*new_macro)->lines = NULL;
   (*new_macro)->line_capacity = 0;
   (*new_macro)->line_count = 0;

   return success;
}

macro_table *create_macro_table() {

   macro_table *m_table = (macro_table *)malloc(sizeof(macro_table));
   if (m_table == NULL) {
      print_system_error(NULL, NULL, m17_create_macro_table);
      return NULL;
   }

   m_table->macros = NULL;
   m_table->macro_count = 0;
   m_table->capacity = 0;

   return m_table;
}

syntax_state *create_syntax_state(void) {
   char *buffer = NULL;
   syntax_state *state = (syntax_state *)calloc(1, sizeof(syntax_state));

   if (state == NULL) {
      print_system_error(NULL, NULL, m2_syntax_state);
      return NULL;
   }

   buffer = (char *)calloc(MAX_LINE_LENGTH, sizeof(char));
   if (buffer == NULL) {
      print_system_error(NULL, NULL, m2_syntax_state);
      free(state);
      state = NULL;
      return NULL;
   }

   state->buffer_without_offset = buffer;
   state->k_table = NULL;
   state->index = -1;
   state->am_filename = NULL;
   state->as_filename = NULL;
   state->continue_reading = false;
   state->extern_or_entry = NEITHER_EXTERN_NOR_ENTRY;
   state->buffer = buffer;
   state->line_number = -1;
   state->_inst = NULL;
   state->label_name_detected = false;
   state->label_key = -1;
   state->comma = false;
   state->whitespace = false;
   state->null_terminator = false;
   state->new_line = false;
   state->minus_sign = false;
   state->plus_sign = false;
   state->end_of_argument_by_space = false;
   state->first_quatiotion_mark = false;
   state->last_quatiotion_mark = false;
   state->digit = false;
   state->is_data = false;
   state->is_string = false;
   return state;
}

void reset_syntax_state(syntax_state *state) {
   size_t i = 0;


   if (state->buffer_without_offset != NULL) {
      state->buffer = state->buffer_without_offset;
      for (i = 0;i < MAX_LINE_LENGTH;i++) {
         state->buffer[i] = '\0';
      }

      state->buffer_without_offset = NULL;
   }

   else {
      for (i = 0;i < MAX_LINE_LENGTH;i++) {
         state->buffer[i] = '\0';
      }
   }


   state->index = -1;
   state->_label = NULL;

   state->extern_or_entry = NEITHER_EXTERN_NOR_ENTRY;
   state->_inst = NULL;
   state->continue_reading = false;
   state->label_name_detected = false;
   state->label_key = -1;
   state->comma = false;
   state->whitespace = false;
   state->null_terminator = false;
   state->new_line = false;
   state->minus_sign = false;
   state->plus_sign = false;
   state->end_of_argument_by_space = false;
   state->first_quatiotion_mark = false;
   state->last_quatiotion_mark = false;
   state->digit = false;
   state->is_data = false;
   state->is_string = false;
   state->tmp_arg = NULL;
}

label *create_label() {
   label *new_label = NULL;

   new_label = (label *)calloc(1, sizeof(label));
   if (new_label == NULL) {
      print_system_error(NULL, NULL, m13_create_label);
      return NULL;
   }

   new_label->address = 0;
   new_label->key = UNSET;
   new_label->instruction_line = UNSET;
   new_label->declared_as_entry = false;
   new_label->declared_as_extern = false;
   new_label->ignore = false;
   new_label->missing_definition = false;

   return new_label;
}

label_table *create_label_table(label_table **new_label_table) {



   if (!(*new_label_table = (label_table *)malloc(sizeof(label_table)))) {
      print_system_error(NULL, NULL, m12_create_label_table);
      return NULL;
   }
   (*new_label_table)->size = 0;
   (*new_label_table)->capacity = 1;
   (*new_label_table)->labels = (label **)calloc(1, sizeof(label *));
   if ((*new_label_table)->labels == NULL) {
      print_system_error(NULL, NULL, m13_create_label);
      free(*new_label_table);
      return NULL;
   }
   return *new_label_table;
}

status create_instruction(inst **new_instruction) {
   inst *_inst = NULL;
   size_t i;

   if (new_instruction == NULL) {
      return failure;
   }

   (*new_instruction) = NULL;
   /* Allocate memory for the new instruction */
   _inst = (inst *)malloc(sizeof(inst));
   if (_inst == NULL) {
      print_system_error(NULL, NULL, m18_create_instruction);
      return failure;
   }

   /* Allocate memory for the tokens of the new instruction */
   _inst->tokens = (char **)calloc(INITIAL_NUM_TOKENS, sizeof(char *));
   if (_inst->tokens == NULL) {
      print_system_error(NULL, NULL, m18_create_instruction);
      free(_inst);
      return failure;
   }

   /* Set the default values */
   _inst->num_tokens = 0;
   _inst->capacity = INITIAL_NUM_TOKENS;
   _inst->num_words_to_generate = 0;
   _inst->opcode = UNSET;
   _inst->cmd_key = UNSET;
   _inst->address = UNSET;
   _inst->line_number = UNSET;
   _inst->label_key = UNSET;

   /* Directibe parameters */
   _inst->num_dot_data_members = 0;
   _inst->num_dot_string_members = 0;
   _inst->is_dot_data = false;
   _inst->is_dot_string = false;
   _inst->is_entry = false;
   _inst->is_extern = false;
   _inst->is_src_entry = false;
   _inst->is_src_extern = false;
   _inst->is_dest_entry = false;
   _inst->is_dest_extern = false;

   /* Operation parameters */
   _inst->src_addressing_method = NO_ADDRESSING_METHOD;
   _inst->dest_addressing_method = NO_ADDRESSING_METHOD;


   /* For immediate addressing */
   _inst->immediate_val_src = UNDEFINED;
   _inst->immediate_val_dest = UNDEFINED;


   /* For direct addressing */
   for (i = 0; i < MAX_LINE_LENGTH; i++) {
      _inst->direct_label_name_src[i] = '\0';
   }
   for (i = 0; i < MAX_LINE_LENGTH; i++) {
      _inst->direct_label_name_dest[i] = '\0';
   }
   _inst->direct_label_key_src = UNSET;
   _inst->direct_label_key_dest = UNSET;
   _inst->direct_label_address_src = UNSET;
   _inst->direct_label_address_dest = UNSET;

   /* For indirect register addressing */
   _inst->indirect_reg_num_src = UNSET;
   _inst->indirect_reg_num_dest = UNSET;

   /* For direct register addressing */
   _inst->direct_reg_num_src = UNSET;
   _inst->direct_reg_num_dest = UNSET;


   /* Binary representation parameters */
   _inst->binary_word_vec = NULL;
   _inst->bin_ARE = 0;
   _inst->bin_opcode = 0;
   _inst->bin_src_method = 0;
   _inst->bin_dest_method = 0;
   _inst->bin_address = 0;


   *(new_instruction) = _inst;

   _inst = NULL;
   return success;
}

status create_empty_token(inst *instruction) {
   char *empty_token = NULL;
   size_t i;

   /* Allocate memory for the empty token */
   empty_token = (char *)calloc(MAX_TOKEN_SIZE, sizeof(char));
   if (empty_token == NULL) {
      print_system_error(NULL, NULL, m20_create_token);
      return failure;
   }

   /* Set the empty token */
   for (i = 0; i < MAX_TOKEN_SIZE; i++) empty_token[i] = '\0';

   /* Insert the empty token to the instruction */
   return insert_token_to_inst(instruction, empty_token);
}

status create_instruction_table(inst_table **new_instruction_table) {
   inst_table *new_table = NULL;

   /* Allocate memory for the new instruction table */
   new_table = (inst_table *)calloc(1, sizeof(inst_table));
   if (new_table == NULL) {
      print_system_error(NULL, NULL, m19_create_inst_table);
      return failure;
   }

   /* Allocate memory for the instructions of the new instruction table */
   new_table->inst_vec = (inst **)calloc(INITIAL_CAPACITY, sizeof(inst *));
   if (new_table->inst_vec == NULL) {
      print_system_error(NULL, NULL, m1_general_memerr);
   }

   /* Set the default values */
   new_table->num_instructions = 0;
   new_table->capacity = INITIAL_CAPACITY;


   *new_instruction_table = new_table;

   return success;
}

status insert_macro_to_table(macro_table *table, macro *macr) {
   if (table == NULL) {
      return failure;
   }

   if (table->macros == NULL) {
      table->macros = (macro **)calloc(INITIAL_MACRO_TABLE_CAPACITY, sizeof(macro *));
      if (table->macros == NULL) {
         print_system_error(NULL, NULL, m21_insert_macro_to_table);
         return failure;
      }
      table->capacity = INITIAL_MACRO_TABLE_CAPACITY;
   }


   if (table->macro_count == table->capacity) {
      table->capacity = table->macro_count + 1;
      table->macros = (macro **)realloc(table->macros, table->capacity * sizeof(macro *));
      if (table->macros == NULL) {
         print_system_error(NULL, NULL, m21_insert_macro_to_table);
         destroy_macro_table(&table);
         return failure;
      }
   }

   table->macros[table->macro_count] = macr;
   table->macro_count++;
   return success;
}

status insert_line_to_macro(macro *mac, char *line) {

   if (mac->lines == NULL) {
      mac->lines = (char **)malloc(sizeof(char *) * INITIAL_MACRO_CAPACITY);
      if (mac->lines == NULL) {
         print_system_error(NULL, NULL, m1_general_memerr);
         return failure;
      }
      mac->line_capacity = INITIAL_MACRO_CAPACITY;
   }

   if (mac->line_count == mac->line_capacity) {
      mac->line_capacity = mac->line_count + 1;
      mac->lines = (char **)realloc(mac->lines, mac->line_capacity * sizeof(char *));
      if (mac->lines == NULL) {
         print_system_error(NULL, NULL, m1_general_memerr);
         return failure;
      }
   }

   mac->lines[mac->line_count] = my_strdup(line);
   if (mac->lines[mac->line_count] == NULL) {
      return failure;
   }
   mac->line_count++;
   return success;
}

status insert_label(label_table *_label_table, label **_label) {

   /* Expand the label table if it is full */
   if (_label_table->size == _label_table->capacity) {
      _label_table->capacity += 1;
      _label_table->labels = (label **)realloc(_label_table->labels, _label_table->capacity * sizeof(label *));
      if (_label_table->labels == NULL) {
         print_system_error(NULL, NULL, m13_create_label);
         free(_label_table);
         return failure;
      }
   }

   /* Insert the label into the label table and update the size */
   _label_table->labels[_label_table->size] = (*_label);
   _label_table->size++;
   return failure;
}

status insert_token_to_inst(inst *instruction, char *token) {
   if (token == NULL) {
      return failure;
   }

   /* Reallocate memory for the tokens if needed */
   if (instruction->num_tokens == instruction->capacity) {
      instruction->capacity += 1;
      instruction->tokens = (char **)realloc(instruction->tokens, sizeof(char *) * instruction->capacity);
      if (instruction->tokens == NULL) {
         print_system_error(NULL, NULL, m20_create_token);
         return failure;
      }
   }

   /* Insert the token to the instruction */
   instruction->tokens[instruction->num_tokens] = token;
   instruction->num_tokens++;


   return success;
}

status insert_inst_to_table(inst_table *_inst_table, inst *instruction) {
   if (_inst_table == NULL || instruction == NULL) {
      return failure;
   }

   /* Reallocate memory for the instructions if needed */
   if (_inst_table->num_instructions == _inst_table->capacity) {
      _inst_table->capacity += 1;
      _inst_table->inst_vec = (inst **)realloc(_inst_table->inst_vec, sizeof(inst *) * _inst_table->capacity);
      if (_inst_table->inst_vec == NULL) {
         print_system_error(NULL, NULL, m18_create_instruction);
         destroy_instruction(&instruction);
         destroy_instruction_table(&_inst_table);
         return failure;
      }
   }

   /* Insert the instruction to the table */
   _inst_table->inst_vec[_inst_table->num_instructions] = instruction;

   /* Increment the number of instructions */
   _inst_table->num_instructions++;

   return success;
}

void destroy_keyword_table(keyword **_keyword_table) {
   if (_keyword_table == NULL || (*_keyword_table) == NULL) return;

   free(*_keyword_table);
   (*_keyword_table) = NULL;
}

void destroy_macro(macro **mac) {
   int index;

   for (index = 0;index < (*mac)->line_count;index++) {
      free((*mac)->lines[index]);
      (*mac)->lines[index] = NULL;
   }
   free((*mac)->lines);
   free((*mac)->name);
   free((*mac));
   *mac = NULL;
}

void destroy_macro_table(macro_table **table) {
   int i;

   if (table == NULL) return;

   if ((*table) && (*table)->macros) {

      for (i = 0; i < (*table)->macro_count; i++) {
         destroy_macro(&((*table)->macros[i]));

      }

      free((*table)->macros);
   }

   free((*table));
   *table = NULL;
}

void destroy_label(label **_label) {
   if (_label == NULL) return;

   free(*_label);
   *_label = NULL;
}

void destroy_label_table(label_table **_label_table) {
   size_t i;
   if (_label_table == NULL || *(_label_table) == NULL) return;
   for (i = 0; i < (*_label_table)->capacity; i++) {
      destroy_label(&(*_label_table)->labels[i]);
   }
   free((*_label_table)->labels);
   free(*_label_table);
   *_label_table = NULL;
}

void destroy_instruction(inst **_inst) {
   size_t i;
   if (_inst == NULL) {
      return;
   }
   if (*(_inst) != NULL) {
      free((*_inst)->binary_word_vec);
      (*_inst)->binary_word_vec = NULL;


      for (i = 0; i < (*_inst)->capacity; i++) {
         free((*_inst)->tokens[i]);
         (*_inst)->tokens[i] = NULL;
      }
      free((*_inst)->tokens);
      (*_inst)->tokens = NULL;

   }
   free(*_inst);
   (*_inst) = NULL;
}

void destroy_instruction_table(inst_table **_inst_table) {
   int i;

   if (_inst_table == NULL) {
      return;
   }
   if ((*_inst_table) && (*_inst_table)->inst_vec != NULL) {
      /* Free the instructions */
      for (i = 0; i < (*_inst_table)->capacity; i++) {
         destroy_instruction(&((*_inst_table)->inst_vec[i]));
      }
      /* Free instruction vector */
      free((*_inst_table)->inst_vec);
      (*_inst_table)->inst_vec = NULL;

   }



   free((*_inst_table));
   (*_inst_table) = NULL;
}

void destroy_syntax_state(syntax_state **state) {
   syntax_state *st = NULL;

   if (state == NULL || (*state) == NULL)
      return;

   st = (*state);
   if (st->buffer != NULL) {

      if (st->buffer_without_offset != NULL) {

         st->buffer = st->buffer_without_offset;
         st->buffer_without_offset = NULL;
         free(st->buffer);
         st->buffer = NULL;
      }
      else {
         free(st->buffer);
         st->buffer = NULL;
      }
   }

   if (st->_inst)
      st->_inst = NULL;

   if (st->k_table)
      st->k_table = NULL;

   if (st->_label)
      st->_label = NULL;

   if (st->m_table)
      st->m_table = NULL;

   if (st->l_table)
      st->l_table = NULL;

   free(st);
   st = NULL;
   (*state) = NULL;
}



