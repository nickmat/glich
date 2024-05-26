/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicElement.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Element classes for formatting and parsing date elements.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     19th March 2023
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

#include "hicElement.h"

#include <glc/glc.h>
#include "glcHelper.h"
#include "hicGrammar.h"
#include "hicLexicon.h"
#include "hicRomanNum.h"

using namespace glich;
using std::string;


void Element::clear()
{
    m_state = State::do_field;
    m_field_name.clear();
    m_dual_field_name.clear();
    m_default_text.clear();
    m_lcode.clear();
    m_spec.clear();
    m_qualifier.clear();
    m_text_only = false;
}

void Element::add_char( char ch )
{
    switch ( m_state )
    {
    case State::do_field:
        switch ( ch )
        {
        case '/': m_state = State::do_dual; return;
        case ':': m_state = State::do_lcode; return;
        }
        m_field_name += ch;
        return;
    case State::do_dual:
        m_dual_field_name += ch;
        return;
    case State::do_lcode:
        switch ( ch )
        {
        case ':': m_state = State::do_spec; return;
        case '=': m_state = State::do_default; return;
        case '*': m_text_only = true; return;
        }
        m_lcode += ch;
        return;
    case State::do_spec:
        switch ( ch )
        {
        case ':': m_state = State::do_qual; return;
        case '=': m_state = State::do_default; return;
        }
        m_spec += ch;
        return;
    case State::do_qual:
        if ( ch == '=' ) {
            m_state = State::do_default;
            return;
        }
        m_qualifier += ch;
        return;
    case State::do_default:
        m_default_text += ch;
        return;
    }
}

void Element::add_string( const string & str )
{
    for ( auto ch : str ) {
        add_char( ch );
    }
}

string Element::get_formatted_element( const Glich& glc, Field field ) const
{
    if( field == f_invalid ) {
        return string();
    }
    if ( m_lcode.empty() && m_spec.empty() ) {
        return field_to_string( field, Context::hics );
    }

    string result;
    StrStyle ss = StrStyle::undefined;
    if ( m_qualifier == "u" ) {
        ss = StrStyle::uppercase;
    } else if ( m_qualifier == "l" ) {
        ss = StrStyle::lowercase;
    }
    if ( m_lcode.empty() ) {
        if ( m_spec == "oa" ) {
            result = get_ordinal_suffix( field, ss );
        } else if ( m_spec == "os" ) {
            result = field_to_string( field, Context::hics ) + get_ordinal_suffix( field, ss );
        } else if ( m_spec == "rn" ) {
            result = get_roman_numerals( field, ss );
        } else if ( m_spec == "lp" ) {
            result = get_left_padded( field, m_qualifier );
        }
    } else {
        Lexicon* lex = glc.get_lexicon( m_lcode );
        if ( lex ) {
            Lexicon::Pseudo abbrev = (m_spec == "a") ?
                Lexicon::Pseudo::abbrev : Lexicon::Pseudo::full;
            result = lex->lookup( field, abbrev );
        }
        if( ss == StrStyle::uppercase ) {
            ascii_toupper( result );
        }
        else if( ss == StrStyle::lowercase ) {
            ascii_tolower( result );
        }
    }
    return result;
}

Field Element::get_converted_field( Glich* glc, const string& str ) const
{
    if ( m_lcode.empty() && m_spec.empty() ) {
        return str_to_field( str );
    }

    if ( m_lcode.empty() ) {
        if ( m_spec == "rn" ) {
            return convert_roman_numerals( str );
        }
        if ( m_spec == "os" ) {
            return str_to_field( str );
        }
        // Currently, can't use any other format.
        return f_invalid;
    }
    Lexicon* lex = glc->get_lexicon( m_lcode );
    if ( lex ) {
        return lex->find( str );
    }
    return f_invalid;
}


void ElementControlIn::clear()
{
    Element::clear();
    m_type = IFT_number;
    m_lex = nullptr;
    m_record_field_name.clear();
    m_dual_record_field_name.clear();
    m_default_value = f_invalid;
}

bool ElementControlIn::expand_specifier( Grammar* gmr )
{
    if( !m_dual_field_name.empty() ) {
        m_type = IFT_dual2;
    }
    m_record_field_name = gmr->resolve_field_alias( m_field_name );
    if( m_lex ) {
        m_type = IFT_lexicon;
        m_default_value = m_lex->find( m_default_text );
    }
    else if( !m_dual_field_name.empty() ) {
        m_dual_record_field_name = gmr->resolve_field_alias( m_dual_field_name );
    }
    return true;
}


std::string ElementControlIn::get_input_text() const
{
    if ( m_default_text.empty() ) {
        return m_field_name;
    }
    return "[" + m_default_text + "]";
}

void ElementControlOut::clear()
{
    Element::clear();
    m_field_output_name.clear();
}

bool ElementControlOut::expand_specifier( Grammar* gmr )
{
    Lexicon* lex = gmr->find_lexicon( m_lcode );
    if( lex ) {
        if( m_field_output_name.empty() ) {
            if( m_spec == "a" ) {
                m_field_output_name = lex->get_pseudo_name( Lexicon::Pseudo::abbrev );
            }
            else {
                m_field_output_name = lex->get_pseudo_name( Lexicon::Pseudo::full );
            }
        }
    }
    else if( m_lcode.empty() && !m_spec.empty() ) {
        StrStyle ss = StrStyle::undefined;
        if( m_qualifier == "u" ) {
            ss = StrStyle::uppercase;
        }
        else if( m_qualifier == "l" ) {
            ss = StrStyle::lowercase;
        }
        if( m_spec == "oa" ) {
            m_field_output_name = get_ordinal_suffix_style( ss );
        }
        else if( m_spec == "os" ) {
            m_field_output_name = gmr->get_num_pseudo_alias( m_field_name ) +
                get_ordinal_suffix_style( ss );
        }
        else if( m_spec == "rn" ) {
            m_field_output_name = gmr->get_num_pseudo_alias( m_field_name ) +
                get_roman_numerals_style( ss );
        }
        else if( m_spec == "lp" ) {
            m_field_output_name = get_left_pad_style( m_field_output_name, m_qualifier );
        }
    }
    else if( !m_dual_field_name.empty() ) {
        m_field_output_name = gmr->get_num_pseudo_alias( m_field_name ) +
            "/" + gmr->get_num_pseudo_alias( m_dual_field_name );
    }
    else if( m_field_output_name.empty() ) {
        m_field_output_name = gmr->get_num_pseudo_alias( m_field_name );
    }
    return true;
}


// End of src/cal/calelement.cpp file
