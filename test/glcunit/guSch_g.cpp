/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        test/gu/guSch_g.cpp
 * Project:     glcUnit: Unit test program for the Glich Hics library.
 * Purpose:     Test Scheme "g" Gregorian conversions.
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


const char* test_strs[guTT_size][5] = {
    // dmy            mdy             wdmy               wmdy+                          iso
    { "24 Jul -586", "Jul 24, -586", "Sun 24 Jul -586", "Sunday, July 24, -586",       "-0586-07-24"},
    { "5 Dec -168",  "Dec 5, -168",  "Wed 5 Dec -168",  "Wednesday, December 5, -168", "-0168-12-05"},
    { "24 Sep 70",   "Sep 24, 70",   "Wed 24 Sep 70",   "Wednesday, September 24, 70", "0070-09-24" },
    { "2 Oct 135",   "Oct 2, 135",   "Sun 2 Oct 135",   "Sunday, October 2, 135",      "0135-10-02" },
    { "8 Jan 470",   "Jan 8, 470",   "Wed 8 Jan 470",   "Wednesday, January 8, 470",   "0470-01-08" },
    { "20 May 576",  "May 20, 576",  "Mon 20 May 576",  "Monday, May 20, 576",         "0576-05-20" },
    { "10 Nov 694",  "Nov 10, 694",  "Sat 10 Nov 694",  "Saturday, November 10, 694",  "0694-11-10" },
    { "2 Jan 988",   "Jan 2, 988",   "Wed 2 Jan 988",   "Wednesday, January 2, 988",   "0988-01-02" },
    { "25 Apr 1013", "Apr 25, 1013", "Sun 25 Apr 1013", "Sunday, April 25, 1013",      "1013-04-25" },
    { "24 May 1096", "May 24, 1096", "Sun 24 May 1096", "Sunday, May 24, 1096",        "1096-05-24" },
    { "23 Mar 1190", "Mar 23, 1190", "Fri 23 Mar 1190", "Friday, March 23, 1190",      "1190-03-23" },
    { "10 Mar 1240", "Mar 10, 1240", "Sat 10 Mar 1240", "Saturday, March 10, 1240",    "1240-03-10" },
    { "2 Apr 1288",  "Apr 2, 1288",  "Fri 2 Apr 1288",  "Friday, April 2, 1288",       "1288-04-02" },
    { "27 Apr 1298", "Apr 27, 1298", "Sun 27 Apr 1298", "Sunday, April 27, 1298",      "1298-04-27" },
    { "12 Jun 1391", "Jun 12, 1391", "Sun 12 Jun 1391", "Sunday, June 12, 1391",       "1391-06-12" },
    { "3 Feb 1436",  "Feb 3, 1436",  "Wed 3 Feb 1436",  "Wednesday, February 3, 1436", "1436-02-03" },
    { "9 Apr 1492",  "Apr 9, 1492",  "Sat 9 Apr 1492",  "Saturday, April 9, 1492",     "1492-04-09" },
    { "19 Sep 1553", "Sep 19, 1553", "Sat 19 Sep 1553", "Saturday, September 19, 1553","1553-09-19" },
    { "5 Mar 1560",  "Mar 5, 1560",  "Sat 5 Mar 1560",  "Saturday, March 5, 1560",     "1560-03-05" },
    { "10 Jun 1648", "Jun 10, 1648", "Wed 10 Jun 1648", "Wednesday, June 10, 1648",    "1648-06-10" },
    { "30 Jun 1680", "Jun 30, 1680", "Sun 30 Jun 1680", "Sunday, June 30, 1680",       "1680-06-30" },
    { "24 Jul 1716", "Jul 24, 1716", "Fri 24 Jul 1716", "Friday, July 24, 1716",       "1716-07-24" },
    { "19 Jun 1768", "Jun 19, 1768", "Sun 19 Jun 1768", "Sunday, June 19, 1768",       "1768-06-19" },
    { "2 Aug 1819",  "Aug 2, 1819",  "Mon 2 Aug 1819",  "Monday, August 2, 1819",      "1819-08-02" },
    { "27 Mar 1839", "Mar 27, 1839", "Wed 27 Mar 1839", "Wednesday, March 27, 1839",   "1839-03-27" },
    { "19 Apr 1903", "Apr 19, 1903", "Sun 19 Apr 1903", "Sunday, April 19, 1903",      "1903-04-19" },
    { "25 Aug 1929", "Aug 25, 1929", "Sun 25 Aug 1929", "Sunday, August 25, 1929",     "1929-08-25" },
    { "29 Sep 1941", "Sep 29, 1941", "Mon 29 Sep 1941", "Monday, September 29, 1941",  "1941-09-29" },
    { "19 Apr 1943", "Apr 19, 1943", "Mon 19 Apr 1943", "Monday, April 19, 1943",      "1943-04-19" },
    { "7 Oct 1943",  "Oct 7, 1943",  "Thur 7 Oct 1943", "Thursday, October 7, 1943",   "1943-10-07" },
    { "17 Mar 1992", "Mar 17, 1992", "Tue 17 Mar 1992", "Tuesday, March 17, 1992",     "1992-03-17" },
    { "25 Feb 1996", "Feb 25, 1996", "Sun 25 Feb 1996", "Sunday, February 25, 1996",   "1996-02-25" },
    { "10 Nov 2038", "Nov 10, 2038", "Wed 10 Nov 2038", "Wednesday, November 10, 2038","2038-11-10" },
    { "18 Jul 2094", "Jul 18, 2094", "Sun 18 Jul 2094", "Sunday, July 18, 2094",       "2094-07-18" }
};


