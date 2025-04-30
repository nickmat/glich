/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicJulian.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Julian (j) calendar source.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     17th March 2023
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

#include "hicJulian.h"

#include "glcHelper.h"
#include "hicMath.h"
#include "hicRecord.h"

#include <cassert>

using namespace glich;
using std::string;

#define BASEDATE_Julian    1721058L

Field glich::julian_to_jdn( Field year, Field month, Field day )
{
    LongField jdn =
        div_f( year, 4 ) * 1461 + mod_f( year, 4 ) * 365
        + latin_diy[month] + day + BASEDATE_Julian;

    // Adjust if in the 1st 2 months of 4 year cycle
    if ( month < 3 && ( year % 4 ) == 0 ) --( jdn );

    if( jdn <= f_minimum || jdn >= f_maximum ) {
        return f_invalid;
    }
    return Field( jdn );
}

/*! Splits the given Julian Day Number date into the day, month and year
*  for the Julian Calendar.
*/
void glich::julian_from_jdn( Field* year, Field* month, Field* day, Field jdn )
{
    jdn -= BASEDATE_Julian;

    *year = div_f( jdn, 1461 ) * 4;
    jdn = mod_f( jdn, 1461 );

    if ( jdn < 60 )
    {
        if ( jdn < 31 )
        {
            *month = 1;
            *day = jdn + 1;
            return;
        }
        *month = 2;
        *day = jdn - 30;
        return;
    }
    --jdn; // remove the leap day
    *year += (int)jdn / 365;
    jdn %= 365;
    *month = 1;
    while ( jdn >= latin_diy[( *month ) + 1] ) ( *month )++;
    *day = jdn - latin_diy[*month] + 1;
    return;
}

Julian::Julian( const StdStrVec& data )
    : m_year_offset(0), Base( StdStrVec(), 3 )
{
    m_fieldnames = { "year", "month", "day" };
    for( const string& word : data ) {
        cal_data( word );
    }
}

void Julian::cal_data( const string& word )
{
    string code, tail;
    split_code( &code, &tail, word );
    if( code == "year" ) {
        m_year_offset = str_to_field( tail );
    }
    else {
        set_data( word );
    }
}

Field Julian::get_jdn( const FieldVec& fields ) const
{
    if( fields.size() < 3 || fields[0] == f_invalid || fields[1] == f_invalid || fields[2] == f_invalid ) {
        return f_invalid;
    }
    Field year = fields[0] + m_year_offset;
    return julian_to_jdn( year, fields[1], fields[2] );
}

Field Julian::get_end_field_value( const FieldVec& fields, size_t index ) const
{
    if( index > 0 && fields[0] == f_maximum ) {
        return f_invalid;
    }
    switch( index )
    {
    case 0:
        return f_maximum;
    case 1: // Last month of year
        return 12;
    case 2: // Last day of month
        return last_day_in_month( fields[0], fields[1] );
    }
    return f_invalid;
}


FieldVec Julian::get_fields( Field jdn ) const
{
    FieldVec fields( record_size(), f_invalid );
    Field year = f_invalid;
    julian_from_jdn( &year, &fields[1], &fields[2], jdn );
    fields[0] = year - m_year_offset;
    return fields;
}

/*! Returns true if the year is a leap year in the Julian Calendar.
static */
bool Julian::leap_year( Field year )
{
    return mod_e( year, 4 ) == 0;
}

/*! Return the jdn for Easter Sunday in the given year.
static */
Field Julian::easter( Field year )
{
    Field shifted_epact = (14 + 11 * (year % 19)) % 30;
    Field paschal_moon = julian_to_jdn( year, 4, 19 ) - shifted_epact;
    return kday_after( WDAY_Sunday, paschal_moon );
}

/*static*/
Field glich::Julian::year_from_jdn( Field jdn )
{
    Field year, month, day;
    julian_from_jdn( &year, &month, &day, jdn );
    return year;
}

/*! Returns the last day of the month for the given month and year.
 */
Field Julian::last_day_in_month( Field year, Field month ) const
{
    switch( month )
    {
    case 1: case 3: case 5: case 7: case 8: case 10: case 12:
        return 31;
    case 4: case 6: case 9: case 11:
        return 30;
    case 2:
        return is_leap_year( year ) ? 29 : 28;
    }
    return f_invalid;
}


// End of src/glc/hicJulian.cpp file
