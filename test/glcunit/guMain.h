/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        test/gu/guMain.h
 * Project:     glcUnit: Unit test program for the Glich Hics library.
 * Purpose:     Program main function header.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     17th August 2023
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

#ifndef TEST_GLCUNIT_GUMAIN_H
#define TEST_GLCUNIT_GUMAIN_H

#include <glc/hic.h>

extern glich::HicGlich* g_glc;

constexpr size_t guTT_size = 34;

constexpr glich::Field guTT_jdn[guTT_size] = {
    1507232,
    1660038,
    1746894,
    1770642,
    1892732,
    1931580,
    1974852,
    2081921,
    2091165,
    2121510,
    2155780,
    2174030,
    2191585,
    2195262,
    2229275,
    2245581,
    2266101,
    2288543,
    2290902,
    2323141,
    2334849,
    2348021,
    2366979,
    2385649,
    2392826,
    2416224,
    2425849,
    2430267,
    2430834,
    2431005,
    2448699,
    2450139,
    2465738,
    2486077
};

#endif // TEST_GLCUNIT_GUMAIN_H
