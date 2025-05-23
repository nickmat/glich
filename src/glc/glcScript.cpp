/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glcScript.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Run Glich script.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     5th February 2023
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

#include "glcScript.h"

#include "glcFile.h"
#include "glcFilesys.h"
#include "glcFunction.h"
#include "glcHelper.h"
#include "glcObject.h"
#include "glcVersion.h"

#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>


using namespace glich;
using std::string;
using std::vector;


STokenStream* Script::s_current_ts = nullptr;

Script::Script( std::istream& in, std::ostream& out)
    : m_ts( in, out ), m_out( &out ), m_err( &out ) {}

bool Script::run()
{
    bool ret = false;
    STokenStream* prev_ts = s_current_ts;
    s_current_ts = &m_ts;
    for( ;;) {
        next_token();
        if( current_token().type() == SToken::Type::End ) {
            ret = true;
            break;
        }
        if( statement() == false ) {
            ret = (m_ts.errors() == 0);
            break;
        }
    }
    s_current_ts = prev_ts;
    return ret;
}

SValue Script::run_script( string& script )
{
    STokenStream prev_ts = m_ts;
    std::istringstream iss( script );
    m_ts.reset_in( &iss );
    SValue value = expr( GetToken::next );
    m_ts = prev_ts;
    return value;
}

bool Script::error_value( const SValue& value )
{
    bool ok;
    string mess = value.get_str( ok ) + "\n";
    if( !ok ) {
        return false;
    }
    return m_ts.error_value( mess );
}

bool Script::statement()
{
    SToken token = current_token();

    if( token.type() == SToken::Type::Name ) {
        string name = token.get_str();
        if( name == "end" ) return false;
        if( name == "mark" ) return do_mark();
        if( name == "if" ) return do_if();
        if( name == "do" ) return do_do();
        if( name == "set" ) return do_set();
        if( name == "let" ) return do_let( VariableType::local );
        if( name == "global" ) return do_let( VariableType::global );
        if( name == "constant" ) return do_let( VariableType::constant );
        if( name == "write" ) return do_write();
        if( name == "writeln" ) return do_writeln();
        if( name == "call" ) return do_call();
        if( name == "function" ) return do_function();
        if( name == "command" ) return do_command();
        if( name == "object" ) return do_object();
        if( name == "file" ) return do_file();
        if( name == "module" ) return do_module();
        if( glc().is_variable( name ) ) return do_assign( name );
    }
    else if( token.type() == SToken::Type::Semicolon ) {
        return true; // Empty statement
    }
    // Assume error
    return error( "Unrecognised statement." );
}

bool Script::do_mark()
{
    string mark = get_name_or_primary( GetToken::next );
    if( current_token().type() != SToken::Type::Semicolon ) {
        error( "';' expected." );
        return false;
    }
    if( mark.empty() ) {
        error( "Mark name string expected." );
        return false;
    }
    if( !glc().is_level_zero() ) {
        error( "Mark is only available in level zero." );
        return false;
    }
    glc().add_or_replace_mark( mark );
    return true;
}

