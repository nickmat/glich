# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Name:        src/glc/CMakeLists.txt
# Project:     glc: Glich script library.
# Author:      Nick Matthews
# Website:     https://github.com/nickmat/glich
# Created:     8th February 2023
# Copyright:   Copyright (c) 2023..2024, Nick Matthews.
# Licence:     GNU GPLv3
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

set( GLC_SOURCES
    glc.cpp
    glcBlob.cpp
    glcFile.cpp
    glcFilesys.cpp
    glcFunction.cpp
    glcGlich.cpp
    glcHelper.cpp
    glcLibScripts.cpp
    glcMark.cpp
    glcMath.cpp
    glcObject.cpp
    glcScript.cpp
    glcStore.cpp
    glcTokenStream.cpp
    glcValue.cpp
    glcVersion.cpp
)

set( GLC_HEADERS
    ../../include/glc/glc.h
    ../../include/glc/glcDefs.h
    ../../include/glc/glcGlich.h
    glcBlob.h
    glcFile.h
    glcFilesys.h
    glcFunction.h
    glcHelper.h
    glcLibScripts.h
    glcMark.h
    glcMath.h
    glcObject.h
    glcScript.h
    glcStore.h
    glcTokenStream.h
    glcValue.h
    glcVersion.h
)

add_library( glc ${GLC_SOURCES} ${GLC_HEADERS} )

target_link_libraries( glc PRIVATE utf8api )
