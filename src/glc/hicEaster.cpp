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
#include "hicMath.h"
#include "hicRecord.h"

#include <cassert>

using namespace glich;
using std::string;


Easter::Easter( const std::string& data )
    : Base( string(), 4 )
{
    m_fieldnames = { "eyear", "repeat", "month", "day" };
    string tail, word = get_first_word( data, &tail );
    while( !word.empty() ) {
        set_data( word );
        word = get_first_word( tail, &tail );
    }
}

Field glich::Easter::get_jdn( const FieldVec& fields ) const
{
    assert( fields.size() >= 4 );
    Field eyear = fields[0];
    Field j1 = julian_to_jdn( eyear, fields[2], fields[3] );
    Field e1 = Julian::easter( eyear );
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
    return julian_to_jdn( year, fields[2], fields[3] );
}

Field glich::Easter::get_end_field_value( const FieldVec& fields, size_t index ) const
{
    return Field();
}

FieldVec glich::Easter::get_fields( Field jdn ) const
{
    Field year, repeat = 0, month, day;
    julian_from_jdn( &year, &month, &day, jdn );
    Field e1 = Julian::easter( year );
    if( e1 > jdn ) {
        --year;
        Field e0 = Julian::easter( year );
        Field y1, m1, d1;
        julian_from_jdn( &y1, &m1, &d1, e0 );
        Field eplus = julian_to_jdn( y1 + 1, m1, d1 );
        if( jdn >= eplus ) {
            repeat = 1;
        }
    }
    return { year, repeat, month, day };
}


// End of src/glc/hicJulian.cpp file
