/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        glc/glc/hicFormatText.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     FormatText class to control formatting and parsing of dates.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     21st March 2023
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

#include "hicFormatText.h"

#include "glcHelper.h"
#include "glcMath.h"
#include "hicBase.h"
#include "hicElement.h"
#include "hicRecord.h"

#include <algorithm>
#include <cassert>

using namespace glich;
using std::string;


FormatText::FormatText( const string& code, Grammar& gmr )
: Format( code, gmr )//, m_separators(":,"), m_sig_rank_size(0), m_shorthand(true)
{
}

bool FormatText::construct()
{
    if( is_ok() ) {
        return true; // Only run once.
    }
    if( !m_control_in.empty() ) {
        setup_control_in();
    }
    if( !m_control_out.empty() ) {
//        setup_control_out();
    }
    set_ok( true );
    return true;
}

void FormatText::setup_control_in()
{
    if( m_default_fieldnames.empty() ) {
        m_default_fieldnames = get_grammar().get_base_fieldnames();
    }

    if( m_rank_fieldnames.empty() ) {
        m_rank_fieldnames = m_default_fieldnames;
    }
    assert( m_default_fieldnames.size() == m_rank_fieldnames.size() );
    m_rank_to_def_index.resize( m_rank_fieldnames.size() );
    for( size_t i = 0; i < m_rank_fieldnames.size(); i++ ) {
        for( size_t j = 0; j < m_default_fieldnames.size(); j++ ) {
            if( m_rank_fieldnames[i] == m_default_fieldnames[j] ) {
                m_rank_to_def_index[i] = j;
                break;
            }
        }
    }
    ElementControlIn ele;
    bool do_output = true;
    string input;
    for( auto it = m_control_in.begin(); it != m_control_in.end(); it++ ) {
        if( do_output ) {
            if( *it == '{' ) {
                do_output = false;
            }
            continue;
        }
        if( *it == '}' ) {
            ele.expand_specifier( get_owner() );

            if( input.size() ) {
                input += " ";
            }
            input += ele.get_input_text();
            InputFieldType type = ele.get_type();
            if( ele.has_dual_field() ) {
                input += "/";
                m_dual2_fieldname = ele.get_dual_record_field_name();
                type = IFT_dual1;
            }
            string fieldname = ele.get_record_field_name();
            if( !fieldname.empty() ) {
                if( ele.has_valid_default() ) {
                    m_default_names.push_back( fieldname );
                    m_default_values.push_back( ele.get_default_value() );
                }
                else {
                    m_format_order.push_back( fieldname );
                }
            }
            ele.clear();
            do_output = true;
        }
        else if( !do_output ) {
            ele.add_char( *it );
        }
    }

    BoolVec unused_fields( m_default_fieldnames.size(), true );
    m_fmt_to_rank_index.resize( m_default_fieldnames.size(), 0 );
    for( size_t i = 0; i < m_default_fieldnames.size(); i++ ) {
        if( i < m_format_order.size() ) {
            for( size_t j = 0; j < m_rank_fieldnames.size(); j++ ) {
                if( m_format_order[i] == m_rank_fieldnames[j] ) {
                    m_fmt_to_rank_index[i] = j;
                    unused_fields[j] = false;
                    break;
                }
            }
        }
        else {
            for( size_t j = 0; j < unused_fields.size(); j++ ) {
                if( unused_fields[j] ) {
                    m_fmt_to_rank_index[i] = j;
                    unused_fields[j] = false;
                    break;
                }
            }
        }
    }
}

string FormatText::get_text_output( const Record& record ) const
{
    Element ele;
    string output, fieldout, value;
    enum State { ignore, dooutput, doelement };
    State state = dooutput;
    for( auto it = m_control_out.begin(); it != m_control_out.end(); it++ ) {
        switch( state )
        {
        case ignore:
            if( *it == '|' ) {
                state = dooutput;
            }
            break;
        case dooutput:
            if( *it == '|' ) {
                output += fieldout;
                fieldout.clear();
            }
            else if( *it == '{' ) {
                state = doelement;
            }
            else {
                fieldout += *it;
            }
            break;
        case doelement:
            if( *it == '}' ) {
                Field f = get_field( record, ele.get_field_name() );
                if( ele.has_dual_field_name() ) {
                    Field d = get_field( record, ele.get_dual_field_name() );
                    value = dual_fields_to_str( f, d );
                }
                else {
                    value = ele.get_formatted_element( get_glich(), f );
                }
                if( value.empty() ) {
                    fieldout.clear();
                    state = ignore;
                }
                else {
                    fieldout += value;
                    state = dooutput;
                }
                ele.clear();
            }
            else {
                ele.add_char( *it );
            }
            break;
        }
    }
    return output + fieldout;
}

