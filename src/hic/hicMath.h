/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        glc/hicMath.h
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

#ifndef SRC_GLC_HICMATH_H_GUARD
#define SRC_GLC_HICMATH_H_GUARD

#include <glc/glcDefs.h>
#include "glcMath.h"
#include <cmath>

namespace glich {

    extern Field latin_diy[14];
    extern Field latin_length_of_month[3][12];

    using calSearchFunc = bool (*)(Field value, const void* data);

    Field min_search( Field start, calSearchFunc func, const void* data );
    Field max_search( Field start, calSearchFunc func, const void* data );

    enum Weekday {
        WDAY_Monday, WDAY_Tuesday, WDAY_Wednesday,
        WDAY_Thursday, WDAY_Friday, WDAY_Saturday, WDAY_Sunday
    };

    Weekday inline day_of_week( Field jdn ) {
        return Weekday( mod_f( jdn, 7 ) );
    }
    Field inline kday_on_or_before( Weekday wday, Field jdn ) {
        return jdn - day_of_week( jdn - wday );
    }
    Field inline kday_on_or_after( Weekday wday, Field jdn ) {
        return kday_on_or_before( wday, jdn + 6 );
    }
    Field inline kday_nearest( Weekday wday, Field jdn ) {
        return kday_on_or_before( wday, jdn + 3 );
    }
    Field inline kday_before( Weekday wday, Field jdn ) {
        return kday_on_or_before( wday, jdn - 1 );
    }
    Field inline kday_after( Weekday wday, Field jdn ) {
        return kday_on_or_before( wday, jdn + 7 );
    }
}

#endif // SRC_GLC_HICMATH_H_GUARD