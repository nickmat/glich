# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Name:        test/glcunit/CMakeLists.txt
# Project:     glcUnit: Unit test program for the Glich and Hics library.
# Author:      Nick Matthews
# Website:     https://github.com/nickmat/glich
# Created:     17th August 2023
# Copyright:   Copyright (c) 2023, Nick Matthews.
# Licence:     GNU GPLv3
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

set( TEST_GLCUNIT_FILES
    guAPI.cpp
    guMain.cpp
    guSch_ay.cpp
    guSch_g.cpp
    guSch_h.cpp
    guSch_j.cpp
    guSch_ja.cpp
)

set( TEST_GLCUNIT_HEADERS
    ../../include/catch2/catch.hpp
    guMain.h
)

add_executable( glcunit ${TEST_GLCUNIT_FILES} ${TEST_GLCUNIT_HEADERS} )

target_link_libraries( glcunit PRIVATE glc )
