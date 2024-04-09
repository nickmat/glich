/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicFormat.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Format class to control the formatting and parsing of dates.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     24th March 2023
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

#ifndef SRC_GLC_HICFORMAT_H_GUARD
#define SRC_GLC_HICFORMAT_H_GUARD

#include <glc/hicDefs.h>
#include "hicGrammar.h"
#include "hicHelper.h"


namespace glich {

    class Record;

    class Format
    {
    public:
        Format( const std::string& code, Grammar& gmr );
        virtual ~Format();
        Format( const Format& ) = delete;
        Format& operator=( const Format& ) = delete;

        virtual bool construct() { m_ok = true; return m_ok; }
        bool is_ok() const { return m_ok; }

        void set_user_input_str( const std::string str ) { m_input_str = str; }
        void set_user_output_str( const std::string str ) { m_output_str = str; }
        void set_style( FormatStyle style ) { m_style = style; }
        void set_ok( bool ok ) { m_ok = ok; }

        std::string get_code() const { return m_code; }
        int get_priority() const { return m_priority; }
        std::string get_input_str() const { return m_input_str; }
        std::string get_output_str() const { return m_output_str; }
        bool has_input() const { return !m_input_str.empty(); }
        bool has_output() const { return !m_output_str.empty(); }
        Grammar* get_owner() const { return &m_owner; }
        const Grammar& get_grammar() const { return m_owner; }
        const Glich& get_glich() const { return get_grammar().get_glich(); }
        FormatStyle get_style() const { return m_style; }
        void get_info( Format_info* info ) const;

        SValue string_to_object( const std::string& ocode, const Base& base, const std::string& input ) const;;

        virtual FormatType get_format_type() const = 0;
        virtual std::string get_text_output( Record& rec ) const = 0;
        virtual RList string_to_rlist( const Base& base, const std::string& input ) const = 0;
        virtual bool set_input( Record& record, const std::string& input, Boundary rb ) const = 0;

        virtual std::string jdn_to_string( const Base& base, Field jdn ) const;
        virtual std::string range_to_string( const Base& base, const Range& rng ) const;
        virtual std::string rlist_to_string( const Base& base, const RList& rlist ) const;

    protected:
        std::string m_code;
        Grammar& m_owner;
        bool m_ok;
        FormatStyle m_style;
        // The priority value is used when the format in/out descriptor string
        // is the same for more than one format.
        // If this string is used to select a format (ie from a pick list)
        // then the format with the highest priority is used.
        int         m_priority;
        std::string m_input_str;
        std::string m_output_str;
    };

}

#endif // SRC_GLC_HICFORMAT_H_GUARD
