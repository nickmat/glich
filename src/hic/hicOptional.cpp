/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicOptional.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Hics global optional values.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     14th April 2023
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

#include "hicOptional.h"

#include "hicJulian.h"
#include "hicMath.h"

using namespace glich;
using std::string;


SValue glich::GetOptional( const string& name, Field jdn )
{
    if( jdn <= f_minimum || jdn >= f_maximum ) {
        return SValue(); // Return null if not a valid jdn value.
    }
    if( name == "wday" ) {  // Mon=1, Sun=7
        return SValue( day_of_week( jdn ) + 1, SValue::Type::field );
    }
    if( name == "wsday" ) {  // Sun=1, Sat=7
        return SValue( day_of_week( jdn + 1 ) + 1, SValue::Type::field );
    }
    if( name == "hyear" ) { // Julian historical year.
        Field y, m, d;
        julian_from_jdn( &y, &m, &d, jdn );
        return SValue( y, SValue::Type::field );
    }
    return SValue();
}

Field glich::GetOptionalField( const string& name, Field jdn )
{
    SValue value = GetOptional( name, jdn );
    if( value.type() == SValue::Type::field ) {
        return value.get_field();
    }
    return f_invalid;
}

// End of src/glc/hicOptional.cpp file
