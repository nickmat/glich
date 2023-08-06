/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicJulian.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Julian (j) calendar source.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     17th March 2023
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

#include "hicJulian.h"

#include "glcHelper.h"
#include "hicmath.h"
#include "hicrecord.h"

#include <cassert>

using namespace glich;
using std::string;

#define BASEDATE_Julian    1721058L

static Field julian_to_jdn( Field year, Field month, Field day )
{
    Field jdn =
        div_f( year, 4 ) * 1461 + mod_f( year, 4 ) * 365
        + latin_diy[month] + day + BASEDATE_Julian;

    // Adjust if in the 1st 2 months of 4 year cycle
    if ( month < 3 && ( year % 4 ) == 0 ) --( jdn );

    return jdn;
}

/*! Splits the given Julian Day Number date into the day, month and year
*  for the Julian Calendar.
*/
static void julian_from_jdn( Field* year, Field* month, Field* day, Field jdn )
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

/*! Return the jdn for Easter Sunday in the given year.
*/
static Field julian_easter( Field year )
{
    Field shifted_epact = ( 14 + 11 * ( year % 19 ) ) % 30;
    Field paschal_moon = julian_to_jdn( year, 4, 19 ) - shifted_epact;
    return kday_after( WDAY_Sunday, paschal_moon );
}

Julian::Julian( const std::string& data )
    : m_year_offset(0), Base( string() )
{
    m_fieldnames = { "year", "month", "day" };
    string tail, word = get_first_word( data, &tail );
    while( !word.empty() ) {
        set_data( word );
        word = get_first_word( tail, &tail );
    }
}

void Julian::set_data( const std::string& data )
{
    string code, tail;
    split_code( &code, &tail, data );
    if( code == "year" ) {
        m_year_offset = str_to_field( tail );
    }
    else {
        Base::set_data( data );
    }
}

bool Julian::set_epoch( Field epoch )
{
    int index = get_fieldname_index( "cyear" );
    if( index != 3 ) {
        return false;
    }
    FieldVec f = get_fields( epoch );
    string f0 = std::to_string( f[0] );
    string f1 = std::to_string( f[1] );
    string f2 = std::to_string( f[2] );
    Field std_epoch = get_jdn( { 1, 1, 1 } );
    string adj_year_true, adj_year_false, adj_cyear_true, adj_cyear_false;
    if( epoch > std_epoch ) {
        string adj = std::to_string( f[0] );
        adj_year_true = "-" + adj;
        adj_cyear_true = "+" + adj;
    }
    else if( epoch < std_epoch ) {
        string adj = std::to_string( 1 - f[0] );
        adj_year_false = "+" + adj;
        adj_cyear_false = "-" + adj;
    }
    m_calculate_input = "{: @if( month<" + f1 + " or (month=" + f1 + " and day<" + f2 + "),"
        " cyear" + adj_cyear_true + ", cyear" + adj_cyear_false + ") }";
    m_calculate_output = "{: ,,,@if( month<" + f1 + " or (month=" + f1 + " and day<" + f2 + "),"
        " year" + adj_year_true + ", year" + adj_year_false + ") }";
    return true;
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
 */
bool Julian::is_leap_year( Field year ) const
{
    return mod_e( year, 4 ) == 0;
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
