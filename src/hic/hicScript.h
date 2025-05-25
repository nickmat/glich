/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicScript.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Create Scheme and associated class implimentations.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     17th March 2023
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

#ifndef SRC_GLC_HICSCRIPT_H
#define SRC_GLC_HICSCRIPT_H

#include "glcScript.h"
#include <glc/hicDefs.h>

namespace glich {
    
    class Base;
    struct FunctionData;

    class HicScript : public Script
    {
    public:
        HicScript( std::istream& in, std::ostream& out ) : Script( in, out ) {}
        ~HicScript() {}

        bool statement() override;
        SValue builtin_function_call( bool& success, const std::string& name ) override;
        SValue get_builtin_var( bool& success, const std::string& name ) override;

        bool do_scheme();
        bool do_lexicon();
        bool do_grammar();
        bool do_format( Grammar* gmr = nullptr );

        SValue do_object_at( bool& success, Object* obj, const std::string& fcode, const SValue& left ) override;

    private:
        Scheme* do_create_scheme( const std::string& code );
        Lexicon* do_create_lexicon( const std::string& code );
        Grammar* do_create_grammar( const std::string& code, const Base* base );
        bool do_create_format( const std::string& code, Grammar* gmr );

        Base* do_base( const std::string& code );
        Base* do_base_hybrid( const std::string& hscode );
        bool do_lexicon_tokens( Lexicon* voc );
        void do_grammar_lexicons( Grammar* gmr );
        bool do_grammar_alias( Grammar* gmr );
        bool do_grammar_function( Grammar* gmr );
        bool do_grammar_use( Grammar* gmr );
    };

}

#endif // SRC_GLC_HICSCRIPT_H
