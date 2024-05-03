/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicScript.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Create Scheme and associated class implimentations.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     17th March 2023
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

#ifndef SRC_GLC_HICSCRIPT_H
#define SRC_GLC_HICSCRIPT_H

#include "glcScript.h"

namespace glich {
    
    class Base;

    Scheme* do_create_scheme( Script& script, const std::string& code );
    Lexicon* do_create_lexicon( Script& script, const std::string& code );
    Grammar* do_create_grammar( Script& script, const std::string& code, const Base* base );
    bool do_create_format( Script& script, const std::string& code, Grammar* gmr );

    SValue hics_at( Script& script, bool& success, Object* obj, const std::string& fcode, const SValue& left );

    SValue at_text( Script& script );
    SValue at_date( Script& script );
    SValue at_record( Script& script );
    SValue at_element( Script& script );
    SValue at_phrase( Script& script );
    SValue at_leapyear( Script& script );
    SValue at_last( Script& script );
    SValue at_first( Script& script );
    SValue at_pseudo_in( Script& script );
    SValue at_pseudo_out( Script& script );
}

#endif // SRC_GLC_HICSCRIPT_H
