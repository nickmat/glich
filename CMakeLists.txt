# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Name:        glich/CMakeLists.txt
# Project:     Glich: Glich Script Language.
# Author:      Nick Matthews
# Website:     https://github.com/nickmat/glich
# Created:     8th February 2023
# Copyright:   Copyright (c) 2023..2025, Nick Matthews.
# Licence:     GNU GPLv3
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

cmake_minimum_required( VERSION 3.25 )

project( glich )

option( BUILD_SHARED_LIBS "Set static libs" OFF )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

include_directories( include )

add_subdirectory( 3rdparty/utf8proc)
add_subdirectory( src/glc )
add_subdirectory( src/hic )
add_subdirectory( src/glcs )
add_subdirectory( src/glich )
add_subdirectory( src/utf8 )
add_subdirectory( test/glctest )
add_subdirectory( test/glcunit )
add_subdirectory( test/glcsoak )
