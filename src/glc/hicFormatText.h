/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        glc/glc/hicFormatText.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     FormatText class to control formatting and parsing of dates.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     21st March 2023
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

#ifndef SRC_GLC_HICFORMATTEXT_H_GUARD
#define SRC_GLC_HICFORMATTEXT_H_GUARD

#include "hicFormat.h"

namespace glich {

    class FormatText : public Format
    {
    public:
        FormatText( const std::string& code, Grammar& gmr );

        bool construct() override;
        void setup_control_in();
        void setup_control_out();

        void set_separators( const std::string& sep ) { m_separators = sep; }
        void set_rank_fieldnames( StdStrVec fieldnames ) { m_rank_fieldnames = fieldnames; }
        void set_rankout_fieldnames( StdStrVec fieldnames ) { m_rankout_fieldnames = fieldnames; }
        void set_control_out( const std::string& format ) { m_control_out = format; }
        void set_control_in( const std::string& format ) { m_control_in = format; }

        std::string get_text_output( const Record& rec ) const override { return get_revealed_output( rec, nullptr ); }
        std::string get_revealed_output( const Record& rec, const BoolVec* reveal ) const;
        RList string_to_rlist( const Base& base, const std::string& input ) const override;

        bool set_input( Record& record, const std::string& input, Boundary rb ) const override;

        std::string range_to_string( const Base& base, Range range ) const override;


    private:
        enum class CP_Group { Hyphen, Digit, Quest, Dual, Sep, Other };

        bool is_significant_rank_name( const std::string& fieldname ) const;
        CP_Group get_cp_group(
            std::string::const_iterator it,
            std::string::const_iterator end ) const;
        Field get_field( const Record& record, const std::string& fname, const BoolVec* reveal ) const;
        int parse_date( InputFieldVec& ifs, const std::string& str ) const;
        bool resolve_input( const Base& base, FieldVec& fields, InputFieldVec& input ) const;

        RList multirange_str_to_rlist( const Base& base, const std::string& input ) const;
        RList bare_str_to_rlist( const Base& base, const std::string& input ) const;

        std::string m_control_in;
        std::string m_control_out;
        std::string m_separators;
        bool m_shorthand; // Shorthand range allowed

        StdStrVec   m_default_fieldnames;
        StdStrVec   m_rank_fieldnames;
        size_t      m_sig_rank_size; // Significant rank size
        XIndexVec   m_rank_to_def_index;

        StdStrVec   m_format_order;
        XIndexVec   m_fmt_to_rank_index;

        // =============[unchecked]================
        StdStrVec   m_rankout_fieldnames;

 
        StdStrVec   m_rankin_order;

        StdStrVec   m_default_names;
        FieldVec    m_default_values;

        std::string m_dual2_fieldname;

    };

}

#endif // SRC_GLC_HICFORMATTEXT_H_GUARD