/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicGrammar.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Grammar class to control date formatting.
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

#include "hicGrammar.h"

#include <glc/glc.h>
#include "hicBase.h"
#include "hicFormatIso.h"
#include "hicFormatText.h"
#include "hicFormatUnit.h"
#include "hicLexicon.h"

#include <cassert>

using namespace glich;
using std::string;


Grammar::Grammar( const string& code, Glich* glc )
    : m_code( code ), m_glc( glc ), m_ok( false ), m_inherit( nullptr )
{
}

Grammar::~Grammar()
{
}

bool Grammar::constuct( const Base* base )
{
    if( m_ok ) {
        return false; // Only run construct once.
    }
    if( m_inherit ) {
        if( m_base_fieldnames.empty() ) {
            m_base_fieldnames = m_inherit->m_base_fieldnames;
        }
        if( m_opt_fieldnames.empty() ) {
            m_opt_fieldnames = m_inherit->m_opt_fieldnames;
        }
        if( m_calc_fieldnames.empty() ) {
            m_calc_fieldnames = m_inherit->m_calc_fieldnames;
        }
        if( m_rank_fieldnames.empty() ) {
            m_rank_fieldnames = m_inherit->m_rank_fieldnames;
        }
    }
    create_def_format();
    create_u_format();
    if( m_pref_input_fcode.empty() ) {
        m_pref_input_fcode = "def";
    }
    if( m_pref_output_fcode.empty() ) {
        m_pref_output_fcode = "def";
    }
    m_ok = true;
    return true;
}

void Grammar::set_inherit( const string& gcode )
{
    if( m_glc ) {
        m_inherit = m_glc->get_grammar( gcode );
    }
}

void Grammar::set_preferred( const string& fcode )
{
    m_pref_output_fcode = fcode;
    m_pref_input_fcode = fcode;
}

FormatText* Grammar::create_format_text( const string& code )
{
    if( m_formats.count( code ) ) {
        // Already there
        return nullptr;
    }
    FormatText* fmt = new FormatText( code, *this );
    m_formats[code] = fmt;
    return fmt;
}

FormatIso* Grammar::create_format_iso( const string& code, const StdStrVec& rules )
{
    if( m_formats.count( code ) ) {
        // Already there
        return nullptr;
    }
    FormatIso* fmt = new FormatIso( code, *this, rules );
    m_formats[code] = fmt;
    return fmt;
}

FormatUnit* Grammar::create_format_unit( const string& code )
{
    if( m_formats.count( code ) ) {
        // Already there
        return nullptr;
    }
    FormatUnit* fmt = new FormatUnit( code, *this );
    m_formats[code] = fmt;
    return fmt;
}

bool Grammar::add_format( Format* fmt )
{
    assert( fmt != NULL );
    string code = fmt->get_code();
    if( m_formats.count( code ) ) {
        // Already there
        return false;
    }
    m_formats[code] = fmt;
#if 0
    // Set preferences to the first format by default.
    if( m_pref_input_fcode.empty() ) {
        m_pref_input_fcode = code;
    }
    if( m_pref_output_fcode.empty() ) {
        m_pref_output_fcode = code;
    }
#endif
    return true;
}

void Grammar::add_alias( const string& alias, const StdStrVec& pairs )
{
    assert( pairs.size() % 2 == 0 ); // pairs must be even.
    if( alias == "field" ) {
        for( size_t i = 0; i < pairs.size(); i += 2 ) {
            m_field_alias[pairs[i]] = pairs[i + 1];
        }
    }
    else if( alias == "pseudo" ) {
        for( size_t i = 0; i < pairs.size(); i += 2 ) {
            m_num_pseudo_alias[pairs[i + 1]] = pairs[i];
        }
    }
    else if( alias == "unit" ) {
        for( size_t i = 0; i < pairs.size(); i += 2 ) {
            m_unit_alias[pairs[i]] = pairs[i + 1];
            string name = pairs[i + 1];
            Unit unit = unit_null;
            if( name == "year" ) {
                unit = unit_year;
            }
            else if( name == "month" ) {
                unit = unit_month;
            }
            else if( name == "week" ) {
                unit = unit_week;
            }
            else if( name == "day" ) {
                unit = unit_day;
            }
            else {
                continue;
            }
            string key = make_key( pairs[i] );
            m_unit_type_alias[key] = unit;
        }
    }
}

