# Test configuration file:
# 
# This file takes the test directories and configures the test executables inside each dir. 
# Test executables are grouped with different label groups to avoid having to pass all the 
# tests each time ctest is run.

function(get_index_list actual_list _idx_list)
    list(LENGTH actual_list list_length)
    math(EXPR list_length "${${list_length}} - 1")

    foreach(idx RANGE list_length)
        list(APPEND _idx_list idx)
    endforeach()

    set(_idx_list ${_idx_list} PARENT_SCOPE)
endfunction()


function(parse_executable_name SRC_FILENAME _out_exe_name)
    #TODO
endfunction()


#Start

set( CARGS_TEST_SOURCE_DIR ${CMAKE_SOURCE_DIR}/test)

set( CARGS_TEST_DIRECTORIES 
    ${CARGS_TEST_SOURCE_DIR}/exp_stack 
    ${CARGS_TEST_SOURCE_DIR}/utils 
    ${CARGS_TEST_SOURCE_DIR}/init_funcs 
    ${CARGS_TEST_SOURCE_DIR}/main_funcs
)

set( CARGS_TEST_GROUPS 
    "expandable stack" "utilities" "init functions" "main functions"
)

get_index_list(CARGS_TEST_DIRECTORIES _index_list)

# Associates each test source directory with it's test group
foreach(_idx _index_list)
    list(GET CARGS_TEST_DIRECTORIES ${_idx} _src_dir)
    list(GET CARGS_TEST_GROUPS ${_idx} _test_group)
    aux_source_directory(${_src_dir} _sources)

    foreach(_src_file _sources)
        parse_executable_name(_src_file _executable_name)
        add_executable(${_executable_name} ${_src_file})
        target_link_libraries(${_executable_name} cargs)
        add_test(NAME ${_executable_name} COMMAND ${_executable_name})
    endforeach()
endforeach()