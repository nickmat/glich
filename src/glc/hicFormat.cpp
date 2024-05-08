/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicFormat.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Format class to control the formatting and parsing of dates.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     24th March 2023
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

#include "hicFormat.h"

#include "glcValue.h"
#include "hicRecord.h"

#include <cassert>

using namespace glich;
using std::string;

Format::Format( const std::string& code, Grammar& gmr )
    : m_code( code ), m_owner( gmr ), m_ok( false ), m_style( FormatStyle::Default ),
    m_priority( 0 )
{
}

Format::~Format()
{
}

void glich::Format::get_info( Format_info* info ) const
{
    info->code = m_code;
    info->gmr_code = m_owner.get_code();
    info->input_str = m_input_str;
    info->output_str = m_output_str;
    info->priority = m_priority;
    info->type = get_format_type();
    info->style = m_style;
}

SValue Format::string_to_object( const string& ocode, const Base& base, const string& input ) const
{
    Record mask( base, input, *this, Boundary::None );
    return mask.get_object( ocode );
}

bool glich::Format::set_input( Record& record, const std::string& input ) const
{
    return set_input( record, input, Boundary::None );
}

string Format::jdn_to_string( const Base& base, Field jdn ) const
{
    if( jdn == f_minimum ) {
        return "past";
    }
    if( jdn == f_maximum ) {
        return "future";
    }
    Record rec( base, jdn );
    return get_text_output( rec );
}

string Format::range_to_string( const Base& base, const Range& rng ) const
{
    string output = jdn_to_string( base, rng.m_beg );
    output += "..";
    output += jdn_to_string( base, rng.m_end );
    return output;
}

string Format::rlist_to_string( const Base& base, const RList& rlist ) const
{
    string output;
    for( const Range& rng : rlist ) {
        if( !output.empty() ) {
            output += " | ";
        }
        output += range_to_string( base, rng );
    }
    return output;
}

StringPairVec glich::Format::string_to_stringpair( std::string& text ) const
{
    StringPair pair;
    StringPairVec pairs;
    for( ;;) {
        size_t pos1 = text.find( '|' );
        string rangestr = text.substr( 0, pos1 );
        size_t pos2 = rangestr.find( '..' );
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
