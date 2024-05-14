/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicRecord.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Header for Record class to hold date values.
 * Author:      Nick Matthews
 * Website:     http://historycal.org
 * Created:     17th March 2023
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

#ifndef SRC_GLC_HICRECORD_H_GUARD
#define SRC_GLC_HICRECORD_H_GUARD

#include <glc/glcDefs.h>
#include <glc/hicDefs.h>
#include "hicBase.h"
#include "hicHelper.h"


namespace glich {

    class Base;
    class Format;

    class Record
    {
    public:
        Record( const Base& base );
        Record( const Base& base, Field jdn );
        Record( const Base& base, const std::string& str, const Format& fmt );
        Record( const Base& base, const std::string& str, const Format& fmt, Boundary rb );
        Record( const Base& base, const FieldVec& fields );
        Record( const Base& base, const SValue& ovalue );

        void set_jdn( Field jdn );
        Field calc_jdn();
        void set_str( const std::string& str, const Format& fmt );
        void set_str( const std::string& str, const Format& fmt, Boundary rb );
        void set_fields( const FieldVec fields );
        void set_object( const SValue& ovalue );

        Field complete_fields_as_beg() { m_base.complete_beg( m_f ); return calc_jdn(); } // Returns jdn for record
        Field complete_fields_as_end() { m_base.complete_end( m_f ); return calc_jdn(); }

        void update_input();
        void update_output();

        void set_field( Field value, size_t index );

        void clear_fields() { std::fill( m_f.begin(), m_f.end(), f_invalid ); }

        Range get_range_from_mask() const;
        SValue get_object( const std::string& ocode ) const;

        BoolVec mark_balanced_fields( Record& rec, const XIndexVec& rank_to_def, size_t size );

        const Base& get_base() const { return m_base; }
        const FieldVec& get_field_vec() const { return m_f; }
        FieldVec& get_field_vec() { return m_f; }
        Field get_field_at( size_t index ) const { return m_f[index]; }
        Field get_field( size_t index ) const;
        Field get_field( size_t index, const BoolVec* mask ) const;
        Field get_jdn() const { return m_jdn; }

        int get_field_index( const std::string& fn ) const { return m_base.get_alias_fieldname_index( fn ); }

    private:
        const Base& m_base;
        FieldVec    m_f;
        Field       m_jdn;
    };
}

#endif // SRC_GLC_HICRECORD_H_GUARD
