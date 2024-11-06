/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/cal/calparse.cpp
 * Project:     Cal: Programmable Historical Calendar library.
 * Purpose:     Functions to help parse scripts.
 * Author:      Nick Matthews
 * Website:     http://historycal.org
 * Created:     20th September 2013
 * Copyright:   Copyright (c) 2013 ~ 2020, Nick Matthews.
 * Licence:     GNU GPLv3
 *
 *  The Cal library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The Cal library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the Cal library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

*/

#include "hicHelper.h"

#include "glcHelper.h"

#include <utf8/utf8api.h>

#include <cassert>


using namespace glich;
using std::string;


Field glich::str_to_dual2( Field dual1, const string& str2 )
{
    string str1 = field_to_string( dual1, Context::hics );
    if( str1.size() <= str2.size() ) {
        return str_to_field( str2 );
    }
    string result;
    string::const_iterator it1 = str1.end(), it2 = str2.end();
    while( it1 != str1.begin() ) {
        --it1;
        if( it2 != str2.begin() ) {
            --it2;
            result = *it2 + result;
        } else {
            result = *it1 + result;
        }
    }
    return str_to_field( result );
}

string glich::make_key( const string& str )
{
    string key;
    for( string::const_iterator it = str.begin() ; it != str.end() ; it++ ) {
        if( *it != ' ' ) {
            key += *it;
        }
    }
    return Utf8api::normal( key );
}

// Parse a string formated as 'scheme:format' into scheme and format.
bool glich::split_code( string* scheme, string* format, const string& codes )
{
    assert( scheme != nullptr );
    assert( format != nullptr );

    size_t pos = codes.find( ':' );
    if( pos != string::npos ) {
        *scheme = codes.substr( 0, pos );
        *format = codes.substr( pos + 1 );
    } else {
        *scheme = codes;
    }
    return true;
}

// End of src/glc/hicHelper.cpp
