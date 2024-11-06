/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        glc/glc/hicFormatIso.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     FormatIso class to control ISO 8601 formatting and parsing.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     11th July 2023
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

#include "hicFormatIso.h"

#include "glcHelper.h"
#include "glcMath.h"
#include "hicGregorian.h"
#include "hicIsoOrdinal.h"
#include "hicIsoWeek.h"
#include "hicRecord.h"
#include "hicScheme.h"

#include <cassert>

using namespace glich;
using std::string;


FormatIso::FormatIso( const string& code, Grammar& gmr, const StdStrVec& rules )
    : Format( code, gmr, FmtRules::Iso8601 ), m_daterep(DateRep::gregorian), m_extended(true),
    m_yplusminus(false), m_yminus(false), m_dateset(false), m_ydigits(4)
{
    m_shorthand = true;
    for( size_t i = 1 ; i < rules.size() ; i++ ) {
        if( rules[i] == "caldate" ) {
            m_daterep = DateRep::gregorian;
        } else if( rules[i] == "week" ) {
            m_daterep = DateRep::week;
        } else if( rules[i] == "ordinal" ) {
            m_daterep = DateRep::ordinal;
        } else if( rules[i] == "basic" ) {
            m_extended = false;
        } else if( rules[i] == "extended" ) {
            m_extended = true;
        } else if( rules[i] == "sign" ) {
            m_yplusminus = true;
            m_yminus = false;
        } else if( rules[i] == "minus" ) {
            m_yplusminus = false;
            m_yminus = true;
        } else if( rules[i] == "nosign" ) {
            m_yplusminus = false;
            m_yminus = false;
        } else if( rules[i] == "year4" ) {
            m_ydigits = 4;
        } else if( rules[i] == "year5" ) {
            m_ydigits = 5;
            m_yplusminus = true;
        } else if( rules[i] == "year6" ) {
            m_ydigits = 6;
            m_yplusminus = true;
        } else if( rules[i] == "dateset" ) {
            m_dateset = true;
        }
    }
    string input, output, rep, sign, sep;
    if( m_yplusminus ) {
        sign = "\302\261";
    } else if( m_yminus ) {
        sign = "-";
    }
    if( m_ydigits == 5 ) {
        sign += "Y";
    } else if( m_ydigits == 6 ) {
        sign += "YY";
    }
    if( m_extended ) {
        sep = "-";
    }

    if( m_daterep == DateRep::gregorian ) {
        output = sign + "YYYY" + sep + "MM" + sep + "DD";
        rep = "Date";
    } else if( m_daterep == DateRep::week ) {
        output = sign + "YYYY" + sep + "Www" + sep + "D";
        rep = "Week";
    } else { // assumed DR_ordinal
        output = sign + "YYYY" + sep + "DDD";
        rep = "Ordinal";
    }
    input = "ISO:8601 " + rep;
    if( !sign.empty() ) {
        input += " " + sign + "YYYY";
    }
    if( m_dateset ) {
        output = "[" + output + "]";
    }
    set_user_input_str( input );
    set_user_output_str( output );
}

std::string FormatIso::get_text_output( Record& record ) const
{
    string str = get_masked_output( record, nullptr );
    if( m_dateset ) {
        return "[" + str + "]";
    }
    return str;
}

BoolVec FormatIso::get_reveal( Record& rec1, Record& rec2 ) const
{
    const Base& base = rec1.get_base();
    XIndexVec xref( base.record_size() );
    for( size_t i = 0; i < xref.size(); i++ ) {
        xref[i] = i;
    }
    return rec1.mark_balanced_fields( rec2, xref, base.record_size() );
}

