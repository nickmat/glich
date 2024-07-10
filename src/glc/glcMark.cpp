/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glcMark.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     ScriptMark class used by the scripts mark statement.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     8th February 2023
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

#include "glcMark.h"

#include "glcFile.h"
#include "glcFunction.h"
#include "glcObject.h"
#include "hicFormat.h"
#include "hicGrammar.h"
#include "hicLexicon.h"
#include "hicScheme.h"

#include <cassert>

using namespace glich;
using std::string;
using std::vector;


Mark::Mark( const string& name, Mark* prev )
    : m_name( name ), m_integer( Integer::number ), m_context( Context::glich ),
    m_ischeme( nullptr ), m_oscheme( nullptr )
{
    if( prev != nullptr ) {
        m_integer = prev->get_integer();
        m_context = prev->get_context();
        m_ischeme = prev->get_ischeme();
        m_oscheme = prev->get_oscheme();
    }
}

Mark::~Mark()
{
    for( auto& code : m_functions ) {
        glc().remove_function( code );
    }
    for( auto& code : m_commands ) {
        glc().remove_command( code );
    }
    for( auto object : m_objects ) {
        delete object;
    }
    for( auto file : m_files ) {
        delete file;
    }
    for( auto lexicon : m_lexicons ) {
        delete lexicon;
    }
    for( auto format : m_formats ) {
        Grammar* owner = format->get_owner();
        owner->remove_format( format->get_code() );
        delete format;
    }
    for( auto grammar : m_grammars ) {
        delete grammar;
    }
}

bool Mark::create_local( const string& name, Store* store )
{
    if( store->exists( name ) ) {
        return false;
    }
    store->add_local( name, SValue() );
    m_locals.push_back( name );
    return true;
}

void Mark::remove_variables()
{
    assert( s_zero_store != nullptr );
    for( auto& var : m_locals ) {
        s_zero_store->remove( var );
    }
}

string Mark::remove_next_object()
{
    string code;
    if( !m_objects.empty() ) {
        Object* obj = m_objects[m_objects.size() - 1];
        code = obj->get_code();
        delete obj;
        m_objects.pop_back();
    }
    return code;
}

string Mark::remove_next_file()
{
    string code;
    if( !m_files.empty() ) {
        File* file = m_files[m_files.size() - 1];
        code = file->get_code();
        delete file;
        m_files.pop_back();
    }
    return code;
}

string Mark::remove_next_lexicon()
{
    string code;
    if( !m_lexicons.empty() ) {
        Lexicon* lex = m_lexicons[m_lexicons.size() - 1];
        code = lex->get_code();
        delete lex;
        m_lexicons.pop_back();
    }
    return code;
}

string Mark::remove_next_grammar()
{
    string code;
    if( !m_grammars.empty() ) {
        Grammar* gmr = m_grammars[m_grammars.size() - 1];
        code = gmr->get_code();
        delete gmr;
        m_grammars.pop_back();
    }
    return code;
}

string Mark::remove_next_format()
{
    string code;
    if( !m_formats.empty() ) {
        Format* fmt = m_formats[m_formats.size() - 1];
        code = fmt->get_code();
        Grammar* gmr = fmt->get_owner();
        if( gmr ) {
            gmr->remove_format( code );
        }
        delete fmt;
        m_formats.pop_back();
    }
    return code;
}

GlcMark Mark::get_mark_data( const Glich* glc ) const
{
    GlcMark mark;
    mark.name = m_name;
    GlcData data;
    for( auto code : m_functions ) {
        data.name = code;
        data.value = string();
        mark.fun.push_back( data );
    }
    for( auto code : m_commands ) {
        data.name = code;
        data.value = string();
        mark.com.push_back( data );
    }
    for( auto file : m_files ) {
        data.name = file->get_code();
        data.value = string();
        mark.file.push_back( data );
    }
    for( auto object : m_objects ) {
        Scheme* sch = dynamic_cast<Scheme*>(object);
        if( sch == nullptr ) {
            data.name = object->get_code();
            data.value = string();
            mark.obj.push_back( data );
        }
        else {
            string code = sch->get_code();
            assert( code.substr( 0, 2 ) == "s:" );
            data.name = code.substr( 2 );
            data.value = sch->get_name();
            mark.sch.push_back( data );
        }
    }
    for( auto lex : m_lexicons ) {
        data.name = lex->get_code();
        data.value = lex->get_name();
        mark.lex.push_back( data );
    }
    for( auto gmr : m_grammars ) {
        data.name = gmr->get_code();
        data.value = gmr->get_name();
        mark.gmr.push_back( data );
    }
    for( auto fmt : m_formats ) {
        data.name = fmt->get_code();
        data.value = string();
        mark.fmt.push_back( data );
    }
    for( auto& local : m_locals ) {
        SValue value = glc->get_local( local );
        data.type = value.type_str();
        data.name = local;
        data.value = value.as_string();
        mark.var.push_back( data );
    }
    return mark;
}

// End of src/glc/glcMark.cpp
