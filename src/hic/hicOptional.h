/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicOptional.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Hics global optional values.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     14th April 2023
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

#ifndef GLC_HICOPTIONAL_H_GUARD
#define GLC_HICOPTIONAL_H_GUARD

#include <glc/hicDefs.h>
#include "glcValue.h"
#include "hicHelper.h"


namespace glich {

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

    Value name  Resolves to.
    wday        7 Day week Mon=1 (1 to 7)
    wsday       7 Day week Sun=1 (1 to 7)
    jwn         Julian Week Number, week count from jdn 0 (week start Monday).

*/

    SValue GetOptional( const std::string& name, Field jdn );
    Field GetOptionalField( const std::string& name, Field jdn );

}

#endif // GLC_HICOPTIONAL_H_GUARD