string FormatIso::get_revealed_text( Record& record, BoolVec& reveal ) const
{
    // This format only works with Gregorian, ISO Week or ISO Ordinal schemes
    // so one of the following will valid and the other two nullptr.
    const Gregorian* greg = dynamic_cast<const Gregorian*>(&record.get_base());
    const IsoWeek* isow = nullptr;
    const IsoOrdinal* isoo = nullptr;
    if( greg == nullptr ) {
        isow = dynamic_cast<const IsoWeek*>(&record.get_base());
        if( isow == nullptr ) {
            isoo = dynamic_cast<const IsoOrdinal*>(&record.get_base());
            if( isoo == nullptr ) {
                return "";
            }
        }
    }
    string str = output_year( record.get_revealed_field( 0, reveal ) );
    if( str.empty() ) {
        return "";
    }
    Field jdn = f_invalid;
    if( m_daterep == DateRep::ordinal ) {
        Field oday = f_invalid;
        if( isoo == nullptr ) {
            jdn = record.get_jdn();
            IsoOrdinal::from_jdn( nullptr, &oday, jdn );
        }
        else {
            oday = record.get_revealed_field( 1, reveal );
        }
        if( oday != f_invalid ) {
            if( m_extended ) {
                str += "-";
            }
            str += get_left_padded( oday, "0", 3 );
        }
        return str;
    }
    if( m_daterep == DateRep::week ) {
        Field week = f_invalid, wday = f_invalid;
        if( isow == nullptr ) {
            jdn = record.get_jdn();
            IsoWeek::from_jdn( nullptr, &week, &wday, jdn );
        }
        else {
            week = record.get_revealed_field( 1, reveal );
            wday = record.get_revealed_field( 2, reveal );
        }
        if( week != f_invalid ) {
            if( m_extended ) {
                str += "-";
            }
            str += "W" + get_left_padded( week, "0", 2 );
            if( wday != f_invalid ) {
                if( m_extended ) {
                    str += "-";
                }
                str += get_left_padded( wday, "0", 1 );
            }
        }
        return str;
    }
    if( m_daterep != DateRep::gregorian ) {
        return "";
    }
    Field year = f_invalid, month = f_invalid, day = f_invalid;
    if( greg == nullptr ) {
        jdn = record.get_jdn();
        gregorian_from_jdn( &year, &month, &day, jdn );
    }
    else {
        month = record.get_revealed_field( 1, reveal );
        day = record.get_revealed_field( 2, reveal );
    }
    if( month != f_invalid ) {
        if( m_extended ) {
            str += "-";
        }
        str += get_left_padded( month, "0", 2 );
        if( day != f_invalid ) {
            if( m_extended ) {
                str += "-";
            }
            str += get_left_padded( day, "0", 2 );
        }
    }
    return str;
}

std::string glich::FormatIso::get_date_text( const std::string& str ) const
{
    if( m_dateset ) {
        return "[" + str + "]";
    }
    return str;
}

Range FormatIso::string_to_range( const Base& base, const string& input, FunctionData* fdata ) const
{
    Range range;
    Record rec1( base ), rec2( base );
    if( set_input( rec1, input, Boundary::Begin ) && set_input( rec2, input, Boundary::End ) ) {
        range = { rec1.get_jdn(), rec2.get_jdn() };
        if( range.is_valid() ) {
            return range;
        }
    }
    return Range();
}

