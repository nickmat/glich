/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        include/glc/hicDefs.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     General defines for hics extension
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     16th March 2023
 * Copyright:   Copyright (c) 2023..2024, Nick Matthews.
 * Licence:     GNU GPLv3
 *
 *  Glich is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Glich is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Glich.  If not, see <http://www.gnu.org/licenses/>.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

*/

#ifndef INCLUDE_GLC_HICDEFS_H_GUARD
#define INCLUDE_GLC_HICDEFS_H_GUARD

#include "glcDefs.h"


namespace glich {

    class Scheme;
    using SchemeMap = std::map<std::string, Scheme*>;
    using SchemeVec = std::vector<Scheme*>;

    struct SchemeData
    {
        std::string code;
        std::string name;
    };
    typedef std::vector<SchemeData> SchemeList;

    class Grammar;
    using GrammarMap = std::map<std::string, Grammar*>;
    using GrammarVec = std::vector<Grammar*>;
    class Lexicon;
    using LexiconMap = std::map<std::string, Lexicon*>;
    using LexiconVec = std::vector<Lexicon*>;
    class Format;
    using FormatMap = std::map<std::string, Format*>;
    using FormatVec = std::vector<Format*>;

    using XIndexVec = std::vector<size_t>;

    using XRefVec = std::vector<int>;

    inline void stringvec_cat( StdStrVec& a, const StdStrVec& b ) {
        a.insert( a.end(), b.begin(), b.end() );
    }

    struct Scheme_info
    {
        std::string name;
        std::string code;
        std::string grammar_code;
        StdStrVec lexicon_codes;
        StdStrVec lexicon_names;
    };

    enum class FmtRules { None, Text, Unit, Iso8601 };
    enum class FormatStyle { Selected, Default, Hidden };

    struct PCode {
        std::string code;
        int priority = 0;
    };

    struct PDesc {
        std::string desc;
        std::vector<PCode> codes;
    };

    struct SchemeFormatInfo {
        std::vector<PDesc> descs;
        size_t current;
    };

    enum class FormatType { null, text, unit, iso };

    struct Format_info {
        Format_info() : type( FormatType::null), priority(0), style( FormatStyle::Default ) {}

        std::string  code;
        FormatType   type;
        std::string  gmr_code;
        std::string  input_str;
        std::string  output_str;
        int          priority;
        FormatStyle  style;
    };

    struct FormatText_info {
        Format_info info;
        std::string control_str;
    };

    struct Lexicon_info
    {
        std::string name;
        std::string lang;
        std::string style_full_name;
        std::string style_abbrev_name;
        StdStrVec words;
        StdStrVec abbrevs;
    };

    enum Unit {
        unit_null,
        unit_year,
        unit_month,
        unit_week,
        unit_day,
        unit_max
    };

    using StrUnitMap = std::map<std::string, Unit>;

    // Normalisation flag. Determines how invalid dates are corrected.
    enum Norm {
        norm_expand,     // Add missing days to the next unit.
        norm_expand_min, // Go to the start of the next unit.
        norm_truncate,   // Truncate the current unit.
        norm_average     // Add the average number of days for the unit.
    };

    // Relative date functions
    enum Rel {
        rel_duration,
        rel_add_to_begin,
        rel_add_to_end,
        rel_age_round_down,
    };

    // Relative date info
    struct Rel_info
    {
        Rel   rel;
        Field value;
        Unit  unit;
        Norm  norm;
    };

    struct HicMarkData {
        GlcMarkData glc;
        GlcDataVec  lex;
        GlcDataVec  gmr;
        GlcDataVec  fmt;
        GlcDataVec  sch;
    };

    using HicMarkDataVec = std::vector<HicMarkData>;

}

#endif // INCLUDE_GLC_HICDEFS_H_GUARD
