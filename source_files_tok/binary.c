#include "binary.h"

#define BIN_WORD_SIZE 15
#define INITIAL_NUM_WORDS 3
#define INITIAL_NUM_INST 10

/* Error handling functions */
static void bits_vec_memory_error(bin_word *_word) {
    free(_word->bits_vec);
    free(_word);
    printf("Could not allocate memory for new binary word. Exiting...\n");
}

static void word_memory_error(bin_word *_word) {
    free(_word);
    printf("Could not allocate memory for new binary word. Exiting...\n");
}

static void inst_memory_error(bin_inst *_inst) {
    destroy_bin_inst(_inst);
    printf("Could not allocate memory for new binary instruction. Exiting...\n");
}

static void table_memory_error(bin_table *_table) {
    printf("Could not allocate memory for new binary table. Exiting...\n");
    destroy_bin_table(_table);
}

/* Create functions */
status create_bin_word(bin_inst *_instr) {
    bin_word *new_bin_word = NULL;

    new_bin_word = (bin_word *)malloc(sizeof(bin_word));
    if (new_bin_word == NULL) {
        word_memory_error(new_bin_word);
        return STATUS_ERROR_MEMORY_ALLOCATION;
    }

    new_bin_word->bits_vec = (int *)malloc(sizeof(int) * BIN_WORD_SIZE);
    if (new_bin_word->bits_vec == NULL) {
        bits_vec_memory_error(new_bin_word);
        return STATUS_ERROR_MEMORY_ALLOCATION;
    }

    init_word(new_bin_word);

    /* Insert the word to the instruction */
    return insert_bin_word_to_inst(_instr, new_bin_word);
}

status create_bin_inst(bin_table *_bin_table) {
    bin_inst *new_bin_inst = (bin_inst *)malloc(sizeof(bin_inst));
    if (new_bin_inst == NULL) {
        printf("Could not allocate memory for new binary instruction. Exiting...\n");
        return STATUS_ERROR_MEMORY_ALLOCATION;
    }

    new_bin_inst->words = (bin_word **)malloc(sizeof(bin_word *) * (INITIAL_NUM_WORDS));
    if (new_bin_inst->words == NULL) {
        inst_memory_error(new_bin_inst);
        return STATUS_ERROR_MEMORY_ALLOCATION;
    }

    init_inst(new_bin_inst);

    return insert_bin_inst_to_table(_bin_table, new_bin_inst);
}

status create_bin_table(bin_table **new_bin_table) {
    *new_bin_table = (bin_table *)malloc(sizeof(bin_table));
    if (*new_bin_table == NULL) {
        printf("Could not allocate memory for new binary table. Exiting...\n");
        return STATUS_ERROR_MEMORY_ALLOCATION;
    }

    (*new_bin_table)->bin_inst_vec = (bin_inst **)malloc(sizeof(bin_inst *) * INITIAL_NUM_INST);
    if ((*new_bin_table)->bin_inst_vec == NULL) {
        table_memory_error(*new_bin_table);
        return STATUS_ERROR_MEMORY_ALLOCATION;
    }

    init_table(*new_bin_table);

    return STATUS_OK;
}


/* Init functions */
void init_word(bin_word *word) {
    size_t i;
    for (i = 0; i < BIN_WORD_SIZE; i++) {
        word->bits_vec[i] = 0;
    }
    word->first_in_instruction = false;
}

void init_inst(bin_inst *instruction) {
    size_t i;
    for (i = 0; i < instruction->capacity; i++) instruction->words[i] = NULL;
    instruction->num_words = 0;
    instruction->capacity = INITIAL_NUM_WORDS;
}

void init_table(bin_table *table) {
    size_t i;
    for (i = 0; i < table->capacity; i++) table->bin_inst_vec[i] = NULL;
    table->bin_inst_vec = NULL;
    table->num_inst = 0;
    table->capacity = INITIAL_NUM_INST;
}

/* Insert functions */
status insert_bin_word_to_inst(bin_inst *_bin_inst, bin_word *word) {
    if (_bin_inst->num_words == _bin_inst->capacity) {
        _bin_inst->capacity *= 2;
        _bin_inst->words = (bin_word **)realloc(_bin_inst->words, sizeof(bin_word *) * _bin_inst->capacity);
        if (_bin_inst->words == NULL) {
            printf("Could not reallocate memory for the binary instruction. Exiting...\n");
            return STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    _bin_inst->words[_bin_inst->num_words] = word;
    _bin_inst->num_words++;

    /* Initialize the pointer of the next word to NULL */
    _bin_inst->words[_bin_inst->num_words] = NULL;

    return STATUS_OK;
}

status insert_bin_inst_to_table(bin_table *table, bin_inst *instruction) {
    if (table->num_inst == table->capacity) {
        table->capacity *= 2;
        table->bin_inst_vec = (bin_inst **)realloc(table->bin_inst_vec, sizeof(bin_inst *) * table->capacity);
        if (table->bin_inst_vec == NULL) {
            printf("Could not reallocate memory for the binary table. Exiting...\n");
            return STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    table->bin_inst_vec[table->num_inst] = instruction;
    table->num_inst++;

    /* Initialize the pointer of the next instruction to NULL */
    table->bin_inst_vec[table->num_inst] = NULL;

    return STATUS_OK;
}

/* Get functions */
bin_word **get_words(bin_inst *instruction) {
    return instruction->words;
}

size_t get_num_words(bin_inst *instruction) {
    return instruction->num_words;
}

bin_inst **get_bin_inst(bin_table *table) {
    return table->bin_inst_vec;
}

size_t get_num_bin_inst(bin_table *_table) {
    return _table->num_inst;
}

/* Destroy functions */
void destroy_bin_word(bin_word *_bin_word) {

    if (_bin_word->bits_vec != NULL) {
        free(_bin_word->bits_vec);
    }
    free(_bin_word);
}

void destroy_bin_inst(bin_inst *_bin_inst) {
    size_t i;
    for (i = 0; i < _bin_inst->num_words; i++) {
        destroy_bin_word(_bin_inst->words[i]);
    }
    free(_bin_inst->words);
    free(_bin_inst);
}

void destroy_bin_table(bin_table *_bin_table) {
    size_t i;
    for (i = 0; i < _bin_table->num_inst; i++) {
        destroy_bin_inst(_bin_table->bin_inst_vec[i]);
    }
    free(_bin_table->bin_inst_vec);
    free(_bin_table);
}


