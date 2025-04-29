/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicIslamic.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Islamic Arithmetical calendar header.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     13th August 2023
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

#ifndef SRC_GLC_HICISLAMIC_H_GUARD
#define SRC_GLC_HICISLAMIC_H_GUARD

#include "hicBase.h"

namespace glich {

    class Islamic : public Base
    {
    public:
        Islamic( const StdStrVec& data );

        void cal_data( const std::string& data );

        const char* basename() const override { return "islamic"; }
        size_t required_size() const override { return 3; }

        Field get_jdn( const FieldVec& fields ) const override;

        Field get_end_field_value( const FieldVec& fields, size_t index ) const override;

        FieldVec get_fields( Field jdn ) const override;

    private:
        bool is_leap_year( Field year ) const override;
        Field last_day_in_month( Field year, Field month ) const;
        Field to_jdn( Field year, Field month, Field day ) const;
        bool from_jdn( Field* year, Field* month, Field* day, Field jdn ) const;

        Field  m_delta;
        Field  m_basedate;
    };

}

#endif // SRC_GLC_HICISLAMIC_H_GUARD
