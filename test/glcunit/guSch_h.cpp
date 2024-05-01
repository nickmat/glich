/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        test/gu/guSch_h.cpp
 * Project:     glcUnit: Unit test program for the Glich Hics library.
 * Purpose:     Test Scheme "h" Hebrew Calendar Scheme.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     15th November 2023
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

#include <catch2/catch.hpp>

#include "guMain.h"

using namespace glich;
using std::string;


const char* table_h[guTT_size][2] = {
    //     dmy                       wdmy
    { "10 Av 3174", "Yom Rishon 10 Av 3174" },
    { "25 Kislev 3593", "Yom Revi'i 25 Kislev 3593" },
    { "3 Tishrei 3831", "Yom Revi'i 3 Tishrei 3831" },
    { "9 Tishrei 3896", "Yom Rishon 9 Tishrei 3896" },
    { "18 Tevet 4230", "Yom Revi'i 18 Tevet 4230" },
    { "4 Sivan 4336", "Yom Sheni 4 Sivan 4336" },
    { "13 Cheshvan 4455", "Yom Shabbat 13 Cheshvan 4455" },
    { "5 Shevat 4748", "Yom Revi'i 5 Shevat 4748" },
    { "6 Iyar 4773", "Yom Rishon 6 Iyar 4773" },
    { "23 Iyar 4856", "Yom Rishon 23 Iyar 4856" },
    { "7 Nissan 4950", "Yom Shishi 7 Nissan 4950" },
    { "8 Adar II 5000", "Yom Shabbat 8 Adar II 5000" },
    { "21 Nissan 5048", "Yom Shishi 21 Nissan 5048" },
    { "7 Iyar 5058", "Yom Rishon 7 Iyar 5058" },
    { "1 Tammuz 5151", "Yom Rishon 1 Tammuz 5151" },
    { "7 Shevat 5196", "Yom Revi'i 7 Shevat 5196" },
    { "3 Nissan 5252", "Yom Shabbat 3 Nissan 5252" },
    { "1 Tishrei 5314", "Yom Shabbat 1 Tishrei 5314" },
    { "27 Adar I 5320", "Yom Shabbat 27 Adar I 5320" },
    { "20 Sivan 5408", "Yom Revi'i 20 Sivan 5408" },
    { "3 Tammuz 5440", "Yom Rishon 3 Tammuz 5440" },
    { "5 Av 5476", "Yom Shishi 5 Av 5476" },
    { "4 Tammuz 5528", "Yom Rishon 4 Tammuz 5528" },
    { "11 Av 5579", "Yom Sheni 11 Av 5579" },
    { "12 Nissan 5599", "Yom Revi'i 12 Nissan 5599" },
    { "22 Nissan 5663", "Yom Rishon 22 Nissan 5663" },
    { "19 Av 5689", "Yom Rishon 19 Av 5689" },
    { "8 Tishrei 5702", "Yom Sheni 8 Tishrei 5702" },
    { "14 Nissan 5703", "Yom Sheni 14 Nissan 5703" },
    { "8 Tishrei 5704", "Yom Hamishi 8 Tishrei 5704" },
    { "12 Adar II 5752", "Yom Shelishi 12 Adar II 5752" },
    { "5 Adar 5756", "Yom Rishon 5 Adar 5756" },
    { "12 Cheshvan 5799", "Yom Revi'i 12 Cheshvan 5799" },
    { "5 Av 5854", "Yom Rishon 5 Av 5854" }
};

TEST_CASE( "Library Scheme h", "[h]" )
{
    Scheme_info info;
    g_glc->get_scheme_info( &info, "h" );
    REQUIRE( info.name == "Hebrew" );
    REQUIRE( info.code == "h" );
    REQUIRE( info.grammar_code == "h" );
    REQUIRE( info.lexicon_codes[0] == "hm" );
    REQUIRE( info.lexicon_codes[1] == "hcm" );
    REQUIRE( info.lexicon_codes[2] == "hw" );
    REQUIRE( info.lexicon_names[0] == "Month names" );
    REQUIRE( info.lexicon_names[1] == "Complete Month names" );
    REQUIRE( info.lexicon_names[2] == "Weekday names" );
}

TEST_CASE( "Scheme h Test Table", "[h TestTable]" )
{
    for( size_t i = 0; i < guTT_size; i++ ) {
        Field jdn = g_glc->text_to_field( table_h[i][0], "h:dmy" );
        REQUIRE( jdn == guTT_jdn[i] );
        string str = g_glc->field_to_text( jdn, "h:dmy" );
        REQUIRE( str == table_h[i][0] );

        jdn = g_glc->text_to_field( table_h[i][1], "h:wdmy" );
        REQUIRE( jdn == guTT_jdn[i] );
        str = g_glc->field_to_text( jdn, "h:wdmy" );
        REQUIRE( str == table_h[i][1] );
    }
}


// End of test/gu/guSch_h.cpp file.
