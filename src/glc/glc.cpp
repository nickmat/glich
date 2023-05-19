/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glc.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Glich language class
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     8th February 2023
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

#include <glc/glc.h>

#include "glcFile.h"
#include "glcFunction.h"
#include "glcMark.h"
#include "glcObject.h"
#include "glcScript.h"
#include "glcValue.h"
#include "hicLibScripts.h"
#include "hicScheme.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace glich;
using std::string;
using std::vector;


string InOut::get_input( const string& prompt ) {
    if( !prompt.empty() ) {
        std::cout << prompt;
    }
    string result;
    std::getline( std::cin, result );
    return result;
}

Glich::Glich( InitLibrary lib, InOut* inout )
    : m_store( new Store ), m_inout( inout )
{
    Mark::set_zero_store( m_store );
    m_marks.push_back( new Mark( "", nullptr ) );
    if( !m_inout ) {
        m_inout = new InOut;
    }
    STokenStream::init( this );
    SValue::init( this );

    switch( lib )
    {
    case InitLibrary::None:
        break;
    case InitLibrary::Hics:
        load_hics_library();
        break;
    }
}

Glich::~Glich()
{
    for( size_t i = m_marks.size(); i > 0; --i ) {
        delete m_marks[i - 1];
    }
    while( pop_store() );
    delete m_store;
    delete m_inout;
}

void Glich::load_hics_library()
{
    for( size_t i = 0; i < hics_default_scripts_size; i++ ) {
        string error = run_script( hics_default_scripts[i].script );
        if( !error.empty() ) {
            m_init_error += "Module: \"" +
                string( hics_default_scripts[i].module ) + "\"\n" + error;
            break;
        }
    }
}

string Glich::run_script( const string& script )
{
    std::istringstream iss( script );
    std::ostringstream oss;
    Script scr( this, iss, oss );
    scr.run();
    return oss.str();
}

bool Glich::create_local( const string& name )
{
    if( is_level_zero() ) {
        assert( m_marks.size() > 0 );
        return m_marks[m_marks.size() - 1]->create_local( name, m_store );
    }
    return m_store->create_local( name );
}

bool Glich::update_local( const string& name, SValue& value )
{
    return m_store->update_local( name, value );
}

SValue Glich::get_local( const string& name ) const
{
    return m_store->get_local( name );
}

SValue* Glich::get_local_ptr( const string& name )
{
    return m_store->get_local_ptr( name );
}

bool Glich::is_local( const string& name ) const
{
    return m_store->exists( name );
}

bool Glich::add_function( Function* fun )
{
    assert( m_marks.size() > 0 );
    m_marks[m_marks.size() - 1]->add_function( fun );
    m_functions[fun->get_code()] = fun;
    return true;
}

Function* Glich::get_function( const string& code ) const
{
    if( m_functions.count( code ) > 0 ) {
        return m_functions.find( code )->second;
    }
    return nullptr;
}

Command* Glich::create_command( const string& code )
{
    Command* com = new Command( code );
    assert( m_marks.size() > 0 );
    m_marks[m_marks.size() - 1]->add_command( com );
    m_commands[code] = com;
    return com;
}

Command* Glich::get_command( const string& code ) const
{
    if( m_commands.count( code ) > 0 ) {
        return m_commands.find( code )->second;
    }
    return nullptr;
}

Object* Glich::create_object( const string& code )
{
    Object* obj = new Object( code );
    add_object( obj, code );
    return obj;
}

bool Glich::add_object( Object* obj, const string& code )
{
    if( obj == nullptr || m_lexicons.count( code ) ) {
        delete obj;
        return false;
    }
    m_marks[m_marks.size() - 1]->add_object( obj );
    m_objects[code] = obj;
    return true;
}

Object* Glich::get_object( const string& code ) const
{
    if( m_objects.count( code ) > 0 ) {
        return m_objects.find( code )->second;
    }
    return nullptr;
}

File* Glich::create_file( const string& code )
{
    File* file = new File( code );
    assert( m_marks.size() > 0 );
    m_marks[m_marks.size() - 1]->add_file( file );
    m_files[code] = file;
    return file;
}

File* Glich::get_file( const string& code ) const
{
    if( m_files.count( code ) > 0 ) {
        return m_files.find( code )->second;
    }
    return nullptr;
}

bool Glich::add_lexicon( Lexicon* lex, const string& code )
{
    // Only add lexicons and that are not already there.
    if( lex == nullptr || m_lexicons.count( code ) ) {
        delete lex;
        return false;
    }
    assert( m_marks.size() > 0 );
    m_marks[m_marks.size() - 1]->add_lexicon( lex );
    m_lexicons[code] = lex;
    return true;
}

