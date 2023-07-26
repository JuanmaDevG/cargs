#include "shared_data.h"

char _arg_id = '-';

/*
    Boolean argument structures
*/

size_t _bool_args_count = 0;
char* _bool_args = NULL;


/*
    Data argument structures
*/

char* _data_args = NULL;
ArgPackageVec _data_packs = {0, NULL};


/*
    Extended arguments management structures
*/

ExtArgVec _extended_args = {0, NULL};

/*
    Mandatory arguments
*/

char* _mandatory_arguments = NULL;

/*
    Utility pointers
*/

char* _read_point;       //Pointer where to start to read a buffer
uint32_t _checkpoint;    //Position of the character next to the one that was found