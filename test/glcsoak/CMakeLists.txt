# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Name:        test/glcsoak/CMakeLists.txt
# Project:     glcSoak: Soak (extended unit) test program for Hics library.
# Author:      Nick Matthews
# Website:     https://github.com/nickmat/glich
# Created:     4th September 2023
# Copyright:   Copyright (c) 2023, Nick Matthews.
# Licence:     GNU GPLv3
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

set( TEST_GLCSOAK_FILES
    gsMain.cpp
    gsSch_g.cpp
    gsSch_j.cpp
    gsSch_ja.cpp
)

set( TEST_GLCSOAK_HEADERS
    ../../include/catch2/catch.hpp
    gsMain.h
)

add_executable( glcsoak ${TEST_GLCSOAK_FILES} ${TEST_GLCSOAK_HEADERS} )

target_link_libraries( glcsoak PRIVATE hic )
