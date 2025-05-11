# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Name:        src/glc/CMakeLists.txt
# Project:     glc: Glich script hic extension library.
# Author:      Nick Matthews
# Website:     https://github.com/nickmat/glich
# Created:     22nd October 2024
# Copyright:   Copyright (c) 2024..2025, Nick Matthews.
# Licence:     GNU GPLv3
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

set( HIC_SOURCES
    hic.cpp
    hicAstro.cpp
    hicAtFunction.cpp
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
    hicGlich.cpp
    hicGrammar.cpp
    hicGregorian.cpp
    hicHebrew.cpp
    hicHelper.cpp
    hicHybrid.cpp
    hicIslamic.cpp
    hicIsoOrdinal.cpp
    hicIsoWeek.cpp
    hicJdn.cpp
    hicJulian.cpp
    hicJwn.cpp
    hicLexicon.cpp
    hicLibScripts.cpp
    hicMark.cpp
    hicMath.cpp
    hicOptional.cpp
    hicRecord.cpp
    hicRomanNum.cpp
    hicScheme.cpp
    hicScript.cpp
)

set( HIC_HEADERS
    ../../include/glc/hic.h
    ../../include/glc/hicDefs.h
    ../../include/glc/hicGlich.h
    hicAstro.h
    hicAtFunction.h
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
    hicMark.h
    hicMath.h
    hicOptional.h
    hicRecord.h
    hicRomanNum.h
    hicScheme.h
    hicScript.h
)

include_directories( ../glc )

add_library( hic ${HIC_SOURCES} ${HIC_HEADERS} )

target_link_libraries( hic PRIVATE glc utf8api )
