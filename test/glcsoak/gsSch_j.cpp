/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        test/gs/gsSch_j.cpp
 * Project:     glcSoak: Soak (extended unit) test program for Hics library.
 * Purpose:     Soak Test Julian Scheme "j".
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     5th September 2023
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

#include <catch2/catch.hpp>

#include "gsMain.h"

#include <iostream>

using namespace glich;
using std::string;

#ifdef GLCTEST_SHORT
#define GLCTEST_J_START_YEAR     1890
#define GLCTEST_J_START_JDN      2411381
#define GLCTEST_J_END_YEAR       2010
#endif

#ifdef GLCTEST_LONG
#define GLCTEST_J_START_YEAR     1200
#define GLCTEST_J_START_JDN      2159358
#define GLCTEST_J_END_YEAR       2050
#endif

#ifdef GLCTEST_SOAK
#define GLCTEST_J_START_YEAR     -1000
#define GLCTEST_J_START_JDN      1355808
#define GLCTEST_J_END_YEAR       2150
#endif

TEST_CASE( "Scheme j Soak Test", "[j Soak]" )
{
    Field LengthOfMonth[2][12] = {
        { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
        { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
    };

    std::cout << ".";

    string sig = "j:def";
    string date = cvt_fields( GLCTEST_J_START_YEAR, 1, 1 );
    Field daycount = g_glc->text_to_field( date, sig );
    REQUIRE( daycount == GLCTEST_J_START_JDN );

    string cvtdate;
    Range range;
    for( Field year = GLCTEST_J_START_YEAR; year < GLCTEST_J_END_YEAR; year++ ) {
        Field month1 = daycount;
        size_t leap_year = 0;
        if( year % 4 == 0 ) leap_year = 1;
        for( Field month = 1; month <= 12; month++ ) {
            int month_length = LengthOfMonth[leap_year][month - 1];
            Field day1 = daycount;
            for( Field day = 1; day <= month_length; day++ ) {
                date = cvt_fields( year, month, day );
                Field jdn = g_glc->text_to_field( date, sig );
                REQUIRE( daycount == jdn );
                cvtdate = g_glc->field_to_text( jdn, sig );
                REQUIRE( date == cvtdate );
                daycount++;
            }
            date = cvt_fields( year, month );
            range = g_glc->text_to_range( date, sig );
            REQUIRE( day1 == range.m_beg );
            REQUIRE( daycount - 1 == range.m_end );
            cvtdate = g_glc->range_to_text( range, sig );
            REQUIRE( date == cvtdate );
        }
        date = cvt_fields( year );
        range = g_glc->text_to_range( date, sig );
        REQUIRE( month1 == range.m_beg );
        REQUIRE( daycount - 1 == range.m_end );
        cvtdate = g_glc->range_to_text( range, sig );
        REQUIRE( date == cvtdate );
    }
}


// End of test/gs/gsSch_g.cpp file.
