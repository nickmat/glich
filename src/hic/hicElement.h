/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicElement.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Element classes for formatting and parsing date elements.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     19th March 2023
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

#ifndef SRC_GLC_HICELEMENT_H_GUARD
#define SRC_GLC_HICELEMENT_H_GUARD

#include <glc/hicDefs.h>
#include "hicHelper.h"

namespace glich {

    class Glich;
    struct ElementField;

    class Element
    {
    public:
        Element() : m_state( State::do_field ), m_text_only( false ) {}
        virtual ~Element() {}

        virtual void clear();
        void add_char( char ch );
        void add_string( const std::string& str );

        std::string get_field_name() const { return m_field_name; }
        std::string get_dual_field_name() const { return m_dual_field_name; }
        std::string get_lcode() const { return m_lcode; }

        std::string get_formatted_element( Field field );
        Field get_converted_field( const std::string& str ) const;

        bool has_dual_field_name() const { return !m_dual_field_name.empty(); }
        bool is_text_only() const { return m_text_only; }

    private:
        enum class State { do_field, do_dual, do_lcode, do_spec, do_qual, do_default };
        State m_state;
    protected:
        std::string m_field_name;
        std::string m_dual_field_name;
        std::string m_lcode;
        bool m_text_only;
        std::string m_spec;
        std::string m_qualifier;
        std::string m_default_text;

    };

    class ElementControlIn : public Element
    {
    public:
        ElementControlIn() : Element(), m_type( IFT_number ), m_lex( nullptr ),
            m_default_value( f_invalid ) {}

        void clear() override;
        bool expand_specifier( Grammar* gmr );

        std::string get_input_text() const;
        std::string get_dual_record_field_name() const { return m_dual_record_field_name; }
        std::string get_record_field_name() const { return m_record_field_name; }
        std::string get_default_text() const { return m_default_text; }

        Lexicon* get_lexicon() const { return m_lex; }
        InputFieldType get_type() const { return m_type; }
        Field get_default_value() const { return m_default_value; }

        bool has_lexicon_only() const { return m_text_only; }
        bool has_dual_field() const { return !m_dual_record_field_name.empty(); }
        bool has_valid_default() const { return m_default_value != f_invalid; }

    private:
        InputFieldType m_type;
        Lexicon* m_lex;
        std::string m_record_field_name;
        std::string m_dual_record_field_name;
        Field m_default_value;
    };

    class ElementControlOut : public Element
    {
    public:
        ElementControlOut() : Element() {}

        void clear() override;
        bool expand_specifier( Grammar* gmr );

        std::string get_field_output_name() const { return m_field_output_name; }

    private:
        std::string m_field_output_name;
    };

}

#endif // SRC_GLC_HICELEMENT_H_GUARD
