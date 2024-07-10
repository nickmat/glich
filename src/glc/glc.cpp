/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glc.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Glich language class
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

#include <glc/glc.h>

#include "glcFile.h"
#include "glcFunction.h"
#include "glcHelper.h"
#include "glcLibScripts.h"
#include "glcMark.h"
#include "glcMath.h"
#include "glcObject.h"
#include "glcScript.h"
#include "glcValue.h"
#include "glcVersion.h"
#include "hicDatePhrase.h"
#include "hicGrammar.h"
#include "hicLexicon.h"
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

Glich::Glich( InOut* inout )
    : m_store( new Store ), m_inout( inout ), m_cur_object( nullptr )
{
    Mark::set_zero_store( m_store );
    m_marks.push_back( new Mark( "", nullptr ) );
    if( !m_inout ) {
        m_inout = new InOut;
    }

    // Add dummy built-in functions to avoid redefinition.
    SpFunction fptr = std::shared_ptr<Function>( new Function( string() ) );
    m_functions = {
        { "boolean", fptr },
        { "error", fptr },
        { "field", fptr },
        { "float", fptr },
        { "if", fptr },
        { "number", fptr },
        { "range", fptr },
        { "read", fptr },
        { "rlist", fptr },
        { "string", fptr },
        { "quote", fptr },
        { "filesys", fptr },
        { "version", fptr },
        { "low", fptr },
        { "high", fptr },
        { "span", fptr },
        { "size", fptr },
        { "envelope", fptr },
        { "type", fptr },
        { "object", fptr },
        // Hics functions
        { "date", fptr },
        { "element", fptr },
        { "first", fptr },
        { "last", fptr },
        { "leapyear", fptr },
        { "phrase", fptr },
        { "pseudo:in", fptr },
        { "pseudo:out", fptr },
        { "record", fptr },
        { "scheme", fptr },
        { "text", fptr }
    };
    
    m_constants = {
        { "true", true },
        { "false", false },
        { "infinity", SValue( f_maximum, SValue::Type::field ) },
        { "past", SValue( f_minimum, SValue::Type::field ) },
        { "future", SValue( f_maximum, SValue::Type::field ) },
        { "inf", std::numeric_limits<double>::infinity() },
        { "nan", std::numeric_limits<double>::quiet_NaN() },
        { "null", SValue() },
        { "empty", SValue( RList() ) },
        { "nl", "\n" },
        { "pi", cal_pi }
    };
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

const char* Glich::version()
{
    return glc_version;
}

SchemeList Glich::get_scheme_list( SchemeStyle style ) const
{
    SchemeList slist;
    SchemeData sdata;

    for( const auto& object : m_objects ) {
        Scheme* sch = dynamic_cast<Scheme*>(object.second);
        if( sch == nullptr ) {
            continue;
        }
        if( style == SchemeStyle::Selected && sch->get_style() != SchemeStyle::Selected ) {
            continue;
        }
        if( style == SchemeStyle::Default && sch->get_style() == SchemeStyle::Hidden ) {
            continue;
        }
        sdata.code = sch->get_scode();
        sdata.name = sch->get_name();
        slist.push_back( sdata );
    }
    return slist;
}

void Glich::get_scheme_info( Scheme_info* info, const string& scode ) const
{
    Scheme* sch = get_scheme( scode );
    if( sch != nullptr ) {
        sch->get_info( info );
    }
}

void Glich::get_input_info( SchemeFormatInfo* info, const string& scode ) const
{
    Scheme* sch = get_scheme( scode );
    if( sch != nullptr ) {
        string incode = sch->get_input_format_code();
        const Grammar* gmr = sch->get_grammar();
        gmr->get_input_formats( info, incode );
    }
}

void Glich::get_output_info( SchemeFormatInfo* info, const string& scode ) const
{
    Scheme* sch = get_scheme( scode );
    if( sch != nullptr ) {
        string outcode = sch->get_output_format_code();
        const Grammar* gmr = sch->get_grammar();
        gmr->get_output_formats( info, outcode );
    }
}

void Glich::get_format_text_info( FormatText_info* info, const string& scode, const string& fcode ) const
{
    Scheme* sch = get_scheme( scode );
    if( sch != nullptr ) {
        sch->get_format_text_info( info, fcode );
    }
}

bool Glich::get_lexicon_info( Lexicon_info* info, const string& code ) const
{
    Lexicon* lex = get_lexicon( code );
    if( lex == nullptr ) {
        return false;
    }
    lex->get_info( info );
    return true;
}

RList Glich::date_phrase_to_rlist( const string& phrase, const string& sig )
{
    string scode, fcode;
    Scheme* sch = nullptr;
    Scheme* prev_sch = nullptr;
    string prev_fcode;
    if( !sig.empty() ) {
        split_code( &scode, &fcode, sig );
        prev_sch = get_ischeme();
        if( !scode.empty() ) {
            sch = get_scheme( scode );
            set_ischeme( sch );
        }
        if( sch == nullptr ) {
            sch = get_ischeme();
            if( sch == nullptr ) {
                return RList();
            }
        }
        if( !fcode.empty() ) {
            prev_fcode = sch->get_input_format_code();
            if( prev_fcode == fcode ) { // No change.
                prev_fcode.clear();
            }
            else {
                sch->set_input_format( fcode );
            }
        }
    }

    string script = parse_date_phrase( phrase );
    SValue value = evaluate( script );
    bool success = false;
    RList rlist = value.get_rlist( success );

    if( !prev_fcode.empty() ) {
        sch->set_input_format( prev_fcode );
    }
    if( prev_sch != nullptr ) {
        set_ischeme( prev_sch );
    }
    return rlist;
}

string Glich::date_phrase_to_text( const string& phrase, const string& sig_in, const string& sig_out )
{
    RList rlist = date_phrase_to_rlist( phrase, sig_in );
    return rlist_to_text( rlist, sig_out );
}

string Glich::rlist_to_text( RList rlist, const string& sig )
{
    string scode, fcode;
    Scheme* sch = nullptr;
    if( !sig.empty() ) {
        split_code( &scode, &fcode, sig );
        if( !scode.empty() ) {
            sch = get_scheme( scode );
        }
    }
    if( sch == nullptr ) {
        sch = get_oscheme();
        if( sch == nullptr ) {
            return string();
        }
    }
    return sch->rlist_to_str( rlist, fcode );
}

string Glich::range_to_text( Range range, const string& sig )
{
    string scode, fcode;
    Scheme* sch = nullptr;
    if( !sig.empty() ) {
        split_code( &scode, &fcode, sig );
        if( !scode.empty() ) {
            sch = get_scheme( scode );
        }
    }
    if( sch == nullptr ) {
        sch = get_oscheme();
        if( sch == nullptr ) {
            return string();
        }
    }
    return sch->range_to_str( range, fcode );
}

string Glich::field_to_text( Field field, const string& sig )
{
    string qual;
    if( !sig.empty() ) {
        qual = ".\"" + sig + "\"";
    }
    string function = "@text" + qual + "(" + field_to_string( field ) + ")";
    SValue value = evaluate( function );
    bool success;
    return value.get_str( success );
}

RList Glich::text_to_rlist( const string& text, const string& sig )
{
    string qual;
    if( !sig.empty() ) {
        qual = ".\"" + sig + "\"";
    }
    string function = "@date" + qual + "(\"" + text + "\")";
    SValue value = evaluate( function );
    bool success;
    return value.get_rlist( success );
}

Range Glich::text_to_range( const string& text, const string& sig )
{
    string qual;
    if( !sig.empty() ) {
        qual = ".\"" + sig + "\"";
    }
    string function = "@date" + qual + "(\"" + text + "\")";
    SValue value = evaluate( function );
    bool success;
    return value.get_range( success );
}

Field Glich::text_to_field( const string& text, const string& sig )
{
    string qual;
    if( !sig.empty() ) {
        qual = ".\"" + sig + "\"";
    }
    string function = "@date" + qual + "(\"" + text + "\")";
    SValue value = evaluate( function );
    bool success;
    return value.get_field( success );
}

GlcMarkVec Glich::get_glc_data() const
{
    GlcMarkVec glcmarks;
    for( auto& mark : m_marks ) {
        GlcMark glcmark = mark->get_mark_data( this );
        glcmarks.push_back( glcmark );
    }
    return glcmarks;
}

void Glich::load_builtin_library()
{
    for( size_t i = 0; i < glc_builtin_scripts_size; i++ ) {
        string error = run_script( glc_builtin_scripts[i].script );
        if( !error.empty() ) {
            m_init_error += "Module: \"" +
                string( glc_builtin_scripts[i].module ) + "\"\n" + error;
            break;
        }
    }
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

std::string glich::Glich::run_script_file( const std::string& filename )
{
    std::ifstream ifs( filename.c_str() );
    std::ostringstream oss;
    Script scr( this, ifs, oss );
    scr.run();
    return oss.str();
}

SValue Glich::evaluate( const string& expression )
{
    std::istringstream iss( expression );
    std::ostringstream oss;
    Script scr( this, iss, oss );
    return scr.evaluate();
}

bool Glich::is_named( const string& name ) const
{
    if( m_constants.count( name ) == 1 ) {
        return true;
    }
    return m_store->exists( name );
}

SValue Glich::get_named( const string& name ) const
{
    if( m_constants.count( name ) == 1 ) {
        return m_constants.find( name )->second;
    }
    return m_store->get_local( name );
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

bool Glich::is_constant( const string& name ) const
{
    return m_constants.count( name ) == 1;
}

bool Glich::add_function( SpFunction fun )
{
    assert( m_marks.size() > 0 );
    const string code = fun->get_code();
    m_marks[m_marks.size() - 1]->add_function( code );
    m_functions[code] = fun;
    return true;
}

void Glich::remove_function( const string& code )
{
    m_functions.erase( code );
}

Function* Glich::get_function( const string& code ) const
{
    if( m_functions.count( code ) > 0 ) {
        return m_functions.find( code )->second.get();
    }
    return nullptr;
}

bool Glich::add_command( SpFunction com )
{
    assert( m_marks.size() > 0 );
    const string code = com->get_code();
    m_marks[m_marks.size() - 1]->add_command( code );
    m_commands[code] = com;
    return true;
}

void Glich::remove_command( const string& code )
{
    m_commands.erase( code );
}

Function* Glich::get_command( const string& code ) const
{
    if( m_commands.count( code ) > 0 ) {
        return m_commands.find( code )->second.get();
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
    File* file = get_file( code );
    if( file != nullptr ) {
        // Already exists.
        return nullptr;
    }
    file = new File( code );

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

bool Glich::add_format( Format* fmt )
{
    assert( m_marks.size() > 0 );
    m_marks[m_marks.size() - 1]->add_format( fmt );
    return true;
}

bool Glich::add_scheme( Scheme* sch, const string& scode )
{
    return add_object( sch, "s:" + scode );
}

Scheme* Glich::get_scheme( const string& scode ) const
{
    return dynamic_cast<Scheme*>(get_object( "s:" + scode ));
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
        m_marks[i]->clear();
        m_marks[i]->remove_variables();
        string code;
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

bool Glich::set_property( const string& property, const string& value )
{
    int i = int( m_marks.size() ) - 1;
    if( i < 0 ) {
        return false;
    }
    if( property == "integer" ) {
        Integer intgr;
        if( value == "number" ) {
            intgr = Integer::number;
        }
        else if( value == "field" ) {
            intgr = Integer::field;
        }
        else {
            return false;
        }
        m_marks[i]->set_integer( intgr );
        return true;
    }
    if( property == "context" ) {
        Context ct;
        if( value == "glich" ) {
            ct = Context::glich;
        }
        else if( value == "hics" ) {
            ct = Context::hics;
        }
        else {
            return false;
        }
        m_marks[i]->set_context( ct );
        return true;
    }
    string scode, fcode;
    split_code( &scode, &fcode, value );
    Scheme* sch = get_scheme( scode );
    if( sch != nullptr ) {
        if( property == "input" ) {
            m_marks[i]->set_ischeme( sch );
            sch->set_input_format( fcode );
            return true;
        }
        if( property == "output" ) {
            m_marks[i]->set_oscheme( sch );
            sch->set_output_format( fcode );
            return true;
        }
        if( property == "inout" ) {
            m_marks[i]->set_ischeme( sch );
            sch->set_input_format( fcode );
            m_marks[i]->set_oscheme( sch );
            sch->set_output_format( fcode );
            return true;
        }
    }
    return false;
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

Integer Glich::get_integer() const
{
    int i = int( m_marks.size() ) - 1;
    if( i >= 0 ) {
        return m_marks[i]->get_integer();
    }
    return Integer::number;
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

const Object* Glich::set_cur_object( const Object* obj )
{
    const Object* prev_obj = m_cur_object;
    m_cur_object = obj;
    return prev_obj;
}

static Glich* s_glc = nullptr;

#if !defined(NDEBUG) || defined(_DEBUG)
Glich* g_glc_test = nullptr; // To allow Visual Studio watch to see it
#endif

void glich::init_glc( InitLibrary lib, InOut* inout )
{
    if( s_glc ) {
        delete s_glc;
    }
    s_glc = new Glich( inout );
    s_glc->load_builtin_library();
    switch( lib )
    {
    case InitLibrary::None:
        break;
    case InitLibrary::Hics:
        s_glc->load_hics_library();
        break;
    }
    // Mark the start of user definitions.
    s_glc->run_script( "mark __user;" );

#if !defined(NDEBUG) || defined(_DEBUG)
    g_glc_test = s_glc;
#endif
}

void glich::exit_glc()
{
    delete s_glc;
}

Glich* glich::get_glc()
{
    assert( s_glc != nullptr );
    return s_glc;
}

Glich& glich::glc()
{
    assert( s_glc != nullptr );
    return *s_glc;
}

// End of src/hg/historygen.cpp