RList FormatText::string_to_rlist( const Base& base, const string& input ) const
{
    if( input.find( ".." ) != string::npos ||
        input.find( '|' ) != string::npos ) {
        return multirange_str_to_rlist( base, input );
    }
    return bare_str_to_rlist( base, input );
}

bool glich::FormatText::set_input( Record& record, const std::string& input, Boundary rb ) const
{
    const Base& base = record.get_base();
    InputFieldVec ifs( base.record_size() );
    parse_date( ifs, input );
    bool ret = resolve_input( base, record.get_field_vec(), ifs );
    if( !ret || rb == Boundary::None ) {
        return ret;
    }
    Record rec( record );
    Field fld;
    if( rb == Boundary::Begin ) {
        fld = record.complete_fields_as_beg();
    }
    if( rb == Boundary::End ) {
        fld = record.complete_fields_as_end();
    }
    return (fld != f_invalid);
}

FormatText::CP_Group FormatText::get_cp_group(
    string::const_iterator it, string::const_iterator end ) const
{
    int ch = *it;
    if( ch < 0 ) {  // eliminate non-ascii 
        return CP_Group::Other;
    }
    for( string::const_iterator sit = m_separators.begin(); sit != m_separators.end(); sit++ ) {
        if( *it == *sit ) {
            return CP_Group::Sep;
        }
    }
    if( ch == '-' ) {
        // If hyphen is followed by a digit treat as digit
        if( it + 1 != end ) {
            int ch1 = *(it + 1);
            if( ch1 > 0 && isdigit( ch1 ) ) {
                return CP_Group::Digit;
            }
        }
        // Otherwise treat it as text
        return CP_Group::Other;
    }
    if( ch == '/' ) {
        return CP_Group::Dual;
    }
    if( ch == '?' ) {
        return CP_Group::Quest;
    }
    if( isdigit( ch ) ) {
        return CP_Group::Digit;
    }
    return CP_Group::Other;
}


Field FormatText::get_field( const Record& record, const string& fname ) const
{
    int index = record.get_field_index( fname );
    if( index < 0 ) {
        return f_invalid;
    }
    return record.get_field( index );
}


int FormatText::parse_date( InputFieldVec& ifs, const string& str ) const
{
    if( str.empty() ) {
        return 0;
    }
    string token;
    CP_Group grp, prev_grp, token_grp;
    auto it = str.begin();
    grp = prev_grp = token_grp = get_cp_group( it, str.end() );
    if( grp == CP_Group::Quest ) {
        prev_grp = CP_Group::Sep;
    }
    bool done = false, ignore = false, dual = false;
    size_t size = ifs.size();
    for( size_t i = 0;;) {
        if( grp != prev_grp ) {
            token = full_trim( token );
            if( token.size() ) {
                if( token_grp == CP_Group::Digit ) {
                    if( grp == CP_Group::Dual ) {
                        ifs[i].value = str_to_field( token );
                        ifs[i].type = IFT_dual1;
                        i++;
                        dual = true;
                    }
                    else if( dual ) {
                        assert( i >= 1 );
                        ifs[i].value = str_to_dual2( ifs[i - 1].value, token );
                        ifs[i].type = IFT_dual2;
                        dual = false;
                    }
                    else {
                        ifs[i].value = str_to_field( token );
                        ifs[i].type = IFT_number;
                        i++;
                    }
                }
                if( token_grp == CP_Group::Other ) {
                    Field f = get_grammar().find_token( &(ifs[i].lexicon), token );
                    if( f == f_invalid ) {
                        return -1; // Unrecognised token
                    }
                    ifs[i].value = f;
                    ifs[i].type = IFT_vocab;
                    i++;
                }
                if( i == size ) {
                    return i;
                }
            }
            token.clear();
            if( grp == CP_Group::Quest ) {
                ifs[i].value = f_invalid;
                ifs[i].type = IFT_quest;
                i++;
                if( i == size ) {
                    return i;
                }
                // Question marks are not grouped
                prev_grp = CP_Group::Sep;
            }
            else {
                prev_grp = grp;
            }
            token_grp = grp;
        }
        if( done ) {
            return i;
        }
        if( token_grp == CP_Group::Digit || token_grp == CP_Group::Other ) {
            token += *it;
        }
        it++;
        if( grp != CP_Group::Digit && grp != CP_Group::Dual ) dual = false;
        if( it == str.end() ) {
            grp = CP_Group::Sep;
            done = true;
        }
        else {
            grp = get_cp_group( it, str.end() );
        }
    }
    return 0;
}

