#pragma once

#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#include "shared_data.h"
#include "error_system.h"
#include "bit_vec_ops.h"


/*
    Simply returns true if the argument character has been declared on the available arguments 
    pointer vector (the arg has been configured and is part of the application).
*/
_cargs_argument* _cargs_find_argument_option(const char option_char);

/*
    Finds the extended argument.
    The char pointer location must be the argument pointer plus the double 
    argument identificator offset, like:
    
    --some-extended-arg -> (pass this) some-extended-arg (resulting argument without double id)

    It is highly recommended for performance that the string first character is equal (first char not case sensitive) 
    to the extended arg's first character, otherwise the finder will have to look into the entire extended arguments 
    buffer.

    Returns true if found, otherwise false
*/
_cargs_argument* _cargs_find_extended_argument(const char* ext_arg);

/*
    Adds a data package to the selected argument data package buffer starting from the point where the argument option was found

    Returns the number of data pieces found.

    WARNING:
    This function stores errors into the error system buffers if the cargs state machine is configured to do it.
*/
uint32_t _cargs_add_argument_data(const int remaining_argc, const char** updated_argv, _cargs_argument* dst_arg, const bool is_it_extended);

/*
    Scans a string looking for an argument option.
    If this option is a data option (non boolean) adds the data.

    This function will register the errors that it registers if there's any and the state machine is correctly configured 
    to recognise them.

    Returns the number of argument values registered respect the updated_argv pointer (as an offset), minimum one
*/
uint32_t _cargs_read_argument(const int updated_argc, const char** updated_argv);

/*
    
*/
void _cargs_set_data_limit(const char* data_arg_string, va_list arg_limits, uint8_t* write_point);

/*
    Checks that mandatory arguments have been written within the program input.

    If any mandatory option is not present, pushes the error to the error system.
*/
void _cargs_check_mandatory_arguments();

/*
    Returns the position of the data after the equals operator found in an argument 
    option.

    If the opeartor is not found returns zero
*/
uint32_t _cargs_search_equals_operator(const char* argument_pointer);

/*
    Stores the equals operator data with it's associated option.

    WARNING:
    This is made overloading the _cargs_equals_operator_pointer_bank so when the 
    bank is full the function will return FALSE and no data will be added.

    It is recommended to use the argument with equals just once per defined argument:
    > program_name -f=file.txt -I=include_dir\ (DON't) -f=another_different_file.txt
*/
bool _cargs_store_equals_operator_data(const char* data_pointer, const uint32_t associated_option);

/*
    Searches the equals operator in the argument given and if found, stores the data into the argument 
    package vector using the pointer bank as nexus to directly point to the argument data.
*/
bool _cargs_configure_and_store_equals_operator_data(const char* arg_option, const uint32_t associated_option);

/*
    Free the memory blocks that have been relocated during getters 
    or free redundant arg data linked lists.

    WARNING:
    Does not set any pointer to NULL
*/
void _cargs_remove_redundant_args_linked_lists();

/*
    Frees the extended argument buffers and sets the pointers 
    NULL to let them be reused
*/
void _cargs_reset_ext_arg_buffers();

/*
    Frees the mandatory argument buffers and sets the pointers 
    to NULL to let them be reused
*/
void _cargs_reset_mandatory_arg_buffers();

/*
    Frees the error argument buffers and sets the pointers 
    to NULL to let them be reused
*/
void _cargs_reset_error_buffers();

/*
    Removes all the anonymous argument buffers cached after 
    any argument load
*/
void _cargs_remove_anonymous_arguments();