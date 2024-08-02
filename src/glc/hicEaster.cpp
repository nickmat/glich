/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicEaster.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Julian Easter (je) calendar source.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     27th July 2024
 * Copyright:   Copyright (c) 2024, Nick Matthews.
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

#include "hicEaster.h"

#include "glcHelper.h"
#include "hicJulian.h"

#include <cassert>

using namespace glich;
using std::string;


Easter::Easter( const StdStrVec& data )
    : m_day_offset(0), m_year_offset(0), Base( StdStrVec(), 4 )
{
    m_fieldnames = { "eyear", "repeat", "month", "day" };
    for( const string& word : data ) {
        cal_data( word );
    }
}

void Easter::cal_data( const string& data )
{
    string code, tail;
    split_code( &code, &tail, data );
    if( code == "day" ) {
        m_day_offset = str_to_field( tail );
    }
    if( code == "year" ) {
        m_year_offset = str_to_field( tail ) - 1;
    }
    else {
        set_data( data );
    }
}

Field Easter::get_jdn( const FieldVec& fields ) const
{
    assert( fields.size() >= 4 );
    Field eyear = fields[0];
    Field j1 = get_julian_to_jdn( eyear, fields[2], fields[3] );
    Field e1 = year_start( eyear );
    Field year;
    if( j1 < e1 ) {
        year = eyear + 1;
    }
    else {
        year = eyear;
    }
    if( fields[1] == 1 ) {
        year++;
    }
    return get_julian_to_jdn( year, fields[2], fields[3] );
}

Field Easter::get_end_field_value( const FieldVec& fields, size_t index ) const
{
    return Field();
}

FieldVec glich::Easter::get_fields( Field jdn ) const
{
    Field year, repeat = 0, month, day;
    get_julian_from_jdn( &year, &month, &day, jdn );
    Field e1 = year_start( year );
    if( e1 > jdn ) {
        --year;
        Field e0 = year_start( year );
        Field y1, m1, d1;
        get_julian_from_jdn( &y1, &m1, &d1, e0 );
        Field eplus = get_julian_to_jdn( y1 + 1, m1, d1 );
        if( jdn >= eplus ) {
            repeat = 1;
        }
    }
    return { year, repeat, month, day };
}

bool Easter::set_epoch( Field epoch )
{
    Field year, month, day;
    julian_from_jdn( &year, &month, &day, epoch );
    Field e = Julian::easter( year );
    m_day_offset = epoch - e;
    m_year_offset = year - 1;
    return true;
}

Field Easter::get_julian_to_jdn( Field year, Field month, Field day ) const
{
    return julian_to_jdn( year + m_year_offset, month, day );
}

void Easter::get_julian_from_jdn( Field* year, Field* month, Field* day, Field jdn ) const
{
    julian_from_jdn( year, month, day, jdn );
    Field y = *year - m_year_offset;
    *year = y;
}

Field Easter::year_start( Field year ) const
{
    return Julian::easter( year + m_year_offset ) + m_day_offset;
}


// End of src/glc/hicEaster.cpp file