Lexicon* Glich::get_lexicon( const string& code ) const
{
    if( m_lexicons.count( code ) > 0 ) {
        return m_lexicons.find( code )->second;
    }
    return nullptr;
}

bool Glich::add_grammar( Grammar* gmr, const string& code )
{
    // Only add lexicons and that are not already there.
    if( gmr == nullptr || m_grammars.count( code ) ) {
        delete gmr;
        return false;
    }
    assert( m_marks.size() > 0 );
    m_marks[m_marks.size() - 1]->add_grammar( gmr );
    m_grammars[code] = gmr;
    return true;
}

Grammar* Glich::get_grammar( const string& code ) const
{
    if( m_grammars.count( code ) > 0 ) {
        return m_grammars.find( code )->second;
    }
    return nullptr;
}

bool Glich::add_format( Format* fmt, const string& code )
{
    assert( m_marks.size() > 0 );
    m_marks[m_marks.size() - 1]->add_format( fmt );
    return true;
}

void Glich::add_or_replace_mark( const string& name )
{
    clear_mark( name );
    Mark* prev = nullptr;
    int i = int( m_marks.size() ) - 1;
    if( i >= 0 ) {
        prev = m_marks[i];
    }
    Mark* mark = new Mark( name, prev );
    m_marks.push_back( mark );
}

bool Glich::clear_mark( const string& name )
{
    assert( !m_marks.empty() );
    assert( !name.empty() );
    size_t end, pos;
    end = pos = m_marks.size() - 1;
    while( pos != 0 && name != m_marks[pos]->get_name() ) {
        --pos;
    }
    if( pos == 0 ) {
        return false; // Can't find mark name.
    }
    for( size_t i = end; i >= pos; --i ) {
        m_marks[i]->remove_variables();
        string code;
        for( ;;) {
            code = m_marks[i]->remove_next_function();
            if( code.empty() ) {
                break;
            }
            m_functions.erase( code );
        }
        for( ;;) {
            code = m_marks[i]->remove_next_command();
            if( code.empty() ) {
                break;
            }
            m_commands.erase( code );
        }
        for( ;;) {
            code = m_marks[i]->remove_next_object();
            if( code.empty() ) {
                break;
            }
            m_objects.erase( code );
        }
        for( ;;) {
            code = m_marks[i]->remove_next_file();
            if( code.empty() ) {
                break;
            }
            m_files.erase( code );
        }
        for( ;;) {
            code = m_marks[i]->remove_next_lexicon();
            if( code.empty() ) {
                break;
            }
            m_lexicons.erase( code );
        }
        for( ;;) {
            code = m_marks[i]->remove_next_grammar();
            if( code.empty() ) {
                break;
            }
            m_grammars.erase( code );
        }
        for( ;;) {
            code = m_marks[i]->remove_next_format();
            if( code.empty() ) {
                break;
            }
        }
        delete m_marks[i];
        m_marks.pop_back();
    }
    return true;
}

void Glich::push_store()
{
    m_store = new Store( m_store );
}

bool Glich::pop_store()
{
    Store* store = m_store->get_prev();
    if( store ) {
        delete m_store;
        m_store = store;
        return true;
    }
    return false;
}

bool Glich::is_level_zero() const
{
    return m_store->is_level_zero();
}

string Glich::read_input( const string& prompt ) const
{
    return m_inout->get_input( prompt );
}

void Glich::set_context( Context ct )
{
    int i = int( m_marks.size() ) - 1;
    if( i >= 0 ) {
        m_marks[i]->set_context( ct );
    }
}

void Glich::set_ischeme( Scheme* sch )
{
    int i = int( m_marks.size() ) - 1;
    if( i >= 0 ) {
        m_marks[i]->set_ischeme( sch );
    }
}

void Glich::set_oscheme( Scheme* sch )
{
    int i = int( m_marks.size() ) - 1;
    if( i >= 0 ) {
        m_marks[i]->set_oscheme( sch );
    }
}

Context Glich::get_context() const
{
    int i = int( m_marks.size() ) - 1;
    if( i >= 0 ) {
        return m_marks[i]->get_context();
    }
    return Context::glich;
}

Scheme* Glich::get_ischeme() const
{
    int i = int( m_marks.size() ) - 1;
    if( i >= 0 ) {
        return m_marks[i]->get_ischeme();
    }
    return nullptr;
}

Scheme* Glich::get_oscheme() const
{
    int i = int( m_marks.size() ) - 1;
    if( i >= 0 ) {
        return m_marks[i]->get_oscheme();
    }
    return nullptr;
}

// End of src/hg/historygen.cpp
