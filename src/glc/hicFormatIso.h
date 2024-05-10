/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        glc/glc/hicFormatIso.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     FormatIso class to control ISO 8601 formatting and parsing.
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

#ifndef SRC_GLC_HICFORMATISO_H_GUARD
#define SRC_GLC_HICFORMATISO_H_GUARD

#include "hicFormat.h"

namespace glich {

    class FormatIso : public Format
    {
        enum class DateRep { null, gregorian, week, ordinal };

    public:
        FormatIso( const std::string& code, Grammar& gmr, const StdStrVec& rules );

        FormatType get_format_type() const override { return FormatType::iso; };
        std::string get_text_output( Record& record ) const override;
        Range string_to_range( const Base& base, const std::string& input, FunctionData* fdata = nullptr ) const override;
        bool set_input( Record& record, const std::string& input, Boundary rb ) const override;

        std::string jdn_to_string( const Base& base, Field jdn ) const override;
        std::string range_to_string( const Base& base, const Range& range ) const override;
        std::string rlist_to_string( const Base& base, const RList& rlist ) const override;

    private:
        std::string jdn_to_str( const Base& base, Field jdn ) const;
        std::string range_to_str( const Base& base, const Range& range ) const;
        std::string get_masked_output( const Record& record, const BoolVec* mask = nullptr ) const;
        std::string output_year( Field year ) const;

        DateRep recurring_interval( FieldVec& fields, const std::string& input, Boundary rb ) const;
        DateRep interval( FieldVec& fields, const std::string& input, Boundary rb ) const;
        DateRep datetime( FieldVec& fields, const std::string& input, Boundary rb ) const;
        DateRep weekdate( FieldVec& fields, const std::string& str, Boundary rb ) const;

        RList string_set_to_rlist( const Base& base, const std::string& input ) const;

        virtual StringPairVec string_to_stringpair( std::string& text ) const override;

        DateRep  m_daterep;
        bool     m_extended;
        bool     m_yplusminus;
        bool     m_yminus; // Ignored if m_plusminus is true
        bool     m_dateset;
        size_t   m_ydigits;
    };

}

#endif // SRC_GLC_HICFORMATISO_H_GUARD