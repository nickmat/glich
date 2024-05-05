/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        test/gu/guSch_j.cpp
 * Project:     glcUnit: Unit test program for the Glich Hics library.
 * Purpose:     Test Scheme "ja" Julian Annunciation conversions.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     8th September 2023
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


const char* table_ja[guTT_size][4] = {
    // dmy            mdy             wdmy               wmdy+
    { "30 Jul -586", "Jul 30, -586", "Sun 30 Jul -586", "Sunday, July 30, -586"       },
    { "8 Dec -168",  "Dec 8, -168",  "Wed 8 Dec -168",  "Wednesday, December 8, -168" },
    { "26 Sep 70",   "Sep 26, 70",   "Wed 26 Sep 70",   "Wednesday, September 26, 70" },
    { "3 Oct 135",   "Oct 3, 135",   "Sun 3 Oct 135",   "Sunday, October 3, 135"      },
    { "7 Jan 469/70",   "Jan 7, 469/70",   "Wed 7 Jan 469/70",   "Wednesday, January 7, 469/70"},
    { "18 May 576",  "May 18, 576",  "Mon 18 May 576",  "Monday, May 18, 576"         },
    { "7 Nov 694",   "Nov 7, 694",   "Sat 7 Nov 694",   "Saturday, November 7, 694"   },
    { "28 Dec 987",  "Dec 28, 987",  "Wed 28 Dec 987",  "Wednesday, December 28, 987" },
    { "19 Apr 1013", "Apr 19, 1013", "Sun 19 Apr 1013", "Sunday, April 19, 1013"      },
    { "18 May 1096", "May 18, 1096", "Sun 18 May 1096", "Sunday, May 18, 1096"        },
    { "16 Mar 1189/90", "Mar 16, 1189/90", "Fri 16 Mar 1189/90", "Friday, March 16, 1189/90"},
    { "3 Mar 1239/40",  "Mar 3, 1239/40",  "Sat 3 Mar 1239/40",  "Saturday, March 3, 1239/40"},
    { "26 Mar 1288", "Mar 26, 1288", "Fri 26 Mar 1288", "Friday, March 26, 1288"      },
    { "20 Apr 1298", "Apr 20, 1298", "Sun 20 Apr 1298", "Sunday, April 20, 1298"      },
    { "4 Jun 1391",  "Jun 4, 1391",  "Sun 4 Jun 1391",  "Sunday, June 4, 1391"        },
    { "25 Jan 1435/6", "Jan 25, 1435/6", "Wed 25 Jan 1435/6", "Wednesday, January 25, 1435/6"},
    { "31 Mar 1492", "Mar 31, 1492", "Sat 31 Mar 1492", "Saturday, March 31, 1492"    },
    { "9 Sep 1553",  "Sep 9, 1553",  "Sat 9 Sep 1553",  "Saturday, September 9, 1553" },
    { "24 Feb 1559/60", "Feb 24, 1559/60", "Sat 24 Feb 1559/60", "Saturday, February 24, 1559/60"},
    { "31 May 1648", "May 31, 1648", "Wed 31 May 1648", "Wednesday, May 31, 1648"     },
    { "20 Jun 1680", "Jun 20, 1680", "Sun 20 Jun 1680", "Sunday, June 20, 1680"       },
    { "13 Jul 1716", "Jul 13, 1716", "Fri 13 Jul 1716", "Friday, July 13, 1716"       },
    { "8 Jun 1768",  "Jun 8, 1768",  "Sun 8 Jun 1768",  "Sunday, June 8, 1768"        },
    { "21 Jul 1819", "Jul 21, 1819", "Mon 21 Jul 1819", "Monday, July 21, 1819"       },
    { "15 Mar 1838/9", "Mar 15, 1838/9", "Wed 15 Mar 1838/9", "Wednesday, March 15, 1838/9"},
    { "6 Apr 1903",  "Apr 6, 1903",  "Sun 6 Apr 1903",  "Sunday, April 6, 1903"       },
    { "12 Aug 1929", "Aug 12, 1929", "Sun 12 Aug 1929", "Sunday, August 12, 1929"     },
    { "16 Sep 1941", "Sep 16, 1941", "Mon 16 Sep 1941", "Monday, September 16, 1941"  },
    { "6 Apr 1943",  "Apr 6, 1943",  "Mon 6 Apr 1943",  "Monday, April 6, 1943"       },
    { "24 Sep 1943", "Sep 24, 1943", "Thur 24 Sep 1943","Thursday, September 24, 1943"},
    { "4 Mar 1991/2",  "Mar 4, 1991/2",  "Tue 4 Mar 1991/2",  "Tuesday, March 4, 1991/2"  },
    { "12 Feb 1995/6", "Feb 12, 1995/6", "Sun 12 Feb 1995/6", "Sunday, February 12, 1995/6"},
    { "28 Oct 2038", "Oct 28, 2038", "Wed 28 Oct 2038", "Wednesday, October 28, 2038" },
    { "5 Jul 2094",  "Jul 5, 2094",  "Sun 5 Jul 2094",  "Sunday, July 5, 2094"        }
};
    
TEST_CASE( "Library Scheme ja", "[ja]" )
{
    Scheme_info info;
    g_glc->get_scheme_info( &info, "ja" );
    REQUIRE( info.name == "Julian Annunciation Florence" );
    REQUIRE( info.code == "ja" );
    REQUIRE( info.grammar_code == "j_sh" );
    REQUIRE( info.lexicon_codes[0] == "m" );
    REQUIRE( info.lexicon_codes[1] == "w" );
    REQUIRE( info.lexicon_names[0] == "Month names" );
    REQUIRE( info.lexicon_names[1] == "Weekday names Monday start" );
}

TEST_CASE( "Scheme ja Test Table", "[ja TestTable]" )
{
    for( size_t i = 0; i < guTT_size; i++ ) {
        Field jdn = g_glc->text_to_field( table_ja[i][0], "ja:dmy" );
        REQUIRE( jdn == guTT_jdn[i] );
        string str = g_glc->field_to_text( jdn, "ja:dmy" );
        REQUIRE( str == table_ja[i][0] );

        jdn = g_glc->text_to_field( table_ja[i][1], "ja:mdy" );
        REQUIRE( jdn == guTT_jdn[i] );
        str = g_glc->field_to_text( jdn, "ja:mdy" );
        REQUIRE( str == table_ja[i][1] );

        jdn = g_glc->text_to_field( table_ja[i][2], "ja:wdmy" );
        REQUIRE( jdn == guTT_jdn[i] );
        str = g_glc->field_to_text( jdn, "ja:wdmy" );
        REQUIRE( str == table_ja[i][2] );

        jdn = g_glc->text_to_field( table_ja[i][3], "ja:wmdy+" );
        REQUIRE( jdn == guTT_jdn[i] );
        str = g_glc->field_to_text( jdn, "ja:wmdy+" );
        REQUIRE( str == table_ja[i][3] );

    }
}

// End of test/gu/guSch_ja.cpp file.
