#pragma once

#include "utils.h"

/*
    Returns a boolean checking the existence of the argument option (wether is boolean or requires data) 
    in the argument input.

    If your code looks like:
    cargs_set_args("abc", "def");
    cargs_load_args(argc, argv);

    The existence of 'b' in the program's input is checked with:
    cargs_check_option('b');
*/
bool cargs_check_option(const char option);

/*
    Returns the number of elements associated to the given data argument option.
    
    WARNING:
    If returns zero does not necessarily mean that the option does not exist in the program 
    input. Make sure you check the argument option existence with: cargs_check_option()
*/
uint32_t cargs_get_data_count(const char option);

/*
    Returns a char pointer vector with all the strings that the 
    argument option has cached after the argument load.

    WARNING:
    If there has been no associated data to an option in the argument 
    load (the_returned_ptr == NULL) does not mean that the option has not 
    been checked, the only way to check options existence in the program's 
    input is with cargs_check_option()
*/
char** cargs_get_data(const char option);

/*
    Returns the number of anonymous arguments cached by cargs after the 
    last argument loads
*/
uint32_t cargs_get_anonymous_arg_count();

/*
    Returns a dual char pointer that contains all the pointed 
    argument strings cached by cargs after the last argument loads
*/
char** cargs_get_anonymous_args();