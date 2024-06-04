/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicHybrid.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Hybrid Calendar Schemes Header.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     15th June 2023
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

#ifndef GLC_HICHYBRID_H_GUARD
#define GLC_HICHYBRID_H_GUARD

#include "hicBase.h"

namespace glich {

    struct HybridData {
        HybridData() : start(f_invalid), end(f_maximum), base(nullptr), scheme(nullptr) {}

        bool ok() { return (start != f_invalid && end != f_invalid && base != nullptr); }

        Field        start;
        Field        end;
        const Base*  base;
        Scheme*      scheme; // NULL if not local
        XRefVec      s_to_h_index;
        XRefVec      h_to_s_index;
    };

    class Hybrid : public Base
    {
    public:
        Hybrid( const StdStrVec& fields, const std::vector<HybridData>& data );
        ~Hybrid();

        bool is_ok() const override;
        size_t required_size() const override { return m_rec_size; }

        Field get_jdn( const FieldVec& fields ) const override;

        Field get_end_field_value( const FieldVec& fields, size_t index ) const override;
        void complete_beg( FieldVec& fields ) const override;
        void complete_end( FieldVec& fields ) const override;

        // Complete the scheme field if possible.
        void update_input( FieldVec& fields ) const override;
        // Converts the given jdn into the Records' Fields.
        FieldVec get_fields( Field jdn ) const override;

        BoolVec mark_balanced_fields(
            const FieldVec& fbeg, const FieldVec& fend, const XIndexVec& rank_to_def, size_t size ) const override;

    private:
        FieldVec get_xref( const FieldVec& fields, Field sch ) const;
        bool is_in_scheme( Field jdn, Field scheme ) const;
        size_t find_scheme( Field jdn ) const;
        void set_hybrid_fields( Field* fields, const Field* mask, Field scheme ) const;
        FieldVec get_scheme_fields( const FieldVec& fields, Field scheme ) const;
        void set_hybrid_fields( FieldVec& hfields, const FieldVec& fields, Field scheme ) const;

        // Note: m_data.size() == m_xref_fields.size()
        std::vector<HybridData> m_data;
        // m_xref_fields gives the index into the bases fields that match the m_fieldnames
        // It would be better to add this to the m_data struct's
        std::vector<XRefVec>    m_xref_fields;
        size_t                  m_rec_size;
    };

}

#endif // GLC_HICHYBRID_H_GUARD