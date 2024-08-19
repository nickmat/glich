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
    glcFile.cpp
    glcFilesys.cpp
    glcFunction.cpp
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
    hicAstro.cpp
    hicBase.cpp
    hicChinese.cpp
    hicDatePhrase.cpp
    hicEaster.cpp
    hicElement.cpp
    hicFormat.cpp
    hicFormatIso.cpp
    hicFormatText.cpp
    hicFormatUnit.cpp
    hicFrench.cpp
    hicGrammar.cpp
    hicGregorian.cpp
    hicHebrew.cpp
    hicHelper.cpp
    hicHybrid.cpp
    hicIslamic.cpp
    hicIsoOrdinal.cpp
    hicIsoWeek.cpp
    hicJulian.cpp
    hicJwn.cpp
    hicLexicon.cpp
    hicLibScripts.cpp
    hicLiturgical.cpp
    hicMath.cpp
    hicOptional.cpp
    hicRecord.cpp
    hicRomanNum.cpp
    hicScheme.cpp
    hicScript.cpp
)

set( GLC_HEADERS
    ../../include/glc/glc.h
    ../../include/glc/glcDefs.h
    ../../include/glc/hicDefs.h
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
    hicAstro.h
    hicBase.h
    hicCalendars.h
    hicChinese.h
    hicDatePhrase.h
    hicEaster.h
    hicElement.h
    hicFormat.h
    hicFormatIso.h
    hicFormatText.h
    hicFormatUnit.h
    hicFrench.h
    hicGrammar.h
    hicGregorian.h
    hicHebrew.h
    hicHelper.h
    hicHybrid.h
    hicIslamic.h
    hicIsoOrdinal.h
    hicIsoWeek.h
    hicJdn.h
    hicJulian.h
    hicJwn.h
    hicLexicon.h
    hicLibScripts.h
    hicLiturgical.h
    hicMath.h
    hicOptional.h
    hicRecord.h
    hicRomanNum.h
    hicScheme.h
    hicScript.h
)

add_library( glc ${GLC_SOURCES} ${GLC_HEADERS} )

target_link_libraries( glc PRIVATE utf8api )
