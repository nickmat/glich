/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicHebrew.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Hebrew (h) calendar source.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     12th August 2023
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

#include "hicHebrew.h"

#include "glcMath.h"

#include <cassert>

using namespace glich;
using std::string;


#define BASEDATE_Hebrew    347998L

namespace {

    Field hebrew_elapsed_days( Field year )
    {
        Field months = fdiv_f( 235 * year - 234, 19 );
        Field days = 29 * months + fdiv_f( 12084L + 13753L * months, 25920 );
        if( ( ( 3 * ( days + 1 ) ) % 7 ) < 3 ) {
            return days + 1;
        }
        return days;
    }

    Field hebrew_new_year( Field year )
    {
        Field ny0 = hebrew_elapsed_days( year - 1 );
        Field ny1 = hebrew_elapsed_days( year );
        Field ny2 = hebrew_elapsed_days( year + 1 );
        Field correction = 0;
        if( ( ny2 - ny1 ) == 356 ) {
            correction = 2;
        } else if( ( ny1 - ny0 ) == 382 ) {
            correction = 1;
        }
        return BASEDATE_Hebrew + ny1 + correction;
    }

    Field hebrew_days_in_year( Field year )
    {
        return hebrew_new_year( year + 1 ) - hebrew_new_year( year );
    }

    /*! Returns true if the year is a leap year in the Hebrew Calendar.
     *  Note that a Hebrew leap years are years with an additional 13th
     *  month. Also note, non-leap years are not all the same length.
     */
    bool hebrew_is_leap_year( Field year )
    {
        return mod_f( 1 + 7 * year, 19 ) < 7;
    }

    /*! Returns the last day of the month for the given year and month
     *  in the Hebrew Calendar.
     */
    Field hebrew_last_day_in_month( Field year, Field month )
    {
    //    wxASSERT( month >= 1 && month <= 13 );
        Field diy;
        switch( month )
        {
        case 2: case 4: case 6: case 10: case 13:
            return 29;
        case 8:
            diy = hebrew_days_in_year( year );
            if( diy != 355 && diy != 385 ) {
                return 29;
            }
            break;
        case 9:
            diy = hebrew_days_in_year( year );
            if( diy == 353 || diy == 383 ) {
                return 29;
            }
            break;
        case 12:
            if( ! hebrew_is_leap_year( year ) ) {
                return 29;
            }
            break;
        }
        return 30;
    }

    /*! Sets jdn to the Julian Day Number for the given day, month and year
     *  in the Hebrew Calendar. Always returns true.
     */
    Field hebrew_to_jdn( Field year, Field month, Field day )
    {
        Field diy = hebrew_days_in_year( year );
        bool leap = hebrew_is_leap_year( year );

        Field mdays = 0;
        switch( month )
        {
        case 6: // fall thru intended
            mdays += 30; // month 5
            [[fallthrough]];
        case 5:
            mdays += 29; // month 4
            [[fallthrough]];
        case 4:
            mdays += 30; // month 3
            [[fallthrough]];
        case 3:
            mdays += 29; // month 2
            [[fallthrough]];
        case 2:
            mdays += 30; // month 1
            [[fallthrough]];
        case 1:
            if( leap ) { // month 13
                mdays += 29;
            }
            [[fallthrough]];
        case 13:
            if( leap ) { // month 12
                mdays += 30;
            } else {
                mdays += 29;
            }
            [[fallthrough]];
        case 12:
            mdays += 30; // month 11
            [[fallthrough]];
        case 11:
            mdays += 29; // month 10
            [[fallthrough]];
        case 10:
            if( diy == 353 || diy == 383 ) { // month 9
                mdays += 29;
            } else {
                mdays += 30;
            }
            [[fallthrough]];
        case 9:
            if( diy == 355 || diy == 385 ) { // month 8
                mdays += 30;
            } else {
                mdays += 29;
            }
            [[fallthrough]];
        case 8:
            mdays += 30;  // month 7
        }
        return hebrew_new_year( year ) + mdays + day - 1;
    }

    /*! Splits the given Hebrew Day Number date into the day, month and year
     *  for the Hebrew Calendar.
     */
    void hebrew_from_jdn( Field* year, Field* month, Field* day, Field jdn )
    {
        Field y = fdiv_f( 4L * ( jdn - BASEDATE_Hebrew ), 1461 );
        for(;;) {
            if( hebrew_new_year( y ) > jdn ) {
                --y;
                break;
            }
            y++;
        }

        Field m = ( jdn < hebrew_to_jdn( y, 1, 1 ) ) ? 7 : 1;
        for(;;) {
            Field ldim = hebrew_last_day_in_month( y, m );
            if( jdn <= hebrew_to_jdn( y, m, ldim ) ) {
                break;
            }
            m++;
            assert( m <= 13 );
        }
        *day = jdn - hebrew_to_jdn( y, m, 1 ) + 1;
        *year = y;
        *month = m;
    }

} // namespace

//===================================

Hebrew::Hebrew( const StdStrVec& data )
    : Base( data, 3 )
{
    m_fieldnames = { s_fields[0], s_fields[1], s_fields[2] };
}

Field Hebrew::get_jdn( const FieldVec& fields ) const
{
    return hebrew_to_jdn( fields[0], fields[1], fields[2] );
}

/* static */
Field Hebrew::beg_field_value( const FieldVec& fields, size_t index )
{
    switch( index )
    {
    case 1: // First month of year
        return 7;
    case 2: // First day of month
        return 1;
    }
    return f_invalid;
}

/* static */
Field Hebrew::end_field_value( const FieldVec& fields, size_t index )
{
    if( fields.size() < index ) {
        return f_invalid;
    }
    switch( index )
    {
    case 1: // Last month of year
        return 6;
    case 2: // Last day of month
        return hebrew_last_day_in_month( fields[0], fields[1] );
    }
    return f_invalid;
}

/* static */
int Hebrew::fieldname_index( const std::string& fieldname )
{
    int index = 0;
    for( const char* name : s_fields ) {
        if( fieldname == name ) {
            return index;
        }
        index++;
    }
    return -1;
}

FieldVec Hebrew::get_fields( Field jdn ) const
{
    FieldVec fields( record_size(), f_invalid );
    hebrew_from_jdn( &fields[0], &fields[1], &fields[2], jdn);
    return fields;
}

/* static */
bool Hebrew::leap_year( Field year )
{
    return hebrew_is_leap_year( year );
}

// End of src/glc/hicHebrew.cpp