bool FormatIso::set_input( Record& record, const string& input, Boundary rb ) const
{
    size_t field_size = 3;
    // This format only works with Gregorian, ISO Week or ISO Ordinal schemes
    // so one of the following will valid and the other two NULL.
    const Gregorian* greg = dynamic_cast<const Gregorian*>(&record.get_base());
    const IsoWeek* isow = nullptr;
    const IsoOrdinal* isoo = nullptr;
    if( greg == nullptr ) {
        isow = dynamic_cast<const IsoWeek*>(&record.get_base());
        if( isow == nullptr ) {
            isoo = dynamic_cast<const IsoOrdinal*>(&record.get_base());
            if( isoo == nullptr ) {
                return false;
            }
            field_size = 2;
        }
    }

    FieldVec fields( field_size, f_invalid );
    DateRep instyle;
    if( input[0] == 'R' ) {
        instyle = recurring_interval( fields, input, rb );
    }
    else if( input.find( '/' ) != string::npos ) {
        instyle = interval( fields, input, rb );
    }
    else {
        instyle = datetime( fields, input, rb );
    }

    if( instyle == DateRep::null ) {
        return false;
    }

    Field jdn = f_invalid;
    if( instyle == DateRep::ordinal ) {
        // If there is no day number, it will be treated as Gregorian
        assert( fields[1] != f_invalid );
        if( isoo ) {
            record.set_fields( fields );
            return true;
        }
        jdn = IsoOrdinal::to_jdn( fields[0], fields[1] );
    }
    if( instyle == DateRep::week && isow == nullptr ) {
        // convert to jdn
        IsoWeek wbase;
        Record rec( wbase, fields );
        if( rb == Boundary::Begin ) {
            rec.complete_fields_as_beg();
        }
        else if( rb == Boundary::End ) {
            rec.complete_fields_as_end();
        }
        else {
            rec.set_fields( fields );
        }
        jdn = rec.get_jdn();
        if( jdn == f_invalid ) {
            return false;
        }
    }
    if( instyle == DateRep::gregorian && greg == nullptr ) {
        // convert to jdn
        Gregorian gbase;
        Record rec( gbase, fields );
        if( rb == Boundary::Begin ) {
            rec.complete_fields_as_beg();
        }
        else if( rb == Boundary::End ) {
            rec.complete_fields_as_end();
        }
        jdn = rec.get_jdn();
        if( jdn == f_invalid ) {
            return false;
        }
    }

    if( jdn != f_invalid ) {
        record.set_jdn( jdn );
        return true;
    }

    record.set_fields( fields );
    if( rb == Boundary::Begin ) {
        return record.complete_fields_as_beg();
    }
    else if( rb == Boundary::End ) {
        return record.complete_fields_as_end();
    }
    return true;
}

string FormatIso::range_to_string( const Scheme& sch, const Range& range ) const
{
    string str = range_to_str( sch.get_base(), range);
    if( m_dateset ) {
        return "[" + str + "]";
    }
    return str;
}

string FormatIso::rlist_to_string( const Scheme& sch, const RList& rlist ) const
{
    string str;
    for( size_t i = 0; i < rlist.size(); i++ ) {
        if( i > 0 ) {
            str += (m_dateset ? "," : " | ");
        }
        str += range_to_str( sch.get_base(), rlist[i]);
    }
    if( m_dateset ) {
        return "[" + str + "]";
    }
    return str;
}

std::string glich::FormatIso::jdn_to_str( const Base& base, Field jdn ) const
{
    Record rec( base, jdn );
    return get_masked_output( rec );
}

std::string glich::FormatIso::range_to_str( const Base& base, const Range& range ) const
{
    if( range.m_beg == range.m_end ) {
        return jdn_to_str( base, range.m_beg );
    }
    string str1, str2;
    if( range.m_beg == f_minimum || range.m_end == f_maximum ) {
        str1 = jdn_to_str( base, range.m_beg );
        str2 = jdn_to_str( base, range.m_end );
    }
    else {
        Record rec1( base, range.m_beg );
        Record rec2( base, range.m_end );

        XIndexVec xref( base.record_size() );
        for( size_t i = 0; i < xref.size(); i++ ) {
            xref[i] = i;
        }
        BoolVec mask = rec1.mark_balanced_fields( rec2, xref, base.record_size() );
        str1 = get_masked_output( rec1, &mask );
        str2 = get_masked_output( rec2, &mask );
        if( str1 == str2 ) {
            return str1;
        }
    }
    string sep = m_dateset ? ".." : "/";
    return str1 + sep + str2;
}

