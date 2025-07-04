/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicChinese.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Chinese Lunisolar calendar.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     13th August 2023
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

#include "hicChinese.h"

#include "hicAstro.h"
#include "hicMath.h"

#include <cmath>
#include <cassert>

using namespace glich;
using std::string;


#define BASEDATE_Chinese 758326L  // g# 15 Feb -2636

namespace {

    enum ChineseField {
        CHIN_cycle, CHIN_cyear, CHIN_month, CHIN_lmonth, CHIN_day
    };

    enum class ChinFN { cycle, cyear, month, lmonth, day };

    // CC3 p262
    const Field chinese_month_name_epoch = 57;
    // CC3 p262
    const Field chinese_day_name_epoch = 10; // jdn#

    // CC3 p248 chinese-location
    // A zone is expressed as the fraction of a day
    double chinese_zone( double moment )
    {
        // g# 1 Jan 1929 = 2425613
        double longitude = ( moment < 2425613 ) ? 116.41667 : 120.0;
        return zone_from_longitude( longitude );
    }

    // CC3 p248
    Field current_major_solar_term( Field jdn )
    {
        double s = 
            solar_longitude( universal_from_standard( jdn, chinese_zone( jdn ) ) );
        return famod_f( 2 + floor_f( s / 30 ), 12 );
    }

    // CC3 p251
    inline double midnight_in_china( Field jdn )
    {
        return universal_from_standard( jdn, chinese_zone( jdn ) );
    }

    bool chinese_winter_solstice_on_or_before_min_func( Field day, const void* data )
    {
        return winter < solar_longitude( midnight_in_china( day + 1 ) );
    }

    // CC3 p251
    Field chinese_winter_solstice_on_or_before( Field jdn )
    {
        double approx = estimate_prior_solar_longitude(
            winter, midnight_in_china( jdn + 1 ) );
        return min_search(
            floor_f( approx ) - 1, chinese_winter_solstice_on_or_before_min_func, NULL );
    }

    // CC3 p251
    Field chinese_new_moon_on_or_after( Field jdn )
    {
        double t = new_moon_at_or_after( midnight_in_china( jdn ) );
        return floor_f( standard_from_universal( t, chinese_zone( t ) ) );
    }

    // CC3 p252
    Field chinese_new_moon_before( Field jdn )
    {
        double t = new_moon_before( midnight_in_china( jdn ) );
        return floor_f( standard_from_universal( t, chinese_zone( t ) ) );
    }
    // CC3 p253 chinese-no-major-solar-term?
    bool chinese_is_no_major_solar_term( Field jdn )
    {
        return current_major_solar_term( jdn ) ==
            current_major_solar_term( chinese_new_moon_on_or_after( jdn + 1 ) );
    }

    // CC3 p255 chinese-prior-leap-month?
    bool chinese_is_prior_leap_month( Field m_dash, Field m )
    {
        return m >= m_dash && (
            chinese_is_no_major_solar_term( m ) ||
            chinese_is_prior_leap_month( m_dash, chinese_new_moon_before( m ) )
        );
    }

    // CC3 p257
    Field chinese_new_year_in_sui( Field jdn )
    {
        Field s1 = chinese_winter_solstice_on_or_before( jdn );
        Field s2 = chinese_winter_solstice_on_or_before( s1 + 370 );
        Field m12 = chinese_new_moon_on_or_after( s1 + 1 );
        Field m13 = chinese_new_moon_on_or_after( m12 + 1 );
        Field next_m11 = chinese_new_moon_before( s2 + 1 );

        if( round_n( double( next_m11 - m12 ) / mean_synodic_month ) == 12 &&
            ( 
                chinese_is_no_major_solar_term( m12 ) || 
                chinese_is_no_major_solar_term( m13 )
            )
        ) {
            return chinese_new_moon_on_or_after( m13 + 1 );
        }
        return m13;
    }

    // CC3 p258
    Field chinese_new_year_on_or_before( Field jdn )
    {
        Field newyear = chinese_new_year_in_sui( jdn );
        if( jdn >= newyear ) {
            return newyear;
        }
        return chinese_new_year_in_sui( jdn - 180 );
    }

