/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        glc/hicMath.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Miscellaneous math functions header for HistoryCal.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     16th March 2023
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

#include "hicMath.h"

#include <cassert>

using namespace glich;

// Days in the year at the start of each month (Jan = 1) (not leap year)
Field glich::latin_diy[14] = {
    // Note: we add an extra 0 to the beginning of the array to save
    //       subtracting 1 from the month number
    0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365
};

Field glich::latin_length_of_month[3][12] = {
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }, // Nomal year
    { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }, // Leap year
    { 31, 30, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }  // For the Swedish calendar scheme
};

#define calSEARCH_MAX 30

// CC3 p20
Field glich::min_search( Field start, calSearchFunc func, const void* data )
{
    for( Field i = 0, d = start; i < calSEARCH_MAX; i++, d++ ) {
        if( func( d, data ) ) {
            return d;
        }
    }
    assert( false ); // Should not be here! Increase calSEARCH_MAX?
    return f_invalid;
}

// CC3 p20
Field glich::max_search( Field start, calSearchFunc func, const void* data )
{
    for( Field i = 0, d = start; i < calSEARCH_MAX; i++, d++ ) {
        if( !func( d, data ) ) {
            return d - 1;
        }
    }
    assert( false ); // Should not be here! Increase calSEARCH_MAX?
    return f_invalid;
}

