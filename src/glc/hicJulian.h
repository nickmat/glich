/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicJulian.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Julian (j) calendar header.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     17th March 2023
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

#ifndef GLC_HICJULIAN_H_GUARD
#define GLC_HICJULIAN_H_GUARD

#include "hicBase.h"

namespace glich {

    class Julian : public Base
    {
    public:
        Julian( const std::string& data );
        ~Julian() {}

        void set_data( const std::string& data );
        bool set_epoch( Field epoch ) override;

        size_t required_size() const override { return 3; }
    
        Field get_jdn( const FieldVec& fields ) const override;
        Field get_end_field_value( const FieldVec& fields, size_t index ) const override;

        FieldVec get_fields( Field jdn ) const override;

        static bool leap_year( Field year );
        bool is_leap_year( Field year ) const override { return leap_year( year ); }

    protected:
        Field last_day_in_month( Field year, Field month ) const;

    private:
        Field m_year_offset;
    };

}

#endif // GLC_HICJULIAN_H_GUARD