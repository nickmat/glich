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
    m_marks.push_back( new HicMark( "", nullptr ) );
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
        { "global", fptr },
        // Hics functions
        { "date", fptr },
        { "easter", fptr },
        { "element", fptr },
        { "first", fptr },
        { "fmt:object", fptr },
        { "last", fptr },
        { "leapyear", fptr },
        { "phrase", fptr },
        { "record", fptr },
        { "sch:object", fptr },
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

SchemeList Glich::get_scheme_list( Visibility vis )
{
    SchemeList slist;
    SchemeData sdata;

    for( const auto& object : m_objects ) {
        string prefix, code;
        split_string( prefix, code, object.first );
        if( prefix != "s" ) {
            continue;
        }
        Scheme* sch = get_scheme( code );
        if( sch == nullptr ) {
            continue;
        }
        if( vis == Visibility::visible && !sch->get_cur_visible() ) {
            continue;
        }
        if( vis == Visibility::hidden && sch->get_cur_visible() ) {
            continue;
        }
        sdata.code = code;
        sdata.name = sch->get_name();
        slist.push_back( sdata );
    }
    return slist;
}

void Glich::get_scheme_info( Scheme_info* info, const string& scode )
{
    Scheme* sch = get_scheme( scode );
    if( sch != nullptr ) {
        sch->get_info( info );
    }
}

void Glich::get_input_info( SchemeFormatInfo* info, const string& scode )
{
    Scheme* sch = get_scheme( scode );
    if( sch != nullptr ) {
        string incode = sch->get_input_format_code();
        const Grammar* gmr = sch->get_grammar();
        gmr->get_input_formats( info, incode );
    }
}

void Glich::get_output_info( SchemeFormatInfo* info, const string& scode )
{
    Scheme* sch = get_scheme( scode );
    if( sch != nullptr ) {
        string outcode = sch->get_output_format_code();
        const Grammar* gmr = sch->get_grammar();
        gmr->get_output_formats( info, outcode );
    }
}

void Glich::get_format_text_info( FormatText_info* info, const string& scode, const string& fcode )
{
    Scheme* sch = get_scheme( scode );
    if( sch != nullptr ) {
        sch->get_format_text_info( info, fcode );
    }
}

bool Glich::get_lexicon_info( Lexicon_info* info, const string& code )
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
        GlcMark glcmark = mark->get_mark_data();
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
    for( size_t i = 0; i < hics_builtin_scripts_size; i++ ) {
        string error = run_script( hics_builtin_scripts[i].script );
        if( !error.empty() ) {
            m_init_error += "Module: \"" +
                string( hics_builtin_scripts[i].module ) + "\"\n" + error;
            break;
        }
    }
}

void Glich::load_hics_library()
{
    run_module( "hics:hic_lib" );
}

string Glich::run_script( const string& script )
{
    std::istringstream iss( script );
    std::ostringstream oss;
    Script scr( iss, oss );
    scr.run();
    return oss.str();
}

string Glich::run_script_file( const string& filename )
{
    std::ifstream ifs( filename.c_str() );
    std::ostringstream oss;
    Script scr( ifs, oss );
    scr.run();
    return oss.str();
}

string Glich::run_module( const string& mod )
{
    string run = mod;
    string location, module;
    split_string( location, module, run );
    if( location == "file" ) {
        return run_script_file( module + ".glcs" );
    }
    else if( location == "hics" ) {
        if( hics_default_scripts.count( module ) == 1 ) {
            return run_script( hics_default_scripts[module] );
        }
        return string(); // Module not found.
    }
    else if( location == "glich" ) {
        return string(); // There are no glich modules yet!
    }
    return std::string();
}

SValue Glich::evaluate( const string& expression )
{
    std::istringstream iss( expression );
    std::ostringstream oss;
    Script scr( iss, oss );
    return scr.evaluate();
}

bool Glich::is_named( const string& name ) const
{
    if( m_constants.count( name ) == 1 ) {
        return true;
    }
    if( m_store->exists( name ) ) {
        return true;
    }
    if( m_globals.count( name ) == 1 ) {
        return true;
    }
    return false;
}

SValue Glich::get_named( const string& name ) const
{
    if( m_constants.count( name ) == 1 ) {
        return m_constants.find( name )->second;
    }
    if( m_store->exists( name ) ) {
        return m_store->get_local( name );
    }
    if( m_globals.count( name ) == 1 ) {
        return m_globals.find( name )->second;
    }
    return SValue();
}

bool Glich::is_variable( const string& name ) const
{
    if( m_store->exists( name ) ) {
        return true;
    }
    if( m_globals.count( name ) == 1 ) {
        return true;
    }
    return false;
}

bool Glich::create_variable( const string& name, VariableType type )
{
    switch( type )
    {
    case VariableType::local:
        create_local( name );
        break;
    case VariableType::global:
        create_global( name );
        break;
    }
    return true;
}

