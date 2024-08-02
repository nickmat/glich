/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicEaster.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Julian Easter (je) calendar header.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     27th July 2024
 * Copyright:   Copyright (c) 2024, Nick Matthews.
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

#ifndef GLC_HICEASTER_H_GUARD
#define GLC_HICEASTER_H_GUARD

#include "hicBase.h"

namespace glich {

    class Easter : public Base
    {
    public:
        Easter( const StdStrVec& data );
        ~Easter() {}

        void cal_data( const std::string& data );

        size_t required_size() const override { return 4; }
    
        Field get_jdn( const FieldVec& fields ) const override;
        Field get_end_field_value( const FieldVec& fields, size_t index ) const override;

        FieldVec get_fields( Field jdn ) const override;

    private:
        Field get_julian_to_jdn( Field year, Field month, Field day ) const;
        void get_julian_from_jdn( Field* year, Field* month, Field* day, Field jdn ) const;
        Field year_start( Field year ) const;

        Field m_day_offset;
        Field m_year_offset;
    };

}

#endif // GLC_HICEASTER_H_GUARD