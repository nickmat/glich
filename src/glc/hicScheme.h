/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicScheme.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Scheme class implimentation.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
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

#ifndef SRC_GLC_HICSCHEME_H_GUARD
#define SRC_GLC_HICSCHEME_H_GUARD

#include <glc/glc.h>
#include <glc/hicDefs.h>
#include "glcObject.h"
#include "hicHybrid.h"

namespace glich {

    class Calendars;
    class Grammar;
    class Base;

    class Scheme : public Object
    {
    public:
        enum class BaseName {
            null, jdn, jwn, julian, gregorian, isoweek, ordinal,
            hebrew, french, islamic, chinese
        };

        Scheme( const std::string& code, const Base& base );
        virtual ~Scheme();

        bool reset();

        void set_output_format( const std::string& fcode );
        void set_input_format( const std::string& fcode );

        SValue complete_object( Field jdn ) const;
        SValue complete_object( const std::string& input, const std::string& fcode ) const;

        std::string get_scode() const;
        std::string get_name() const { return m_name; }
        SchemeStyle get_style() const { return m_style; }
        const Base& get_base() const { return m_base; }
        const Grammar* get_grammar() const { return m_base.get_grammar(); }
        StdStrVec get_format_list() const;
        std::string get_input_format_code() const { return m_input_fcode; }
        std::string get_output_format_code() const { return m_output_fcode; }
        Format* get_output_format( const std::string& fcode ) const;
        Format* get_input_format( const std::string& fcode ) const;
        void get_info( Scheme_info* info ) const;
        void get_format_text_info( FormatText_info* info, const std::string& fcode ) const;
        bool is_leap_year( Field year ) const;

        FieldVec get_object_fields( const SValueVec& values ) const;

        void set_name( const std::string& name ) { m_name = name; }
        void set_style( SchemeStyle style ) { m_style = style; }

        std::string jdn_to_str( Field jdn, const std::string& fcode ) const;
        std::string range_to_str( const Range& rng, const std::string& fcode ) const;
        std::string rlist_to_str( const RList& rlist, const std::string& fcode ) const;

        std::string object_to_str( const SValue& ovalue, const std::string& fcode ) const;
        SValue object_to_demoted_rlist( const SValue& ovalue ) const;

        bool create_epoch_functions( Field epoch, int line );

        static Base* create_base( BaseName bs, const std::string& data );
        static Base* create_base_hybrid(
            const StdStrVec& fieldnames,
            const std::vector<HybridData>& data );

    private:
        std::string  m_name;
        SchemeStyle  m_style;
        const Base&  m_base;
        std::string  m_input_fcode;
        std::string  m_output_fcode;
    };

}

#endif // SRC_GLC_HICSCHEME_H_GUARD
