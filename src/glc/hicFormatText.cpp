/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicFormatText.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     FormatText class to control formatting and parsing of dates.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     21st March 2023
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

#include "hicFormatText.h"

#include "glcHelper.h"
#include "glcMath.h"
#include "hicBase.h"
#include "hicElement.h"
#include "hicLexicon.h"
#include "hicRecord.h"

#include <algorithm>
#include <cassert>

using namespace glich;
using std::string;


FormatText::FormatText( const string& code, Grammar& gmr )
    : Format( code, gmr ), m_separators(":,"), m_sig_rank_size(0)
{
    m_shorthand = true;
}

bool FormatText::construct()
{
    if( is_ok() ) {
        return true; // Only run once.
    }
    const Grammar& gmr = get_grammar();
    assert( m_default_fieldnames.empty() );
    m_default_fieldnames = gmr.get_base_fieldnames();
    size_t base_size = m_default_fieldnames.size();
    vec_append( m_default_fieldnames, gmr.get_calc_fieldnames() );
    vec_append( m_default_fieldnames, gmr.get_opt_fieldnames() );
    if( m_rank_fieldnames.empty() ) {
        m_rank_fieldnames = gmr.get_rank_fieldnames();
    }
    m_sig_rank_size = m_rank_fieldnames.size();
    if( m_rank_fieldnames.empty() ) {
        m_rank_fieldnames = m_default_fieldnames;
        m_sig_rank_size = base_size;
    }
    for( size_t i = 0; i < m_default_fieldnames.size(); i++ ) {
        if( m_default_fieldnames.size() == m_rank_fieldnames.size() ) {
            break;
        }
        string fname = m_default_fieldnames[i];
        bool found = false;
        for( size_t i = 0; i < m_rank_fieldnames.size(); i++ ) {
            if( fname == m_rank_fieldnames[i] ) {
                found = true;
                break;
            }
        }
        if( !found ) {
            m_rank_fieldnames.push_back( fname );
        }
    }
    assert( m_default_fieldnames.size() == m_rank_fieldnames.size() );
    if( !m_control_in.empty() ) {
        setup_control_in();
    }
    m_rank_to_def_index.resize( m_rank_fieldnames.size() );
    for( size_t i = 0; i < m_rank_fieldnames.size(); i++ ) {
        for( size_t j = 0; j < m_default_fieldnames.size(); j++ ) {
            if( m_rank_fieldnames[i] == m_default_fieldnames[j] ) {
                m_rank_to_def_index[i] = j;
                break;
            }
        }
    }
    if( !m_control_out.empty() ) {
        setup_control_out();
    }
    if( m_input_function.empty() ) {
        m_input_function = gmr.get_funcode( "from:text" );
    }
    set_ok( true );
    return true;
}

