/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        glc/glc/hicFormatText.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     FormatText class to control formatting and parsing of dates.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     21st March 2023
 * Copyright:   Copyright (c) 2023..2025, Nick Matthews.
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

        std::string get_control_in() const { return m_control_in; }
        std::string get_control_out() const { return m_control_out; }

        FormatType get_format_type() const override { return FormatType::text; };
        std::string get_text_output( Record& record ) const override;
        BoolVec get_reveal( Record& rec1, Record& rec2 ) const override;
        std::string get_revealed_text( Record& record, BoolVec& reveal ) const override;
        std::string get_revealed_output( const Record& rec, const BoolVec* reveal ) const;
        Range string_to_range( const Base& base, const std::string& input, FunctionData* fdata = nullptr ) const override;

        bool set_input( Record& record, const std::string& input, Boundary rb ) const override;
        bool set_input( Record& record, const std::string& input ) const override;

    private:
        enum class CP_Group { Hyphen, Digit, Quest, Dual, Sep, Other };

        bool is_significant_rank_name( const std::string& fieldname ) const;
        void remove_from_rank( const std::string& fieldname );
        bool is_separator( char ch ) const;
        bool is_padding( const std::string& word ) const;
        CP_Group get_cp_group(
            std::string::const_iterator it,
            std::string::const_iterator end ) const;
        Field get_field( const Record& record, const std::string& fname, const BoolVec* reveal ) const;
        int parse_date( InputFieldVec& ifs, const std::string& str ) const;
        Field find_token( Lexicon** lex, const std::string& word ) const;
        bool resolve_input( const Base& base, FieldVec& fields, InputFieldVec& input ) const;

        std::string m_control_in;
        std::string m_control_out;
        std::string m_separators;
        StdStrVec   m_padding;
        bool        m_has_roman;

        StdStrVec   m_default_fieldnames;
        StdStrVec   m_rank_fieldnames;
        StdStrVec   m_lex_only_fielnames;
        size_t      m_sig_rank_size; // Significant rank size
        XIndexVec   m_rank_to_def_index;

        StdStrVec   m_format_order;
        XIndexVec   m_fmt_to_rank_index;

        StdStrVec   m_lexicons;

        // =============[unchecked]================
        StdStrVec   m_rankout_fieldnames;

 
        StdStrVec   m_rankin_order;

        StdStrVec   m_default_names;
        FieldVec    m_default_values;
    };

}

#endif // SRC_GLC_HICFORMATTEXT_H_GUARD