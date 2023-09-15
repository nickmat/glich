/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glcScript.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Run Glich script.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     5th February 2023
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

#ifndef SRC_GLC_GLCSCRIPT_H_GUARD
#define SRC_GLC_GLCSCRIPT_H_GUARD

#include <glc/glc.h>
#include "glcStore.h"
#include "glcTokenStream.h"

namespace glich {

    class Object;

    class Script {
    public:
        Script( Glich* db, std::istream& in, std::ostream& out );

        bool run();
        SValue evaluate() { return expr( GetToken::next ); }

        SValue run_script( std::string& script );

        static STokenStream* get_current_ts() { return s_current_ts; }

        SToken& current_token() { return m_ts.current(); }
        SToken& next_token() { return m_ts.next(); }
        std::string read_until( const std::string& name, const std::string& esc = std::string() )
            { return m_ts.read_until( name, esc ); }
        Glich* get_glich() { return m_glc; }

        bool error( const std::string& mess ) { return m_ts.error( mess ); }
        bool error_value( const SValue& value );
        static SValue create_error( const std::string& mess );

        bool statement();
        bool do_mark();
        bool do_if();
        bool do_do();
        bool do_set();
        bool do_let();
        bool do_assign( const std::string& name );
        bool do_write( const std::string& term = "" );
        bool do_writeln() { return do_write( "\n" ); }
        Function* create_function( const std::string& code );
        bool do_function();
        bool do_command();
        bool do_call();
        bool do_object();
        bool do_file();
        bool do_scheme();
        bool do_lexicon();
        bool do_grammar();
        bool do_format( Grammar* gmr = nullptr );

        SValue expr( GetToken get );
        SValue compare( GetToken get );
        SValue combine( GetToken get );
        SValue range( GetToken get );
        SValue sum( GetToken get );
        SValue term( GetToken get );
        SValue subscript( GetToken get );
        SValue primary( GetToken get );
        std::string get_name_or_primary( GetToken get );
        StdStrVec get_string_list( GetToken get );
        SValue get_object( GetToken get );

        SValue do_subscript( const SValue& left, const SValue& right );
        SValue do_dot( const SValue& left, const SValue& right );
        SValue text_cast();
        SValue date_cast();
        SValue record_cast();
        SValue element_cast();
        SValue error_cast();
        StdStrVec get_qualifiers( GetToken get );
        SValueVec get_args( SValue& value, GetToken get );
        SValue function_call();
        SValue run_function( Function* fun, const Object* obj = nullptr, const SValue* left = nullptr );
        SValue dot_mask( const Object* obj, const SValue* left );
        SValue command_call();
        SValue at_if();
        SValue at_read();
        SValue at_error();

        SValue get_value_var( const std::string& name );
        SValue get_cur_object();

        static STokenStream* s_current_ts;

        Glich* m_glc;
        STokenStream m_ts;
        std::ostream* m_out;
        std::ostream* m_err;
        const Object* m_cur_obj;
    };

} // namespace HistoryGen

#endif // SRC_GLC_GLCSCRIPT_H_GUARD
