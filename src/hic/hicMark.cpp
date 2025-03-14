/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicMark.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     HicMark class used by the hics extension.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     16th October 2024
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

#include "hicMark.h"

#include "hicFormat.h"
#include "hicGrammar.h"
#include "hicLexicon.h"
#include "hicScheme.h"

#include <cassert>

using namespace glich;
using std::string;
using std::vector;


HicMark::HicMark( const string& name, HicMark* prev )
    : m_ischeme( nullptr ), m_oscheme( nullptr ), m_context( Context::glich )
    , Mark( name, prev )
{
    if( prev != nullptr ) {
        m_context = prev->get_context();
        m_ischeme = prev->get_ischeme();
        m_oscheme = prev->get_oscheme();
    }
}

HicMark::~HicMark()
{
    for( auto code : m_lexicons ) {
        hic().remove_lexicon( code );
    }
    for( auto code : m_formats ) {
        string gcode, fcode;
        split_code( &gcode, &fcode, code );
        Grammar* gmr = hic().get_grammar( gcode );
        assert( gmr != nullptr );
        gmr->remove_format( fcode );
    }
    for( auto code : m_grammars ) {
        hic().remove_grammar( code );
    }
    for( auto code : m_schemes ) {
        glc().remove_object( "s:" + code );
    }

}

void glich::HicMark::get_mark_hic_data( HicMarkData& mark ) const
{
    get_mark_glc_data( mark.glc );
    GlcData data;
    for( auto code : m_lexicons ) {
        data.name = code;
        Lexicon* lex = hic().get_lexicon( code );
        data.value = lex->get_name();
        mark.lex.push_back( data );
    }
    for( auto code : m_grammars ) {
        data.name = code;
        Grammar* gmr = hic().get_grammar( code );
        data.value = gmr->get_name();
        mark.gmr.push_back( data );
    }
    for( auto code : m_formats ) {
        data.name = code;
        data.value = string();
        mark.fmt.push_back( data );
    }
    for( auto code : m_schemes ) {
        data.name = code;
        Scheme* sch = hic().get_scheme( code );
        data.value = sch->get_name();
        mark.sch.push_back( data );
    }
}


// End of src/glc/hicMark.cpp
