/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        glc/glc/hicIsoWeek.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Base ISO Week calendar implimentation.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     11th July 2023
 * Copyright:   Copyright (c) 2023, Nick Matthews.
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

#include "hicIsoWeek.h"

#include "hicGregorian.h"
#include "hicMath.h"


using namespace glich;
using std::string;


Field IsoWeek::get_jdn( const FieldVec& fields ) const
{
    if( fields[0] == f_invalid || fields[1] == f_invalid || fields[2] == f_invalid ) {
        return f_invalid;
    }
    return to_jdn( fields[0], fields[1], fields[2] );
}

Field IsoWeek::get_end_field_value( const FieldVec& fields, size_t index ) const
{
    switch( index )
    {
    case 0: // Last year?
        return f_maximum;
    case 1: // Last week of year
        return last_week_of_year( fields[0] );
    case 2: // Last day of week
        return 7;
    }
    return f_invalid;
}

FieldVec IsoWeek::get_fields( Field jdn ) const
{
    FieldVec fields( record_size(), f_invalid );
    from_jdn( &fields[0], &fields[1], &fields[2], jdn );
    return fields;
}

Field IsoWeek::to_jdn( Field year, Field week, Field day )
{
    Field jdn = kday_nearest( WDAY_Monday, Gregorian::to_jdn( year, 1, 1 ) );
    return jdn + ( week - 1 ) * 7 + day - 1;
}

void IsoWeek::from_jdn( Field* year, Field* week, Field* day, Field jdn )
{
    if( jdn != f_invalid ) {
        if( year ) {
            *year = year_from_jdn( jdn );
        }
        if( week ) {
            Field y = year ? *year : year_from_jdn( jdn );
            *week = div_f( jdn - to_jdn( y, 1, 1 ), 7 ) + 1;
        }
        if( day ) {
            *day = day_of_week( jdn ) + 1; // Mon=1, Sun=7
        }
    }
}

Field IsoWeek::year_from_jdn( Field jdn )
{
    Field year = Gregorian::year_from_jdn( jdn - 3 );
    if( jdn >= to_jdn( year + 1, 1, 1 ) ) {
        return year + 1;
    }
    return year;
}

/* static */
bool IsoWeek::leap_year( Field year )
{
    return last_week_of_year( year ) == 53;
}

/* static */
Field IsoWeek::last_week_of_year( Field year )
{
    return ( to_jdn( year + 1, 1, 1 ) - to_jdn( year, 1, 1 ) ) / 7;
}

// End of glc/glc/hicIsoWeek.cpp file
