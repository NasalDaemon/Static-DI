set(DI_GENERATOR_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(DI_GENERATOR_VENV ${CMAKE_CURRENT_BINARY_DIR}/venv)
if(WIN32)
    set(DI_GENERATOR_PYTHON ${DI_GENERATOR_VENV}/scripts/python.exe)
else()
    set(DI_GENERATOR_PYTHON ${DI_GENERATOR_VENV}/bin/python3)
endif()

set_property(GLOBAL PROPERTY DI_GENERATOR_DIR ${DI_GENERATOR_DIR})
set_property(GLOBAL PROPERTY DI_GENERATOR_VENV ${DI_GENERATOR_VENV})
set_property(GLOBAL PROPERTY DI_GENERATOR_PYTHON ${DI_GENERATOR_PYTHON})

macro(di_import_properties)
    get_property(DI_GENERATOR_DIR GLOBAL PROPERTY DI_GENERATOR_DIR)
    get_property(DI_GENERATOR_VENV GLOBAL PROPERTY DI_GENERATOR_VENV)
    get_property(DI_GENERATOR_PYTHON GLOBAL PROPERTY DI_GENERATOR_PYTHON)
    set(DI_GENERATOR_CLONE_DIR ${CMAKE_CURRENT_BINARY_DIR}/di_generator/clones)
    set(DI_GENERATOR_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/di_generator/src)
    set(DI_GENERATOR_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/di_generator/include)
    set(DI_GENERATOR_MODULES_DIR ${CMAKE_CURRENT_BINARY_DIR}/di_generator/modules)
endmacro()

find_package(Python3 REQUIRED)

add_custom_command(
    OUTPUT "${DI_GENERATOR_PYTHON}"
    COMMAND ${Python3_EXECUTABLE} -m venv ${DI_GENERATOR_VENV}
    COMMENT "di_generator: Initialising python venv"
)
add_custom_command(
    OUTPUT "${DI_GENERATOR_VENV}.stamp"
    DEPENDS "${DI_GENERATOR_PYTHON}" "${DI_GENERATOR_DIR}/requirements.txt"
    COMMAND ${DI_GENERATOR_PYTHON} -m pip install -r "${DI_GENERATOR_DIR}/requirements.txt" --upgrade
    COMMAND touch ${DI_GENERATOR_VENV}.stamp
    COMMENT "di_generator: Installing dependencies"
)
add_custom_target(di_generator_python DEPENDS "${DI_GENERATOR_VENV}.stamp")

function(target_generate_di_src TARGET_NAME)
    di_import_properties()

    set(options UNITY PRECOMPILE_HEADERS)
    set(oneValueArgs GRAPH_HEADER GRAPH_MODULE GRAPH_TYPE ID)
    set(multiValueArgs COMMON_HEADERS COMMON_MODULES NODES)
    cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if((DEFINED arg_GRAPH_HEADER) EQUAL (DEFINED arg_GRAPH_MODULE))
        message(FATAL_ERROR "One and only one of GRAPH_HEADER OR GRAPH_MODULE must be defined")
    elseif(DEFINED arg_GRAPH_MODULE)
        set(IS_MODULE TRUE)
    else()
        set(IS_MODULE FALSE)
    endif()

    if(IS_MODULE)
        if (${arg_UNITY})
            message(FATAL_ERROR "UNITY flag not supported with modules")
        elseif(${arg_PRECOMPILE_HEADERS})
            message(FATAL_ERROR "PRECOMPILE_HEADERS flag not supported with modules")
        endif()
    else()
        if (DEFINED arg_COMMON_MODULES)
            message(FATAL_ERROR "COMMON_MODULES flag not supported with headers")
        endif()
    endif()

    # Prepare common includes
    list(APPEND arg_COMMON_HEADERS "di/macros.hpp")
    if(IS_MODULE)
        list(APPEND arg_COMMON_MODULES "di")
    else()
        list(APPEND arg_COMMON_HEADERS "di/graph.hpp")
    endif()

    list(SORT arg_COMMON_HEADERS)
    list(REMOVE_DUPLICATES arg_COMMON_HEADERS)
    foreach(COMMON_HEADER ${arg_COMMON_HEADERS})
        string(APPEND COMMON_HEADERS "#include \"${COMMON_HEADER}\"\n")
    endforeach()
    if(IS_MODULE)
        list(SORT arg_COMMON_MODULES)
        list(REMOVE_DUPLICATES arg_COMMON_MODULES)
        foreach(COMMON_MODULE ${arg_COMMON_MODULES})
            string(APPEND COMMON_MODULES "import ${COMMON_MODULE};\n")
        endforeach()
    endif()

    function(write_src_file OUT_SRC_FILES)
        # Canonicalise node paths list
        list(SORT NODE_DOT_PATHS)
        list(REMOVE_DUPLICATES NODE_DOT_PATHS)
        foreach(NODE_DOT_PATH ${NODE_DOT_PATHS})
            string(APPEND NODE_INSTANTIATIONS "DI_INSTANTIATE(${arg_GRAPH_TYPE}, ${NODE_DOT_PATH})\n")
        endforeach()

        # Generate canonical list of module imports/header includes
        if(IS_MODULE)
            list(LENGTH NODE_IMPLS NODE_IMPLS_LEN)
            if(NODE_IMPLS_LEN GREATER 1)
                message(FATAL_ERROR "NODES may not list more than one node module per entry")
            endif()
            string(REPLACE ":" ";" MODULE_PART_PAIR ${NODE_IMPLS})
            list(LENGTH MODULE_PART_PAIR MODULE_PART_PAIR_LEN)
            if(MODULE_PART_PAIR_LEN EQUAL 1)
                set(NODE_MODULE "${NODE_IMPLS}")
                set(NODE_MODULE_PART "impl")
            elseif(MODULE_PART_PAIR_LEN EQUAL 2)
                list(POP_FRONT MODULE_PART_PAIR NODE_MODULE NODE_MODULE_PART)
            else()
                message(FATAL_ERROR "NODES module '${NODE_IMPLS}' may have at most one colon ':' to specify the implementation module partition name")
            endif()
            string(SHA1 PARTITION_ID "${arg_GRAPH_TYPE}_${NODE_DOT_PATHS}")
        else()
            list(SORT NODE_IMPLS)
            list(REMOVE_DUPLICATES NODE_IMPLS)
            foreach(NODE_IMPL ${NODE_IMPLS})
                string(APPEND NODE_INCLUDES "#include \"${NODE_IMPL}\"\n")
            endforeach()
        endif()

        if(IS_MODULE)
            set(SRC_FILE_CONTENTS [=[
// DO NOT MODIFY: This file was generated by target_generate_di_src during the CMake configure step
module\;
${COMMON_HEADERS}
#if !DI_STD_MODULE
#include <type_traits>
#endif
module ${NODE_MODULE}:${NODE_MODULE_PART}_${PARTITION_ID}\;
import :${NODE_MODULE_PART}\;

import ${arg_GRAPH_MODULE}\;
${COMMON_MODULES}
#if DI_STD_MODULE
import std\;
#endif

${NODE_INSTANTIATIONS}
]=])
        else()
            set(SRC_FILE_CONTENTS [=[
// DO NOT MODIFY: This file was generated by target_generate_di_src during the CMake configure step

#include "${arg_GRAPH_HEADER}"
${COMMON_HEADERS}
${NODE_INCLUDES}
${NODE_INSTANTIATIONS}
]=])
        endif()

        if(${arg_UNITY})
            set(SRC_NAME "unity/src")
        else()
            list(JOIN NODE_DOT_PATHS "_" SRC_NAME)
        endif()
        if(IS_MODULE)
            set(SRC_FILE "${DI_GENERATOR_SOURCE_DIR}/m/${arg_GRAPH_MODULE}/${arg_ID}/${SRC_NAME}.cpp")
        else()
            set(SRC_FILE "${DI_GENERATOR_SOURCE_DIR}/h/${arg_GRAPH_HEADER}/${arg_ID}/${SRC_NAME}.cpp")
        endif()
        set(${OUT_SRC_FILES} "${${OUT_SRC_FILES}}" "${SRC_FILE}" PARENT_SCOPE)
        file(CONFIGURE OUTPUT "${SRC_FILE}" CONTENT ${SRC_FILE_CONTENTS})
    endfunction(write_src_file)

    string(REGEX REPLACE "[; ]*,[; ]*" "\\\;" NODES "${arg_NODES}")
    while(NODES)
        list(POP_FRONT NODES tmp_NODE_DOT_PATHS tmp_NODE_IMPLS)
        list(APPEND NODE_DOT_PATHS ${tmp_NODE_DOT_PATHS})
        list(APPEND NODE_IMPLS ${tmp_NODE_IMPLS})

        if(NOT ${arg_UNITY})
            write_src_file(SRC_FILES)
            set(NODE_IMPLS)
            set(NODE_DOT_PATHS)
        endif()
    endwhile()

    if(${arg_UNITY})
        write_src_file(SRC_FILES)
    endif()

    list(SORT SRC_FILES)
    list(REMOVE_DUPLICATES SRC_FILES)
    if(IS_MODULE)
        target_sources(${TARGET_NAME} PRIVATE
            FILE_SET di_src_modules TYPE CXX_MODULES
            BASE_DIRS ${DI_GENERATOR_SOURCE_DIR}
            FILES ${SRC_FILES})
    else()
        target_sources(${TARGET_NAME} PRIVATE ${SRC_FILES})
    endif()

    if(${arg_PRECOMPILE_HEADERS})
        set(PCHS \"di/di.hpp\" \"${arg_GRAPH_HEADER}\")
        foreach(PCH ${arg_COMMON_HEADERS})
            list(APPEND PCHS \"${PCH}\")
        endforeach()
        list(SORT PCHS)
        list(REMOVE_DUPLICATES PCHS)
        target_precompile_headers(${TARGET_NAME} PRIVATE ${PCHS})
    endif()

endfunction(target_generate_di_src)

function(target_generate_di_headers TARGET_NAME)
    di_import_properties()

    set(options)
    set(oneValueArgs INCLUDE_DIR ID)
    set(multiValueArgs GLOB FILES EMBED)
    cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${arg_INCLUDE_DIR}")

    if(arg_GLOB)
        foreach(SUB_DIR ${arg_GLOB})
            list(APPEND GLOB_EXPRESSIONS "${INCLUDE_DIR}/${SUB_DIR}/*.hxx.dig")
        endforeach()
        file(GLOB_RECURSE INPUT_FILES RELATIVE ${INCLUDE_DIR} CONFIGURE_DEPENDS ${GLOB_EXPRESSIONS})
    elseif((NOT DEFINED arg_FILES AND NOT DEFINED arg_EMBED) OR "GLOB" IN_LIST arg_KEYWORDS_MISSING_VALUES)
        file(GLOB_RECURSE INPUT_FILES RELATIVE ${INCLUDE_DIR} CONFIGURE_DEPENDS "${INCLUDE_DIR}/*.hxx.dig")
    endif()

    macro(generate WORKING_DIR INPUT_FILE EMBED_PATH)
        get_filename_component(INPUT_FILE_NAME "${INPUT_FILE}" NAME)
        get_filename_component(INPUT_FILE_LE "${INPUT_FILE}" LAST_EXT)
        get_filename_component(INPUT_CLONE_PATH "${DI_GENERATOR_CLONE_DIR}/${INPUT_FILE}" DIRECTORY)
        if(INPUT_FILE_LE STREQUAL ".dig")
            get_filename_component(OUTPUT_INCLUDE_PATH "${DI_GENERATOR_INCLUDE_DIR}/${INPUT_FILE}" DIRECTORY)
            get_filename_component(INPUT_FILE_WLE "${INPUT_FILE}" NAME_WLE)
            set(OUTPUT_FILE "${OUTPUT_INCLUDE_PATH}/${INPUT_FILE_WLE}")
        else()
            set(OUTPUT_FILE "${DI_GENERATOR_INCLUDE_DIR}/${EMBED_PATH}")
            get_filename_component(OUTPUT_INCLUDE_PATH "${OUTPUT_FILE}" DIRECTORY)
        endif()
        set(INPUT_CLONE "${INPUT_CLONE_PATH}/${INPUT_FILE_NAME}")
        list(APPEND INPUT_CLONES "${INPUT_CLONE}")

        add_custom_command(
            OUTPUT ${INPUT_CLONE}
            BYPRODUCTS ${OUTPUT_FILE}
            DEPENDS
                di_generator_python
                "${WORKING_DIR}/${INPUT_FILE}"
                "${DI_GENERATOR_DIR}/generate.py"
                "${DI_GENERATOR_DIR}/dig_header.lark"
                "${DI_GENERATOR_DIR}/dig.lark"
                "${DI_GENERATOR_DIR}/template.hxx.jinja"
                "${DI_GENERATOR_DIR}/template.jinja"
            COMMAND ${CMAKE_COMMAND} -E copy "${INPUT_FILE}" "${INPUT_CLONE}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_INCLUDE_PATH}"
            COMMAND ${DI_GENERATOR_PYTHON} ${DI_GENERATOR_DIR}/generate.py -i ${INPUT_FILE} -o ${OUTPUT_FILE}
            WORKING_DIRECTORY "${WORKING_DIR}"
            COMMENT "${TARGET_NAME}_hxx_dig: ${INPUT_FILE}"
        )
    endmacro(generate)

    if(arg_ID)
        set(CUSTOM_TARGET "${TARGET_NAME}_${arg_ID}_hxx_dig")
    else()
        set(CUSTOM_TARGET "${TARGET_NAME}_hxx_dig")
    endif()

    add_dependencies(${TARGET_NAME} ${CUSTOM_TARGET})

    target_include_directories(${TARGET_NAME} PUBLIC ${DI_GENERATOR_INCLUDE_DIR})

    list(APPEND INPUT_FILES ${arg_FILES})
    list(SORT INPUT_FILES)
    list(REMOVE_DUPLICATES INPUT_FILES)

    foreach(INPUT_FILE ${INPUT_FILES})
        generate("${INCLUDE_DIR}" "${INPUT_FILE}" "")
    endforeach()

    while(arg_EMBED)
        list(POP_FRONT arg_EMBED INPUT_FILE EMBED_PATH)
        generate("${CMAKE_CURRENT_SOURCE_DIR}" "${INPUT_FILE}" "${EMBED_PATH}")
    endwhile()

    add_custom_target("${CUSTOM_TARGET}" DEPENDS ${INPUT_CLONES})

endfunction(target_generate_di_headers)


function(target_generate_di_modules TARGET_NAME)
    di_import_properties()

    set(options)
    set(oneValueArgs MODULE_DIR ID)
    set(multiValueArgs GLOB FILES EMBED)
    cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${arg_MODULE_DIR}")

    if(arg_GLOB)
        foreach(SUB_DIR ${arg_GLOB})
            list(APPEND GLOB_EXPRESSIONS "${MODULE_DIR}/${SUB_DIR}/*.ixx.dig")
        endforeach()
        file(GLOB_RECURSE INPUT_FILES RELATIVE ${MODULE_DIR} CONFIGURE_DEPENDS ${GLOB_EXPRESSIONS})
    elseif((NOT DEFINED arg_FILES AND NOT DEFINED arg_EMBED) OR "GLOB" IN_LIST arg_KEYWORDS_MISSING_VALUES)
        file(GLOB_RECURSE INPUT_FILES RELATIVE ${MODULE_DIR} CONFIGURE_DEPENDS "${MODULE_DIR}/*.ixx.dig")
    endif()

    list(APPEND INPUT_FILES ${arg_FILES})
    list(APPEND INPUT_FILES ${arg_EMBED})
    list(SORT INPUT_FILES)
    list(REMOVE_DUPLICATES INPUT_FILES)

    foreach(INPUT_FILE ${INPUT_FILES})
        get_filename_component(INPUT_FILE_NAME "${INPUT_FILE}" NAME)
        get_filename_component(INPUT_FILE_LE "${INPUT_FILE}" LAST_EXT)
        get_filename_component(INPUT_CLONE_PATH "${DI_GENERATOR_CLONE_DIR}/${INPUT_FILE}" DIRECTORY)
        get_filename_component(OUTPUT_MODULE_PATH "${DI_GENERATOR_MODULES_DIR}/${INPUT_FILE}" DIRECTORY)
        set(INPUT_CLONE "${INPUT_CLONE_PATH}/${INPUT_FILE_NAME}")
        if(INPUT_FILE_LE STREQUAL ".dig")
            get_filename_component(INPUT_FILE_WLE "${INPUT_FILE}" NAME_WLE)
            set(OUTPUT_FILE "${OUTPUT_MODULE_PATH}/${INPUT_FILE_WLE}")
        else()
            set(OUTPUT_FILE "${OUTPUT_MODULE_PATH}/${INPUT_FILE_NAME}.embed.ixx")
        endif()
        list(APPEND INPUT_CLONES "${INPUT_CLONE}")
        list(APPEND OUTPUT_FILES "${OUTPUT_FILE}")

        add_custom_command(
            OUTPUT ${INPUT_CLONE}
            BYPRODUCTS ${OUTPUT_FILE}
            DEPENDS
                di_generator_python
                "${MODULE_DIR}/${INPUT_FILE}"
                "${DI_GENERATOR_DIR}/generate.py"
                "${DI_GENERATOR_DIR}/dig.lark"
                "${DI_GENERATOR_DIR}/dig_module.lark"
                "${DI_GENERATOR_DIR}/template.jinja"
                "${DI_GENERATOR_DIR}/template.ixx.jinja"
            COMMAND ${CMAKE_COMMAND} -E copy "${INPUT_FILE}" "${INPUT_CLONE}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_MODULE_PATH}"
            COMMAND ${DI_GENERATOR_PYTHON} ${DI_GENERATOR_DIR}/generate.py -m -i "${INPUT_FILE}" -o "${OUTPUT_FILE}"
            WORKING_DIRECTORY "${MODULE_DIR}"
            COMMENT "${TARGET_NAME}_ixx_dig: ${INPUT_FILE}"
        )
    endforeach()

    if(arg_ID)
        set(CUSTOM_TARGET "${TARGET_NAME}_${arg_ID}_ixx_dig")
    else()
        set(CUSTOM_TARGET "${TARGET_NAME}_ixx_dig")
    endif()

    add_custom_target(${CUSTOM_TARGET} DEPENDS ${INPUT_CLONES})

    add_dependencies(${TARGET_NAME} ${CUSTOM_TARGET})

    target_sources(${TARGET_NAME} PUBLIC
        FILE_SET di_dig_modules TYPE CXX_MODULES
            BASE_DIRS
                ${DI_GENERATOR_MODULES_DIR}
            FILES
                ${OUTPUT_FILES}
    )

endfunction(target_generate_di_modules)