SValue* Glich::get_variable_ptr( const string& name )
{
    SValue* var = m_store->get_local_ptr( name );
    if( var == nullptr ) {
        var = get_global_ptr( name );
    }
    return var;
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

bool Glich::create_global( const string& name )
{
    assert( m_marks.size() > 0 );
    m_marks[m_marks.size() - 1]->add_global( name );
    m_globals[name] = SValue();
    return true;
}

SValue* Glich::get_global_ptr( const string& name )
{
    if( m_globals.count( name ) > 0 ) {
        return &m_globals.find( name )->second;
    }
    return nullptr;
}

void Glich::remove_global( const string& name )
{
    m_globals.erase( name );
}

bool Glich::create_constant( const string& name )
{
    if( is_named( name ) ) {
        return false;
    }
    assert( m_marks.size() > 0 );
    m_marks[m_marks.size() - 1]->add_constant( name );
    m_constants[name] = SValue();
    return true;
}

SValue* Glich::get_constant_ptr( const string& name )
{
    if( m_constants.count( name ) > 0 ) {
        return &m_constants.find( name )->second;
    }
    return nullptr;
}

bool Glich::is_constant( const string& name ) const
{
    return m_constants.count( name ) == 1;
}

void glich::Glich::remove_constant( const std::string& name )
{
    m_constants.erase( name );
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

bool Glich::add_object( Object* obj, const string& code )
{
    DefinedStatus status = get_object_status( code );
    if( status == DefinedStatus::defined ) {
        delete obj;
        return false;
    }
    if( status == DefinedStatus::none ) {
        m_marks[m_marks.size() - 1]->add_object( code );
    }
    m_objects[code] = obj;
    return true;
}

void Glich::remove_object( const string& code )
{
    if( m_objects.count( code ) == 0 ) {
        return;
    }
    delete m_objects.find( code )->second;
    m_objects.erase( code );
}

Object* Glich::get_object( const string& code )
{
    if( m_objects.count( code ) > 0 ) {
        Object* obj = m_objects.find( code )->second;
        if( obj == nullptr && m_object_mods.count( code ) == 1 ) {
            string mess = run_module( m_object_mods.find( code )->second );
            obj = m_objects.find( code )->second;
        }
        return obj;
    }
    return nullptr;
}

DefinedStatus glich::Glich::get_object_status( const std::string& code ) const
{
    if( m_objects.count( code ) == 0 ) {
        return DefinedStatus::none;
    }
    Object* obj = m_objects.find( code )->second;
    if( obj == nullptr ) {
        return DefinedStatus::module;
    }
    return DefinedStatus::defined;
}

bool Glich::add_file( File* file, const string& code )
{
    if( file == nullptr || m_files.count( code ) ) {
        delete file;
        return false;
    }
    m_marks[m_marks.size() - 1]->add_file( code );
    m_files[code] = file;
    return true;
}

void Glich::remove_file( const string& code )
{
    if( m_files.count( code ) == 0 ) {
        return;
    }
    delete m_files.find( code )->second;
    m_files.erase( code );
}

File* Glich::get_file( const string& code ) const
{
    if( m_files.count( code ) > 0 ) {
        return m_files.find( code )->second;
    }
    return nullptr;
}

bool Glich::add_module( const Module& mod )
{
    m_modules.insert( mod.m_code );
    m_marks[m_marks.size() - 1]->add_module( mod.m_code );

    for( auto& def : mod.m_defs ) {
        if( def.m_definition == "object" ) {
            for( auto& obj : def.m_codes ) {
                if( !add_object( nullptr, obj ) ) {
                    return false;
                }
                m_object_mods[obj] = mod.m_code;
            }
            continue;
        }
        if( def.m_definition == "scheme" ) {
            for( auto& sch : def.m_codes ) {
                string code = "s:" + sch;
                if( !add_object( nullptr, code ) ) {
                    return false;
                }
                m_object_mods[code] = mod.m_code;
            }
            continue;
        }
        if( def.m_definition == "lexicon" ) {
            for( auto& lex : def.m_codes ) {
                if( !add_lexicon( nullptr, lex ) ) {
                    return false;
                }
                m_lexicon_mods[lex] = mod.m_code;
            }
            continue;
        }
        if( def.m_definition == "grammar" ) {
            for( auto& gmr : def.m_codes ) {
                if( !add_grammar( nullptr, gmr ) ) {
                    return false;
                }
                m_grammar_mods[gmr] = mod.m_code;
            }
            continue;
        }
    }
    return true;
}

bool Glich::module_exists( const string& code ) const
{
    return m_modules.find(code) != m_modules.end();
}

void Glich::remove_module( const string& code )
{
    // Don't need to remove definitions, mark will look after that.
    StdStrVec items;
    for( auto& item : m_object_mods ) {
        if( item.second == code ) {
            items.push_back( item.first );
        }
    }
    for( string& item : items ) {
        m_object_mods.erase( item );
    }
    m_modules.erase( code );
}

bool Glich::add_lexicon( Lexicon* lex, const string& code )
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

void Glich::remove_lexicon( const string& code )
{
    if( m_lexicons.count( code ) == 0 ) {
        return;
    }
    delete m_lexicons.find( code )->second;
    m_lexicons.erase( code );
}

Lexicon* Glich::get_lexicon( const string& code )
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

DefinedStatus Glich::get_lexicon_status( const string& code ) const
{
    if( m_lexicons.count( code ) == 0 ) {
        return DefinedStatus::none;
    }
    Lexicon* lex = m_lexicons.find( code )->second;
    return lex ? DefinedStatus::defined : DefinedStatus::module;
}

bool Glich::add_grammar( Grammar* gmr, const string& code )
{
    DefinedStatus status = get_grammar_status( code );
    if( status == DefinedStatus::defined ) {
        delete gmr;
        return false;
    }
    if( status == DefinedStatus::none ) {
        HicMark* mark = dynamic_cast<HicMark*>(m_marks[m_marks.size() - 1]);
        assert( mark != nullptr );
        mark->add_grammar( code );
    }
    m_grammars[code] = gmr;
    return true;
}

void Glich::remove_grammar( const string& code )
{
    if( m_grammars.count( code ) == 0 ) {
        return;
    }
    delete m_grammars.find( code )->second;
    m_grammars.erase( code );
}

Grammar* Glich::get_grammar( const string& code )
{
    if( m_grammars.count( code ) > 0 ) {
        Grammar* gmr = m_grammars.find( code )->second;
        if( gmr == nullptr && m_grammar_mods.count( code ) == 1 ) {
            string mod = m_grammar_mods.find( code )->second;
            string mess = run_module( mod );
            gmr = m_grammars.find( code )->second;
        }
        return gmr;
    }
    return nullptr;
}

DefinedStatus Glich::get_grammar_status( const string& code ) const
{
    if( m_grammars.count( code ) == 0 ) {
        return DefinedStatus::none;
    }
    Grammar* gmr = m_grammars.find( code )->second;
    return gmr ? DefinedStatus::defined : DefinedStatus::module;
}

bool Glich::add_format( const string& code )
{
    assert( m_marks.size() > 0 );
    HicMark* mark = dynamic_cast<HicMark*>(m_marks[m_marks.size() - 1]);
    assert( mark != nullptr );
    mark->add_format( code );
    return true;
}

bool Glich::add_scheme( Scheme* sch, const string& scode )
{
    if( sch == nullptr ) {
        return false;
    }
    return add_object( sch, "s:" + scode );
}

Scheme* Glich::get_scheme( const string& scode )
{
    string ocode = "s:" + scode;
    DefinedStatus status = get_object_status( ocode );
    if( status == DefinedStatus::module ) {
        run_module( m_object_mods[ocode] );
    }
    return dynamic_cast<Scheme*>(get_object( ocode ));
}

DefinedStatus glich::Glich::get_scheme_status( const std::string& code ) const
{
    return get_object_status( "s:" + code );
}

StdStrVec Glich::get_scheme_list() const
{
    StdStrVec list;
    for( auto& pair : m_objects ) {
        string prefix, code;
        split_string( prefix, code, pair.first );
        if( prefix == "s" ) {
            list.push_back( code );
        }
    }
    return list;
}

void Glich::add_or_replace_mark( const string& name )
{
    clear_mark( name );
    Mark* prev = nullptr;
    int i = int( m_marks.size() ) - 1;
    if( i >= 0 ) {
        prev = m_marks[i];
    }
    Mark* mark = new HicMark( name, dynamic_cast<HicMark*>( prev ) );
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
        HicMark* mark = dynamic_cast<HicMark*>(m_marks[i]);
        if( property == "input" ) {
            mark->set_ischeme( sch );
            sch->set_input_format( fcode );
            return true;
        }
        if( property == "output" ) {
            mark->set_oscheme( sch );
            sch->set_output_format( fcode );
            return true;
        }
        if( property == "inout" ) {
            mark->set_ischeme( sch );
            sch->set_input_format( fcode );
            mark->set_oscheme( sch );
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
        dynamic_cast<HicMark*>( m_marks[i] )->set_ischeme( sch );
    }
}

void Glich::set_oscheme( Scheme* sch )
{
    int i = int( m_marks.size() ) - 1;
    if( i >= 0 ) {
        dynamic_cast<HicMark*>(m_marks[i])->set_oscheme( sch );
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
        return dynamic_cast<HicMark*>(m_marks[i])->get_ischeme();
    }
    return nullptr;
}

Scheme* Glich::get_oscheme() const
{
    int i = int( m_marks.size() ) - 1;
    if( i >= 0 ) {
        return dynamic_cast<HicMark*>(m_marks[i])->get_oscheme();
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

Glich& glich::glc()
{
    assert( s_glc != nullptr );
    return *s_glc;
}

// End of src/hg/historygen.cpp
