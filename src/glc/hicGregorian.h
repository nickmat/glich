/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicGregorian.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Gregorian (g) calendar header.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     6th April 2023
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

#ifndef CAL_CALGREGORIAN_H_GUARD
#define CAL_CALGREGORIAN_H_GUARD

#include "hicJulian.h"

namespace glich {

    class Gregorian : public Julian
    {
    public:
        Gregorian() {}
        Gregorian( const std::string& data ) : Julian( data ) {}

        Field get_jdn( const FieldVec& fields ) const override;

        FieldVec get_fields( Field jdn ) const override;

        static Field to_jdn( Field year, Field month, Field day );
        static Field year_from_jdn( Field jdn );
        static bool leap_year( Field year );
        static Field today();

    protected:
        bool is_leap_year( Field year ) const override { return leap_year( year ); }

    };

}

#endif // CAL_CALGREGORIAN_H_GUARD
