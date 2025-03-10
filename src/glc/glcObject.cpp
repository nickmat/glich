/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glcObject.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Object class to handle script objects.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     4th March 2023
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

#include "glcObject.h"

#include "glcFunction.h"

using namespace glich;
using std::string;
using std::vector;


Object::~Object()
{
}

void Object::set_value_names( const StdStrVec& vnames )
{
    m_values = vnames;
    size_t i = 1;
    for( auto& vname : vnames ) {
        m_vnames[vname] = i;
        i++;
    }
}

size_t Object::get_vindex( const string& str ) const
{
    if( m_vnames.count( str ) > 0 ) {
        return m_vnames.find(str)->second;
    }
    return 0;
}

bool Object::add_function( SpFunction fun )
{
    if( m_functions.count( fun->get_code() ) > 0 ) {
        return false;
    }
    m_functions[fun->get_code()] = fun;
    return true;
}

Function* Object::get_function( const std::string& fcode ) const
{
    if( m_functions.count( fcode ) == 0 ) {
        return nullptr;
    }
    return m_functions.find( fcode )->second.get();
}

string Object::get_list( const SValueVec& elements ) const
{
    string result;
    size_t size = std::max( elements.size() - 1, m_values.size() );
    for( size_t i = 0; i < size; i++ ) {
        if( i > 0 ) {
            result += "\n";
        }
        if( i < m_values.size() ) {
            result += m_values[i];
        }
        else {
            result += std::to_string( i );
        }
        if( i < elements.size() - 1 ) {
            const SValue& value = elements[i + 1];
            if( value.type() == SValue::Type::String ) {
                result += " = \"" + value.as_string() + "\"";
            }
            else {
                result += " = " + value.as_string();
            }
        }
        else {
            result += " = null";
        }
    }
    return result;
}

// End of src/glc/glcObject.cpp
