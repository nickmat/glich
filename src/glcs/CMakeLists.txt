# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Name:        src/glcs/CMakeLists.txt
# Project:     glcs: Interactive terminal for higs script language.
# Author:      Nick Matthews
# Website:     https://github.com/nickmat/glich
# Created:     8th February 2023
# Copyright:   Copyright (c) 2023, Nick Matthews.
# Licence:     GNU GPLv3
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

set( GLCS_SOURCES
    glcs.cpp
)

set( GLCS_HEADERS
    glcsresource.h
)

set( GLCS_RCS
    glcs.rc
)

add_executable( glcs ${GLCS_SOURCES} ${GLCS_HEADERS} ${GLCS_RCS} )

target_link_libraries( glcs PRIVATE hic )
