/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        glc/glc/hicIsoOrdinal.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Base ISO Ordinal calendar implimentation.
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

#include "hicIsoOrdinal.h"

#include "glcMath.h"
#include "hicGregorian.h"

using namespace glich;
using std::string;


Field IsoOrdinal::get_jdn( const FieldVec& fields ) const
{
    if( fields[0] == f_invalid || fields[1] == f_invalid ) {
        return f_invalid;
    }
    return to_jdn( fields[0], fields[1] );
}

Field IsoOrdinal::get_end_field_value( const FieldVec& fields, size_t index ) const
{
    switch( index )
    {
    case 0: // Last year?
        return f_maximum;
    case 1: // Last week of year
        return last_day_of_year( fields[0] );
    }
    return f_invalid;
}

FieldVec IsoOrdinal::get_fields( Field jdn ) const
{
    FieldVec fields( record_size(), f_invalid );
    from_jdn( &fields[0], &fields[1], jdn );
    return fields;
}

Field IsoOrdinal::to_jdn( Field year, Field day )
{
    return Gregorian::to_jdn( year, 1, 1 ) + day - 1;
}

void IsoOrdinal::from_jdn( Field* year, Field* day, Field jdn )
{
    if( jdn != f_invalid ) {
        if( year ) {
            *year = year_from_jdn( jdn );
        }
        if( day ) {
            Field y = year ? *year : year_from_jdn( jdn );
            *day = jdn - Gregorian::to_jdn( y, 1, 1 ) + 1;
        }
    }
}

Field IsoOrdinal::year_from_jdn( Field jdn )
{
    return Gregorian::year_from_jdn( jdn );
}

/* static */
bool IsoOrdinal::leap_year( Field year )
{
    return Gregorian::leap_year( year );
}

Field IsoOrdinal::last_day_of_year( Field year ) const
{
    return Gregorian::leap_year( year) ? 366 : 365;
}

// End of glc/glc/hicIsoOrdinal.cpp file
