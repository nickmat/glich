/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicGrammar.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Grammar class to control date formatting.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     24th March 2023
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

#ifndef GLC_HICGRAMMAR_H_GUARD
#define GLC_HICGRAMMAR_H_GUARD

#include "hicHelper.h"
#include <glc/hicDefs.h>

namespace glich {

    class Glich;
    class Base;
    class Grammar;
    class FormatText;
    class FormatIso;
    class FormatUnit;

    class Grammar
    {
    public:
        Grammar( const std::string& code, Glich* glc );
        ~Grammar();
        Grammar( const Grammar& ) = delete;
        Grammar& operator=( const Grammar& ) = delete;

        bool is_ok() const { return m_ok; }
        void create_error( const std::string& mess );
        bool constuct();

        void set_name( const std::string& name ) { m_name = name; }
        void set_inherit( const std::string& gcode );
        void set_inherit( StdStrVec inherit );
        void set_base_fieldnames( StdStrVec fieldnames ) { m_base_fieldnames = fieldnames; }
        void set_opt_fieldnames( StdStrVec fieldnames ) { m_opt_fieldnames = fieldnames; }
        void set_calc_fieldnames( StdStrVec fieldnames ) { m_calc_fieldnames = fieldnames; }
        void set_rank_fieldnames( StdStrVec fieldnames ) { m_rank_fieldnames = fieldnames; }
        void set_preferred( const std::string& fcode );
        void add_lexicon( Lexicon* lex ) { m_lexicons.push_back( lex ); }
        FormatText* create_format_text( const std::string& code );
        FormatIso* create_format_iso( const std::string& code, const StdStrVec& rules );
        FormatUnit* create_format_unit( const std::string& code );
        bool add_format( Format* fmt );
        void add_alias( const std::string& alias, const StdStrVec& pairs );
        void set_calculate(
            const std::string& input, const std::string& output,
            const std::string& first, const std::string& last );

        std::string get_code() const { return m_code; }
        std::string get_name() const { return m_name; }
        std::string get_error_string() const { return m_create_error; }
        std::string resolve_field_alias( const std::string& alias );
        std::string resolve_unit_alias( const std::string& alias );
        std::string resolve_lex_alias( const std::string& alias ) const;
        std::string get_num_pseudo_alias( const std::string& fname ) const;
        std::string get_unit_alias( const std::string& fieldname ) const;

        bool has_input_format() const { return has_format( InOut::input ); }
        void get_input_formats( SchemeFormatInfo* info, const std::string& cur_code ) const;
        bool has_output_format() const { return has_format( InOut::output ); }
        void get_output_formats( SchemeFormatInfo* info, const std::string& cur_code ) const;

        std::string get_calc_output() const { return m_calculate_output; }
        std::string get_calc_input() const { return m_calculate_input; }
        Format* get_format( const std::string& code ) const;
        std::string get_pref_input_fcode() const { return m_pref_input_fcode; }
        std::string get_pref_output_fcode() const { return m_pref_output_fcode; }
        StdStrVec get_lexicon_codes() const;
        StdStrVec get_lexicon_names() const;

        Field find_token( Lexicon** lex, const std::string& word ) const;
        Lexicon* find_lexicon( const std::string& code ) const;

        StdStrVec get_base_fieldnames() const { return m_base_fieldnames; }
        StdStrVec get_opt_fieldnames() const { return m_opt_fieldnames; }
        StdStrVec get_calc_fieldnames() const { return m_calc_fieldnames; }
        StdStrVec get_rank_fieldnames() const { return m_rank_fieldnames; }
        Glich& get_glich() const { return *m_glc; }
        void remove_format( const std::string& fcode ) { m_formats.erase( fcode ); }

        static Grammar* create_default_grammar( const Base* base, Glich* glc );

    private:
        enum class InOut { input, output };
        bool has_format( InOut type ) const;
        void get_format_info( SchemeFormatInfo* info, const std::string& cur_code, InOut type ) const;
        void create_def_format();
        void create_u_format();

        Glich*      m_glc;
        std::string m_code;
        std::string m_name;
        bool        m_ok;
        std::string m_create_error;
        Grammar*    m_inherit;
        bool        m_inherit_lexicons;
        LexiconVec  m_lexicons;
        FormatMap   m_formats;
        std::string m_pref_input_fcode;
        std::string m_pref_output_fcode;
        StdStrVec   m_base_fieldnames;
        StdStrVec   m_opt_fieldnames;
        StdStrVec   m_calc_fieldnames;
        StdStrVec   m_rank_fieldnames;
        StdStrMap   m_field_alias;
        StdStrMap   m_lex_alias;
        StdStrMap   m_num_pseudo_alias;
        StdStrMap   m_unit_alias;
        StrUnitMap  m_unit_type_alias;
        std::string m_calculate_input;
        std::string m_calculate_output;
        std::string m_calculate_first;
        std::string m_calculate_last;
    };

}

#endif // GLC_HICGRAMMAR_H_GUARD
