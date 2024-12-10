/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicLiturgical.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Liturgical calendar header.
 * Author:      Nick Matthews
 * Website:     http://historycal.org
 * Created:     18th August 2024
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

#ifndef GLC_GLCLITURGICAL_H_GUARD
#define GLC_GLCLITURGICAL_H_GUARD

#include "hicBase.h"

namespace glich {

    class Liturgical : public Base
    {
    public:
        Liturgical( const StdStrVec& data );
        ~Liturgical() { delete m_base; }

        void cal_data( const std::string& data );

        const char* basename() const override { return "liturgical"; }
        size_t required_size() const override { return 6; }

        Field get_jdn( const FieldVec& fields ) const override;
        Field get_end_field_value( const FieldVec& fields, size_t index ) const override;

        // Complete any redundant fields.
        void update_input( FieldVec& fields ) const override;
        FieldVec get_fields( Field jdn ) const override;

    private:
        enum class BaseType { julian, gregorian };
        enum Block { B1 = 1, B2 = 5, B3 = 7, B4 = 8, B5 = 14 };

        Field base_jdn( Field year, Field month, Field day ) const;
        Field xmas1( Field year ) const;
        Field xmas2( Field year ) const;
        Field epiph( Field year ) const;
        Field epiph1( Field year ) const;
        Field septuag( Field year ) const;
        Field advent( Field year ) const;

        BaseType m_basetype;
        Base* m_base;
    };

}

#endif // GLC_GLCLITURGICAL_H_GUARD
