#include "utils.h"
#include <stdio.h> //MOD

/*
    ---------------------------
    Extended argument utilities
    ---------------------------
*/

void _cargs_push_extended_argument(
    const char* argument, const uint32_t associated_opt, const char* read_point, 
    const uint32_t vec_pos
) {
    _extended_args.args[vec_pos].associated_opt = associated_opt;
    _extended_args.args[vec_pos].read_point = (char*)read_point;

    size_t length = strlen(argument) +1;
    _extended_args.args[vec_pos].name = (char*)malloc(length);
    memcpy(_extended_args.args[vec_pos].name, argument, length);
}

/*
    ------------------------------
    Readpoint/writepoint utilities
    ------------------------------
*/

extern inline void _obtain_read_point()
{
    _read_point = _cargs_bool_args;
    if(_read_point == NULL) _read_point = _cargs_data_args;
}

extern inline void _swap_read_point()
{
    if(_cargs_bool_args == NULL) _read_point = _cargs_data_args;
    else if(_cargs_data_args == NULL) _read_point = _cargs_bool_args;
    else    //Both not null
    {
        if(_read_point == _cargs_bool_args) _read_point = _cargs_data_args;
        else //is _cargs_data_args
            _read_point = _cargs_bool_args;
    }
}

extern inline char* _get_actual_read_point() { return _read_point; }

extern inline uint32_t _get_actual_checkpoint() { return _checkpoint; }

extern inline uint32_t _get_actual_ext_checkpoint() { return _extended_checkpoint; }

extern inline void _reset_finders() { _checkpoint = 0; _read_point = NULL; }

extern inline void _reset_ext_finders() { _extended_checkpoint = 0; }

/*
    -----------------------
    Error control utilities
    -----------------------
*/

extern inline void _cargs_declare_error(const char* error_arg, const uint8_t is_extended, const uint32_t error_code)
{
    _cargs_error_argument = (char*)error_arg;
    _cargs_is_error_extended = is_extended;
    cargs_error_code = error_code;
}


/*
    -------------------------
    General purpose utilities
    -------------------------
*/

void _remove_redundancies(const uint32_t mode)
{
    if(_cargs_bool_args == NULL || _cargs_data_args == NULL) return;
    char const* read_point = NULL;
    size_t* read_length = NULL;
    char* write_point = NULL;
    size_t* write_length = NULL;

    if(mode == REMOVE_BOOL_REDUNDANCIES)
    {
        read_point = _cargs_data_args;
        read_length = &(_cargs_data_args_count);
        write_point = _cargs_bool_args;
        write_length = &_cargs_bool_args_count;
    }
    else    //REMOVE_DATA_REDUNDANCIES
    {
        read_point = _cargs_bool_args;
        read_length = &_cargs_bool_args_count;
        write_point = _cargs_data_args;
        write_length = &(_cargs_data_args_count);
    }

    //Look for redundancies and remove them
    for(uint64_t i=0; i < *read_length; i++)
    {
        for(uint64_t j=0; j < *write_length; j++)
            if(read_point[i] == write_point[j])
            {
                memcpy(write_point +j, write_point +j +1, (*write_length) - j);
                (*write_length)--;
                j--;
            }
    }

    //Does not reallocate because the null terminating character is copied and can be a waste of time
    //It is expected so, that the user does not write a lot of redundancies
}

uint32_t _find_argument_char(const char argument_char)
{
    if(!_read_point)
    {
        _obtain_read_point();
        if(_read_point == NULL) return 0; //Cannot fins read point
    }

    uint32_t j = _checkpoint;
    char* checkpoint_read_point = _read_point;
    while(1)
    {
        //Swap read_point beacause finished actual
        if(_read_point[j] == '\0')
        {
            j = 0;
            _swap_read_point();
        }

        if(_read_point[j] == argument_char) //Finally found
        {
            _checkpoint = j+1;
            checkpoint_read_point = _read_point;
            return _checkpoint;
        }
        
        //Not found, and if looked everywhere, return
        if(
            (j == _checkpoint -1 && checkpoint_read_point == _read_point) ||                            //Checkpoint is not zero
            (
                _checkpoint == 0 && _read_point[j+1] == '\0' && 
                (
                    (_read_point != checkpoint_read_point) || 
                    (_get_actual_read_point() == _cargs_data_args && !_cargs_bool_args) ||
                    (_get_actual_read_point() == _cargs_bool_args && !_cargs_data_args)
                )
            )
        ) {
            _reset_finders();
            return _checkpoint;
        }
        
        j++;
    }
}

