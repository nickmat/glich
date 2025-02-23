# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Name:        src/glich/CMakeLists.txt
# Project:     glich: Run a Glich script file.
# Author:      Nick Matthews
# Website:     https://github.com/nickmat/glich
# Created:     23th February 2025
# Copyright:   Copyright (c) 2025, Nick Matthews.
# Licence:     GNU GPLv3
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

set( GLICH_SOURCES
    glich.cpp
)

set( GLICH_HEADERS
    glcsresource.h
)

set( GLICH_RCS
    glcs.rc
)

add_executable( glich ${GLICH_SOURCES} ${GLICH_HEADERS} ${GLICH_RCS} )

target_link_libraries( glich PRIVATE hic )
