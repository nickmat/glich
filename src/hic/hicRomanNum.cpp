/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        glc/glc/hicRomanNum.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Convert Roman Numerals to and from Fields
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     17th March 2023
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


#include "hicRomanNum.h"


using namespace glich;
using std::string;


string glich::get_roman_numerals( Field field, StrStyle style )
{
    // We can only convert numbers 1 to 4999 (4000 = "MMMM")
    if( field >= 5000 || field <= 0 ) {
        return field_to_string( field, Context::hics );
    }
    static const char* units[10]
        = { "", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX" };
    static const char* tens[10]
        = { "", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC" };
    static const char* hunds[10]
        = { "", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM" };
    static const char* thous[5]
        = { "", "M", "MM", "MMM", "MMMM" };

    size_t n = field;
    string result = string( thous[n / 1000] )
        + string( hunds[(n / 100) % 10] )
        + string( tens[(n / 10) % 10] )
        + string( units[n % 10] );

    if( style == StrStyle::lowercase ) {
        ascii_tolower( result );
    }
    return result;
}

string glich::get_roman_numerals_style( StrStyle style )
{
    return style == StrStyle::lowercase ? "[x]" : "[X]";
}

static Field get_roman_value( int ch, bool& decimal )
{
    switch( ch )
    {
    case 'I': case 'i': decimal = true;  return 1;
    case 'V': case 'v': decimal = false; return 5;
    case 'X': case 'x': decimal = true;  return 10;
    case 'L': case 'l': decimal = false; return 50;
    case 'C': case 'c': decimal = true;  return 100;
    case 'D': case 'd': decimal = false; return 500;
    case 'M': case 'm': decimal = true;  return 1000;
    default:  decimal = false; return f_invalid;
    }
}

Field glich::convert_roman_numerals( const string& num )
{
    Field result = 0, prev = 1001, curr;
    bool dec = false;
    for( auto it = num.begin(); it != num.end(); it++ ) {
        curr = get_roman_value( *it, dec );
        if( curr == f_invalid ) {
            return f_invalid;
        }
        if( curr >= prev ) {
            return f_invalid;
        }
        prev = curr;
        auto it2 = it + 1;
        if( it2 != num.end() ) {
            bool dec2;
            Field curr2 = get_roman_value( *it2, dec2 );
            if( curr2 == f_invalid ) {
                return f_invalid;
            }
            if( dec ) {
                if( curr == curr2 ) { // Repeated char
                    auto it3 = it2 + 1;
                    if( it3 != num.end() ) {
                        Field curr3 = get_roman_value( *it3, dec2 );
                        if( curr3 == curr ) {
                            result += curr;
                            it++;
                        }
                    }
                    result += curr;
                    it++;
                }
                else if( (curr == 1 && (curr2 == 5 || curr2 == 10)) ||
                    (curr == 10 && (curr2 == 50 || curr2 == 100)) ||
                    (curr == 100 && (curr2 == 500 || curr2 == 1000)) ) {
                    // Subtract from next.
                    result -= curr;
                    curr = curr2;
                    it++;
                }
            }
        }
        result += curr;
    }
    return result;
}


// End of src/glc/hicRomanNum.cpp
