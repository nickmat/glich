/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicJwn.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Jwn (Julian Week Number) calendar header.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     27th July 2023
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

#ifndef SRC_GLC_HICJWN_H_GUARD
#define SRC_GLC_HICJWN_H_GUARD

#include "hicBase.h"

namespace glich {

    class Jwn : public Base
    {
    public:
        Jwn( const StdStrVec& data );

        void cal_data( const std::string& data );
        size_t required_size() const override { return 2; }

        Field get_jdn( const FieldVec& fields ) const override;
        Field get_end_field_value( const FieldVec& fields, size_t index ) const override;

        FieldVec get_fields( Field jdn ) const override; // { return { jdn }; }

    private:
        static Field to_jdn( Field week, Field day, Field offset );
        static void from_jdn( Field* week, Field* day, Field jdn, Field offset );

        Field m_offset;
    };

}

#endif // SRC_GLC_HICJWN_H_GUARD
