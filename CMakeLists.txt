cmake_minimum_required(VERSION 3.26)
project(PretyPrinter
        LANGUAGES C
        VERSION 0.0.1
        DESCRIPTION "A prety printer project for MPPL language")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB src_files ./*.c)

add_executable(PretyPrinter ${src_files})

#target_include_directories(Scanner PUBLIC .)
target_compile_features(PretyPrinter PUBLIC c_std_11)

# For test
#option(TEST_OPTION "Build the program that can test by pytest." OFF)
#
#if (TEST_OPTION)
#    set_target_properties(Scanner
#            PROPERTIES
#            OUTPUT_NAME "tc"
#            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
#    )
#else ()
#    target_compile_definitions(Scanner PRIVATE EXTRA_OUTPUT_OPTION)
#endif (TEST_OPTION)

#include(CTest)
#enable_testing()
#add_subdirectory(test)