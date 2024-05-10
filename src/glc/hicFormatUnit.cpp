/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicFormatUnit.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     FormatUnit class to read and write as units.
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

#include "hicFormatUnit.h"

#include "glcHelper.h"
#include "hicBase.h"
#include "hicRecord.h"

using namespace glich;
using std::string;


FormatUnit::FormatUnit( const std::string& code, Grammar& gmr )
    : Format( code, gmr )
{
    StdStrVec fieldnames = gmr.get_base_fieldnames();
    fieldnames = vec_append( fieldnames, gmr.get_calc_fieldnames() );
    string pseudo;
    bool started = false;
    for( string& fname : fieldnames ) {
        if( started ) {
            pseudo += " ";
        }
        started = true;
        string unit = gmr.find_unit_key( fname );
        pseudo += "9" + unit;
    }
    set_user_input_str( pseudo );
    set_user_output_str( pseudo );
}

std::string FormatUnit::get_text_output( Record& record ) const
{
    string output;
    const Base& base = record.get_base();

    for( size_t i = 0; i < base.record_size(); i++ ) {
        Field fld = record.get_field_at( i );
        if( fld != f_invalid ) {
            string fname = base.get_fieldname( i );
            string uname = get_grammar().get_unit_alias( fname );
            if( !output.empty() ) {
                output += " ";
            }
            output += std::to_string( fld ) + uname;
        }
    }
    return output;
}

Range glich::FormatUnit::string_to_range( const Base& base, const std::string& input, FunctionData* fdata ) const
{
    Range range;
    Record rec1( base ), rec2( base );
    if( set_input( rec1, input, Boundary::Begin ) && set_input( rec2, input, Boundary::End ) ) {
        range = { rec1.get_jdn(), rec2.get_jdn() };
        if( range.m_beg != f_invalid && range.m_end != f_invalid ) {
            return range;
        }
    }
    return Range();
}

// Ignores the boundary as we need a mask for multiple ranges.
bool FormatUnit::set_input( Record& record, const std::string& input, Boundary rb ) const
{
    record.clear_fields();
    enum step_t { START, VALUE, UNIT };
    step_t step = START;
    string value, unit;
    for( string::const_iterator it = input.begin(); it != input.end(); it++ ) {
        int ch = *it;
        if( u8_isspace( ch ) ) {
            continue;
        }
        if( step != VALUE && (ch == '-' || u8_isdigit( ch )) ) {
            if( !value.empty() ) {
                set_field_by_unit( record, value, unit );
            }
            value.clear();
            unit.clear();
            step = VALUE;
            value += ch;
            continue;
        }
        if( step == VALUE ) {
            if( u8_isdigit( ch ) ) {
                value += ch;
                continue;
            }
            step = UNIT;
        }
        unit += ch;
    }
    set_field_by_unit( record, value, unit );
    if( rb == Boundary::Begin ) {
        record.complete_fields_as_beg();
    }
    else if( rb == Boundary::End ) {
        record.complete_fields_as_end();
    }
    return true;
}

void FormatUnit::set_field_by_unit( Record& record, const string& value, const string& unit ) const
{
    Field v = str_to_field( value );
    if( v == f_invalid ) {
        return;
    }
    string fn = get_owner()->resolve_unit_alias( unit );
    int i = record.get_base().get_fieldname_index( fn );

    if( i < 0 ) {
        return;
    }
    record.set_field( v, i );
}

// End of src/cal/calformatunit.cpp file
