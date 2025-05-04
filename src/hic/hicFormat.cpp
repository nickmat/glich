/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicFormat.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Format class to control the formatting and parsing of dates.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     24th March 2023
 * Copyright:   Copyright (c) 2023..2025, Nick Matthews.
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

#include "hicFormat.h"

#include "glcValue.h"
#include "hicRecord.h"

#include <cassert>

using namespace glich;
using std::string;

Format::Format( const std::string& code, Grammar& gmr, FmtRules rules )
    : m_code( code ), m_owner( gmr ), m_rules( rules ), m_order( 0 ), m_ok( false ),
    m_visible( true ), m_priority( 0 ), m_shorthand( false )
{
}

Format::~Format()
{
}

void Format::get_info( Format_info* info ) const
{
    info->code = m_code;
    info->gmr_code = m_owner.get_code();
    info->input_str = m_input_str;
    info->output_str = m_output_str;
    info->priority = m_priority;
    info->type = get_format_type();
}

SValue Format::string_to_object( const string& ocode, const Base& base, const string& input ) const
{
    Record mask( base, input, *this, Boundary::None );
    return mask.get_object( ocode );
}

string Format::get_revealed_text( Record& record, BoolVec& reveal ) const
{
    return get_text_output( record );
}

string Format::get_range_text( const string& beg, const string& end ) const
{
    return beg + ".." + end;
}

bool Format::set_input( Record& record, const string& input ) const
{
    return set_input( record, input, Boundary::None );
}

string Format::jdn_to_string( const Scheme& sch, Field jdn ) const
{
    if( jdn == f_minimum ) {
        return "past";
    }
    if( jdn == f_maximum ) {
        return "future";
    }
    Record rec( sch, jdn );
    return get_text_output( rec );
}

string Format::range_to_string( const Scheme& sch, const Range& rng ) const
{
    Record rec1( sch, rng.m_beg );
    Record rec2( sch, rng.m_end );

    string str1, str2;
    if( allow_shorthand() ) {
        BoolVec reveal = get_reveal( rec1, rec2 );
        str1 = get_revealed_text( rec1, reveal );
        str2 = get_revealed_text( rec2, reveal );
    }
    else {
        str1 = get_text_output( rec1 );
        str2 = get_text_output( rec2 );
    }
    if( str1 == str2 ) {
        return get_date_text( str1 );
    }
    return get_range_text( str1, str2 );
}

string Format::rlist_to_string( const Scheme& sch, const RList& rlist ) const
{
    string output;
    for( const Range& rng : rlist ) {
        if( !output.empty() ) {
            output += " | ";
        }
        output += range_to_string( sch, rng );
    }
    return output;
}

StringPairVec Format::string_to_stringpair( string& text ) const
{
    StringPair pair;
    StringPairVec pairs;
    for( ;;) {
        size_t pos1 = text.find( '|' );
        string rangestr = text.substr( 0, pos1 );
        size_t pos2 = rangestr.find( ".." );
        if( pos2 == string::npos ) {
            pair.first = rangestr;
            pair.second = string();
        }
        else {
            pair.first = rangestr.substr( 0, pos2 );
            pair.second = rangestr.substr( pos2 + 2 );
        }
        pairs.push_back( pair );
        if( pos1 == string::npos ) {
            break;
        }
        text = text.substr( pos1 + 1 );
    }
    return pairs;
}


// End of src/cal/calformat.cpp file
