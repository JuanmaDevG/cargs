#include "cargs.h"


static char arg_id = '-';

/*
    Boolean argument structures
*/
static size_t _bool_args_count = 0;
static char* _bool_args = NULL;
static uint8_t* _bool_args_bit_vec = NULL;

/*
    Data argument structures
*/
static char* _data_args = NULL;
static ArgPackageVec _data_packs;

/*
    Library user functions
*/

void cargs_set_identificator(const char new_id) { arg_id = new_id; }

//Makes the boolean args bit vector
void cargs_set_boolean_args(const char* arg_letters) {
    if(arg_letters == NULL) return;
    _bool_args_count = strlen(arg_letters);

    _bool_args = (char*)malloc(_bool_args_count);
    memcpy(_bool_args, arg_letters, _bool_args_count * sizeof(char));

    //Calculating the bits for bit boolean vector
    _bool_args_bit_vec = (uint8_t*)malloc(
        (_bool_args_count >> 3) + (size_t)(_bool_args_count & (size_t)0b111 == 0 ? 0 : 1)
    );
}

void cargs_associate_extended(const char** ext_names, const char* arg_letters) {
    // TODO
}

void cargs_set_data_args(const char* arg_letters) {
    if(!arg_letters) return;
    _data_packs.size = strlen(arg_letters);

    _data_args = (char*)malloc(_data_packs.size);
    memcpy(_data_args, arg_letters, _data_packs.size);
    _data_packs.packages = (ArgPackage*)malloc(_data_packs.size * sizeof(ArgPackage));
}
