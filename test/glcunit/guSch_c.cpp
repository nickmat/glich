/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        test/gu/guSch_c.cpp
 * Project:     glcUnit: Unit test program for the Glich Hics library.
 * Purpose:     Test Scheme "c" Chinese conversions.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     29th August 2023
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

#include "guMain.h"

using namespace glich;
using std::string;


const char* table_c[guTT_size][1] = {
//    cymld //   cyc yr  mon lp day
    { "35 11 6 f 12" },
    { "42 9 10 f 27" },
    { "46 7 8 f 4" },
    { "47 12 8 f 9" },
    { "52 46 11 f 20" },
    { "54 33 4 f 5" },
    { "56 31 10 f 15" },
    { "61 24 12 f 6" },
    { "61 50 3 f 7" },
    { "63 13 4 f 24" },
    { "64 47 2 f 9" },
    { "65 37 2 f 9" },
    { "66 25 2 f 23" },
    { "66 35 3 f 9" },
    { "68 8 5 f 2" },
    { "68 53 1 f 8" },
    { "69 49 3 f 4" },
    { "70 50 8 f 2" },
    { "70 57 1 f 29" },
    { "72 25 4 s 20" },
    { "72 57 6 f 5" },
    { "73 33 6 f 6" },
    { "74 25 5 f 5" },
    { "75 16 6 f 12" },
    { "75 36 2 f 13" },
    { "76 40 3 f 22" },
    { "77 6 7 f 21" },
    { "77 18 8 f 9" },
    { "77 20 3 f 15" },
    { "77 20 9 f 9" },
    { "78 9 2 f 14" },
    { "78 13 1 f 7" },
    { "78 55 10 f 14" },
    { "79 51 6 f 7" }
};



TEST_CASE( "Library Scheme c", "[c]" )
{
    Scheme_info info;
    g_glc->get_scheme_info( &info, "c" );
    REQUIRE( info.name == "Chinese" );
    REQUIRE( info.code == "c" );
    REQUIRE( info.grammar_code == "c" );
    REQUIRE( info.lexicon_codes[0] == "lmon" );
    REQUIRE( info.lexicon_codes[1] == "fs" );
    REQUIRE( info.lexicon_codes[2] == "cstem" );
    REQUIRE( info.lexicon_codes[3] == "cbrch" );
    REQUIRE( info.lexicon_codes[4] == "animal" );
    REQUIRE( info.lexicon_names[0] == "Leap month" );
    REQUIRE( info.lexicon_names[1] == "First and Second" );
    REQUIRE( info.lexicon_names[2] == "Chinese Stem" );
    REQUIRE( info.lexicon_names[3] == "Chinese Branch" );
    REQUIRE( info.lexicon_names[4] == "Chinese Year Animals" );
}

TEST_CASE( "Scheme c Test Table", "[c TestTable]" )
{
    for( size_t i = 0; i < guTT_size; i++ ) {
        Field jdn = g_glc->text_to_field( table_c[i][0], "c:cymld" );
        REQUIRE( jdn == guTT_jdn[i] );
        string str = g_glc->field_to_text( jdn, "c:cymld" );
        REQUIRE( str == table_c[i][0] );
    }
}

// End of test/gu/guSch_c.cpp file
