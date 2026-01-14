/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glcFunction.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Function class to handle script functions.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     8th February 2023
 * Copyright:   Copyright (c) 2023..2026, Nick Matthews.
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

#ifndef SRC_HG_HGFUNCTION_H_GUARD
#define SRC_HG_HGFUNCTION_H_GUARD

#include <glc/glcDefs.h>
#include "glcValue.h"

namespace glich {

    class Function
    {
    public:
        Function( const std::string& code ) : m_code( code ), m_line( 0 ) {}

        void set_script( const std::string& script ) { m_script = script; }
        void set_line( const std::string& module, int line ) { m_module = module; m_line = line; }
        void set_qualifiers( const StdStrVec& quals ) { m_qualifiers = quals; }
        void set_args( const StdStrVec& args ) { m_args = args; }
        void set_defaults( const SValueVec& defs ) { m_defs = defs; }

        std::string get_code() const { return m_code; }
        int get_line() const { return m_line; }
        std::string get_script() const { return m_script; }
        size_t get_qualifier_size() const { return m_qualifiers.size(); }
        std::string get_qualifier_name( size_t index ) const { return m_qualifiers[index]; }
        size_t get_arg_size() const { return m_args.size(); }
        std::string get_arg_name( size_t index ) const { return m_args[index]; }
        SValue get_default_value( size_t index ) const { return m_defs[index]; }

        SValue run( StdStrVec& qual, SValueVec& args, std::ostream& out ) const;
        SValue run( const SValue* left, StdStrVec& qual, SValueVec& args, std::ostream& out ) const;

    private:
        void create_locals( StdStrVec& qual, SValueVec& args ) const;

        std::string  m_code;
        std::string  m_module;
        int          m_line;
        std::string  m_script;
        StdStrVec    m_qualifiers;
        StdStrVec    m_args;
        SValueVec    m_defs;
    };

}

#endif // SRC_HG_HGFUNCTION_H_GUARD