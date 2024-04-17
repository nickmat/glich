/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glcObject.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Object class to handle script objects.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     4th March 2023
 * Copyright:   Copyright (c) 2023..24, Nick Matthews.
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

#ifndef SRC_GLC_GLCOBJECT_H_GUARD
#define SRC_GLC_GLCOBJECT_H_GUARD

#include <glc/glcDefs.h>
#include "glcValue.h"

namespace glich {

    

    class Object
    {
    public:
        Object( const std::string& ocode ) : m_ocode( ocode ) {}
        virtual ~Object();

        void set_value_names( const StdStrVec& vnames );

        std::string get_code() const { return m_ocode; }
        size_t get_min_size() const { return m_vnames.size() + 1; }
        size_t get_vindex( const std::string& str ) const;
        const NameIndexMap& get_vnames_map() const { return m_vnames; }

        bool add_function( SpFunction fun );
        Function* get_function( const std::string& fcode ) const;

    private:
        SpFunctionMap m_functions;
        std::string  m_ocode;
        NameIndexMap m_vnames; // map variable name to value index
    };

}

#endif // SRC_GLC_GLCOBJECT_H_GUARD
