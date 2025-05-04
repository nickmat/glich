/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicGregorian.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Gregorian (g) calendar source.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     6th April 2023
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


#include "hicGregorian.h"

#include "hicMath.h"

#include <cassert>
#include <ctime>

#define BASEDATE_Gregorian 1721060

using namespace glich;
using std::string;


void glich::gregorian_from_jdn( Field* year, Field* month, Field* day, Field jdn )
{
    Field date = jdn - BASEDATE_Gregorian;
    *year = fdiv_f( date, 146097 ) * 400;
    date = fmod_f( date, 146097 );

    if( date < 60 )
    {
        if( date < 31 )
        {
            *month = 1;
            *day = date + 1;
            return;
        }
        *month = 2;
        *day = date - 30;
        return;
    }
    --date; // remove the leap day
    *year += ((date / 36524) * 100);
    date %= 36524;

    if( date < 59 ) // Note, this is not a leap year
    {
        if( date < 31 )
        {
            *month = 1;
            *day = date + 1;
            return;
        }
        *month = 2;
        *day = date - 30;
        return;
    }
    ++date; // add the missing the leap day
    *year += (date / 1461) * 4;
    date %= 1461;

    if( date < 60 )
    {
        if( date < 31 )
        {
            *month = 1;
            *day = date + 1;
            return;
        }
        *month = 2;
        *day = date - 30;
        return;
    }
    --date; // remove the leap day
    *year += date / 365;
    date %= 365;
    *month = 1;
    while( date >= latin_diy[(*month) + 1] ) {
        (*month)++;
    }
    *day = date - latin_diy[*month] + 1;
    return;
}

Field glich::gregorian_to_jdn( Field year, Field month, Field day )
{
    Field jdn =
        fdiv_f( year, 400 ) * 146097           //     days in 400 year cycles
        + (fmod_f( year, 400 ) / 100) * 36524    // - 1 days in 100 year cycles
        + (fmod_f( year, 100 ) / 4) * 1461       // + 1 days in 4 year cycles
        + fmod_f( year, 4 ) * 365              // + 1 days in year
        + latin_diy[month] + day            // - 1 days numbered from 1 not 0
        + BASEDATE_Gregorian;

    // Adjust if in the 1st 2 months of 4 year cycle
    if( month < 3 && year % 4 == 0 ) --jdn;

    // Adjust if in the 1st 2 months of 100 year cycle
    if( year % 100 == 0 && month < 3 ) ++jdn;

    // Adjust if in the 1st 2 months of 400 year cycle
    if( year % 400 == 0 && month < 3 ) --jdn;

    return jdn;
}


Field Gregorian::get_jdn( const FieldVec& fields ) const
{
    if( fields.size() < 3 || fields[0] == f_invalid || fields[1] == f_invalid || fields[2] == f_invalid ) {
        return f_invalid;
    }
    return gregorian_to_jdn( fields[0], fields[1], fields[2] );
}

FieldVec Gregorian::get_fields( Field jdn ) const
{
    FieldVec fields( record_size(), f_invalid );
    gregorian_from_jdn( &fields[0], &fields[1], &fields[2], jdn );
    return fields;
}

/*static*/
Field Gregorian::to_jdn( Field year, Field month, Field day )
{
    return gregorian_to_jdn( year, month, day );
}

/*static*/
Field Gregorian::year_from_jdn( Field jdn )
{
    Field year, month, day;
    gregorian_from_jdn( &year, &month, &day, jdn );
    return year;
}

/*static*/
bool Gregorian::leap_year( Field year )
{
    return ( year%4 == 0 && year%100 != 0 ) || year%400 == 0;
}

/*! Return the jdn for Easter Sunday in the given year.
static */
Field Gregorian::easter( Field year )
{
    Field century = year / 100 + 1;
    Field epact =
        (14 + 11 * (year % 19) - (3 * century) / 4 + (5 + 8 * century) / 25) % 30;
    if( epact == 0 || (epact == 1 && 10 < (year % 19)) ) {
        epact++;
    }
    Field paschal_moon = gregorian_to_jdn( year, 4, 19 ) - epact;
    return kday_after( WDAY_Sunday, paschal_moon );
}


/*static*/
Field Gregorian::today()
{
    time_t now;
    time( &now );
#if defined(_MSC_VER)  
    // Avoid MSC warning about localtime
    struct tm tm_struct;
    struct tm* tp;
    localtime_s( &tm_struct, &now );
    tp = &tm_struct;
#else
    struct tm* tp = localtime( &now );
#endif

    return gregorian_to_jdn( tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday );
}

// End of src/cal/calgregorian.cpp file
