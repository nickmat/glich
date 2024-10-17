/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicMark.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     HicMark class used by the hics extension.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     16th October 2024
 * Copyright:   Copyright (c) 2024, Nick Matthews.
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

#ifndef SRC_GLC_HICMARK_H_GUARD
#define SRC_GLC_HICMARK_H_GUARD

#include "glcMark.h"
#include <glc/hicDefs.h>


namespace glich {

    class Lexicon;
    class Grammar;
    class Format;
    class Scheme;

    class HicMark : public Mark {
    public:
        HicMark( const std::string& name, HicMark* prev );
        ~HicMark();
   
        void add_lexicon( const std::string& code ) { m_lexicons.push_back( code ); }
        void add_grammar( const std::string& code ) { m_grammars.push_back( code ); }
        void add_format( const std::string& code ) { m_formats.push_back( code ); }

        void set_ischeme( Scheme* sch ) { m_ischeme = sch; }
        void set_oscheme( Scheme* sch ) { m_oscheme = sch; }

        Scheme* get_ischeme() const { return m_ischeme; }
        Scheme* get_oscheme() const { return m_oscheme; }

        void get_mark_hic_data( HicMarkData& hicdata ) const;

    private:
        StdStrVec m_lexicons;
        StdStrVec m_grammars;
        StdStrVec m_formats;
        Scheme* m_ischeme;
        Scheme* m_oscheme;
    };

}

#endif // SRC_GLC_HICMARK_H_GUARD