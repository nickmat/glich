/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        test/glcunit/guSch_ay.cpp
 * Project:     glcUnit: Unit test program for the Glich Hics library.
 * Purpose:     Test Scheme "ay" The Astronomical Year calendar. 
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     7th May 2024
 * Copyright:   Copyright (c) 2024, Nick Matthews.
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

#include "guMain.h"

using namespace glich;
using std::string;


const char* test_ay[guTT_size][5] = {
    // dmys            mdy
    { "30 Jul -586 j", "Jul 30, -586" },
    { "8 Dec -168 j", "Dec 8, -168" },
    { "26 Sep 70 j", "Sep 26, 70" },
    { "3 Oct 135 j", "Oct 3, 135" },
    { "7 Jan 470 j", "Jan 7, 470" },
    { "18 May 576 j", "May 18, 576" },
    { "7 Nov 694 j", "Nov 7, 694" },
    { "28 Dec 987 j", "Dec 28, 987" },
    { "19 Apr 1013 j", "Apr 19, 1013" },
    { "18 May 1096 j", "May 18, 1096" },
    { "16 Mar 1190 j", "Mar 16, 1190" },
    { "3 Mar 1240 j", "Mar 3, 1240" },
    { "26 Mar 1288 j", "Mar 26, 1288" },
    { "20 Apr 1298 j", "Apr 20, 1298" },
    { "4 Jun 1391 j", "Jun 4, 1391" },
    { "25 Jan 1436 j", "Jan 25, 1436" },
    { "31 Mar 1492 j", "Mar 31, 1492" },
    { "9 Sep 1553 j", "Sep 9, 1553"},
    { "24 Feb 1560 j", "Feb 24, 1560" },

    { "10 Jun 1648 g", "Jun 10, 1648" },
    { "30 Jun 1680 g", "Jun 30, 1680" },
    { "24 Jul 1716 g", "Jul 24, 1716" },
    { "19 Jun 1768 g", "Jun 19, 1768" },
    { "2 Aug 1819 g",  "Aug 2, 1819"  },
    { "27 Mar 1839 g", "Mar 27, 1839" },
    { "19 Apr 1903 g", "Apr 19, 1903" },
    { "25 Aug 1929 g", "Aug 25, 1929" },
    { "29 Sep 1941 g", "Sep 29, 1941" },
    { "19 Apr 1943 g", "Apr 19, 1943" },
    { "7 Oct 1943 g",  "Oct 7, 1943"  },
    { "17 Mar 1992 g", "Mar 17, 1992" },
    { "25 Feb 1996 g", "Feb 25, 1996" },
    { "10 Nov 2038 g", "Nov 10, 2038" },
    { "18 Jul 2094 g", "Jul 18, 2094" }
};

TEST_CASE( "Library Scheme ay", "[ay]" )
{
    Scheme_info info;
    g_glc->get_scheme_info( &info, "ay" );
    REQUIRE( info.name == "Astronomical Year" );
    REQUIRE( info.code == "ay" );
    REQUIRE( info.grammar_code == "jg" );
    REQUIRE( info.lexicon_codes[0] == "m" );
    REQUIRE( info.lexicon_names[0] == "Month names" );
    REQUIRE( info.lexicon_codes[1] == "jg" );
    REQUIRE( info.lexicon_names[1] == "Hybrid Julian and Gregorian scheme names" );
}

TEST_CASE( "Scheme ay Test Table", "[ay TestTable]" )
{
    for( size_t i = 0; i < guTT_size; i++ ) {
        Field jdn = g_glc->text_to_field( test_ay[i][0], "ay:dmys" );
        REQUIRE( jdn == guTT_jdn[i] );
        string str = g_glc->field_to_text( jdn, "ay:dmys" );
        REQUIRE( str == test_ay[i][0] );

        jdn = g_glc->text_to_field( test_ay[i][1], "ay:mdy" );
        REQUIRE( jdn == guTT_jdn[i] );
        str = g_glc->field_to_text( jdn, "ay:mdy" );
        REQUIRE( str == test_ay[i][1] );
    }
}

// End of test/glcunit/guSch_ay.cpp file.
