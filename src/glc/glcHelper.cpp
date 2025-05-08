/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        glc/glc/glcHelper.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Helpful functions and strucs
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     4th March 2023
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


#include "glcHelper.h"

#include "glcMath.h"

#include <cassert>
#include <cstring>
#include <limits>
#include <sstream>

using namespace glich;
using std::string;


Num glich::str_to_num( const string& str )
{
    if( str.empty() ) {
        return 0;
    }
    return strtoll( str.c_str(), nullptr, 10 );
}

Num glich::str_to_num( const std::string& str, bool& success )
{
    const char* cptr = str.c_str();
    char* cptr_end = nullptr;
    Num num = strtoll( cptr, &cptr_end, 10 );
    success = (cptr != cptr_end);
    return num;
}

double glich::str_to_float( const string& str )
{
    if( str.empty() ) {
        return 0.0;
    }
    return std::stod( str );
}

double glich::str_to_double( const std::string& str, bool& success )
{
    const char* cptr = str.c_str();
    char* cptr_end = nullptr;
    double dbl = strtod( cptr, &cptr_end );
    success = (cptr != cptr_end);
    if( !success ) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return dbl;
}

Field glich::str_to_field( const string& str )
{
    Num num = str_to_num( str );
    if( num <= f_maximum && num >= f_minimum ) {
        return static_cast<Field>(num);
    }
    return f_invalid;
}

Field glich::str_to_field( const std::string& str, bool& success )
{
    Num num = str_to_num( str, success );
    if( !success ) {
        return f_invalid;
    }
    if( num <= f_maximum && num >= f_minimum ) {
        return static_cast<Field>(num);
    }
    success = false;
    return f_invalid;
}

Field glich::num_to_field( Num num, bool& success )
{
    if( num < f_maximum && num > f_minimum ) {
        success = true;
        return static_cast<Field>(num);
    }
    success = false;
    return f_invalid;
}

double glich::num_to_double( Num num, bool& success )
{
    success = true;
    return static_cast<double>(num);
}

Num glich::field_to_num( Field fld, bool& success )
{
    success = true;
    if( fld < f_maximum && fld > f_minimum ) {
        return static_cast<Num>(fld);
    }
    success = false;
    return 0;
}

double glich::field_to_double( Field fld, bool& success )
{
    success = true;
    if( fld < f_maximum && fld > f_minimum ) {
        return static_cast<double>(fld);
    }
    switch( fld )
    {
    case f_invalid: return std::numeric_limits<double>::quiet_NaN();
    case f_minimum: return -std::numeric_limits<double>::infinity();
    case f_maximum: return std::numeric_limits<double>::infinity();
    }
    success = false;
    return std::numeric_limits<double>::quiet_NaN();
}

Field glich::double_to_field( double dbl, bool& success )
{
    success = true;
    if( std::isnan( dbl ) ) {
        return f_invalid;
    }
    if( dbl == std::numeric_limits<double>::infinity() ) {
        return f_maximum;
    }
    if( dbl == -std::numeric_limits<double>::infinity() ) {
        return f_minimum;
    }
    Num num = static_cast<Num>(std::round( dbl ));
    if( num < f_maximum && num > f_minimum ) {
        return static_cast<Field>(num);
    }
    success = false;
    return f_invalid;
}

string glich::bool_to_string( bool b )
{
    return b ? "true" : "false";
}

string glich::float_to_string( double real )
{
    if( std::isnan( real ) ) {
        return "nan";
    }
    if( is_zero( real ) ) {
        return "0.0";
    }
    std::stringstream ss;
    ss.precision( 6 );
    ss << std::fixed;
    ss << real;
    string res = ss.str();
    if( res.find( '.' ) != std::string::npos ) {
        size_t pos = res.length();
        while( res[pos - 1] == '0' ) {
            --pos;
        }
        if( res[pos - 1] == '.' && pos < res.length() ) {
            pos++;
        }
        res = res.substr( 0, pos );
    }
    return res;
}

string glich::field_to_string( Field fld, Context ctx, ShowInvalid si )
{
    switch( fld )
    {
    case f_invalid:
        return "?";
    case f_maximum:
        return (ctx == Context::glich) ? "+infinity" : "future";
    case f_minimum:
        return (ctx == Context::glich) ? "-infinity" : "past";
    }
    return std::to_string( fld );
}

string glich::range_to_string( Range rng, Context ctx )
{
    if( rng.m_beg == rng.m_end ) {
        return field_to_string( rng.m_beg, ctx );
    }
    return 
        field_to_string( rng.m_beg, ctx ) + ".." +
        field_to_string( rng.m_end, ctx );
}

string glich::rlist_to_string( RList rlist, Context ctx )
{
    if( rlist.empty() ) {
        return "empty";
    }
    std::string str;
    bool first = true;
    for( Range rng : rlist ) {
        if( !first ) {
            str += " | ";
        }
        str += range_to_string( rng, ctx );
        first = false;
    }
    return str;
}

bool glich::is_name( const std::string& str )
{
    if( str.empty() ) return false;
    bool first = true;
    for( auto ch : str ) {
        if( first ) {
            if( isdigit( ch ) ) return false;
            first = false;
        }
        if( isalnum( ch ) || ch == '_' || ch == ':' ) {
            continue;
        }
        return false;
    }
    // check for keywords
    const char* keywords[] =
    {
        "or", "and", "not", "div", "mod", "error"
    };
    for( auto word : keywords ) {
        if( std::strcmp( str.c_str(), word) == 0 ) {
            return false;
        }
    }
    return true;
}

