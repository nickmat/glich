/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicDatePhrase.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Function to convert phrase string to script string.
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

#include "hicDatePhrase.h"

#include "glcHelper.h"

using namespace glich;
using std::string;


namespace {
    enum CastType { CT_date, CT_record, CT_none };

    string create_date_str( string& sig, string& date, CastType& ct )
    {
        string d = full_trim( date );
        if ( !d.empty() ) {
            string cast;
            switch ( ct )
            {
            case CT_date:
                d = "@date" + sig + "(\"" + d + "\")";
                break;
            case CT_record:
                d = "@record" + sig + "(\"" + d + "\")";
                break;
            case CT_none:
                break;
            }
        }
        date.clear();
        sig.clear();
        ct = CT_date;
        return d;
    }
}

// Convert a date phrase string to a script string.
string glich::parse_date_phrase( const string& str )
{
    string script, date, sig;
    string::const_iterator it, nit;
    CastType ct = CT_date;
    for( it = str.begin() ; it != str.end() ; it++ ) {
        switch( *it )
        {
        case '"':
            it++;
            while( it != str.end() && *it != '"' ) {
                date += *it;
                it++;
            }
            if( it != str.end() ) {
                it++;
            }
            break;
        case '[':  // Ignore comments in square brackets.
            for( int cnt = 1 ; it != str.end() && cnt != 0 ; it++ ) {
                if( *it == ']' ) {
                    --cnt;
                } else if( *it == '[' ) {
                    cnt++;
                }
            }
            break;
        case '.': case '/': // Must be followed by dot.
            nit = it + 1;
            if( nit != str.end() && (*nit == '.') ) {
                script += create_date_str( sig, date, ct );
                script += *it; // It's an operator.
                if( *it == '.' ) {
                    script += ".";
                }
                it++;       // Step over dot.
            }
            else {
                date += *it; // Treat & as part of date string.
            }
            break;
        case '|': case '\\': case '&': case '^': // Is doubled up?.
            nit = it + 1;
            if( nit != str.end() && (*nit == *it) ) {
                script += create_date_str( sig, date, ct );
                script += *it; // It's an operator.
                script += *nit;
                it++;       // Step over second.
            }
            else {
                date += *it; // Treat & as part of date string.
            }
            break;
        case '(': case ')': // Always recognised operators.
        case '!': case '+': case '*':
            script += create_date_str( sig, date, ct );
            script += *it;
            break;
        case ';': // Use instead of comma.
            script += create_date_str( sig, date, ct );
            script += ',';
            break;
        case '#':
            nit = it + 1;
            if ( nit != str.end() && ( *nit == '#' ) ) {
                // Use ## to mean record cast
                ct = CT_record;
            }
            date = full_trim( date );
            if ( date.empty() ) {
                if ( ct == CT_date ) {
                    ct = CT_none;
                }
            } else {
                sig = ".\"" + date + "\"";
                date.clear();
            }
            break;
        case '@':
            // TODO: date should be empty at this point, do error checks? 
            for ( ; it != str.end() ; it++ ) {
                date += *it;
                nit = it + 1;
                if ( *nit == '(' ) {
                    break;
                }
            }
            break;
        default:
            date += *it;
        }
        if( it == str.end() ) {
            break;
        }
    }
    script += create_date_str( sig, date, ct );
    return script;
}

// End of src/glc/hicDatePhrase.cpp file
