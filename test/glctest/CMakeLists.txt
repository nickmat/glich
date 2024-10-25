# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Name:        src/glctest/CMakeLists.txt
# Project:     glctest: Glich script library script test suite.
# Author:      Nick Matthews
# Website:     https://github.com/nickmat/glich
# Created:     8th February 2023
# Copyright:   Copyright (c) 2023, Nick Matthews.
# Licence:     GNU GPLv3
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

set( HGTEST_SOURCES
    glcsGetfns.cpp
    glcsTest.cpp
)

set( HGTEST_HEADERS
    ../../include/win/dirent.h
    glcsTestmain.h
)

add_executable( glctest ${HGTEST_SOURCES} ${HGTEST_HEADERS} )

target_link_libraries( glctest PRIVATE hic )
