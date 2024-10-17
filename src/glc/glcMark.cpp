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

#include <glc/glc.h>

#include "glcFile.h"
#include "glcFunction.h"
#include "glcObject.h"

#include "hicScheme.h" // <<======<<<< REMOVE!!

#include <cassert>

using namespace glich;
using std::string;
using std::vector;


Mark::Mark( const string& name, Mark* prev )
    : m_name( name ), m_integer( Integer::number ), m_context( Context::glich )
{
    if( prev != nullptr ) {
        m_integer = prev->get_integer();
        m_context = prev->get_context();
    }
}

Mark::~Mark()
{
    for( auto& var : m_locals ) {
        s_zero_store->remove( var );
    }
    for( auto& name : m_globals ) {
        glc().remove_global( name );
    }
    for( auto& name : m_constants ) {
        glc().remove_constant( name );
    }
    for( auto& code : m_functions ) {
        glc().remove_function( code );
    }
    for( auto& code : m_commands ) {
        glc().remove_command( code );
    }
    for( auto code : m_objects ) {
        glc().remove_object( code );
    }
    for( auto code : m_modules ) {
        glc().remove_module( code );
    }
    for( auto code : m_files ) {
        glc().remove_file( code );
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

void Mark::get_mark_glc_data( GlcMarkData& mark ) const
{
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
    for( auto code : m_files ) {
        data.name = code;
        data.value = string();
        mark.file.push_back( data );
    }
    for( auto object : m_objects ) {
        Object* obj = glc().get_object( object );
        data.name = object;
        data.value = string();
        mark.obj.push_back( data );
    }
    for( auto& local : m_locals ) {
        SValue value = glc().get_local( local );
        data.type = value.type_str();
        data.name = local;
        data.value = value.as_string();
        mark.var.push_back( data );
    }
}


// End of src/glc/glcMark.cpp
