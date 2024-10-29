/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/hic/hicGlich.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     HicGlich hic extension language class.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     22nd October 2024
 * Copyright:   Copyright (c) 2024, Nick Matthews.
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

#include <glc/hicGlich.h>

#include "glcValue.h"
#include "hicLexicon.h"
#include "hicScript.h"
#include "hicMark.h"

#include <cassert>
#include <iostream>
#include <sstream>

using namespace glich;
using std::string;
using std::vector;


HicGlich::~HicGlich()
{
    for( auto pair : m_lexicons ) {
        delete pair.second;
    }
    m_lexicons.clear();
}

SValue HicGlich::evaluate( const string& expression )
{
    std::istringstream iss( expression );
    std::ostringstream oss;
    HicScript scr( iss, oss );
    return scr.evaluate();
}

bool HicGlich::run( std::istream& in, std::ostream& out, int line )
{
    HicScript scr( in, out );
    scr.set_line( line );
    return scr.run();
}

bool glich::HicGlich::add_module_def( const ModuleDef& def, const std::string& code )
{
    if( def.m_definition == "lexicon" ) {
        for( auto& lex : def.m_codes ) {
            if( !add_lexicon( nullptr, lex ) ) {
                return false;
            }
            m_lexicon_mods[lex] = code;
        }
        return true;
    }
    return Glich::add_module_def( def, code );
}

bool HicGlich::get_lexicon_info( Lexicon_info* info, const string& code )
{
    Lexicon* lex = get_lexicon( code );
    if( lex == nullptr ) {
        return false;
    }
    lex->get_info( info );
    return true;
}

bool HicGlich::add_lexicon( Lexicon* lex, const string& code )
{
    DefinedStatus status = get_lexicon_status( code );
    if( status == DefinedStatus::defined ) {
        delete lex;
        return false;
    }
    if( status == DefinedStatus::none ) {
        HicMark* mark = dynamic_cast<HicMark*>(m_marks[m_marks.size() - 1]);
        assert( mark != nullptr );
        mark->add_lexicon( code );
    }
    m_lexicons[code] = lex;
    return true;
}

void HicGlich::remove_lexicon( const string& code )
{
    if( m_lexicons.count( code ) == 0 ) {
        return;
    }
    delete m_lexicons.find( code )->second;
    m_lexicons.erase( code );
}

Lexicon* HicGlich::get_lexicon( const string& code )
{
    if( m_lexicons.count( code ) > 0 ) {
        Lexicon* lex = m_lexicons.find( code )->second;
        if( lex == nullptr && m_lexicon_mods.count( code ) == 1 ) {
            string mod = m_lexicon_mods.find( code )->second;
            string mess = run_module( mod );
            lex = m_lexicons.find( code )->second;
        }
        return lex;
    }
    return nullptr;
}

DefinedStatus HicGlich::get_lexicon_status( const string& code ) const
{
    if( m_lexicons.count( code ) == 0 ) {
        return DefinedStatus::none;
    }
    Lexicon* lex = m_lexicons.find( code )->second;
    return lex ? DefinedStatus::defined : DefinedStatus::module;
}



// End of src/hic/hicGlich.cpp