string FormatIso::get_masked_output( const Record& record, const BoolVec* mask ) const
{
    // This format only works with Gregorian, ISO Week or ISO Ordinal schemes
    // so one of the following will valid and the other two nullptr.
    const Gregorian* greg = dynamic_cast<const Gregorian*>( &record.get_base() );
    const IsoWeek* isow = nullptr;
    const IsoOrdinal* isoo = nullptr;
    if( greg == nullptr ) {
        isow = dynamic_cast<const IsoWeek*>( &record.get_base() );
        if( isow == nullptr ) {
            isoo = dynamic_cast<const IsoOrdinal*>( &record.get_base() );
            if( isoo == nullptr ) {
                return "";
            }
        }
    }
    string str = output_year( record.get_field( 0, mask ) );
    if( str.empty() ) {
        return "";
    }
    Field jdn = f_invalid;
    if( m_daterep == DateRep::ordinal ) {
        Field oday = f_invalid;
        if( isoo == nullptr ) {
            jdn = record.get_jdn();
            IsoOrdinal::from_jdn( nullptr, &oday, jdn );
        } else {
            oday = record.get_field( 1, mask );
        }
        if( oday != f_invalid ) {
            if( m_extended ) {
                str += "-";
            }
            str += get_left_padded( oday, "0", 3 );
        }
        return str;
    }
    if( m_daterep == DateRep::week ) {
        Field week = f_invalid, wday = f_invalid;
        if( isow == nullptr ) {
            jdn = record.get_jdn();
            IsoWeek::from_jdn( nullptr, &week, &wday, jdn );
        } else {
            week = record.get_field( 1, mask );
            wday = record.get_field( 2, mask );
        }
        if( week != f_invalid ) {
            if( m_extended ) {
                str += "-";
            }
            str += "W" + get_left_padded( week, "0", 2 );
            if( wday != f_invalid ) {
                if( m_extended ) {
                    str += "-";
                }
                str += get_left_padded( wday, "0", 1 );
            }
        }
        return str;
    }
    if( m_daterep != DateRep::gregorian ) {
        return "";
    }
    Field year = f_invalid, month = f_invalid, day = f_invalid;
    if( greg == nullptr ) {
        jdn = record.get_jdn();
        gregorian_from_jdn( &year, &month, &day, jdn );
    } else {
        month = record.get_field( 1, mask );
        day = record.get_field( 2, mask );
    }
    if( month != f_invalid ) {
        if( m_extended ) {
            str += "-";
        }
        str += get_left_padded( month, "0", 2 );
        if( day != f_invalid ) {
            if( m_extended ) {
                str += "-";
            }
            str += get_left_padded( day, "0", 2 );
        }
    }
    return str;
}

string FormatIso::output_year( Field year ) const
{
    string sign;
    if( year < 0 ) {
        if( !m_yplusminus && !m_yminus ) {
            return "";
        }
        sign = '-';
        year = -year;
    } else {
        if( m_yplusminus ) {
            sign = '+';
        }
    }
    string value = get_left_padded( year, "0", m_ydigits );
    if( value.length() != m_ydigits ) {
        return "";
    }
    return sign + value;
}

FormatIso::DateRep FormatIso::recurring_interval( FieldVec& fields, const string& input, Boundary rb ) const
{
    return DateRep::null;
}

FormatIso::DateRep FormatIso::interval( FieldVec& fields, const string& input, Boundary rb ) const
{
    size_t pos = input.find( '/' );
    string in;
    if( rb == Boundary::Begin ) {
        in = input.substr( 0, pos );
    } else if( rb == Boundary::End ) {
        in = input.substr( pos + 1 );
    }
    if( in.empty() ) {
        return DateRep::null;
    }
    return datetime( fields, in, rb );
}

