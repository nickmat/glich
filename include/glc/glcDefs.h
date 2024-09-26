/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        include/glc/glcDefs.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     General defines
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     8th February 2023
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

#ifndef INCLUDE_GLC_GLCDEFS_H
#define INCLUDE_GLC_GLCDEFS_H

#include <cstdint>
#include <cstdlib>
#include <cinttypes>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace glich {

    using Num = int64_t;

#define NUM "%" PRId64 

    using  StdStrVec = std::vector< std::string >;
    using  StdStrMap = std::map< std::string, std::string >;
    using  IntVec = std::vector< int >;
    using  DoubleVec = std::vector< double >;
    using  BoolVec = std::vector< bool >;

    using  NumStringMap = std::map< Num, std::string >;
    using  NumVec = std::vector< Num >;

    using Field = int32_t;

#define FLD "%" PRId32

    using LongField = int64_t;

    using FieldVec = std::vector<Field>;
    using FieldVecVec = std::vector<FieldVec>;

    constexpr Field f_invalid = -2147483648;  // INT_MIN
    constexpr Field f_minimum = -2147483645;  // -(INT_MAX - 2)
    constexpr Field f_maximum =  2147483645;  // INT_MAX - 2
    constexpr Field f_end     =  2147483646;  // INT_MAX - 1
    constexpr Field f_invalid2 = 2147483647;  // INT_MAX

    struct Range
    {
        Field m_beg;
        Field m_end;

        Range() : m_beg( f_invalid ), m_end( f_invalid ) {}
        Range( Field fld ) : m_beg( fld ), m_end( fld ) {}
        Range( Field beg, Field end ) : m_beg( beg ), m_end( end ) {}

        bool is_valid() const { return m_beg != f_invalid && m_end != f_invalid && m_beg <= m_end; }
    };

    inline bool operator==( const Range& left, const Range& right ) {
        return left.m_beg == right.m_beg && left.m_end == right.m_end;
    }
    inline bool operator!=( const Range& left, const Range& right ) {
        return !(left == right);
    }

    using RList = std::vector< Range >;

    inline bool operator==( const RList& left, const RList& right ) {
        if( left.size() != right.size() ) {
            return false;
        }
        for( size_t i = 0; i < left.size(); i++ ) {
            if( left[i] != right[i] ) {
                return false;
            }
        }
        return true;
    }
    inline bool operator!=( const RList& left, const RList& right ) {
        return !(left == right);
    }

    class Function;
    using FunctionMap = std::map<std::string, Function*>;
    using SpFunction = std::shared_ptr<Function>;
    using SpFunctionMap = std::map<std::string, SpFunction>;
    using SpFunctionVec = std::vector<SpFunction>;
    using NameIndexMap = std::map<std::string, size_t>;

    class SValue;
    using SValueMap = std::map<std::string, SValue>;

    enum class VariableType { unknown, local, global, constant };
    enum class Integer { number, field };
    enum class Context { glich, hics };

    enum class GetToken { current, next }; // Was a bool, next=true : current=false

    using StringPair = std::pair<std::string, std::string>;
    using StringPairVec = std::vector<StringPair>;

    struct GlcData {
        std::string type;
        std::string name;
        std::string value;
    };

    using GlcDataVec = std::vector<GlcData>;

    struct GlcMark {
        std::string name;
        GlcDataVec  var;
        GlcDataVec  obj;
        GlcDataVec  fun;
        GlcDataVec  com;
        GlcDataVec  file;
        GlcDataVec  lex;
        GlcDataVec  gmr;
        GlcDataVec  fmt;
        GlcDataVec  sch;
    };

    using GlcMarkVec = std::vector<GlcMark>;

    struct ModuleDef {
        std::string m_definition;
        StdStrVec m_codes;
    };
    using ModuleDefVec = std::vector<ModuleDef>;

    struct Module {
        std::string m_code;
        std::string m_find;
        ModuleDefVec m_defs;
    };

    enum class DefinedStatus { none, module, defined };

}

#endif // INCLUDE_GLC_GLCDEFS_H
