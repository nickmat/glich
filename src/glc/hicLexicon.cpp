/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicLexicon.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Lexicon class to handle date element names.
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

#include "hicLexicon.h"

#include "hicHelper.h"


using namespace glich;
using std::string;


Lexicon::Lexicon( const std::string& code ) : m_code( code ), m_inherit( nullptr )
{
}

void Lexicon::set_inherit( const std::string& inherit )
{
    m_inherit = get_glc()->get_lexicon( inherit );
}

void Lexicon::set_pseudo_names( const StdStrVec & pseudos )
{
    if ( pseudos.size() > 0 ) {
        m_pseudo_name = pseudos[0];
    }
    if ( pseudos.size() > 1 ) {
        m_pseudo_a_name = pseudos[1];
    }
}

void Lexicon::add_token( Field value, const string& name, const string& abbrev )
{
    if( name.empty() ) {
        return;
    }
    Token token( value, name, abbrev );
    string key = make_key( name );
    m_words[key] = token;
    if( abbrev.size() && abbrev != name ) {
        key = make_key( abbrev );
        m_words[key] = token;
    }
    m_fields[value] = token;
}

string Lexicon::get_fieldname() const
{
    if( m_fieldname.empty() && m_inherit ) {
        m_inherit->get_fieldname();
    }
    return m_fieldname;
}

string Lexicon::get_lang() const
{
    if( m_lang.empty() && m_inherit ) {
        m_inherit->get_lang();
    }
    return m_lang;
}

string Lexicon::get_pseudo_name( Pseudo style ) const
{
    if( style == Lexicon::Pseudo::full ) {
        if( m_pseudo_name.empty() && m_inherit ) {
            m_inherit->get_pseudo_name( style );
        }
        return m_pseudo_name;
    }
    else if( style == Lexicon::Pseudo::abbrev ) {
        if( m_pseudo_a_name.empty() && m_inherit ) {
            m_inherit->get_pseudo_name( style );
        }
        return m_pseudo_a_name;
    }
    return string();
}

StdStrVec glich::Lexicon::get_token_words() const
{
    StdStrVec words;
    for( const auto& pair : m_fields ) {
        words.push_back( pair.second.get_word() );
    }
    return words;
}

StdStrVec glich::Lexicon::get_token_abbrev() const
{
    StdStrVec abbrevs;
    for( const auto& pair : m_fields ) {
        string abbrev = pair.second.get_abbrev();
        if( abbrev.empty() ) {
            abbrev = pair.second.get_word();
        }
        abbrevs.push_back( abbrev );
    }
    return abbrevs;
}

void Lexicon::get_info( Lexicon_info* info ) const
{
    if( info ) {
        info->name = get_name();
        info->lang = get_lang();
        info->style_full_name = get_pseudo_name( Pseudo::full );
        info->style_abbrev_name = get_pseudo_name( Pseudo::abbrev );
        if( m_inherit ) {
            info->words = m_inherit->get_token_words();
            info->abbrevs = m_inherit->get_token_abbrev();
        }
        vec_append( info->words, get_token_words() );
        vec_append( info->abbrevs, get_token_abbrev() );
    }
}

Field Lexicon::find( const string& word ) const
{
    string key = make_key( word );
    if( m_words.count( key ) > 0 ) {
        Token token = m_words.find( key )->second;
        return token.get_field();
    }
    return f_invalid;
}

string Lexicon::lookup( Field field, Pseudo style ) const
{
    string result;
    if( m_fields.count( field ) > 0 ) {
        Token token = m_fields.find( field )->second;
        if( style == Lexicon::Pseudo::abbrev ) {
            result = token.get_abbrev();
        }
        if( result.empty() ) {
            result = token.get_word();
        }
    }
    return result;
}

// End of src/glc/hicLexicon.cpp file
