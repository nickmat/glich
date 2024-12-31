/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        test/glcunit/guSch_jlit.cpp
 * Project:     glcUnit: Unit test program for the Glich Hics library.
 * Purpose:     Test Scheme "jlit" Julian Liturgical conversions.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     4th December 2024
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


const char* table_jlit[guTT_size][2] = {
    //             wdmyl_a                    wly_a
    { "Sun 30 Jul -586 Trinity IV",   "Sun, Trinity IV -586",   },
    { "Wed 8 Dec -168 Advent II",     "Wed, Advent II -168",    },
    { "Wed 26 Sep 70 Trinity XV",     "Wed, Trinity XV 70",     },
    { "Sun 3 Oct 135 Trinity XVI",    "Sun, Trinity XVI 135",   },
    { "Wed 7 Jan 470 Xmas II",        "Wed, Xmas II 470",       }, // Checked [1]
    { "Mon 18 May 576 Ascen I",       "Mon, Ascen I 576",       }, // Checked [1]
    { "Sat 7 Nov 694 Trinity XX",     "Sat, Trinity XX 694",    }, // Checked [1]
    { "Wed 28 Dec 987 Xmas I",        "Wed, Xmas I 987",        },
    { "Sun 19 Apr 1013 Easter II",    "Sun, Easter II 1013",    }, // Checked [1]
    { "Sun 18 May 1096 Rogation",     "Sun, Rogation 1096",     }, // Checked [1]
    { "Fri 16 Mar 1190 Passion",      "Fri, Passion 1190",      }, // Checked [1]
    { "Sat 3 Mar 1240 Quinquag",      "Sat, Quinquag 1240",     }, // Checked [1]
    { "Fri 26 Mar 1288 Holy",         "Fri, Holy 1288",         }, // Checked [1]
    { "Sun 20 Apr 1298 Easter II",    "Sun, Easter II 1298",    }, // Checked [1]
    { "Sun 4 Jun 1391 Trinity II",    "Sun, Trinity II 1391",   }, // Checked [1]
    { "Wed 25 Jan 1436 Epiph III",    "Wed, Epiph III 1436",    }, // Checked [1]
    { "Sat 31 Mar 1492 Lent III",     "Sat, Lent III 1492",     }, // Checked [1]
    { "Sat 9 Sep 1553 Trinity XIV",   "Sat, Trinity XIV 1553",  }, // Checked [1]
    { "Sat 24 Feb 1560 Sexages",      "Sat, Sexages 1560",      }, // Checked [1]
    { "Wed 31 May 1648 Trinity",      "Wed, Trinity 1648",      }, // Checked [1]
    { "Sun 20 Jun 1680 Trinity II",   "Sun, Trinity II 1680",   }, // Checked [1]
    { "Fri 13 Jul 1716 Trinity VI",   "Fri, Trinity VI 1716",   }, // Checked [1]
    { "Sun 8 Jun 1768 Trinity II",    "Sun, Trinity II 1768",   },
    { "Mon 21 Jul 1819 Trinity VII",  "Mon, Trinity VII 1819",  },
    { "Wed 15 Mar 1839 Passion",      "Wed, Passion 1839",      },
    { "Sun 6 Apr 1903 Easter",        "Sun, Easter 1903",       },
    { "Sun 12 Aug 1929 Trinity VIII", "Sun, Trinity VIII 1929", },
    { "Mon 16 Sep 1941 Trinity XV",   "Mon, Trinity XV 1941",   },
    { "Mon 6 Apr 1943 Holy",          "Mon, Holy 1943",         },
    { "Thur 24 Sep 1943 Trinity XV",  "Thur, Trinity XV 1943",  },
    { "Tue 4 Mar 1992 Quadrag",       "Tue, Quadrag 1992",      },
    { "Sun 12 Feb 1996 Quinquag",     "Sun, Quinquag 1996",     },
    { "Wed 28 Oct 2038 Trinity XX",   "Wed, Trinity XX 2038",   },
    { "Sun 5 Jul 2094 Trinity VI",    "Sun, Trinity VI 2094",   }
};
// Checked [1] - A Handbook of Dates, C R Cheney revised Micheal Jones 
#if 0
TEST_CASE( "Library Scheme jlit", "[jlit]" )
{
    Scheme_info info;
    hic().get_scheme_info(&info, "jlit");
    REQUIRE( info.name == "Julian Liturgical" );
    REQUIRE( info.code == "jlit" );
    REQUIRE( info.grammar_code == "lit" );
    REQUIRE( info.lexicon_codes[0] == "ws" );
    REQUIRE( info.lexicon_codes[1] == "m" );
    REQUIRE( info.lexicon_codes[2] == "lit" );
    REQUIRE( info.lexicon_names[0] == "Weekday names Sunday start" );
    REQUIRE( info.lexicon_names[1] == "Month names" );
    REQUIRE( info.lexicon_names[2] == "Liturgy Weeks" );
}

TEST_CASE( "Scheme jlit Test Table", "[jlit TestTable]" )
{
    for( size_t i = 0; i < guTT_size; i++ ) {
        Field jdn = g_glc->text_to_field( table_jlit[i][0], "jlit:wdmyl_a" );
        REQUIRE( jdn == guTT_jdn[i] );
        string str = g_glc->field_to_text( jdn, "jlit:wdmyl_a" );
        REQUIRE( str == table_jlit[i][0] );

        jdn = g_glc->text_to_field( table_jlit[i][1], "jlit:wly_a" );
        REQUIRE( jdn == guTT_jdn[i] );
        str = g_glc->field_to_text( jdn, "jlit:wly_a" );
        REQUIRE( str == table_jlit[i][1] );
    }
}
#endif

// End of test/glcunit/guSch_jlit.cpp file
