/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        glc/glc/hicIsoWeek.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Base ISO Week calendar implimentation.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     11th July 2023
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

#ifndef SRC_GLC_HICISOWEEK_H_GUARD
#define SRC_GLC_HICISOWEEK_H_GUARD

#include "hicBase.h"

namespace glich {

    class IsoWeek : public Base
    {
    public:
        IsoWeek( const StdStrVec& data = StdStrVec() ) : Base( data, 3 ) {
            m_fieldnames = { "year", "week", "day" };
        }

        const char* basename() const override { return "isoweek"; }
        size_t required_size() const override { return 3; }

        Field get_jdn( const FieldVec& fields ) const override;
        Field get_end_field_value( const FieldVec& fields, size_t index ) const override;

        FieldVec get_fields( Field jdn ) const override;

        static Field to_jdn( Field year, Field week, Field day );
        static void from_jdn( Field* year, Field* week, Field* day, Field jdn );
        static Field year_from_jdn( Field jdn );

        static bool leap_year( Field year );
        bool is_leap_year( Field year ) const override { return leap_year( year ); }

    private:
        static Field last_week_of_year( Field year );
    };

}

#endif // SRC_GLC_HICISOWEEK_H_GUARD
