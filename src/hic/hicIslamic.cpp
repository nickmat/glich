/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicIslamic.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Islamic Arithmetical calendar source.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     13th August 2023
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

#include "hicIslamic.h"

#include "glcHelper.h"
#include "hicMath.h"

using namespace glich;
using std::string;

#define BASEDATE_Islamic    1948440

Islamic::Islamic( const StdStrVec& data )
    : m_delta(14), m_basedate(BASEDATE_Islamic), Base( StdStrVec(), 3)
{
    m_fieldnames = { "year", "month", "day" };
    for( const string& word : data ) {
        cal_data( word );
    }
}

void Islamic::cal_data( const string& data )
{
    if ( data == "Ia" ) {
        m_delta = 15;
        m_basedate = BASEDATE_Islamic - 1;
    } else if ( data == "Ic" ) {
        m_delta = 15;
        m_basedate = BASEDATE_Islamic;
    } else if ( data == "IIa" ) {
        m_delta = 14;
        m_basedate = BASEDATE_Islamic - 1;
    } else if ( data == "IIc" ) {
        m_delta = 14;
        m_basedate = BASEDATE_Islamic;
    } else if ( data == "IIIa" ) {
        m_delta = 11;
        m_basedate = BASEDATE_Islamic - 1;
    } else if ( data == "IIIc" ) {
        m_delta = 11;
        m_basedate = BASEDATE_Islamic;
    } else if ( data == "IVa" ) {
        m_delta = 9;
        m_basedate = BASEDATE_Islamic - 1;
    } else if ( data == "IVc" ) {
        m_delta = 9;
        m_basedate = BASEDATE_Islamic;
    } else {
        set_data( data );
    }
}

Field Islamic::get_jdn( const FieldVec& fields ) const
{
    if( fields[0] == f_invalid ||
        fields[1] == f_invalid ||
        fields[2] == f_invalid
    ) {
        return f_invalid;
    }
    return to_jdn( fields[0], fields[1], fields[2] );
}

Field Islamic::get_end_field_value( const FieldVec& fields, size_t index ) const
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

FieldVec Islamic::get_fields( Field jdn ) const
{
    FieldVec fields( record_size(), f_invalid );
    from_jdn( &fields[0], &fields[1], &fields[2], jdn );
    return fields;
}



//===================================

/*! Returns true if the year is a leap year in the Islamic Calendar.
 */
bool Islamic::is_leap_year( Field year ) const
{
    return mod_f( m_delta + 11 * year, 30 ) < 11;
}

/*! Returns the last day of the month for the given month and year
 *  in the Islamic Calendar.
 */
Field Islamic::last_day_in_month( Field year, Field month ) const
{
    if( month == 12 ) {
        return is_leap_year( year ) ? 30 : 29;
    } else {
        return (month % 2) == 0 ? 29 : 30;
    }
}

/*! Sets jdn to the Julian Day Number for the given day, month and year
 *  in the Islamic Calendar.
 */
Field Islamic::to_jdn( Field year, Field month, Field day ) const
{
    return
        m_basedate - 1
        + ( year - 1 ) * 354
        + fdiv_f( year * 11 + 3, 30 )
        + 29 * ( month - 1 )
        + fdiv_f( month, 2 ) + day;
}

/*! Splits the given Julian Day Number date into the day, month and year
 *  for the Islamic Calendar.
 */
bool Islamic::from_jdn( Field* year, Field* month, Field* day, Field jdn ) const
{
    *year = fdiv_f( 30 * ( jdn - m_basedate ) + 10646, 10631 );

    Field temp = to_jdn( *year, 1, 1 );
    *month = fdiv_f( 11 * ( jdn - temp ) + 330, 325 );

    temp = to_jdn( *year, *month, 1 );
    *day = 1 + jdn - temp;

    return true;
}


// End of src/glc/hicIslamic.cpp
