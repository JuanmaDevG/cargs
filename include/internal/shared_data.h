#pragma once

#include "exp_stack.h"
#include <string.h>


//Argument package associated to a single option
typedef struct _cargs_data_package {
    uint32_t count;
    char** values;                  //Pointer to first data value position in argv
    uint8_t inline_data_offset;     //If the argument's data is inlined, this is the offset in characters (like with -L/my/libdir/ offset will be 2 [-, L])
} _cargs_data_package;

typedef struct _cargs_option_data {
    uint8_t maximum_data_count;         //Maximum data pieces supported
    uint8_t minimum_data_count;         //Minimum data pieces supported
    uint32_t actual_data_count;         //Actual string data pieces that the argument contains (could be many more for argument options with no declared limits)
    _expandable_stack data;             //Contains the _cargs_data_packages and can be extracted with the expandable stack functions
    char** data_relocation_buffer;      //If not NULL, means that the expandable stack has been erased to move all the data string pointers to this buffer
} _cargs_option_data;

typedef struct _cargs_argument {
    bool is_mandatory;                      //If true, the error system will offer the possibility to interpret as error
    bool cannot_be_repeated;                //If false and has been used, the error system will interpret the argument repetition as an error
    bool has_been_used_already;             //If true, the error system will offer the possibility to interpret it's repetition as an error
    _cargs_option_data* data_container;     //Won't be null if the argument is a data argument
    char* extended_version;                 //Pointer to extended version: {-f -> --force} (without '-', that's only the id, sintactically double for extended)
} _cargs_argument;


/*
    ------------------------
    Shared data declarations
    ------------------------
*/

extern char _arg_id;

#define INVALID_CHARS 33
#define INVALID_CHARS_FRONT 127
#define ASCII_TABLE_SIZE (256 - INVALID_CHARS - INVALID_CHARS_FRONT)

extern _expandable_stack _cargs_general_buffer;                         //A buffer for general usage data

extern _cargs_argument* _cargs_valid_arg_options[ASCII_TABLE_SIZE];     //A list of independent pointers to the memory block that contains the information from available arguments

extern char* _cargs_declared_option_chars;                              //Points to the declared options (first bool options, then data options)
extern uint32_t _cargs_option_count;                                    //Counts the number of argument options
extern _cargs_argument* _cargs_declared_arg_options;                    //The memory block that contains the available argument options information
extern _cargs_option_data* _cargs_declared_options_data;                //The memory block that contains the input data from the available data argument options

extern size_t _cargs_ext_arg_count;                                     //Stores the extended argument count

extern size_t _cargs_anon_arg_count;                                    //Contains the total number of anonymous arguments
extern _expandable_stack _cargs_anonymous_args;                         //Contains _cargs_data_package objects that point to anonymous arguments groups
extern char** _cargs_anonymous_relocated_args;                          //Extracts the buffer information to generate
