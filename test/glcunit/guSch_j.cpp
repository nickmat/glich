/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        test/gu/guSch_j.cpp
 * Project:     glcUnit: Unit test program for the Glich Hics library.
 * Purpose:     Test Scheme "j" Julian conversions.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     7th September 2023
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


const char* table_j[guTT_size][4] = {
    // dmy            mdy             wdmy               wmdy+
    { "30 Jul -586", "Jul 30, -586", "Sun 30 Jul -586", "Sunday, July 30, -586"       },
    { "8 Dec -168",  "Dec 8, -168",  "Wed 8 Dec -168",  "Wednesday, December 8, -168" },
    { "26 Sep 70",   "Sep 26, 70",   "Wed 26 Sep 70",   "Wednesday, September 26, 70" },
    { "3 Oct 135",   "Oct 3, 135",   "Sun 3 Oct 135",   "Sunday, October 3, 135"      },
    { "7 Jan 470",   "Jan 7, 470",   "Wed 7 Jan 470",   "Wednesday, January 7, 470"   },
    { "18 May 576",  "May 18, 576",  "Mon 18 May 576",  "Monday, May 18, 576"         },
    { "7 Nov 694",   "Nov 7, 694",   "Sat 7 Nov 694",   "Saturday, November 7, 694"   },
    { "28 Dec 987",  "Dec 28, 987",  "Wed 28 Dec 987",  "Wednesday, December 28, 987" },
    { "19 Apr 1013", "Apr 19, 1013", "Sun 19 Apr 1013", "Sunday, April 19, 1013"      },
    { "18 May 1096", "May 18, 1096", "Sun 18 May 1096", "Sunday, May 18, 1096"        },
    { "16 Mar 1190", "Mar 16, 1190", "Fri 16 Mar 1190", "Friday, March 16, 1190"      },
    { "3 Mar 1240",  "Mar 3, 1240",  "Sat 3 Mar 1240",  "Saturday, March 3, 1240"     },
    { "26 Mar 1288", "Mar 26, 1288", "Fri 26 Mar 1288", "Friday, March 26, 1288"      },
    { "20 Apr 1298", "Apr 20, 1298", "Sun 20 Apr 1298", "Sunday, April 20, 1298"      },
    { "4 Jun 1391",  "Jun 4, 1391",  "Sun 4 Jun 1391",  "Sunday, June 4, 1391"        },
    { "25 Jan 1436", "Jan 25, 1436", "Wed 25 Jan 1436", "Wednesday, January 25, 1436" },
    { "31 Mar 1492", "Mar 31, 1492", "Sat 31 Mar 1492", "Saturday, March 31, 1492"    },
    { "9 Sep 1553",  "Sep 9, 1553",  "Sat 9 Sep 1553",  "Saturday, September 9, 1553" },
    { "24 Feb 1560", "Feb 24, 1560", "Sat 24 Feb 1560", "Saturday, February 24, 1560" },
    { "31 May 1648", "May 31, 1648", "Wed 31 May 1648", "Wednesday, May 31, 1648"     },
    { "20 Jun 1680", "Jun 20, 1680", "Sun 20 Jun 1680", "Sunday, June 20, 1680"       },
    { "13 Jul 1716", "Jul 13, 1716", "Fri 13 Jul 1716", "Friday, July 13, 1716"       },
    { "8 Jun 1768",  "Jun 8, 1768",  "Sun 8 Jun 1768",  "Sunday, June 8, 1768"        },
    { "21 Jul 1819", "Jul 21, 1819", "Mon 21 Jul 1819", "Monday, July 21, 1819"       },
    { "15 Mar 1839", "Mar 15, 1839", "Wed 15 Mar 1839", "Wednesday, March 15, 1839"   },
    { "6 Apr 1903",  "Apr 6, 1903",  "Sun 6 Apr 1903",  "Sunday, April 6, 1903"       },
    { "12 Aug 1929", "Aug 12, 1929", "Sun 12 Aug 1929", "Sunday, August 12, 1929"     },
    { "16 Sep 1941", "Sep 16, 1941", "Mon 16 Sep 1941", "Monday, September 16, 1941"  },
    { "6 Apr 1943",  "Apr 6, 1943",  "Mon 6 Apr 1943",  "Monday, April 6, 1943"       },
    { "24 Sep 1943", "Sep 24, 1943", "Thur 24 Sep 1943","Thursday, September 24, 1943"},
    { "4 Mar 1992",  "Mar 4, 1992",  "Tue 4 Mar 1992",  "Tuesday, March 4, 1992"      },
    { "12 Feb 1996", "Feb 12, 1996", "Sun 12 Feb 1996", "Sunday, February 12, 1996"   },
    { "28 Oct 2038", "Oct 28, 2038", "Wed 28 Oct 2038", "Wednesday, October 28, 2038" },
    { "5 Jul 2094",  "Jul 5, 2094",  "Sun 5 Jul 2094",  "Sunday, July 5, 2094"        }
};
    
TEST_CASE( "Library Scheme j", "[j]" )
{
    Scheme_info info;
    g_glc->get_scheme_info( &info, "j" );
    REQUIRE( info.name == "Julian" );
    REQUIRE( info.code == "j" );
    REQUIRE( info.grammar_code == "j" );
    REQUIRE( info.lexicon_codes[0] == "m" );
    REQUIRE( info.lexicon_codes[1] == "w" );
    REQUIRE( info.lexicon_names[0] == "Month names" );
    REQUIRE( info.lexicon_names[1] == "Weekday names Monday start" );
}