bool Script::do_if()
{
    bool done = false, result = false;
    int nested = 0;
    const char* enderr = "if ended unexpectedly.";
    for( ;;) {
        if( !result && !done ) {
            SValue value = expr( GetToken::next );
            if( value.type() == SValue::Type::Error ) {
                error_value( value );
                return false;
            }
            if( value.type() != SValue::Type::Bool ) {
                error( "Boolean expression expected." );
                return false;
            }
            result = value.get_bool();
        }
        SToken token = current_token();
        if( result ) {
            // Run the statements
            for( ;;) {
                if( token.type() == SToken::Type::End ) {
                    error( enderr );
                    return false;
                }
                if( token.type() == SToken::Type::Name ) {
                    string name = token.get_str();
                    if( name == "endif" ) {
                        return true;
                    }
                    if( name == "else" || name == "elseif" ) {
                        break;
                    }
                }
                if( statement() == false ) {
                    return false;
                }
                token = next_token();
            }
            done = true;
            result = false;
        }
        else {
            // move on to the next "elseif" or "else"
            nested = 0;
            for( ;;) {
                if( token.type() == SToken::Type::At ) {
                    token = next_token();
                    token = next_token(); // Ignore function name
                }
                if( token.type() == SToken::Type::End ) {
                    error( enderr );
                    return false;
                }
                if( token.type() == SToken::Type::Name ) {
                    string name = token.get_str();
                    if( name == "if" ) {
                        nested++;
                    }
                    else if( name == "endif" ) {
                        if( nested > 0 ) {
                            --nested;
                        }
                        else {
                            return true;
                        }
                    }
                    else if( name == "else" && nested == 0 ) {
                        next_token();
                        result = true;
                        break;
                    }
                    else if( name == "elseif" && nested == 0 ) {
                        break;
                    }
                }
                token = next_token();
            }
        }
        if( done ) {
            // Skip to "endif"
            nested = 0;
            for( ;;) {
                token = next_token();
                if( token.type() == SToken::Type::End ) {
                    error( enderr );
                    return false;
                }
                if( token.type() == SToken::Type::Name ) {
                    string name = token.get_str();
                    if( name == "if" ) {
                        nested++;
                    }
                    else if( name == "endif" ) {
                        if( nested > 0 ) {
                            --nested;
                        }
                        else {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool Script::do_do()
{
    int start_line = m_ts.get_line();
    string code = m_ts.read_until( "loop", "do" );
    if( code.empty() ) {
        error( "Do loop not terminated." );
        return false;
    }
    int end_line = m_ts.get_line();
    std::istringstream iss( code );
    std::istream* prev_iss = m_ts.reset_in( &iss );
    for( size_t i = 1000; i > 0; --i ) {  // We have a limit of 1000 reiterations
        m_ts.set_line( start_line );
        bool exit = false;
        SToken token = next_token();
        for( ;;) {
            if( token.type() == SToken::Type::End ) {
                exit = true;
                break;
            }
            if( token.type() == SToken::Type::Name ) {
                string name = token.get_str();
                if( name == "until" || name == "while" ) {
                    bool ok = false;
                    exit = expr( GetToken::next ).get_bool( ok );
                    if( !ok ) {
                        error( "Boolean expression expected." );
                        exit = true;
                        break;
                    }
                    if( name == "while" ) {
                        exit = !exit;
                    }
                    if( exit ) {
                        break;
                    }
                    token = current_token();
                    continue;
                }
                if( name == "loop" ) {
                    break;
                }
            }
            if( statement() == false ) {
                exit = true;
                break;
            }
            token = next_token();
        }
        if( exit ) {
            break;
        }
        iss.clear();
        iss.seekg( 0 );
    }
    m_ts.reset_in( prev_iss );
    m_ts.set_line( end_line );
    return true;
}

bool Script::do_set()
{
    string prop;
    SToken token = next_token();
    if( token.type() == SToken::Type::Name ) {
        prop = token.get_str();
    }
    else {
        error( "Set property expected." );
        return false;
    }
    string value = get_name_or_primary( GetToken::next );
    if( current_token().type() != SToken::Type::Semicolon ) {
        error( "set statement is \"set propery value;\"." );
        return false;
    }
    if( !glc().set_property( prop, value ) ) {
        error( "Set property \"" + prop + "\" value \"" + value + "\" not recognised." );
        return false;
    }
    return true;
}

bool Script::do_let( VariableType vartype )
{
    SToken token = next_token();
    if( token.type() != SToken::Type::Name ) {
        error( "Variable name expected." );
        return false;
    }
    string name = token.get_str();
    if( glc().is_constant( name ) ) {
        error( "\"" + name + "\" is a constant." );
        return false;
    }
    bool create = glc().create_variable( name, vartype ); // Creates variable if it doesn't exist.
    if( !create ) {
        error( "Unable to create variable " + name );
        return false;
    }
    return do_assign( name, vartype );
}

bool Script::do_assign( const string& name, VariableType vartype )
{
    SToken token = next_token();
    SValue* vp = nullptr;
    switch( vartype )
    {
    case VariableType::local:
        vp = glc().get_local_ptr( name );
        break;
    case VariableType::global:
        vp = glc().get_global_ptr( name );
        break;
    case VariableType::constant:
        if( glc().create_constant( name ) ) {
            vp = glc().get_constant_ptr( name );
        }
        else {
            assert( false ); // This should already be checked for.
            return false;
        }
        break;
    default:
        vp = glc().get_variable_ptr( name );
    }
    assert( vp != nullptr );

    while( token.type() == SToken::Type::LSbracket ) {
        if( vp->type() != SValue::Type::Object ) {
            error( "Object type expected." );
            return false;
        }
        size_t index = 0;
        string istr = get_name_or_primary( GetToken::next );
        if( istr.size() > 0 && isdigit( istr[0] ) ) {
            index = str_to_num( istr );
        }
        else {
            Object* obj = vp->get_object_ptr();
            index = obj->get_vindex( istr );
            if( index == 0 ) {
                error( "Subscript not found." );
                return false;
            }
            --index;
        }
        vp = vp->get_object_element( index );
        if( vp == nullptr ) {
            error( "Object index error." );
            return false;
        }
        if( current_token().type() != SToken::Type::RSbracket ) {
            error( "']' expected." );
            return false;
        }
        token = next_token();
    }

    SValue value;
    if( token.type() == SToken::Type::Equal ) {
        value = expr( GetToken::next );
    }
    else if( glc().is_variable( name ) ) {
        value = *vp;
        switch( token.type() )
        {
        case SToken::Type::PlusEq:
            value.plus( expr( GetToken::next ) );
            break;
        case SToken::Type::MinusEq:
            value.minus( expr( GetToken::next ) );
            break;
        case SToken::Type::DivideEq:
            value.divide( expr( GetToken::next ) );
            break;
        case SToken::Type::StarEq:
            value.multiply( expr( GetToken::next ) );
            break;
        case SToken::Type::AtEq:
            value = do_at( value, get_name_or_primary( GetToken::next ) );
            break;
        default:
            error( "Assign operator expected." );
            return false;
        }
    }
    else {
        error( "'=' expected." );
        return false;
    }
    *vp = value;
    if( value.type() == SValue::Type::Error ) {
        m_ts.skip_to( SToken::Type::Semicolon );
    }
    if( current_token().type() != SToken::Type::Semicolon ) {
        error( "';' expected." );
        return false;
    }
    return true;
}

bool Script::do_write( const string& term )
{
    SToken token = next_token();
    string filecode;
    if( token.type() == SToken::Type::Dot ) {
        filecode = get_name_or_primary( GetToken::next );
    }
    std::ostream* out = nullptr;
    if( !filecode.empty() ) {
        File* file = glc().get_file( filecode );
        if( file == nullptr ) {
            error( "File \"" + filecode + "\" not found." );
            return false;
        }
        out = file->get_out();
    }
    if( out == nullptr ) {
        out = m_out;
    }

    for( ;;) {
        if( current_token().type() == SToken::Type::Semicolon ) {
            break;
        }
        if( current_token().type() == SToken::Type::Comma ) {
            next_token();
            *out << ", ";
            continue;
        }
        if( current_token().type() == SToken::Type::Name && current_token().get_str() == "nl" ) {
            next_token();
            *out << "\n";
            continue;
        }

        SValue value = expr( GetToken::current );
        if( value.type() == SValue::Type::String && glc().get_write_text() == WriteText::quoted ) {
            *out << string_to_quote( value.get_str() );
        }
        else {
            *out << value.as_string();
        }

        if( value.type() == SValue::Type::Error ) {
            m_ts.skip_to( SToken::Type::Semicolon );
            break;
        }
    }
    *out << term;
    out->flush();
    return true;
}

SpFunction Script::create_function( const string& code )
{
    SToken token = current_token();
    StdStrVec quals;
    StdStrVec args;
    SValueVec defs;
    while( token.type() == SToken::Type::Dot ) {
        string qual = get_name_or_primary( GetToken::next );
        if( qual.empty() ) {
            error( "Qualifier name required." );
            return nullptr;
        }
        quals.push_back( qual );
        token = current_token();
    }
    if( token.type() == SToken::Type::Lbracket ) {
        token = next_token();
        for( ;;) {
            if( token.type() == SToken::Type::Rbracket ) {
                break;
            }
            string str = current_token().get_str();
            if( token.type() != SToken::Type::Name || str.empty() ) {
                error( "Argument name expected." );
                return nullptr;
            }
            args.push_back( str );
            token = next_token();
            SValue value;
            if( token.type() == SToken::Type::Equal ) {
                value = expr( GetToken::next );
            }
            defs.push_back( value );
            token = current_token();
            if( token.type() == SToken::Type::Comma ) {
                token = next_token();
            }
        }
        token = next_token();
    }
    if( token.type() != SToken::Type::LCbracket ) {
        error( "'{' expected." );
        return nullptr;
    }
    int line = m_ts.get_line();
    string script = m_ts.read_until( "}", "{" );
    if( script.empty() ) {
        error( "Terminating '}' not found." );
        return nullptr;
    }
    SpFunction fun = SpFunction( new Function(code) );
    if( fun == nullptr ) {
        return nullptr;
    }
    fun->set_qualifiers( quals );
    fun->set_args( args );
    fun->set_defaults( defs );
    fun->set_line( line );
    fun->set_script( script );
    return fun;
}

bool Script::do_function()
{
    string code = get_name_or_primary( GetToken::next );
    if( code.empty() ) {
        error( "Function name missing." );
        return false;
    }
    if( glc().get_function( code ) != nullptr ) {
        error( "The function \"" + code + "\" already exists." );
        return false;
    }
    SpFunction fun = create_function( code );
    if( fun == nullptr ) {
        return false;
    }
    return glc().add_function( fun );
}

bool Script::do_command()
{
    string code = get_name_or_primary( GetToken::next );
    if( code.empty() ) {
        error( "Command name missing." );
        return false;
    }
    if( glc().get_command( code ) != nullptr ) {
        error( "command \"" + code + "\" already exists." );
        return false;
    }

    SpFunction com = create_function( code );
    if( com == nullptr ) {
        return false;
    }
    return glc().add_command( com );
}

bool Script::do_call()
{
    SValue value = command_call();
    if( value.is_error() ) {
        error_value( value );
        return false;
    }
    if( current_token().type() != SToken::Type::Semicolon ) {
        error( "';' expected." );
        return false;
    }
    return true;
}

bool Script::do_object()
{
    string code = get_name_or_primary( GetToken::next );
    if( code.empty() ) {
        error( "Object name missing." );
        return false;
    }
    DefinedStatus status = glc().get_object_status( code );
    if( status == DefinedStatus::defined ) {
        error( "object \"" + code + "\" already exists." );
        return false;
    }
    if( current_token().type() != SToken::Type::LCbracket ) {
        error( "'{' expected." );
        return false;
    }
    Object* obj = new Object( code );
    bool ok = true;
    for( ;;) {
        SToken token = next_token();
        if( token.type() == SToken::Type::RCbracket ||
            token.type() == SToken::Type::End ) {
            break; // All done.
        }
        else if( token.type() == SToken::Type::Name ) {
            string name = token.get_str();
            if( name == "values" ) {
                StdStrVec values = get_string_list( GetToken::next );
                obj->set_value_names( values );
            }
            else if( name == "function" ) {
                string fcode = get_name_or_primary( GetToken::next );
                SpFunction fun = create_function( fcode );
                if( fun == nullptr ) {
                    error( "Unable to create function." );
                    ok = false;
                    break;
                }
                obj->add_function( fun );
            }
            else {
                error( "Unknown object subcommand." );
                ok = false;
                break;
            }
        }
    }
    if( ok ) {
        glc().add_object( obj, code );
        return true;
    }
    delete obj;
    return false;
}

bool Script::do_file()
{
    SToken token = next_token();
    string filecode;
    File* file = nullptr;
    if( token.type() == SToken::Type::Dot ) {
        filecode = get_name_or_primary( GetToken::next );
        file = glc().get_file(filecode);
        if( file == nullptr ) {
            error( "File code \"" + filecode + "\" does not exist." );
            return false;
        }
    }
    else {
        filecode = get_name_or_primary( GetToken::current );
        file = glc().get_file( filecode );
        if( file != nullptr ) {
            error( "File code \"" + filecode + "\" already exists." );
            return false;
        }
        file = new File( filecode );
        if( !glc().add_file(file, filecode ) ) {
            error( "Unable to create file." );
            return false;
        }
        if( current_token().type() == SToken::Type::Semicolon ) {
            return true;
        }
    }

    File::FileType type = File::FT_null;
    if( current_token().type() != SToken::Type::Semicolon ) {
        string type_str = get_name_or_primary( GetToken::current );
        if( type_str.empty() ) {
            error( "';' or switch expected." );
            return false;
        }
        if( type_str == "read" ) {
            type = File::FT_read;
        }
        else if( type_str == "write" ) {
            type = File::FT_write;
        }
        else if( type_str == "append" ) {
            type = File::FT_append;
        }
        else {
            error( "File mode \"" + type_str + "\" not understood." );
            return false;
        }
    }
    string filename = expr( GetToken::current ).as_string();
    if( current_token().type() != SToken::Type::Semicolon ) {
        error( "';' expected." );
        return false;
    }

    file->close();
    if( !filename.empty() ) {
        file->set_filename( filename );
    }
    file->set_filetype( type );

    bool ok = file->open();
    if( !ok ) {
        error( "Unable to open file: " + filename );
        return false;
    }
    return true;
}

bool Script::do_module()
{
    string code = get_name_or_primary( GetToken::next );
    if( code.empty() ) {
        error( "Module name missing." );
        return false;
    }
    if( glc().module_exists( code ) ) {
        error( "Module \"" + code + "\" already exists." );
        return false;
    }
    if( current_token().type() != SToken::Type::LCbracket ) {
        error( "'{' expected." );
        return false;
    }
    Module mod;
    mod.m_code = code;
    ModuleDef def;
    for( ;;) {
        SToken token = next_token();
        if( token.type() == SToken::Type::RCbracket ||
            token.type() == SToken::Type::End ) {
            break; // All done.
        }
        else if( token.type() == SToken::Type::Name ) {
            string name = token.get_str();
            if( !glc().is_module_name( name ) ) {
                error( "Unknown module subcommand." );
                break;
            }
            def.m_definition = name;
            def.m_codes = get_string_list( GetToken::next );
            mod.m_defs.push_back( def );
        }
    }
    if( !glc().add_module( mod ) ) {
        error( "Unable to add module." );
        return false;
    }
    return true;
}

SValue Script::expr( GetToken get )
{
    SValue left = compare( get );
    for( ;;) {
        SToken token = current_token();
        switch( token.type() )
        {
        case SToken::Type::Or:
            left.logical_or( compare( GetToken::next ) );
            break;
        case SToken::Type::And:
            left.logical_and( compare( GetToken::next ) );
            break;
        default:
            return left;
        }
    }
}

SValue Script::compare( GetToken get )
{
    SValue left = combine( get );
    for( ;;) {
        SToken token = current_token();
        switch( token.type() )
        {
        case SToken::Type::Equal:
            left.equal( combine( GetToken::next ) );
            break;
        case SToken::Type::NotEqual:
            left.equal( combine( GetToken::next ) );
            left.logical_not();
            break;
        case SToken::Type::GtThan:
            left.greater_than( combine( GetToken::next ) );
            break;
        case SToken::Type::GtThanEq:
            left.less_than( combine( GetToken::next ) );
            left.logical_not();
            break;
        case SToken::Type::LessThan:
            left.less_than( combine( GetToken::next ) );
            break;
        case SToken::Type::LessThanEq:
            left.greater_than( combine( GetToken::next ) );
            left.logical_not();
            break;
        default:
            return left;
        }
    }
}

SValue Script::combine( GetToken get )
{
    SValue left = range( get );

    for( ;;) {
        SToken token = current_token();
        switch( token.type() )
        {
        case SToken::Type::UNION:
            left.rlist_union( range( GetToken::next ) );
            break;
        case SToken::Type::INTERSECTION:
            left.intersection( range( GetToken::next ) );
            break;
        case SToken::Type::REL_COMPLEMENT:
            left.rel_complement( range( GetToken::next ) );
            break;
        case SToken::Type::SYM_DIFFERENCE:
            left.sym_difference( range( GetToken::next ) );
            break;
        default:
            return left;
        }
    }
}

SValue Script::range( GetToken get )
{
    SValue left = sum( get );

    for( ;;) {
        SToken token = current_token();
        switch( token.type() )
        {
        case SToken::Type::DotDot:
            left.range_op( sum( GetToken::next ) );
            break;
        default:
            return left;
        }
    }
}

SValue Script::sum( GetToken get )
{
    SValue left = term( get );
    for( ;;) {
        SToken token = current_token();
        switch( token.type() )
        {
        case SToken::Type::Plus:
            left.plus( term( GetToken::next ) );
            break;
        case SToken::Type::Minus:
            left.minus( term( GetToken::next ) );
            break;
        default:
            return left;
        }
    }
}

SValue Script::term( GetToken get )
{
    SValue left = subscript( get );

    for( ;;) {
        SToken token = current_token();
        switch( token.type() )
        {
        case SToken::Type::Star:
            left.multiply( subscript( GetToken::next ) );
            break;
        case SToken::Type::Divide:
            left.divide( subscript( GetToken::next ) );
            break;
        case SToken::Type::Div:
            left.int_div( subscript( GetToken::next ) );
            break;
        case SToken::Type::Mod:
            left.modulus( subscript( GetToken::next ) );
            break;
        default:
            return left;
        }
    }
}

SValue Script::subscript( GetToken get )
{
    SValue left = primary( get );

    for( ;;) {
        SToken token = current_token();
        switch( token.type() )
        {
        case SToken::Type::LSbracket: {
                SValue right;
                token = next_token();
                if( token.type() == SToken::Type::Name ) {
                    right.set_str( token.get_str() );
                    next_token();
                }
                else {
                    right = expr( GetToken::current );
                }
                left = do_subscript( left, right );
                if( current_token().type() != SToken::Type::RSbracket ) {
                    error( "']' expected." );
                }
                next_token();
            }
            break;
        case SToken::Type::At:
            left = do_at( left, get_name_or_primary( GetToken::next ) );
            break;
        default:
            return left;
        }
    }
}

SValue Script::primary( GetToken get )
{
    SValue value;
    SToken token = (get == GetToken::next) ? next_token() : current_token();
    switch( token.type() )
    {
    case SToken::Type::Number:
    case SToken::Type::Field:
    case SToken::Type::Real:
        value = token.value();
        next_token();
        break;
    case SToken::Type::Qmark:
        value.set_field( f_invalid );
        next_token();
        break;
    case SToken::Type::String:
        value.set_str( token.get_str() );
        next_token();
        break;
    case SToken::Type::Name:
        value = get_value_var( token.get_str() );
        next_token();
        break;
    case SToken::Type::Lbracket:
        value = expr( GetToken::next );
        if( current_token().type() != SToken::Type::Rbracket ) {
            error( "')' expected." );
            break;
        }
        next_token();
        break;
    case SToken::Type::LCbracket:
        value = get_object( GetToken::next );
        next_token();
        break;
    case SToken::Type::At:
        value = function_call();
        break;
    case SToken::Type::Minus:
        value = subscript( GetToken::next );
        value.negate();
        break;
    case SToken::Type::Plus:
        value = subscript( GetToken::next );
        break;
    case SToken::Type::Not:
        value = subscript( GetToken::next );
        value.logical_not();
        break;
    case SToken::Type::COMPLEMENT:
        value = subscript( GetToken::next );
        value.compliment();
        break;
    default:
        value.set_error( "Primary value expected." );
    }
    return value;
}

string Script::get_name_or_primary( GetToken get )
{
    string str;
    SToken token = (get == GetToken::next) ? next_token() : current_token();
    if( token.type() == SToken::Type::Name ) {
        str = token.get_str();
        next_token();
    }
    else {
        SValue value = primary( GetToken::current );
        if( value.type() == SValue::Type::String || value.type() == SValue::Type::Number
            || value.type() == SValue::Type::field || value.type() == SValue::Type::Float ) {
            str = value.as_string();
        }
    }
    return str;
}

StdStrVec Script::get_string_list( GetToken get )
{
    StdStrVec vec;
    SToken token = (get == GetToken::next) ? next_token() : current_token();
    while( token.type() != SToken::Type::Semicolon &&
        token.type() != SToken::Type::End )
    {
        string str = get_name_or_primary( GetToken::current );
        if( !str.empty() ) {
            vec.push_back( str );
        }
        token = current_token();
        if( token.type() == SToken::Type::Comma ) {
            token = next_token();
        }
    }
    return vec;
}

SValue Script::get_object( GetToken get )
{
    string ocode = get_name_or_primary( get );
    Object* obj = glc().get_object( ocode );
    SValue value;
    if( obj == nullptr ) {
        value.set_error( "Object name not recognised." );
        return value;
    }

    SValueVec vlist;
    SValue vo( ocode );
    vlist.push_back( vo );

    SToken token = current_token();
    bool comma_next = false;
    bool done = false;
    for( ;;) {
        switch( token.type() )
        {
        case SToken::Type::End:
        case SToken::Type::RCbracket:
            if( vlist.size() > 1 && !comma_next ) {
                vlist.push_back( SValue() );
            }
            done = true;
            break;
        case SToken::Type::Comma:
            if( comma_next ) {
                comma_next = false;
            }
            else {
                vlist.push_back( SValue() );
            }
            break;
        default: {
                SValue value = expr( GetToken::current );
                vlist.push_back( value );
            }
            token = current_token();
            comma_next = true;
            continue;
        }
        if( done ) break;
        token = next_token();
    }
    size_t size = obj->get_min_size();
    while( vlist.size() < size ) {
        vlist.push_back( SValue() );
    }
    return SValue( vlist );
}

SValue Script::do_subscript( const SValue& left, const SValue& right )
{
    SValue value( left );
    if( left.type() == SValue::Type::Object ) {
        SValueVec values = left.get_object();
        if( values.empty() || values[0].type() != SValue::Type::String ) {
            value.set_error( "Malformed Object." );
            return value;
        }
        string code = values[0].get_str();
        Object* obj = glc().get_object( code );
        if( right.type() == SValue::Type::String ) {
            size_t index = obj->get_vindex( right.get_str() );
            if( index < values.size() ) {
                return values[index];
            }
        }
        else {
            bool success = true;
            Num num = right.get_number( success );
            if( success && num >= 0 && num < (values.size() - 1) ) {
                return values[num + 1];
            }
        }
    }
    return SValue();
}

SValue Script::do_at( const SValue& left, const SValue& right )
{
    string ocode = left.get_object_code();
    if( ocode.empty() ) {
        return SValue::create_error( "Object expected." );
    }
    Object* obj = glc().get_object( ocode );
    if( obj == nullptr ) {
        return SValue::create_error( "Object not found." );
    }
    string fcode = right.as_string();
    if( fcode.empty() ) {
        return SValue::create_error( "Object function expected." );
    }
    Function* fun = obj->get_function( fcode );
    if( fun == nullptr ) {
        bool success = false;
        SValue value = do_object_at( success, obj, fcode, left );
        if( success ) {
            return value;
        }
        return value.create_error( "Function not found." );
    }
    return run_function( fun, &left );
}

SValue Script::do_object_at( bool& success, Object* obj, const string& fcode, const SValue& left )
{
    success = true;
    if( fcode == "mask" ) {
        return dot_mask( obj, &left );
    }
    else if( fcode == "size" ) {
        const SValueVec* elements = left.get_object_values();
        return SValue( elements->size() - 1, SValue::Type::Number );
    }
    else if( fcode == "obj:name" ) {
        return obj->get_code();
    }
    else if( fcode == "obj:list" ) {
        return obj->get_list( *left.get_object_values() );
    }
    success = false;
    return SValue();
}

StdStrVec Script::get_qualifiers( GetToken get )
{
    SToken token = (get == GetToken::next) ? m_ts.next() : current_token();
    StdStrVec quals;
    while( token.type() == SToken::Type::Dot ) {
        string str = get_name_or_primary( GetToken::next );
        quals.push_back( str );
        token = current_token();
    }
    return quals;
}

SValueVec Script::get_args( GetToken get )
{
    SToken token = (get == GetToken::next) ? m_ts.next() : current_token();
    SValueVec args;
    if( token.type() == SToken::Type::Lbracket ) {
        for( ;; ) {
            token = m_ts.next();
            SValue arg;
            if( token.type() != SToken::Type::Comma && token.type() != SToken::Type::Rbracket ) {
                arg = expr( GetToken::current );
            }
            args.push_back( arg );
            token = current_token();
            if( token.type() == SToken::Type::Rbracket ||
                token.type() == SToken::Type::End )
            {
                break;
            }
            if( token.type() != SToken::Type::Comma ) {
                return SValueVec();
            }
        }
        token = next_token();
    }
    return args;
}

SValue Script::function_call()
{
    SToken token = next_token();
    if( token.type() != SToken::Type::Name ) {
        return SValue::create_error( "Function name expected." );
    }
    string name = token.get_str();

    bool success = false;
    SValue value = builtin_function_call( success, name );
    if( success ) {
        return value;
    }

    Function* fun = glc().get_function( name );
    if( fun == nullptr ) {
        return SValue::create_error( "Function \"" + name + "\" not found." );
    }
    return run_function( fun );
}

SValue Script::builtin_function_call( bool& success, const string& name )
{
    enum f {
        f_if, f_error, f_string, f_quote, f_field, f_range, f_rlist, f_number, f_float, f_read, f_filesys,
        f_version, f_low, f_high, f_span, f_size, f_envelope, f_type, f_object, f_global, f_load_blob
    };
    const static std::map<string, f> fmap = {
        { "if", f_if }, { "error", f_error }, { "string", f_string }, { "quote", f_quote }, { "field", f_field },
        { "range", f_range }, { "rlist", f_rlist }, { "number", f_number }, { "float", f_float },
        { "read", f_read }, { "filesys", f_filesys }, { "version", f_version }, { "low", f_low },
        { "high", f_high }, { "span", f_span }, { "size", f_size }, { "envelope", f_envelope },
        { "type", f_type }, { "object", f_object }, { "global", f_global }, { "load:blob", f_load_blob }
    };

    auto fnum = fmap.find( name );
    if( fnum != fmap.end() ) {
        success = true;
        switch( fnum->second )
        {
        case f_if: return at_if();
        case f_error: return at_error();
        case f_string: return at_string();
        case f_quote: return at_quote();
        case f_field: return at_field();
        case f_range: return at_range();
        case f_rlist: return at_rlist();
        case f_number: return at_number();
        case f_float: return at_float();
        case f_read: return at_read();
        case f_filesys: return at_filesys();
        case f_version: return at_version();
        case f_low:
        case f_high:
        case f_span:
        case f_size:
        case f_envelope:
        case f_type:
        case f_object: return do_at_property( name );
        case f_global: return at_global();
        case f_load_blob: return at_load_blob();
        }
        return SValue::create_error( "Built-in hics function error." );
    }
    success = false;
    return SValue();
}

SValue Script::run_function( Function* fun, const SValue* left )
{
    GetToken get = (left == nullptr) ? GetToken::next : GetToken::current;
    StdStrVec qualifiers = get_qualifiers( get );
    SValueVec args = get_args( GetToken::current );

    if( left != nullptr ) {
        return fun->run( left, qualifiers, args, get_out_stream() );
    }
    return fun->run( qualifiers, args, get_out_stream() );
}

SValue Script::dot_mask( const Object* obj, const SValue* left )
{
    assert( obj != nullptr );
    SValueVec args = get_args( GetToken::current );
    string name = left->get_object_code();
    if( name.empty() ) {
        return SValue::create_error( "Object type reqired for mask function." );
    }
    if( args.size() != 1 || args[0].type() != SValue::Type::Object ) {
        return SValue::create_error( "Function argument must be object type." );
    }
    const SValueVec& vleft = left->get_object();
    const SValueVec& vright = args[0].get_object();
    size_t size = std::max( vleft.size(), vright.size() );
    SValueVec result = { name };
    for( size_t i = 1; i < size; i++ ) {
        SValue value;
        if( i < vleft.size() ) {
            value = vleft[i];
        }
        if( i < vright.size() && value.type() == SValue::Type::Null ) {
            value = vright[i];
        }
        result.push_back( value );
    }
    return SValue( result );
}

SValue Script::command_call()
{
    SToken token = next_token();
    if( token.type() == SToken::Type::At ) {
        return function_call();
    }
    if( token.type() != SToken::Type::Name ) {
        return SValue::create_error( "Command name expected." );
    }
    string name = token.get_str();

    bool success = false;
    SValue value = builtin_command_call( success, name );
    if( success ) {
        return value;
    }

    Function* com = nullptr;
    com = glc().get_command( name );
    if( com == nullptr ) {
        value.set_error( "Command \"" + name + "\" not found." );
        return value;
    }
    return run_function( com );
}

SValue glich::Script::builtin_command_call( bool& success, const std::string& name )
{
    enum c {
        c_save_blob
    };
    const static std::map<string, c> cmap = {
        { "save:blob", c_save_blob }
    };

    auto cnum = cmap.find( name );
    if( cnum != cmap.end() ) {
        success = true;
        switch( cnum->second )
        {
        case c_save_blob: return com_save_blob();
        }
        return SValue::create_error( "Built-in glich command error." );
    }
    success = false;
    return SValue();
}

// Evaluate the built-in @if function:
// @if( c, a, b )
// An arithmetic 'if' expression,
// result is 'a' if 'c' is true, or 'b' if 'c' is false.
// Equivalent to the statement:
// function if(c,a,b) { if c result=a; else result=b; endif }
SValue Script::at_if()
{
    SValueVec args = get_args( GetToken::next );
    if( args.size() != 3 ) {
        return SValue::create_error( "@if requires 3 arguments." );
    }
    if( args[0].type() != SValue::Type::Bool ) {
        return SValue::create_error( "1st argument of @if must be a boolean." );
    }
    if( args[0].get_bool() ) {
        return args[1];
    }
    return args[2];
}

// Evaluate the built-in @read function:
// @read( prompt = "" )
// If prompt is given, it is output to stdout first,
// then input is read from stdin until a newline.
// The result is always a string.
SValue Script::at_read()
{
    StdStrVec quals = get_qualifiers( GetToken::next );
    SValueVec args = get_args( GetToken::current );
    string prompt;
    if( args.size() > 0 && args[0].type() == SValue::Type::String ) {
        prompt = args[0].get_str();
    }
    if( quals.empty() ) {
        return glc().read_input( prompt );
    }
    string filecode = quals[0];
    File* file = glc().get_file( filecode );
    if( file == nullptr ) {
        return SValue::create_error( "File \"" + filecode + "\" not found." );
    }
    std::istream* input = file->get_input();
    assert( input != nullptr );
    string line;
    std::getline( *input, line );
    return SValue( line );
}

SValue glich::Script::at_load_blob()
{
    StdStrVec quals = get_qualifiers( GetToken::next );
    SValueVec args = get_args( GetToken::current );
    string filename;
    if( args.size() > 0 && args[0].type() == SValue::Type::String ) {
        filename = args[0].get_str();
    }
    else {
        return SValue::create_error( "Filename required." );
    }
    Blob blob;
    if( !blob.load( filename ) ) {
        return SValue::create_error( "Unable to load blob file." );
    }
    return SValue( blob );
}

SValue Script::at_filesys()
{
    StdStrVec quals = get_qualifiers( GetToken::next );
    SValueVec args = get_args( GetToken::current );
    return action_at_filesys( quals, args );
}

// Return an error value
// @error( message = "" )
SValue Script::at_error()
{
    SValueVec args = get_args( GetToken::next );
    string mess;
    if( args.size() > 0 && args[0].type() == SValue::Type::String ) {
        mess = args[0].get_str();
    }
    return SValue::create_error( mess );
}

SValue Script::at_string()
{
    SValueVec args = get_args( GetToken::next );
    if( args.empty() ) {
        return SValue::create_error( "Function @string requires one argument." );
    }
    return SValue( args[0].as_string());
}

SValue Script::at_quote()
{
    SValue value = at_string();
    return SValue( string_to_quote( value.get_str() ) );
}

SValue Script::at_field()
{
    SValueVec args = get_args( GetToken::next );
    if( args.empty() ) {
        return SValue::create_error( "Function @field requires one argument." );
    }

    Field field = f_invalid;
    bool success = false;
    SValue value = ( args[0].type() == SValue::Type::String ) ? glc().evaluate( args[0].get_str() ) : args[0];
    switch( value.type() )
    {
    case SValue::Type::field:
        return value;
    case SValue::Type::Number:
        field = num_to_field( value.get_number(), success );
        break;
    case SValue::Type::range:
    case SValue::Type::rlist:
        field = value.get_field( success );
        break;
    case SValue::Type::Float:
        field = std::round( value.get_float( success ) );
        break;
    default:
        field = f_invalid;
        break;
    }
    if( !success ) {
        if( args.size() > 1 ) {
            field = args[1].get_field( success );
        }
        if( !success ) {
            field = f_invalid;
        }
    }
    value.set_field( field );
    return value;
}

SValue Script::at_range()
{
    SValueVec args = get_args( GetToken::next );
    if( args.empty() ) {
        return SValue::create_error( "Function @range requires one argument." );
    }

    SValue value;
    Range rng;
    Field fld = f_invalid;
    bool success = false;
    if( args[0].type() == SValue::Type::String ) {
        value = glc().evaluate( args[0].get_str() );
    }
    else {
        value = args[0];
    }
    SValue::Type type = value.type();
    switch( type )
    {
    case SValue::Type::field:
        fld = value.get_field( success );
        rng = Range( fld, fld );
        break;
    case SValue::Type::Number:
        fld = num_to_field( value.get_number(), success );
        rng = Range( fld, fld );
        break;
    case SValue::Type::range:
    case SValue::Type::rlist:
        rng = value.get_range( success );
        break;
    default:
        break;
    }
    if( !success ) {
        if( args.size() > 1 ) {
            rng = args[1].get_range( success );
        }
        if( !success ) {
            rng = Range( 0, 0 );
        }
    }
    value.set_range( rng );
    return value;
}

SValue Script::at_rlist()
{
    SValueVec args = get_args( GetToken::next );
    if( args.empty() ) {
        return SValue::create_error( "Function @rlist requires one argument." );
    }

    SValue value;
    RList rlist;
    Field fld = 0;
    bool success = false;
    if( args[0].type() == SValue::Type::String ) {
        value = glc().evaluate( args[0].get_str() );
    }
    else {
        value = args[0];
    }
    SValue::Type type = value.type();
    switch( type )
    {
    case SValue::Type::field:
        fld = value.get_field( success );
        rlist = { {fld, fld } };
        break;
    case SValue::Type::Number:
        fld = num_to_field( value.get_number(), success );
        rlist = { { fld, fld } };
        break;
    case SValue::Type::range:
        rlist = { value.get_range( success ) };
        break;
    case SValue::Type::rlist:
        rlist = value.get_rlist( success );
        break;
    default:
        break;
    }
    if( !success ) {
        if( args.size() > 1 ) {
            rlist = args[1].get_rlist( success );
        }
        if( !success ) {
            rlist = RList();
        }
    }
    value.set_rlist( rlist );
    return value;
}

SValue Script::at_number()
{
    SValueVec args = get_args( GetToken::next );
    if( args.empty() ) {
        return SValue::create_error( "Function @number requires one argument." );
    }

    Num number = 0;
    bool success = false;
    SValue value = (args[0].type() == SValue::Type::String) ? glc().evaluate( args[0].get_str() ) : args[0];
    switch( value.type() )
    {
    case SValue::Type::field:
        number = field_to_num( args[0].get_field(), success );
        break;
    case SValue::Type::Number:
        return value;
    case SValue::Type::range:
    case SValue::Type::rlist:
    {
        Field field = value.get_field( success );
        if( success ) {
            number = field_to_num( field, success );
        }
    }
        break;
    case SValue::Type::Float:
        number = std::round( value.get_float( success ) );
        break;
    default:
        break;
    }
    if( !success ) {
        if( args.size() > 1 ) {
            number = args[1].get_number( success );
        }
        if( !success ) {
            number = 0;
        }
    }
    value.set_number( number );
    return value;
}

SValue Script::at_float()
{
    SValueVec args = get_args( GetToken::next );
    if( args.empty() ) {
        return SValue::create_error( "Function @field requires one argument." );
    }

    double dbl = std::numeric_limits<double>::quiet_NaN();
    bool success = false;
    SValue value = (args[0].type() == SValue::Type::String) ? glc().evaluate( args[0].get_str() ) : args[0];
    switch( value.type() )
    {
    case SValue::Type::field:
        dbl = field_to_double( value.get_field(), success );
        break;
    case SValue::Type::Number:
        dbl = num_to_double( value.get_number(), success );
        break;
    case SValue::Type::range:
    case SValue::Type::rlist:
        {
            Field field = value.get_field( success );
            if( success ) {
                dbl = field_to_double( field, success );
            }
        }
        break;
    case SValue::Type::Float:
        return value;
    default:
        break;
    }
    if( !success ) {
        if( args.size() > 1 ) {
            dbl = args[1].get_any_as_float( success );
        }
        if( !success ) {
            dbl = std::numeric_limits<double>::quiet_NaN();
        }
    }
    value.set_float( dbl );
    return value;
}

SValue Script::at_version()
{
    StdStrVec quals = get_qualifiers( GetToken::next );
    if( !quals.empty() ) {
        if( quals[0] == "number" ) {
            return glc_version_number;
        }
    }
    return glc_version;
}

SValue Script::do_at_property( const string& property )
{
    SValueVec args = get_args( GetToken::next );
    if( args.empty() ) {
        return SValue::create_error( "Function @span requires one argument." );
    }
    SValue value = args[0];
    value.property_op( SValue( property ) );
    return value;
}

SValue Script::at_global()
{
    StdStrVec quals = get_qualifiers( GetToken::next );
    if( quals.empty() ) {
        return SValue::create_error( "Global name required." );
    }
    SValue* vptr = glc().get_global_ptr( quals[0] );
    if( vptr == nullptr ) {
        return SValue::create_error( "Global name not found." );
    }
    return *vptr;
}

SValue Script::com_save_blob()
{
    StdStrVec quals = get_qualifiers( GetToken::next );
    SValueVec args = get_args( GetToken::current );
    Blob blob;
    if( args.size() > 0 && args[0].type() == SValue::Type::blob ) {
        blob = args[0].get_blob();
    }
    else {
        return SValue::create_error( "Blob required as 1st argument." );
    }
    string filename;
    if( args.size() > 1 && args[1].type() == SValue::Type::String ) {
        filename = args[1].get_str();
    }
    else {
        return SValue::create_error( "Filename required as 2nd argument." );
    }
    if( !blob.save( filename ) ) {
        return SValue::create_error( "Unable to save blob file." );
    }
    return SValue();
}

SValue Script::get_value_var( const string& name )
{
    bool success = false;
    SValue value = get_builtin_var( success, name );
    if( success ) {
        return value;
    }
    if( glc().is_named( name ) ) {
        return glc().get_named( name );
    }
    return SValue::create_error( "Variable \"" + name + "\" not found." );
}

SValue Script::get_builtin_var( bool& success, const std::string& name )
{
    success = false;
    return SValue();
}

// End of src/glc/glcScript.cpp
