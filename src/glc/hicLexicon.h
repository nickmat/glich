/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicLexicon.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Lexicon class to handle date element names.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     19th March 2023
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

#ifndef SRC_GLC_HICLEXICON_H_GUARD
#define SRC_GLC_HICLEXICON_H_GUARD

#include <glc/glc.h>
#include <glc/hicDefs.h>

namespace glich {

    class Token
    {
    public:
        Token( Field field, const std::string& str, const std::string& astr = "" )
            : m_field(field), m_word(str), m_abbrev(astr) {}
        Token() : m_field(f_invalid) {}

        Field get_field() const { return m_field; }
        std::string get_word() const { return m_word; }
        std::string get_abbrev() const { return m_abbrev; }

    private:
        Field       m_field;
        std::string m_word;
        std::string m_abbrev;
    };

    class Lexicon
    {
    public:
        enum class Pseudo { full, abbrev, ordsuffix };

        Lexicon( const std::string& code );

        void set_name( const std::string& name ) { m_name = name; }
        void set_inherit( const std::string& inherit );// { m_inherit = inherit; }
        void set_fieldname(const std::string& fname ) { m_fieldname = fname; }
        void set_lang(const std::string& lang ) { m_lang = lang; }
        void set_pseudo_names( const StdStrVec& pseudos );
        void add_token( Field value, const std::string& name, const std::string& abbrev );

        std::string get_code() const { return m_code; }
        std::string get_name() const { return m_name; }
        std::string get_fieldname() const;
        std::string get_lang() const;
        std::string get_pseudo_name( Pseudo style ) const;
        StdStrVec get_token_words() const;
        StdStrVec get_token_abbrev() const;
        void get_info( Lexicon_info* info ) const;
        Field find( const std::string& word ) const;
        std::string lookup( Field field, Pseudo style ) const;

    private:
        std::string  m_code;
        std::string  m_name;
        Lexicon*  m_inherit;
        std::string  m_fieldname;
        std::string  m_lang;
        std::string  m_pseudo_name;
        std::string  m_pseudo_a_name;
        std::map<std::string,Token> m_words;
        std::map<Field,Token> m_fields;
    };

}

#endif // SRC_GLC_HICLEXICON_H_GUARD