TEST_CASE( "Scheme j Test Table", "[j TestTable]" )
{
    for( size_t i = 0; i < guTT_size; i++ ) {
        Field jdn = g_glc->text_to_field( table_j[i][0], "j:dmy" );
        REQUIRE( jdn == guTT_jdn[i] );
        string str = g_glc->field_to_text( jdn, "j:dmy" );
        REQUIRE( str == table_j[i][0] );

        jdn = g_glc->text_to_field( table_j[i][1], "j:mdy" );
        REQUIRE( jdn == guTT_jdn[i] );
        str = g_glc->field_to_text( jdn, "j:mdy" );
        REQUIRE( str == table_j[i][1] );

        jdn = g_glc->text_to_field( table_j[i][2], "j:wdmy" );
        REQUIRE( jdn == guTT_jdn[i] );
        str = g_glc->field_to_text( jdn, "j:wdmy" );
        REQUIRE( str == table_j[i][2] );

        jdn = g_glc->text_to_field( table_j[i][3], "j:wmdy+" );
        REQUIRE( jdn == guTT_jdn[i] );
        str = g_glc->field_to_text( jdn, "j:wmdy+" );
        REQUIRE( str == table_j[i][3] );
    }
}

TEST_CASE( "Scheme j Test Range", "[j TestRange]" )
{
    struct data { string in; Range rng; string out; } t[] = {
        { "6sep1948", { 2432814, 2432814 }, "6 Sep 1948" },
        { "sep1948", { 2432809, 2432838 },"Sep 1948" },
        { "1948", { 2432565, 2432930 }, "1948" },
        { "1feb1948..29feb1948", { 2432596, 2432624 }, "Feb 1948" },
        { "1jan1948..31dec1948", { 2432565, 2432930 }, "1948" },
        { "1948..sep1948", { 2432565, 2432838 }, "Jan 1948..Sep 1948" },
        { "1948..19sep1948", { 2432565, 2432827 }, "1 Jan 1948..19 Sep 1948" },
        { "19sep1948..1948", { 2432827, 2432930 }, "19 Sep 1948..31 Dec 1948" }
    };
    size_t count = sizeof( t ) / sizeof( data );

    for( size_t i = 0; i < count; i++ ) {
        Range range = g_glc->text_to_range( t[i].in, "j:dmy" );
        REQUIRE( range == t[i].rng );
        string str = g_glc->range_to_text( range, "j:dmy" );
        REQUIRE( str == t[i].out );
    }
}

TEST_CASE( "Scheme j Test RList", "[j TestRList]" )
{
    string str = "30aug2023 | 1800..1837 | may2030..future | past..1756";
    RList expect = { { f_minimum, 2362802 }, { 2378508, 2392387 }, { 2460200, 2460200 }, { 2462636, f_maximum } };
    RList rlist = g_glc->text_to_rlist( str, "j:dmy" );
    REQUIRE( rlist.size() == expect.size() );
    for( size_t i = 0; i < expect.size(); i++ ) {
        REQUIRE( rlist[i] == expect[i] );
    }
    string result = g_glc->rlist_to_text( rlist, "j:dmy" );
    REQUIRE( result == "past..1756 | 1800..1837 | 30 Aug 2023 | May 2030..future" );
}

TEST_CASE( "Scheme j Test Formats", "[j Formats]" )
{
    Field jdn = 2460204;
    // Format "j"
    Field result = g_glc->text_to_field( "3sep2023", "j" );
    REQUIRE( jdn == result );
    string str = g_glc->field_to_text( result, "j" );
    REQUIRE( str == "3 Sep 2023" );
    // Format "j:dmy"
    result = g_glc->text_to_field( "3sep2023", "j:dmy" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( result, "j:dmy" );
    REQUIRE( str == "3 Sep 2023" );
    // Format "j:dmy+"
    result = g_glc->text_to_field( "3september2023", "j:dmy+" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( result, "j:dmy+" );
    REQUIRE( str == "3 September 2023" );
    // Format "j:wdmy"
    result = g_glc->text_to_field( "sun3sep2023", "j:wdmy" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( result, "j:wdmy" );
    REQUIRE( str == "Sat 3 Sep 2023" );
    // Format "j:wdmy+"
    result = g_glc->text_to_field( "saturday3september2023", "j:wdmy+" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( result, "j:wdmy+" );
    REQUIRE( str == "Saturday 3 September 2023" );
    // Format "j:std"
    result = g_glc->text_to_field( "3rd september2023", "j:std" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( jdn, "j:std" );
    REQUIRE( str == "3rd September 2023" );
    // Format "j:full" output only
    result = g_glc->text_to_field( "saturday3rd september2023", "j:std" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( jdn, "j:full" );
    REQUIRE( str == "Saturday 3rd September 2023" );
    // Format "j:mdy"
    result = g_glc->text_to_field( "sep3,2023", "j:mdy" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( result, "j:mdy" );
    REQUIRE( str == "Sep 3, 2023" );
    // Format "j:mdy+"
    result = g_glc->text_to_field( "september3,2023", "j:mdy+" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( result, "j:mdy+" );
    REQUIRE( str == "September 3, 2023" );
    // Format "j:wmdy"
    result = g_glc->text_to_field( "sat,sep3,2023", "j:wmdy" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( result, "j:wmdy" );
    REQUIRE( str == "Sat, Sep 3, 2023" );
    // Format "j:wmdy+"
    result = g_glc->text_to_field( "saturday,september3,2023", "j:wmdy+" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( result, "j:wmdy+" );
    REQUIRE( str == "Saturday, September 3, 2023" );
    // Format "j:ymd"
    result = g_glc->text_to_field( "2023:sep:3", "j:ymd" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( result, "j:ymd" );
    REQUIRE( str == "2023:Sep:3" );
}

// End of test/gu/guSch_j.cpp file.
