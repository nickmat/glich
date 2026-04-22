/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        test/gs/gsSch_c.cpp
 * Project:     glcSoak: Soak (extended unit) test program for Hics library.
 * Purpose:     Soak Test Chinese Scheme "c".
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     17th April 2026
 * Copyright:   Copyright (c) 2026, Nick Matthews.
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

#include <catch2/catch.hpp>

#include "gsMain.h"

#include <iostream>

using namespace glich;
using std::string;

struct LeapMonths {
    Field year;
    Field month;
};

LeapMonths leap_months[] = { // 1900 - 2044
    { 4537, 8 }, { 4540, 5 }, { 4543, 4 }, { 4546, 2 },
    { 4548, 6 }, { 4551, 5 }, { 4554, 2 }, { 4556, 7 },
    { 4559, 5 }, { 4562, 4 }, { 4565, 2 }, { 4567, 6 },
    { 4570, 5 }, { 4573, 3 }, { 4575, 7 }, { 4578, 6 },
    { 4581, 4 }, { 4584, 2 }, { 4586, 7 }, { 4589, 5 },
    { 4592, 3 }, { 4594, 8 }, { 4597, 6 }, { 4600, 4 },
    { 4603, 3 }, { 4605, 7 }, { 4608, 5 }, { 4611, 4 },
    { 4613, 8 }, { 4616, 6 }, { 4619, 4 }, { 4621, 10 },
    { 4624, 6 }, { 4627, 5 }, { 4630, 3 }, { 4632, 8 },
    { 4635, 5 }, { 4638, 4 }, { 4641, 2 }, { 4643, 7 },
    { 4646, 5 }, { 4649, 4 }, { 4651, 9 }, { 4654, 6 },
    { 4657, 4 }, { 4660, 2 }, { 4662, 6 }, { 4665, 5 },
    { 4668, 3 }, { 4670, 11 }, { 4673, 6 }, { 4676, 5 },
    { 4679, 2 }, { 4681, 7 },
    { 9999, 0 } // Sentinel
};
size_t leap_months_size = sizeof( leap_months ) / sizeof( LeapMonths );

static Field month_length( Field jdn )
{
    return glich::chinese_next_new_moon( jdn ) - jdn;
}

TEST_CASE( "Scheme c Soak Test", "[c Soak]" )
{
    std::cout << "c ";

    Field start_year = 4537; // 1900
    Field start_jdn = 2415051;
    Field end_year = 4682;  // 2045
    size_t lmonth_index = 0;

    if ( g_depth == Depth::Short ) {
        start_year = 4560; // 1923
        start_jdn = 2423467;
        end_year = 4563;  // 1925
        lmonth_index = 9;
    }

    string sig = "c:r";
    string date_str = cvt_fields( start_year, 1, 0, 1 );
    Field daycount = g_glc->text_to_field( date_str, sig );
    REQUIRE( daycount == start_jdn );

    string cvtdate;
    Range range;
    for( Field year = start_year; year <= end_year; year++ ) {
        Field month1 = daycount;
        size_t leap_month = 0;
        for( Field month = 1; month <= 12; month++ ) {
            if( leap_months[lmonth_index].year == year &&
                leap_months[lmonth_index].month == month )
            {
                leap_month = 1;
                lmonth_index++;
                REQUIRE( lmonth_index < leap_months_size );
            } else {
                leap_month = 0;
            }
            Field day2 = daycount;
            for( Field lmonth = 0; lmonth <= leap_month; lmonth++ ) {
                int month_len = month_length( daycount );
                REQUIRE( month_len >= 29 );
                REQUIRE( month_len <= 30 );
                Field day1 = daycount;
                for( Field day = 1; day <= month_len; day++ ) {
                    date_str = cvt_fields( year, month, lmonth, day );
                    Field jdn = g_glc->text_to_field( date_str, sig );
                    REQUIRE( daycount == jdn );
                    cvtdate = g_glc->field_to_text( jdn, sig );
                    REQUIRE( date_str == cvtdate );
                    daycount++;
                }
                if( leap_month == 0 ) {
                    date_str = cvt_fields( year, month );
                }
                else {
                    date_str = cvt_fields( year, month, lmonth );
                }
                range = g_glc->text_to_range( date_str, sig );
                REQUIRE( day1 == range.m_beg );
                REQUIRE( daycount - 1 == range.m_end );
                cvtdate = g_glc->range_to_text( range, sig );
                REQUIRE( date_str == cvtdate );
            }
            date_str = cvt_fields( year, month );
            range = g_glc->text_to_range( date_str, sig );
            REQUIRE( day2 == range.m_beg );
            REQUIRE( daycount - 1 == range.m_end );
            cvtdate = g_glc->range_to_text( range, sig );
            REQUIRE( date_str == cvtdate );
        }
        date_str = cvt_fields( year );
        range = g_glc->text_to_range( date_str, sig );
        REQUIRE( month1 == range.m_beg );
        REQUIRE( daycount - 1 == range.m_end );
        cvtdate = g_glc->range_to_text( range, sig );
        REQUIRE( date_str == cvtdate );
    }
}

// End of test/gs/gsSch_g.cpp file.
