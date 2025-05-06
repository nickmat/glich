/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        test/gs/gsSch_ja.cpp
 * Project:     glcSoak: Soak (extended unit) test program for Hics library.
 * Purpose:     Soak Test Julian Annunciation Scheme "ja".
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     8th September 2023
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

#include <catch2/catch.hpp>

#include "gsMain.h"

#include <iostream>

using namespace glich;
using std::string;


TEST_CASE( "Scheme ja Soak Test", "[ja Soak]" )
{
    Field LengthOfMonth[2][12] = {
        { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
        { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
    };

    std::cout << ".";

    Field start_year = 0;
    Field start_jdn = 0;
    Field end_year = 0;
    switch( g_depth ) {
    case Depth::Short:
        start_year = 1496;
        start_jdn = 2267472;
        end_year = 1506;
        break;
    case Depth::Long:
    case Depth::Soak:
        start_year = 796;
        start_jdn = 2011797;
        end_year = 2110;
        break;
    }

    string sig = "ja:cdef";
    Field cyear = start_year - 1; // Civil year.
    string date = cvt_fields( cyear, 1, 1 );
    Field daycount = g_glc->text_to_field( date, sig );
    REQUIRE( daycount == start_jdn );

    string cvtdate;
    for( Field year = start_year; year < end_year; year++ ) {
        size_t leap_year = 0;
        if( year % 4 == 0 ) leap_year = 1;
        for( Field month = 1; month <= 12; month++ ) {
            int month_length = LengthOfMonth[leap_year][month - 1];
            for( Field day = 1; day <= month_length; day++ ) {
                if( day == 25 && month == 3 ) {
                    cyear++;
                }
                date = cvt_fields( cyear, month, day );
                Field jdn = g_glc->text_to_field( date, sig );
                REQUIRE( daycount == jdn );
                cvtdate = g_glc->field_to_text( jdn, sig );
                REQUIRE( date == cvtdate );
                daycount++;
            }
        }
    }
}


// End of test/gs/gsSch_g.cpp file.