FormatIso::DateRep FormatIso::datetime( FieldVec& fields, const string& input, Boundary rb ) const
{
    // Remove any time elements.
    size_t pos = input.find( 'T' );
    char start = input[0];
    bool neg = ( start == '-' );

    string str;
    if( neg || start == '+' ) {
        str = input.substr( 1, pos );
    } else {
        str = input.substr( 0, pos );
    }

    if( str.length() == m_ydigits - 2 ) {
        if( rb == Boundary::Begin ) {
            str += "00";
        } else if( rb == Boundary::End ) {
            str += "99";
        }
    }
    if( str.length() == m_ydigits - 1 ) {
        if( rb == Boundary::Begin ) {
            str += "0";
        } else if( rb == Boundary::End ) {
            str += "9";
        }
    }
    if( str.length() < m_ydigits ) {
        return DateRep::null;
    }
    
    fields[0] = str_to_field( str.substr( 0, m_ydigits ) );
    if( neg ) {
        fields[0] *= -1;
    }
    str = str.substr( m_ydigits );

    if( str[0] == '-' ) {
        str = str.substr( 1 );
    }

    if( str[0] == 'W' ) {
        return weekdate( fields, str, rb );
    }
    if( str.length() == 3 ) {
        // Ordinal day number
        fields[1] = str_to_field( str );
        return DateRep::ordinal;
    }

    if( str.length() >= 2 ) {
        fields[1] = str_to_field( str.substr( 0, 2 ) );
        str = str.substr( 2 );
    }

    if( str[0] == '-' ) {
        str = str.substr( 1 );
    }
    if( str.length() == 2 ) {
        fields[2] = str_to_field( str );
        str = str.substr( 2 );
    }
    if( str.empty() ) {
        return DateRep::gregorian;
    }
    return DateRep::null;
}

// Note, fields[0] must already be filled in with the year and
// str starts with a 'W' character.
FormatIso::DateRep FormatIso::weekdate( FieldVec& fields, const string& input, Boundary rb ) const
{
    string str = input.substr( 1 ); // Remove the 'W'.

    if( str.length() >= 2 ) {
        fields[1] = str_to_field( str.substr( 0, 2 ) );
        str = str.substr( 2 );
    }

    if( str[0] == '-' ) {
        str = str.substr( 1 );
    }
    if( str.length() == 1 ) {
        fields[2] = str_to_field( str );
        str = str.substr( 1 );
    }
    if( str.empty() ) {
        return DateRep::week;
    }
    return DateRep::null;
}

RList FormatIso::string_set_to_rlist( const Base& base, const string& input ) const
{
    RList rlist;
    if( input.size() < 6 ) {
        return rlist;
    }
    string str = input.substr( 1, input.size() - 2 ); // Strip '[' .. ']'
    string rangestr, begval, endval;
    size_t pos1;
    do {
        pos1 = str.find( ',' );
        rangestr = str.substr( 0, pos1 );
        size_t pos2 = rangestr.find( ".." );
        if( pos2 == string::npos ) {
            // single value
            begval = endval = rangestr;
        } else {
            // start and end
            begval = rangestr.substr( 0, pos2 );
            endval = rangestr.substr( pos2 + 2 );
        }
        Record rec1( base ), rec2( base );
        bool ret1 = true, ret2 = true;
        Range range;
        if( begval.empty() ) {
            range.m_beg = f_minimum;
        } else {
            Record rec( base );
            ret1 = set_input( rec, begval, Boundary::Begin );
            range.m_beg = rec.get_jdn();
        }
        if( endval.empty() ) {
            range.m_end = f_maximum;
        } else {
            Record rec( base );
            ret2 = set_input( rec, endval, Boundary::End );
            range.m_end = rec.get_jdn();
        }
        if( ret1 && ret2 && range.m_beg != f_invalid && range.m_end != f_invalid ) {
            rlist.push_back( range );
        }
        str = str.substr( pos1 + 1 );
    } while( pos1 != string::npos );
    return op_set_well_order( rlist );
}

StringPairVec FormatIso::string_to_stringpair( string& text ) const
{
    if( text.size() >= 2 && text[0] == '[' && text[text.size() - 1] == ']' ) {
        text = text.substr( 1, text.size() - 2 );
    }
    StringPair pair;
    StringPairVec pairs;
    for( ;;) {
        size_t pos1 = text.find( ',' );
        string rangestr = text.substr( 0, pos1 );
        size_t pos2 = rangestr.find( '..' );
        if( pos2 == string::npos ) {
            pair.first = rangestr;
            pair.second = string();
        }
        else {
            pair.first = rangestr.substr( 0, pos2 );
            pair.second = rangestr.substr( pos2 + 2 );
        }
        pairs.push_back( pair );
        if( pos1 == string::npos ) {
            break;
        }
        text = text.substr( pos1 + 1 );
    }
    return pairs;
}

// End of glc/glc/hicFormatIso.cpp file
