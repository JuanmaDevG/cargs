#pragma once

#include "utils.h"

/*
    Checks the existence of the boolean argument

    If you set:
    cargs_set_args("abc", NULL); //And then cargs_load_args(......);

    The existence of 'b' is checked with:
    cargs_check_bool_arg(1);
*/
extern inline bool cargs_check_bool_arg(const uint32_t __arg_index);


/*
    Checks the existence of the data argument

    If you set:
    cargs_set_args(NULL, "abc"); //And then cargs_load_args(...whatever);

    The existence of 'c' is checked with:
    cargs_check_data_arg(2);
*/
extern inline bool cargs_check_data_arg(const uint32_t __arg_index);

/*
    Returns the number of elements associated to the given data argument 
    index AND fills a char pointer vector with all the strings that the 
    argument option has cached after the argument load.
*/
extern inline uint32_t cargs_get_data(const uint32_t arg_index, char** __data_dst);