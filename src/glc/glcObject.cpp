/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glcObject.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Object class to handle script objects.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     4th March 2023
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

#include "glcObject.h"

using namespace glich;
using std::string;
using std::vector;


void Object::set_value_names( const StdStrVec& vnames )
{
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

// End of src/glc/glcValue.cpp
