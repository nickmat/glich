/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicChinese.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Chinese Lunisolar calendar.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     13th August 2023
 * Copyright:   Copyright (c) 2023..2025, Nick Matthews.
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

#ifndef SRC_GLC_HICCHINESE_H_GUARD
#define SRC_GLC_HICCHINESE_H_GUARD

#include "hicBase.h"

namespace glich {

    class Chinese : public Base
    {
    public:
        Chinese( const StdStrVec& data );

        const char* basename() const override { return "chinese"; }
        size_t required_size() const override { return 4; }

        Field get_jdn( const FieldVec& fields ) const override;

        Field get_beg_field_value( const FieldVec& fields, size_t index ) const override;
        Field get_end_field_value( const FieldVec& fields, size_t index ) const override;

        FieldVec get_fields( Field jdn ) const override;

        static bool is_leap_month( Field year, Field month );

    };

}

#endif // SRC_GLC_HICCHINESE_H_GUARD