uint8_t _find_extended_argument(const char* ext_arg)
{
    if(_extended_args.size == 0 || ext_arg == NULL) return 0;

    uint32_t j = _extended_checkpoint;
    while(1)
    {
        if(j == _extended_args.size) j = 0; //When j surpasses the vector limit

        uint32_t max_arg_length = _cargs_search_equals_operator(ext_arg);
        if(max_arg_length == 0) max_arg_length = UINT32_MAX;
        if(_extended_args.args[j].name != NULL && strncmp(_extended_args.args[j].name, ext_arg, max_arg_length) == 0) 
        {
            _extended_checkpoint = j+1;
            return 1;
        }
        
        //Not found, and if we're done with all the vector, reset and return
        if(
            j == _extended_checkpoint -1 ||
            (_extended_checkpoint == 0 && j == _extended_args.size -1)
        ) {
            _reset_ext_finders();
            return 0;
        }

        j++;
    }
}

bool _add_argument_data(const int argc, const char* argv[], uint32_t* index, const uint32_t* ext_arg_position)
{
    bool is_extended = (ext_arg_position == NULL ? false : true);
    uint32_t associated_option = 
        (!is_extended ? _get_actual_checkpoint() -1 : _extended_args.args[*ext_arg_position].associated_opt);
    if(_cargs_configure_and_store_equals_operator_data(argv[*index], associated_option)) return true;

    const uint32_t pointer_offset = (*index) +1;
    const char** data_pointer = argv + pointer_offset;
    uint32_t count = 0;
    while(count + pointer_offset < (uint32_t)argc && data_pointer[count][0] != _arg_id) 
    {
        if(count == (uint32_t)_cargs_maximum_data[associated_option] && _cargs_maximum_data[associated_option] > 0) break;
        count++;
    }

    //Check that the data count reaches the minimum data required
    if(count < (uint32_t)_cargs_minimum_data[associated_option])
    {
        _cargs_declare_error(argv[(*index)] + (is_extended ? 2 : 1), is_extended, CARGS_NOT_ENOUGH_DATA);
        return false;
    }

    if(_cargs_get_bit(_cargs_data_bit_vec, associated_option))
    {
        if(_cargs_treat_repeated_args_as_errors)
        {
            _cargs_declare_error(argv[*index], is_extended, CARGS_REDUNDANT_ARGUMENT);
            return false;
        }
        else if(count > 0)
            _cargs_push_list_node(
                &(_cargs_redundant_opt_data[associated_option].first_node),
                &(_cargs_redundant_opt_data[associated_option].last_node), 
                data_pointer, count
            );
    }
    else    //No redundant and new arg so add data
    {
        _cargs_data_packs[associated_option].count = count;
        _cargs_data_packs[associated_option].values = (count == 0 ? NULL : (char**)data_pointer);
        _cargs_set_bit(_cargs_data_bit_vec, associated_option, true);
    }

    //Set offset to argument iterator
    (*index) += count;
    return true;
}

bool _read_non_extended_argument(const int argc, const char* argv[], uint32_t* index)
{
    for(uint32_t j=1; argv[(*index)][j] != '\0'; j++)
    {
        if(_find_argument_char(argv[(*index)][j]))
        {
            if( //It is repeated and is treated as an error
                _cargs_treat_repeated_args_as_errors
                && _cargs_get_bit(
                    (_get_actual_read_point() == _cargs_bool_args ? _cargs_bool_bit_vec : _cargs_data_bit_vec),
                    _get_actual_checkpoint() -1
                )
            ) {
                _cargs_declare_error(argv[*index] +j, false, CARGS_REDUNDANT_ARGUMENT);
                return false;
            }

            //Check if data argument and write data
            if(_get_actual_read_point() == _cargs_data_args)
            {
                if(j == 1 && (argv[*index][2] == '=' || argv[*index][2] == '\0'))
                {
                    if(!_add_argument_data(argc, argv, index, NULL)) return false;
                    else
                    {
                        _cargs_set_bit(_cargs_data_bit_vec, _get_actual_checkpoint() -1, true);
                        return true;
                    }
                }
                _cargs_set_bit(_cargs_data_bit_vec, _get_actual_checkpoint() -1, true);
            }
            else _cargs_set_bit(_cargs_bool_bit_vec, _get_actual_checkpoint() -1, true);
        }
        else
        {
            _cargs_declare_error(argv[(*index)] +j, 0, CARGS_NON_EXISTENT);
            return false;
        }
    }

    return true;
}

extern inline void _cargs_set_data_limit(const char* data_arg_string, va_list arg_limits, uint8_t* write_point)
{
    if(!write_point) return;
    for(uint32_t i=0; data_arg_string[i] != '\0'; i++)
    {
        if(_find_argument_char(data_arg_string[i]))
            write_point[_get_actual_checkpoint() -1] = (uint8_t)va_arg(arg_limits, int);
    }
}

