/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicFormat.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Format class to control the formatting and parsing of dates.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     24th March 2023
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

#ifndef SRC_GLC_HICFORMAT_H_GUARD
#define SRC_GLC_HICFORMAT_H_GUARD

#include <glc/hicDefs.h>
#include "hicGrammar.h"
#include "hicHelper.h"


namespace glich {

    class Format
    {
    public:
        Format( const std::string& code, Grammar* gmr );
        virtual ~Format();

        virtual bool construct() { m_ok = true; return m_ok; }
        bool is_ok() const { return m_ok; }

        void set_user_input_str( const std::string str ) { m_input_str = str; }
        void set_user_output_str( const std::string str ) { m_output_str = str; }
        void set_style( FormatStyle style ) { m_style = style; }

        std::string get_code() const { return m_code; }
        Grammar* get_owner()  const { return m_owner; }

    private:
        std::string m_code;
        Grammar* m_owner;
        bool m_ok;
        FormatStyle m_style;
        std::string m_input_str;
        std::string m_output_str;
    };

}

#endif // SRC_GLC_HICFORMAT_H_GUARD