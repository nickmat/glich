/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        glc/glc/glcHelper.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Helpful functions and strucs
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     13th February 2023
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

#ifndef SRC_GLC_GLCHELPER_H
#define SRC_GLC_GLCHELPER_H

#include <glc/glcDefs.h>


namespace glich {

    enum class StrStyle { undefined, lowercase, uppercase };
    enum class ShowInvalid { blank, qmark, context };

    Num str_to_num( const std::string& str );
    Num str_to_num( const std::string& str, bool& success );
    bool is_str_size_t( const std::string& str );
    double str_to_float( const std::string& str );
    // str_to_float refers to glich float. To avoid confusion add the following. 
    inline double str_to_double( const std::string& str ) { return str_to_float( str ); }
    double str_to_double( const std::string& str, bool& success );
    Field str_to_field( const std::string& str );
    Field str_to_field( const std::string& str, bool& success );
    Field num_to_field( Num num, bool& success );
    Num field_to_num( Field fld, bool& success );

    std::string bool_to_string( bool b );
    std::string float_to_string( double real );
    std::string field_to_string( 
        Field fld, Context ctx = Context::glich, ShowInvalid si = ShowInvalid::context );
    std::string range_to_string( Range rng, Context ctx = Context::glich );
    std::string rlist_to_string( RList rlist, Context ctx = Context::glich );

    bool is_name( const std::string& str );

    std::string ascii_tolower( const std::string& str );
    void ascii_tolower( std::string& str );
    std::string ascii_toupper( const std::string& str );
    void ascii_toupper( std::string& str );

    std::string dual_fields_to_str( Field field, Field dual );
    std::string get_ordinal_suffix( Field field, StrStyle style = StrStyle::lowercase );
    std::string get_ordinal_suffix_style( StrStyle style = StrStyle::lowercase );

    std::string get_left_padded( Field field, const std::string& specifier );
    std::string get_left_padded( Field field, const std::string& ch, size_t width );
    std::string get_left_pad_style(
        const std::string& fieldstyle, const std::string& specifier );

    std::string left_trim( const std::string& str );
    std::string right_trim( const std::string& str );
    std::string full_trim( const std::string& str );
    std::string get_first_word( const std::string& str, std::string* tail = nullptr, char sep = ' ' );

    inline bool u8_isspace( int ch ) { return (ch > 0 && isspace( ch )); }
    inline bool u8_isdigit( int ch ) { return (ch > 0 && isdigit( ch )); }

}

#endif // SRC_GLC_GLCHELPER_H