std::string glich::Grammar::resolve_field_alias( const std::string& alias )
{
    if( m_field_alias.count( alias ) > 0 ) {
        return m_field_alias.find( alias )->second;
    }
    if( m_inherit ) {
        return m_inherit->resolve_field_alias( alias );
    }
    return alias;
}

std::string Grammar::resolve_unit_alias( const std::string& alias )
{
    if( m_unit_alias.count( alias ) > 0 ) {
        return m_unit_alias.find( alias )->second;
    }
    if( m_inherit ) {
        return m_inherit->resolve_unit_alias( alias );
    }
    return resolve_field_alias( alias );
}

string Grammar::get_num_pseudo_alias( const string& fname ) const
{
    if( m_num_pseudo_alias.count( fname ) > 0 ) {
        return m_num_pseudo_alias.find( fname )->second;
    }
    if( m_field_alias.count( fname ) > 0 ) {
        string fname_alias = m_field_alias.find( fname )->second;
        if( m_num_pseudo_alias.count( fname_alias ) > 0 ) {
            return m_num_pseudo_alias.find( fname_alias )->second;
        }
    }
    if( m_inherit ) {
        return m_inherit->get_num_pseudo_alias( fname );
    }
    return fname;
}

string Grammar::get_unit_alias( const string& fieldname ) const
{
    for( StdStrMap::const_iterator it = m_unit_alias.begin(); it != m_unit_alias.end(); it++ ) {
        if( it->second == fieldname ) {
            return it->first;
        }
    }
    if( m_inherit ) {
        return m_inherit->get_unit_alias( fieldname );
    }
    return fieldname;
}

Format* Grammar::get_format( const string& code ) const
{
    auto it = m_formats.find( code );
    if( it == m_formats.end() ) {
        if( m_inherit ) {
            return m_inherit->get_format( code );
        }
        return nullptr;
    }
    return it->second;
}

Field Grammar::find_token( Lexicon** lex, const std::string& word ) const
{
    Field field = f_invalid;
    for( size_t i = 0; i < m_lexicons.size(); i++ ) {
        field = m_lexicons[i]->find( word );
        if( field != f_invalid ) {
            if( lex ) {
                *lex = m_lexicons[i];
            }
            return field;
        }
    }
    if( m_inherit ) {
        field = m_inherit->find_token( lex, word );
        if( field != f_invalid ) {
            return field;
        }
    }
    if( lex ) {
        *lex = nullptr;
    }
    return f_invalid;
}

Lexicon* Grammar::find_lexicon( const string& code ) const
{
    if( code.empty() ) {
        return nullptr;
    }
    for( auto lexicon : m_lexicons ) {
        if( lexicon->get_code() == code ) {
            return lexicon;
        }
    }
    if( m_inherit ) {
        return m_inherit->find_lexicon( code );
    }
    return nullptr;
}

Grammar* Grammar::create_default_grammar( const Base* base, Glich* glc )
{
    Grammar* gmr = new Grammar( "", glc );
    gmr->set_base_fieldnames( base->get_fieldnames() );
    gmr->constuct( base );
    return gmr;
}

void Grammar::create_def_format()
{
    FormatText* fmt = create_format_text( "def" );
    if( fmt == nullptr ) {
        return;
    }
    string control;
    for( string fieldname : m_base_fieldnames ) {
        if( !control.empty() ) {
            control += "| ";
        }
        control += "{" + fieldname + "}";
    }
    fmt->set_control_in( control );
    fmt->set_control_out( control );
    fmt->set_style( FormatStyle::Hide );
    fmt->construct();
}

void Grammar::create_u_format()
{
    FormatUnit* fmt = create_format_unit( "u" );
    if( fmt == nullptr ) {
        return;
    }
    fmt->set_style( FormatStyle::Hide );
    fmt->construct();
}

// End of src/cal/calgrammar.cpp file
