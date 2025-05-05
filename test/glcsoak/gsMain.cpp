/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        test/gs/gsMain.cpp
 * Project:     glcSoak: Soak (extended unit) test program for Hics library.
 * Purpose:     Program main function source.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     4th September 2023
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

#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#include "gsMain.h"

#include <ctime>

using namespace glich;
using std::string;

HicGlich* g_glc = nullptr;

#ifdef GLCTEST_SHORT
#define GLCTEST_TYPE  "Short "
#endif

#ifdef GLCTEST_LONG
#define GLCTEST_TYPE  "Long "
#endif

#ifdef GLCTEST_SOAK
#define GLCTEST_TYPE  "Soak "
#endif

Depth g_depth = Depth::Soak;

string gs_ascii_tolower( const string& str )
{
    string result;
    for( string::const_iterator it = str.begin(); it != str.end(); it++ ) {
        if( *it >= 'A' && *it <= 'Z' ) {
            result += *it + ('a' - 'A');
        }
        else {
            result += *it;
        }
    }
    return result;
}

int main( int argc, char* argv[] )
{
    init_hic( InitLibrary::Hics );
    g_glc = &hic();

    Catch::Session session;

    string type = "Soak";
    auto cli = session.cli()
        | Catch::clara::Opt( type, "test-type" )
        ["-T"]["--test-type"]("Test type: Short, Long, Soak");
    session.cli( cli );
    auto cli_result = cli.parse( Catch::clara::detail::Args( argc, argv ) );
    string ltype = gs_ascii_tolower( type );
    if( ltype == "short" ) {
        g_depth = Depth::Short;
    }
    else if( ltype == "long" ) {
        g_depth = Depth::Long;
    }
    else if( ltype == "soak" ) {
        g_depth = Depth::Soak;
    }
    else {
        std::cout << "Test type not recognised.\n";
        return 1;
    }

    std::cout << type;

    clock_t t = clock();

    int result = session.run( argc, argv );

    clock_t s = (clock() - t) / CLOCKS_PER_SEC;
    clock_t m = s / 60;
    std::cout << "Timed: " << m << "m " << s - (m * 60) << "s" << std::endl;

    exit_hic();
    return result;
}

// End of test/gu/guMain.cpp file.