bool FormatText::resolve_input( const Base& base, FieldVec& fields, InputFieldVec& input ) const
{
    size_t size = base.record_size();
    FieldVec fs( size, f_invalid );
    FieldVec element_list;
    bool has_elements = false;;
    for( size_t i = 0; i < input.size(); i++ ) {
#if 0
        if( input[i].type == IFT_dual2 ) {
            int index = base.get_fieldname_index( m_dual2_fieldname );
            if( index >= 0 ) {
                fields[index] = input[i].value;
            }
            continue;
        }
        if( input[i].type == IFT_vocab ) {
            string vname = input[i].vocab->get_fieldname();
            if( !is_input_field( vname ) ) {
                int index = base->get_fieldname_index( vname );
                if( index >= 0 ) {
                    fields[index] = input[i].value;
                    continue;
                }
                else {
                    // Assume field is an element
                    input[i].type = IFT_calc;
                    has_elements = true;
                    continue;
                }
            }
        }
#endif
        if( input[i].type != IFT_null ) {
            element_list.push_back( input[i].value );
        }
    }
    StdStrVec format_order;
    size_t element_size = element_list.size();
    size_t start = 0;
    for( size_t i = 0; i < element_size; i++ ) {
        for( size_t j = start; j < m_fmt_to_rank_index.size(); j++ ) {
            if( m_fmt_to_rank_index[j] < element_size ) {
                fields[m_rank_to_def_index[m_fmt_to_rank_index[j]]] = element_list[i];
                start = j;
                start++;
                break;
            }
        }
    }
    return true;
}

RList FormatText::multirange_str_to_rlist( const Base& base, const string& input ) const
{
    RList rlist;
    string str = input;
    string rangestr, begval, endval;
    size_t pos1;
    for( ;;) {
        pos1 = str.find( '|' );
        rangestr = str.substr( 0, pos1 );
        size_t pos2 = rangestr.find( '~' );
        if( pos2 == string::npos ) {
            pos2 = rangestr.find( ".." );
            if( pos2 != string::npos ) {
                pos2++; // Step over second dot.
            }
        }
        if( pos2 == string::npos ) {
            // single value
            RList rl = bare_str_to_rlist( base, rangestr );
            rlist.insert( rlist.end(), rl.begin(), rl.end() );
        }
        else {
            // start and end
            begval = rangestr.substr( 0, pos2 );
            endval = rangestr.substr( pos2 + 1 );
            bool ret1 = true, ret2 = true;
            Range range;
            if( begval.empty() ) {
                range.m_beg = f_minimum;
            }
            else {
                Record rec( base );
                ret1 = set_input( rec, begval, Boundary::Begin );
                range.m_beg = rec.get_jdn();
            }
            if( endval.empty() ) {
                range.m_end = f_maximum;
            }
            else {
                Record rec( base );
                ret2 = set_input( rec, endval, Boundary::End );
                range.m_end = rec.get_jdn();
            }
            if( ret1 && ret2 && range.m_beg != f_invalid && range.m_end != f_invalid ) {
                rlist.push_back( range );
            }
        }
        if( pos1 == string::npos ) {
            break;
        }
        str = str.substr( pos1 + 1 );
    }
    return op_set_well_order( rlist );
}

RList FormatText::bare_str_to_rlist( const Base& base, const string& input ) const
{
    Record mask( base, input, *this, Boundary::None );
    return mask.get_rlist_from_mask();
}

// End of src/cal/calformattext.cpp file
