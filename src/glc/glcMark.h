/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glcMark.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Mark class used by the scripts mark statement.
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

#ifndef SRC_GLC_GLCMARK_H_GUARD
#define SRC_GLC_GLCMARK_H_GUARD

#include <glc/glcDefs.h>
#include "glcStore.h"


namespace glich {

    class Function;
    class Object;
    class File;

    class Mark
    {
    public:
        Mark( const std::string& name, Mark* prev );
        virtual ~Mark();

        static void set_zero_store( Store* store ) { s_zero_store = store; }

        std::string get_name() const { return m_name; }

        bool create_local( const std::string& name, Store* store );

        void add_global( const std::string& code ) { m_globals.push_back( code ); }
        void add_constant( const std::string& code ) { m_constants.push_back( code ); }
        void add_function( const std::string& code ) { m_functions.push_back( code ); }
        void add_command( const std::string& code ) { m_commands.push_back( code ); }
        void add_object( const std::string& code ) { m_objects.push_back( code ); }
        void add_module( const std::string& code ) { m_modules.push_back( code ); }
        void add_file( const std::string& code ) { m_files.push_back( code ); }

        void set_integer( Integer intgr ) { m_integer = intgr; }
        void set_write_text( WriteText wt ) { m_write_text = wt; }

        Integer get_integer() const { return m_integer; }
        WriteText get_write_text() const { return m_write_text; }

        void get_mark_glc_data( GlcMarkData& glcdata ) const;

    private:
        static inline Store*   s_zero_store = nullptr;

        std::string m_name;
        StdStrVec m_locals;
        StdStrVec m_globals;
        StdStrVec m_constants;
        StdStrVec m_functions;
        StdStrVec m_commands;
        StdStrVec m_objects;
        StdStrVec m_modules;
        StdStrVec m_files;
        Integer m_integer;
        WriteText m_write_text;
    };

}

#endif // SRC_GLC_GLCMARK_H_GUARD