// Add quote to start and end and double up any quotes in-between.
std::string glich::string_to_quote( const std::string& str )
{
    string quote = "\"";
    for( auto ch : str ) {
        quote += ch;
        if( ch == '"' ) {
            quote += ch;
        }
    }
    return quote + '\"';
}

// TODO: This should handle utf8
string glich::ascii_tolower( const string& str )
{
    string result;
    for( string::const_iterator it = str.begin(); it != str.end(); it++ ) {
        if( *it >= 'A' && *it <= 'Z' ) {
            result += *it + ('a' - 'A');
        }
        else {
            result += *it;
        }
    }
    return result;
}

// TODO: This should handle utf8
void glich::ascii_tolower( string& str )
{
    for( string::iterator it = str.begin(); it != str.end(); it++ ) {
        if( *it >= 'A' && *it <= 'Z' ) {
            *it = *it + ('a' - 'A');
        }
    }
}

// TODO: This should handle utf8
string glich::ascii_toupper( const string& str )
{
    string result;
    for( string::const_iterator it = str.begin(); it != str.end(); it++ ) {
        result += std::toupper( *it );
    }
    return result;
}

// TODO: This should handle utf8
void glich::ascii_toupper( string& str )
{
    for( string::iterator it = str.begin(); it != str.end(); it++ ) {
        *it = std::toupper( *it );
    }
}

string glich::dual_fields_to_str( Field field, Field dual )
{
    string result = field_to_string( field, Context::hics );
    string dualstr = field_to_string( dual, Context::hics );
    if( result.size() && dualstr.size() && dualstr != result ) {
        if( result.size() != dualstr.size() ) {
            return result + "/" + dualstr;
        }
        string suffix = "/";
        bool matched = true;
        for( string::iterator rit = result.begin(), dit = dualstr.begin()
            ; rit != result.end(); rit++, dit++
            ) {
            if( matched && *rit != *dit ) {
                matched = false;
            }
            if( !matched ) {
                suffix += *dit;
            }
        }
        result += suffix;
    }
    return result;
}


string glich::get_ordinal_suffix( Field field, StrStyle style )
{
    if( field <= f_minimum || field >= f_maximum ) {
        return "";
    }
    const char* result = "TH";
    if( (field % 100) < 4 || (field % 100) > 20 ) {
        switch( field % 10 )
        {
        case 1:
            result = "ST";
            break;
        case 2:
            result = "ND";
            break;
        case 3:
            result = "RD";
            break;
        }
    }
    string str( result );
    if( style != StrStyle::uppercase ) {
        ascii_tolower( str );
    }
    return str;
}

string glich::get_ordinal_suffix_style( StrStyle style )
{
    return style == StrStyle::uppercase ? "TH" : "th";
}

static string left_padded_str( const string& str, const string& ch, size_t size )
{
    assert( ch.size() > 0 );
    string result = str;
    while( result.size() < size ) {
        result = ch + result;
    }
    return result;
}

string glich::get_left_padded( Field field, const std::string& specifier )
{
    // TODO: This assumes fieldstyle and specifier are ASCII.
    // Rewrite for utf8.
    if( specifier.size() < 2 ) {
        return field_to_string( field );
    }
    size_t width = std::strtol( specifier.substr( 1 ).c_str(), nullptr, 10 );
    string ch = specifier.substr( 0, 1 );
    return get_left_padded( field, ch, width );
}

string glich::get_left_padded( Field field, const std::string& ch, size_t width )
{
    if( field == f_invalid ) {
        return "";
    }
    if( ch.empty() ) {
        return field_to_string( field );
    }
    bool neg = (field < 0 && ch == "0");
    if( neg ) {
        field = -field;
    }
    string result = field_to_string( field );
    if( result.size() < width ) {
        result = left_padded_str( result, ch, width );
    }
    if( neg ) {
        result = "-" + result;
    }
    return result;
}

string glich::get_left_pad_style(
    const string& fieldstyle, const string& specifier )
{
    // TODO: This assumes fieldstyle and specifier are ASCII.
    // Rewrite for utf8.
    if( specifier.size() < 2 ) {
        return fieldstyle;
    }
    size_t width = std::strtol( specifier.substr( 1 ).c_str(), NULL, 10 );
    if( width < 2 ) {
        return fieldstyle;
    }
    string ch = specifier.substr( 0, 1 );
    string style = fieldstyle;
    if( style.size() >= width ) {
        style = fieldstyle.substr( fieldstyle.size() - (width - 1), width - 1 );
    }
    return left_padded_str( style, ch, width );
}

string glich::left_trim( const string& str )
{
    size_t pos = str.find_first_not_of( " " );
    return (pos == string::npos) ? "" : str.substr( pos );
}

std::string glich::right_trim( const string& str )
{
    size_t pos = str.find_last_not_of( " " );
    return (pos == string::npos) ? "" : str.substr( 0, pos + 1 );
}

string glich::full_trim( const string& str )
{
    return right_trim( left_trim( str ) );
}

string glich::get_first_word( const string& str, string* tail, char sep )
{
    size_t pos = str.find( sep );
    // Note, tail may point str (ie, str and tail could be the same),
    // so get result before setting tail.
    string result = str.substr( 0, pos );
    if( tail ) {
        *tail = (pos == string::npos) ? "" : left_trim( str.substr( pos + 1 ) );
    }
    return result;
}

bool glich::split_string( string& first, string& tail, const string& input, char separator )
{
    size_t pos = input.find( separator );
    if( pos != string::npos ) {
        first = input.substr( 0, pos );
        tail = input.substr( pos + 1 );
    }
    else {
        first = input;
        tail = string();
        return false;
    }
    return true;
}


// End of src/glc/glcHelper.cpp