void FormatText::setup_control_in()
{
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

            if( ele.is_text_only() ) {
                remove_from_rank( ele.get_field_name() );
            }
            if( input.size() ) {
                input += " ";
            }
            input += ele.get_input_text();
            InputFieldType type = ele.get_type();
            if( ele.has_dual_field() ) {
                type = IFT_dual1;
            }
            string fieldname = ele.get_record_field_name();
            if( !fieldname.empty() ) {
                if( ele.has_lexicon_only() ) {
                    m_lex_only_fielnames.push_back( fieldname );
                }
                else if( ele.has_valid_default() ) {
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
    if( get_input_str().empty() ) {
        set_user_input_str( input );
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

void FormatText::setup_control_out()
{
    assert( get_owner() );

    if( m_rankout_fieldnames.empty() ) {
        m_rankout_fieldnames = m_rank_fieldnames;
    }

    ElementControlOut ele;
    string fieldout, output, lcode;
    bool do_output = true;
    for( auto it = m_control_out.begin(); it != m_control_out.end(); it++ ) {
        if( do_output ) {
            if( *it == '|' ) {
                output += fieldout;
                fieldout.clear();
            }
            else if( *it == '{' ) {
                do_output = false;
            }
            else {
                fieldout += *it;
            }
            continue;
        }
        if( *it == '}' ) {
            ele.expand_specifier( get_owner() );
            fieldout += ele.get_field_output_name();
            lcode = ele.get_lcode();
            if( !lcode.empty() ) {
                m_lexicons.push_back( lcode );
            }
            if( m_shorthand ) {
                string field_name = get_owner()->resolve_field_alias( ele.get_field_name() );
                if( !is_significant_rank_name( field_name ) ) {
                    m_shorthand = false;
                }
            }
            ele.clear();
            do_output = true;
        }
        else if( !do_output ) {
            ele.add_char( *it );
        }
    }
    output += fieldout;
    if( m_output_str.empty() ) {
        set_user_output_str( output );
    }
}

std::string glich::FormatText::get_text_output( Record& record ) const
{
    const Base& base = record.get_base();
    return get_revealed_output( record, nullptr );
}

BoolVec glich::FormatText::get_reveal( Record& rec1, Record& rec2 ) const
{
    return rec1.mark_balanced_fields( rec2, m_rank_to_def_index, m_sig_rank_size );
}

string FormatText::get_revealed_text( Record& record, BoolVec& reveal ) const
{
    BoolVec* rev = &reveal;
    return get_revealed_output( record, rev );
}

string FormatText::get_revealed_output( const Record& record, const BoolVec* reveal ) const
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
                Field f = get_field( record, ele.get_field_name(), reveal );
                if( ele.has_dual_field_name() ) {
                    Field d = get_field( record, ele.get_dual_field_name(), reveal );
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

Range glich::FormatText::string_to_range( const Base& base, const std::string& input, FunctionData* fdata ) const
{
    Record mask( base, input, *this, Boundary::None );
    if( fdata ) {
        SValue value = mask.get_object( fdata->ocode );
        value = fdata->run( &value );
        mask.set_object( value );
    }
    return mask.get_range_from_mask();
}

bool FormatText::set_input( Record& record, const string& input, Boundary rb ) const
{
    const Base& base = record.get_base();
    InputFieldVec ifs( base.object_size() );
    parse_date( ifs, input );
    bool ret = resolve_input( base, record.get_field_vec(), ifs );

    if( ret && has_use_function() ) {
        string in_function = get_from_text_funcode();
        Function* fun = m_owner.get_function( in_function );
    }
    record.update_input();
    if( !ret || rb == Boundary::None ) {
        record.calc_jdn();
        return ret;
    }
    Record rec( record );
    Field fld = f_invalid;
    if( rb == Boundary::Begin ) {
        fld = record.complete_fields_as_beg();
    }
    if( rb == Boundary::End ) {
        fld = record.complete_fields_as_end();
    }
    return (fld != f_invalid);
}

bool FormatText::set_input( Record& record, const string& input ) const
{
    const Base& base = record.get_base();
    InputFieldVec ifs( base.object_size() );
    parse_date( ifs, input );
    bool ret = resolve_input( base, record.get_field_vec(), ifs );
    record.update_input();
    return ret;
}

string FormatText::range_to_string( const Base& base, const Range& range ) const
{
    if( range.m_beg == range.m_end ) {
        return jdn_to_string( base, range.m_beg );
    }
    string str1, str2;
    Record rec1( base, range.m_beg );
    rec1.update_output();
    Record rec2( base, range.m_end );
    rec2.update_output();

    if( m_shorthand ) {
        BoolVec reveal = rec1.mark_balanced_fields( rec2, m_rank_to_def_index, m_sig_rank_size );
        str1 = get_revealed_output( rec1, &reveal );
        str2 = get_revealed_output( rec2, &reveal );
    }
    else {
        str1 = get_text_output( rec1 );
        str2 = get_text_output( rec2 );
    }
    if( str1 == str2 ) {
        return str1;
    }
    return str1 + ".." + str2;
}

bool FormatText::is_significant_rank_name( const string& fieldname ) const
{
    for( size_t i = 0; i < m_sig_rank_size; i++ ) {
        if( fieldname == m_rank_fieldnames[i] ) {
            return true;
        }
    }
    return false;
}

void glich::FormatText::remove_from_rank( const std::string& fieldname )
{
    for( size_t i = 0; i < m_sig_rank_size; i++ ) {
        if( fieldname == m_rank_fieldnames[i] ) {
            m_rank_fieldnames.erase( m_rank_fieldnames.begin() + i );
            --m_sig_rank_size;
            return;
        }
    }
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

Field FormatText::get_field( const Record& record, const string& fname, const BoolVec* reveal ) const
{
    int index = record.get_field_index( fname );
    if( index < 0 ) {
        return f_invalid;
    }
    if( reveal && !(*reveal)[index] ) {
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
                    Field f = find_token( &(ifs[i].lexicon), token );
                    if( f == f_invalid ) {
                        return -1; // Unrecognised token
                    }
                    ifs[i].value = f;
                    ifs[i].type = IFT_lexicon;
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

Field glich::FormatText::find_token( Lexicon** lex, const std::string& word ) const
{
    Field field = f_invalid;
    for( size_t i = 0; i < m_lexicons.size(); i++ ) {
        Lexicon* lex_ptr = get_glc()->get_lexicon( m_lexicons[i] );
        field = lex_ptr->find( word );
        if( field != f_invalid ) {
            if( lex ) {
                *lex = lex_ptr;
            }
            return field;
        }
    }

    return get_grammar().find_token( lex, word );
}

bool FormatText::resolve_input( const Base& base, FieldVec& fields, InputFieldVec& input ) const
{
    size_t size = base.record_size();
    FieldVec fs( size, f_invalid );
    FieldVec element_list;
    for( size_t i = 0; i < input.size(); i++ ) {
        if( input[i].type == IFT_dual2 ) {
            continue;
        }
        if( input[i].type == IFT_lexicon ) {
            string vname = input[i].lexicon->get_fieldname();
            vname = get_grammar().resolve_lex_alias( vname );
            if( !is_significant_rank_name( vname ) ) {
                int index = base.get_fieldname_record_index( vname );
                if( index >= 0 ) {
                    fields[index] = input[i].value;
                }
                continue;
            }
        }
        if( input[i].type != IFT_null ) {
            element_list.push_back( input[i].value );
        }
    }
    StdStrVec format_order;
    size_t element_size = element_list.size();
    size_t start = 0;
    for( size_t i = 0; i < element_size; i++ ) {
        for( size_t j = start; j < m_fmt_to_rank_index.size(); j++ ) {
            size_t default_index = m_fmt_to_rank_index[j];
            if( default_index < element_size ) {
                fields[m_rank_to_def_index[default_index]] = element_list[i];
                start = j;
                start++;
                break;
            }
        }
    }
    return true;
}

// End of src/glc/hicFormatText.cpp file
