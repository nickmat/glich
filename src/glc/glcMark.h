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

#include "glc/glcDefs.h"
#include "glcStore.h"


namespace glich {

    class Function;
    class Object;
    class File;
    class Lexicon;
    class Grammar;
    class Format;
    class Scheme;

    class Mark
    {
    public:
        Mark( const std::string& name, Mark* prev );
        ~Mark();

        static void set_zero_store( Store* store ) { s_zero_store = store; }

        std::string get_name() const { return m_name; }

        bool create_local( const std::string& name, Store* store );

        void add_function( const std::string& code ) { m_functions.push_back( code ); }
        void add_command( const std::string& code ) { m_commands.push_back( code ); }
        void add_object( const std::string& code ) { m_objects.push_back( code ); }
        void add_file( const std::string& code ) { m_files.push_back( code ); }
        void add_lexicon( Lexicon* lex ) { m_lexicons.push_back( lex ); }
        void add_grammar( Grammar* gmr ) { m_grammars.push_back( gmr ); }
        void add_format( Format* fmt ) { m_formats.push_back( fmt ); }

        std::string remove_next_lexicon();
        std::string remove_next_grammar();
        std::string remove_next_format();

        void set_integer( Integer intgr ) { m_integer = intgr; }
        void set_context( Context ct ) { m_context = ct; }
        void set_ischeme( Scheme* sch ) { m_ischeme = sch; }
        void set_oscheme( Scheme* sch ) { m_oscheme = sch; }

        Integer get_integer() const { return m_integer; }
        Context get_context() const { return m_context; }
        Scheme* get_ischeme() const { return m_ischeme; }
        Scheme* get_oscheme() const { return m_oscheme; }

        GlcMark get_mark_data( const Glich* glc ) const;

    private:
        static inline Store*   s_zero_store = nullptr;

        std::string            m_name;
        StdStrVec              m_locals;
        StdStrVec              m_functions;
        StdStrVec              m_commands;
        StdStrVec              m_objects;
        StdStrVec              m_files;
        std::vector<Lexicon*>  m_lexicons;
        std::vector<Grammar*>  m_grammars;
        std::vector<Format*>   m_formats;
        Integer m_integer;
        Context m_context;
        Scheme* m_ischeme;
        Scheme* m_oscheme;
    };

}

#endif // SRC_GLC_GLCMARK_H_GUARD