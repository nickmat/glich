/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glcGlich.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Glich language class
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     8th February 2023
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

#include <glc/glcGlich.h>

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
    : m_store( new Store ), m_inout( inout )
{
    Mark::set_zero_store( m_store );
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
        { "load:blob", fptr }
    };
    m_commands = {
        { "save:blob", fptr }
    };
    
    m_constants = {
        { "true", true },
        { "false", false },
        { "infinity", SValue( f_maximum, SValue::Type::field ) },
        { "inf", std::numeric_limits<double>::infinity() },
        { "nan", std::numeric_limits<double>::quiet_NaN() },
        { "null", SValue() },
        { "empty", SValue( RList() ) },
        { "blob", SValue( Blob() ) },
        { "nl", "\n" },
        { "pi", cal_pi }
    };
    m_module_names = { "object" };
}

Glich::~Glich()
{
    while( pop_store() );
    delete m_store;
    delete m_inout;
}

void Glich::init()
{
    m_marks.push_back( new Mark( "", nullptr ) );
}

const char* Glich::version()
{
    return glc_version;
}

GlcMarkDataVec Glich::get_glc_data() const
{
    GlcMarkDataVec glcdata;
    for( auto& mark : m_marks ) {
        GlcMarkData data;
        mark->get_mark_glc_data( data );
        glcdata.push_back( data );
    }
    return glcdata;
}

void Glich::load_builtin_library( StdStrVec args )
{
    for( size_t i = 0; i < glc_builtin_scripts_size; i++ ) {
        string error = run_script( glc_builtin_scripts[i].script );
        if( !error.empty() ) {
            m_init_error += "Module: \"" +
                string( glc_builtin_scripts[i].module ) + "\"\n" + error;
            return;
        }
    }
    string args_var;
    for( string arg : args ) {
        if( !args_var.empty() ) {
            args_var += ", ";
        }
        args_var += string_to_quote( arg );
    }
    args_var = "constant args = {: " + args_var + "};";
    string error = run_script( args_var );
    if( !error.empty() ) {
        m_init_error += "Create arg: \"" + args_var + "\"\n" + error;
    }
}

string Glich::run_script( const string& script )
{
    std::istringstream iss( script );
    std::ostringstream oss;
    run( iss, oss );
    return oss.str();
}

string Glich::run_script_file( const string& filename )
{
    std::ifstream ifs( filename.c_str() );
    std::ostringstream oss;
    run( ifs, oss );
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

bool Glich::run( std::istream& in, std::ostream& out, int line )
{
    Script scr( in, out );
    scr.set_line( line );
    return scr.run();
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
        if( !add_module_def( def, mod.m_code ) ) {
            return false;
        }
    }
    return true;
}

bool glich::Glich::add_module_def( const ModuleDef& def, const std::string& code )
{
    if( def.m_definition == "object" ) {
        for( auto& obj : def.m_codes ) {
            if( !add_object( nullptr, obj ) ) {
                return false;
            }
            m_object_mods[obj] = code;
        }
        return true;
    }
    return false;
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

Mark* Glich::create_mark( const string& name, Mark* prev )
{
    return new Mark( name, prev );
}

void Glich::add_or_replace_mark( const string& name )
{
    clear_mark( name );
    Mark* prev = nullptr;
    int i = int( m_marks.size() ) - 1;
    if( i >= 0 ) {
        prev = m_marks[i];
    }
    Mark* mark = create_mark( name, prev );
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
    if( property == "write:text" ) {
        WriteText wt;
        if( value == "plain" ) {
            wt = WriteText::plain;
        }
        else if( value == "quoted" ) {
            wt = WriteText::quoted;
        }
        else {
            return false;
        }
        m_marks[i]->set_write_text( wt );
        return true;
    }
    return false;
}

Integer Glich::get_integer() const
{
    int i = int( m_marks.size() ) - 1;
    if( i >= 0 ) {
        return m_marks[i]->get_integer();
    }
    return Integer::number;
}

WriteText glich::Glich::get_write_text() const
{
    int i = int( m_marks.size() ) - 1;
    if( i >= 0 ) {
        return m_marks[i]->get_write_text();
    }
    return WriteText::plain;
}

string Glich::get_special_value_string( SpecialValue val )
{
    switch( val )
    {
    case SpecialValue::plus_inf:
        return "+infinity";
    case SpecialValue::minus_inf:
        return "-infinity";
    case SpecialValue::unknown:
        return "?";
    }
    return std::string();
}

bool Glich::is_module_name( const string& name ) const
{
    return m_module_names.find( name ) != m_module_names.end();
}


// End of src/glc/glcGlich.cpp