void _cargs_store_anonymous_arguments(const int argc, const char* argv[], uint32_t* arg_index)
{
    uint32_t count = 1;                         //We know this position contains anonymous arg
    const uint32_t anon_arg_pack_position = (*arg_index);  //Store the first position for the anonymous list node
    (*arg_index)++;
    while((*arg_index) < (uint32_t)argc && argv[(*arg_index)][0] != _arg_id) //Count anonymous arguments
    {
        count++;
        (*arg_index)++;
    }
    (*arg_index)--;     //Next iteration, the index variable will increment
    _cargs_anon_arg_count += count;
    _cargs_push_list_node(&_cargs_anon_args, &_cargs_anon_last, argv + anon_arg_pack_position, count);
}

bool _cargs_check_mandatory_arguments()
{
    for(uint32_t i=0; i < _cargs_mandatory_arg_count; i++)
    {
        if(!_cargs_mandatory_args[i].read_point) break; //NULL read point means no more mandatory args
        if(!_cargs_get_bit(_cargs_mandatory_args[i].read_point, _cargs_mandatory_args[i].position))
        {
            _cargs_declare_error(
                (_cargs_mandatory_args[i].read_point == _cargs_bool_bit_vec ? _cargs_bool_args : _cargs_data_args) + _cargs_mandatory_args[i].position,
                0, CARGS_MANDATORY
            );
            return false;
        }
    }

    return true;
}

extern inline uint32_t _cargs_search_equals_operator(const char* argument_pointer)
{
    for(uint32_t i=0; argument_pointer[i] != '\0'; i++)
        if(argument_pointer[i] == '=') return i+1;
    return 0;
}

extern inline bool _cargs_store_equals_operator_data(const char* data_pointer, const uint32_t associated_option)
{
    if(_cargs_bank_stack_pointer == _cargs_data_args_count) return false; //The pointer bank is full

    _cargs_data_packs[associated_option].count = 1;
    _cargs_equals_operator_pointer_bank[_cargs_bank_stack_pointer] = (char*)data_pointer;
    _cargs_data_packs[associated_option].values = _cargs_equals_operator_pointer_bank + _cargs_bank_stack_pointer;
    _cargs_bank_stack_pointer++;
    return true;
}

extern inline bool _cargs_configure_and_store_equals_operator_data(const char* arg_option, const uint32_t associated_option)
{
    const uint32_t cur_data_location = _cargs_search_equals_operator(arg_option);
    if(cur_data_location != 0)
    {
        _cargs_set_bit(_cargs_data_bit_vec, associated_option, true);
        _cargs_store_equals_operator_data(arg_option + cur_data_location, associated_option);
        return true;
    }

    return false;
}

extern inline void _cargs_remove_redundant_args_linked_lists()
{
    for(uint32_t i=0; i < _cargs_data_args_count; i++)
    {
        if(_cargs_get_bit(_cargs_is_data_relocated_bit_vec, i))
            free(_cargs_data_packs[i].values);
        else //data getter was never used so deallocate linked list
            _cargs_free_data_list(_cargs_redundant_opt_data + i);
    }
}

extern inline void _cargs_reset_ext_arg_buffers()
{
    if(_extended_args.args) 
    {
        for(size_t i=0; i < _extended_args.size; i++)
            free(_extended_args.args[i].name);
        free(_extended_args.args); _extended_args.args = NULL; _extended_args.size = 0;
    }
}

extern inline void _cargs_reset_mandatory_arg_buffers()
{
    if(_cargs_mandatory_args) 
    { 
        free(_cargs_mandatory_args); 
        _cargs_mandatory_args = NULL; 
        _cargs_mandatory_arg_count = 0; 
    }
}

extern inline void _cargs_reset_error_buffers()
{
    if(_cargs_error_argument)
    {
        _cargs_error_argument = NULL;
        free(_cargs_error_buffer_str); _cargs_error_buffer_str = NULL;
        cargs_error_code = CARGS_NO_ERROR;
    }
}

extern inline void _cargs_remove_anonymous_arguments()
{
    if(_cargs_anonymous_relocated_args) 
    {
        free(_cargs_anonymous_relocated_args); 
        _cargs_anonymous_relocated_args = NULL; 
        _cargs_anonymous_relocated_buf_size = 0;
    }

    while(_cargs_anon_args) _cargs_delete_list_head(&_cargs_anon_args, &_cargs_anon_last);
    _cargs_anon_arg_count = 0;
}