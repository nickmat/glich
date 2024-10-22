# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Name:        src/glc/CMakeLists.txt
# Project:     glc: Glich script hic extension library.
# Author:      Nick Matthews
# Website:     https://github.com/nickmat/glich
# Created:     22nd October 2024
# Copyright:   Copyright (c) 2024, Nick Matthews.
# Licence:     GNU GPLv3
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

set( HIC_SOURCES
    hic.cpp
    hicGlich.cpp
)

set( HIC_HEADERS
    ../../include/glc/hic.h
    ../../include/glc/hicGlich.h
)

include_directories( ../glc )

add_library( hic ${HIC_SOURCES} ${HIC_HEADERS} )

target_link_libraries( hic PRIVATE glc utf8api )
