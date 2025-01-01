/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glcScriptStore.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     ScriptStore class definition header.
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
#ifndef SRC_GLC_GLCSTORE_H_GUARD
#define SRC_GLC_GLCSTORE_H_GUARD

#include "glcValue.h"

namespace glich {

    class Store {
    public:
        Store() : m_prev( nullptr ) {}
        Store( Store* ss ) : m_prev( ss ) {}

        Store* get_prev() const { return m_prev; }

        bool exists( const std::string& name ) const;
        bool create_local( const std::string& name );
        SValue get_local( const std::string& name );
        SValue* get_local_ptr( const std::string& name );
        bool update_local( const std::string& name, SValue& value );
        bool add_local( const std::string& name, SValue& value );

        void remove( const std::string& name ) { m_variables.erase( name ); }
        void clear() { m_variables.clear(); }
        bool is_level_zero() const { return m_prev == nullptr; }
        void set_prev( Store* prev ) { m_prev = prev; }

    private:
        SValueMap m_variables;
        Store* m_prev;
    };

}

#endif // SRC_GLC_GLCSTORE_H_GUARD
