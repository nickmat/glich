/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glcFunction.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Function class to handle script functions.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     11th April 2024
 * Copyright:   Copyright (c) 2024..2025, Nick Matthews.
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


#include "glcFunction.h"

#include "glcObject.h"
#include "glcTokenStream.h"
#include "glcScript.h"

#include <cassert>
#include <sstream>

using namespace glich;
using std::string;


SValue Function::run( StdStrVec& qual, SValueVec& args, std::ostream& out ) const
{
    std::istringstream iss( m_script );
    glc().push_store();
    glc().create_local( "result" );
    create_locals( qual, args );
    glc().run( iss, out, m_line );
    SValue value = glc().get_local( "result" );
    glc().pop_store();
    return value;
}


SValue Function::run( const SValue* left, StdStrVec& qual, SValueVec& args, std::ostream& out ) const
{
    std::istringstream iss( m_script );
    glc().push_store();
    glc().create_local( "result" );
    glc().create_local( "this" );
    SValue value = *left;
    glc().update_local( "this", value );

    assert( left != nullptr );
    string ocode = left->get_object_code();
    assert( !ocode.empty() );
    const Object* obj = glc().get_object( ocode );
    assert( obj != nullptr );

    const Object* prev_obj = glc().set_cur_object( obj );
    create_locals( qual, args );
    glc().run( iss, out, m_line );
    value = glc().get_local( "result" );
    glc().pop_store();
    glc().set_cur_object( prev_obj );
    return value;
}

void Function::create_locals( StdStrVec& qual, SValueVec& args ) const
{
    SValue value;
    for( size_t i = 0; i < m_qualifiers.size(); i++ ) {
        glc().create_local( m_qualifiers[i] );
        if( i < qual.size() ) {
            value = qual[i];
        }
        else {
            value = "";
        }
        glc().update_local( m_qualifiers[i], value );
    }
    for( size_t i = 0; i < m_args.size(); i++ ) {
        glc().create_local( m_args[i] );
        if( i < args.size() && args[i].type() != SValue::Type::Null ) {
            value = args[i];
        }
        else if( i < m_defs.size() ) {
            value = m_defs[i];
        }
        else {
            value = SValue();
        }
        glc().update_local( m_args[i], value );
    }
}

// End of src/glc/glcFunction.cpp file