TEST_CASE( "Library Scheme g", "[g]" )
{
    Scheme_info info;
    g_glc->get_scheme_info( &info, "g" );
    REQUIRE( info.name == "Gregorian" );
    REQUIRE( info.code == "g" );
    REQUIRE( info.grammar_code == "g" );
    REQUIRE( info.lexicon_codes[0] == "m" );
    REQUIRE( info.lexicon_codes[1] == "w" );
    REQUIRE( info.lexicon_names[0] == "Month names" );
    REQUIRE( info.lexicon_names[1] == "Weekday names Monday start" );
}

TEST_CASE( "Scheme g Test Table", "[g TestTable]" )
{
    for( size_t i = 0; i < guTT_size; i++ ) {
        Field jdn = g_glc->text_to_field( test_strs[i][0], "g:dmy" );
        REQUIRE( jdn == guTT_jdn[i] );
        string str = g_glc->field_to_text( jdn, "g:dmy" );
        REQUIRE( str == test_strs[i][0] );

        jdn = g_glc->text_to_field( test_strs[i][1], "g:mdy" );
        REQUIRE( jdn == guTT_jdn[i] );
        str = g_glc->field_to_text( jdn, "g:mdy" );
        REQUIRE( str == test_strs[i][1] );

        jdn = g_glc->text_to_field( test_strs[i][2], "g:wdmy" );
        REQUIRE( jdn == guTT_jdn[i] );
        str = g_glc->field_to_text( jdn, "g:wdmy" );
        REQUIRE( str == test_strs[i][2] );

        jdn = g_glc->text_to_field( test_strs[i][3], "g:wmdy+" );
        REQUIRE( jdn == guTT_jdn[i] );
        str = g_glc->field_to_text( jdn, "g:wmdy+" );
        REQUIRE( str == test_strs[i][3] );

        jdn = g_glc->text_to_field( test_strs[i][4], "g:iso" );
        REQUIRE( jdn == guTT_jdn[i] );
        str = g_glc->field_to_text( jdn, "g:iso" );
        REQUIRE( str == test_strs[i][4] );
    }
}

TEST_CASE( "Scheme g Test Range", "[g TestRange]" )
{
    struct data { string in; Range rng; string out; } t[] = {
        { "19sep1948", { 2432814, 2432814 }, "19 Sep 1948" },
        { "sep1948", { 2432796, 2432825 },"Sep 1948" },
        { "1948", { 2432552, 2432917 }, "1948" },
        { "1feb1948..29feb1948", { 2432583, 2432611 }, "Feb 1948" },
        { "1jan1948..31dec1948", { 2432552, 2432917 }, "1948" },
        { "1948..sep1948", { 2432552, 2432825 }, "Jan 1948..Sep 1948" },
        { "1948..19sep1948", { 2432552, 2432814 }, "1 Jan 1948..19 Sep 1948" },
        { "19sep1948..1948", { 2432814, 2432917 }, "19 Sep 1948..31 Dec 1948" }
    };
    size_t count = sizeof( t ) / sizeof( data );

    for( size_t i = 0; i < count; i++ ) {
        Range range = g_glc->text_to_range( t[i].in, "g:dmy" );
        REQUIRE( range == t[i].rng );
        string str = g_glc->range_to_text( range, "g:dmy" );
        REQUIRE( str == t[i].out );
    }
}