    // CC3 p259 chinese-from-fixed
    bool chinese_from_jdn(
        Field* cycle, Field* year, Field* month, Field* lmonth, Field* day, Field jdn )
    {
        Field s1 = chinese_winter_solstice_on_or_before( jdn );
        Field s2 = chinese_winter_solstice_on_or_before( s1 + 370 );
        Field m12 = chinese_new_moon_on_or_after( s1 + 1 );
        Field nextm11 = chinese_new_moon_before( s2 + 1 );
        Field m = chinese_new_moon_before( jdn + 1 );
        bool leapyear =
            round_n( double( nextm11 - m12 ) / mean_synodic_month ) == 12;

        Field mon = round_f( double( m - m12 ) / mean_synodic_month );
        Field addm = 0;
        if( leapyear && chinese_is_prior_leap_month( m12, m ) ) {
            addm = 1;
        }
        *month = famod_f( mon - addm, 12 );

        *lmonth = 0;
        if( leapyear && 
            chinese_is_no_major_solar_term( m ) &&
            !chinese_is_prior_leap_month( m12, chinese_new_moon_before( m ) )
        ) {
            *lmonth = 1;
        }
        Field elapsed_years = floor_f( 1.5 - double( *month ) / 12.0
            + double( jdn - BASEDATE_Chinese ) / mean_tropical_year );
        *cycle = fdiv_f( elapsed_years - 1, 60 ) + 1;
        *year = famod_f( elapsed_years, 60 );
        *day = jdn - m + 1;

        return true;
    }

    // CC3 p260 fixed-from-chinese
    Field chinese_to_jdn( Field cycle, Field year, Field month, Field lmonth, Field day )
    {
        Field midyear = BASEDATE_Chinese + 
            floor_f( ( double( (cycle-1) * 60 + year ) - 0.5 ) * mean_tropical_year );

        Field newyear = chinese_new_year_on_or_before( midyear );
        Field p = chinese_new_moon_on_or_after( newyear + ( month - 1 ) * 29 );

        Field d[5];
        bool ret = chinese_from_jdn( &d[0], &d[1], &d[2], &d[3], &d[4], p );
        if( !ret ) {
            return f_invalid;
        }

        Field prior_newmoon;
        if( month == d[CHIN_month] && lmonth == d[CHIN_lmonth] ) {
            prior_newmoon = p;
        } else {
            prior_newmoon = chinese_new_moon_on_or_after( p + 1 );
        }
        return prior_newmoon + day - 1;
    }

    Field chinese_last_day_of_month( Field cycle, Field year, Field month, Field leap )
    {
        Field jdn1 = chinese_to_jdn( cycle, year, month, leap, 1 );
        Field jdn2 = chinese_new_moon_on_or_after( jdn1 + 1 );
        return jdn2 - jdn1;
    }

    bool chinese_is_leap_month( Field cycle, Field year, Field month )
    {
        Field jdn1 = chinese_to_jdn( cycle, year, month, 0, 1 );
        Field m = month + 1;
        Field y = year;
        Field c = cycle;
        if( m == 13 ) {
            m = 1;
            y++;
            if( y == 61 ) {
                y = 1;
                c++;
            }
        }
        Field jdn2 = chinese_to_jdn( c, y, m, 0, 1 );
        return (jdn2 - jdn1) > 30;
    }

} // namespace


Chinese::Chinese( const StdStrVec& data )
    : Base( data, 5 )
{
    m_fieldnames = { "year", "month", "lmonth", "day" };
}

Field Chinese::get_jdn( const FieldVec& fields ) const
{
    if( fields[0] == f_invalid ||
        fields[1] == f_invalid ||
        fields[2] == f_invalid ||
        fields[3] == f_invalid
    ) {
        return f_invalid;
    }
    Field cycle = (fields[0] / 60 ) + 1;
    Field cyear = famod_f( fields[0], 60 );
    return chinese_to_jdn( cycle, cyear, fields[1], fields[2], fields[3] );
}


Field Chinese::get_beg_field_value( const FieldVec& fields, size_t index ) const
{
    if( index > 0 && fields[0] == f_minimum ) {
        return f_invalid;
    }
    switch( index )
    {
    case 0: // year
        return f_minimum;
    case 2: // lmonth
        return 0;
    case 1: // month
    case 3: // day
        return 1;
    }
    return f_invalid;
}

Field Chinese::get_end_field_value( const FieldVec& fields, size_t index ) const
{
    if( index > 0 && fields[0] == f_maximum ) {
        return f_invalid;
    }
    switch( index )
    {
    case 0: // year
        return f_maximum;
    case 1: // month
        return 12;
    case 2: // lmonth
        return chinese_is_leap_month(
            fields[CHIN_cycle], fields[CHIN_cyear], fields[CHIN_month] ) ? 1 : 0;
    case 3: // day
        return chinese_last_day_of_month( fields[CHIN_cycle], fields[CHIN_cyear],
             fields[CHIN_month], fields[CHIN_lmonth] );
    }
    return f_invalid;
}

FieldVec Chinese::get_fields( Field jdn ) const
{
    FieldVec fields( record_size(), f_invalid );
    Field cycle = f_invalid;
    Field cyear = f_invalid;
    chinese_from_jdn( &cycle, &cyear, &fields[1], &fields[2], &fields[3], jdn );
    if( cycle == f_invalid || cyear == f_invalid ) {
        return fields; // Invalid date
    }
    fields[0] = (cycle - 1) * 60 + cyear;
    return fields;
}


// End of src/glc/hicChinese.cpp
