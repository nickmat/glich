/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        test/glcunit/guAPI_phrase.cpp
 * Project:     glcUnit: Unit test program for the Glich Hics library.
 * Purpose:     Test general Glich API phrase functions.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     17th May 2025
 * Copyright:   Copyright (c) 2025, Nick Matthews.
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


TEST_CASE( "Test date phase string", "[date_phrase]" )
{
    struct data { string in; string out; } t[] = {
        { "1940..1950 | 1945..1955", "1940..1955" },   // 0
        { "1940..1950 && 1945..1955", "1945..1950" },
        { "1940..1950 \\\\ 1945..1955", "1940..1944" },
        { "1940..1950 ^^ 1945..1955", "1940..1944 | 1951..1955" },
        { "!( 1940..1950 )", "past..1939 | 1951..future" },
        { "1940..1950 && ( 1945..1955 | 1948..1957 )", "1945..1950" }, // 5
        { "( 1940..1950 && 1945..1955 ) | 1948..1957", "1945..1957" },
        { "ja:dmy#23feb1942/3", "8 Mar 1943" },
        { "19sep1948+#7", "26 Sep 1948" },
    };
    size_t count = sizeof( t ) / sizeof( data );

    for( size_t i = 0; i < count; i++ ) {
        string line = string("[") + std::to_string(i) + string("]");
        string result = g_glc->date_phrase_to_text( t[i].in );
        CHECK( result+line == t[i].out+line );
    }
}

// End of test/glcunit/guAPI_phrase.cpp file.