TEST_CASE( "Scheme g Test RList", "[g TestRList]" )
{
    string str = "30aug2023 | 1800..1837 | may2030..future | past..1756";
    RList expect = { { f_minimum, 2362791 }, { 2378497, 2392375 }, { 2460187, 2460187 }, { 2462623, f_maximum } };
    RList rlist = g_glc->text_to_rlist( str, "g:dmy" );
    REQUIRE( rlist.size() == expect.size() );
    for( size_t i = 0; i < expect.size(); i++ ) {
        REQUIRE( rlist[i] == expect[i] );
    }
    string result = g_glc->rlist_to_text( rlist, "g:dmy" );
    REQUIRE( result == "past..1756 | 1800..1837 | 30 Aug 2023 | May 2030..future" );
}

TEST_CASE( "Scheme g Test Formats", "[g Formats]" )
{
    Field jdn = 2460191;
    // Default scheme and format.
    Field result = g_glc->text_to_field( "3sep2023" );
    REQUIRE( jdn == result );
    string str = g_glc->field_to_text( result );
    REQUIRE( str == "3 Sep 2023" );
    // Format ":dmy"
    result = g_glc->text_to_field( "3sep2023", ":dmy" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( result, ":dmy" );
    REQUIRE( str == "3 Sep 2023" );
    // Format "g:dmy"
    result = g_glc->text_to_field( "3sep2023", "g:dmy" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( result, "g:dmy" );
    REQUIRE( str == "3 Sep 2023" );
    // Format "g:dmy+"
    result = g_glc->text_to_field( "3september2023", "g:dmy+" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( result, "g:dmy+" );
    REQUIRE( str == "3 September 2023" );
    // Format "g:wdmy"
    result = g_glc->text_to_field( "sun3sep2023", "g:wdmy" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( result, "g:wdmy" );
    REQUIRE( str == "Sun 3 Sep 2023" );
    // Format "g:wdmy+"
    result = g_glc->text_to_field( "sunday3september2023", "g:wdmy+" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( result, "g:wdmy+" );
    REQUIRE( str == "Sunday 3 September 2023" );
    // Format "g:std"
    result = g_glc->text_to_field( "3rd september2023", "g:std" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( jdn, "g:std" );
    REQUIRE( str == "3rd September 2023" );
    // Format "g:full"
    result = g_glc->text_to_field( "sunday3rd september2023", "g:full" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( jdn, "g:full" );
    REQUIRE( str == "Sunday 3rd September 2023" );
    // Format "g:mdy"
    result = g_glc->text_to_field( "sep3,2023", "g:mdy" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( result, "g:mdy" );
    REQUIRE( str == "Sep 3, 2023" );
    // Format "g:mdy+"
    result = g_glc->text_to_field( "september3,2023", "g:mdy+" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( result, "g:mdy+" );
    REQUIRE( str == "September 3, 2023" );
    // Format "g:wmdy"
    result = g_glc->text_to_field( "sun,sep3,2023", "g:wmdy" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( result, "g:wmdy" );
    REQUIRE( str == "Sun, Sep 3, 2023" );
    // Format "g:wmdy+"
    result = g_glc->text_to_field( "sunday,september3,2023", "g:wmdy+" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( result, "g:wmdy+" );
    REQUIRE( str == "Sunday, September 3, 2023" );
    // Format "g:ymd"
    result = g_glc->text_to_field( "2023:sep:3", "g:ymd" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( result, "g:ymd" );
    REQUIRE( str == "2023:Sep:3" );
    // Format "g:iso"
    result = g_glc->text_to_field( "2023-09-03", "g:iso" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( result, "g:iso" );
    REQUIRE( str == "2023-09-03" );
    // Format "g:iso_set"
    result = g_glc->text_to_field( "[2023-09-03]", "g:iso_set" );
    REQUIRE( jdn == result );
    str = g_glc->field_to_text( result, "g:iso_set" );
    REQUIRE( str == "[2023-09-03]" );
}

// End of test/gu/guSch_g.cpp file.
