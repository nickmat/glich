/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicBase.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Hics Base class, used as base for all calendar calulations.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     17th March 2023
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

#include "hicBase.h"

#include "glcHelper.h"
#include "hicGrammar.h"
#include "hicMath.h"

#include <cassert>

using namespace glich;
using std::string;

Base::Base( const StdStrVec& data, size_t size )
    : m_record_size( size ), m_grammar( nullptr ), m_allow_shorthand( true )
{
    for( const string& word : data ) {
        set_data( word );
    }
}

Base::~Base()
{
}

void Base::set_data( const string& data )
{
    string code, tail;
    split_code( &code, &tail, data );
    if( code == "loc" ) {
        split_code( &code, &tail, tail );
        m_locale.lat = str_to_double( code );
        split_code( &code, &tail, tail );
        m_locale.lon = str_to_double( code );
    }
}

bool Base::attach_grammar( Grammar* gmr )
{
    if( gmr == nullptr || m_grammar != nullptr ) {
        return false;
    }
    // Check base and gmr fieldnames are the same.
    StdStrVec  base_fns = get_fieldnames();
    StdStrVec gmr_fns = gmr->get_base_fieldnames();
    size_t size = required_size();
    if( base_fns.size() != size || gmr_fns.size() != size ) {
        return false; // At lest one list is short.
    }
    for( size_t i = 0; i < size; i++ ) {
        if( base_fns[i] != gmr_fns[i] ) {
            return false;
        }
    }
    // All checks ok

    m_grammar = gmr;
    vec_append( m_fieldnames, gmr->get_calc_fieldnames() );
    m_record_size = m_fieldnames.size();
    vec_append( m_fieldnames, gmr->get_opt_fieldnames() );
    return true;
}

int Base::get_fieldname_index( const std::string& fieldname ) const
{
    int index = 0;
    for( auto& name : m_fieldnames ) {
        if( name == fieldname ) {
            return index;
        }
        index++;
    }
    return -1;
}

int Base::get_fieldname_record_index( const std::string& fieldname ) const
{
    for( size_t i = 0; i < m_record_size; i++ ) {
        if( fieldname == m_fieldnames[i] ) {
            return int( i );
        }
    }
    return -1;
}

int Base::get_alias_fieldname_index( const string& alias ) const
{
    string fieldname = m_grammar->resolve_field_alias( alias );
    return  get_fieldname_index( fieldname );
}

Format* Base::get_format( const string& fcode ) const
{
    assert( m_grammar );
    return m_grammar->get_format( fcode );
}

std::string glich::Base::get_input_fcode() const
{
    assert( m_grammar );
    return m_grammar->get_pref_input_fcode();
}

std::string glich::Base::get_output_fcode() const
{
    assert( m_grammar );
    return m_grammar->get_pref_output_fcode();
}

bool Base::is_complete( const FieldVec& fields ) const
{
    size_t size = required_size();
    for( size_t i = 0; i < size; i++ ) {
        if( fields[i] == f_invalid ) {
            return false;
        }
    }
    return true;
}

Field Base::get_beg_field_value( const FieldVec& fields, size_t index ) const
{
    if( index > 0 && fields[0] == f_minimum ) {
        return f_invalid;
    }
    if( index == 0 ) {
        return f_minimum;
    }
    if( index >= required_size() ) {
        return f_invalid;
    }
    return 1;
}

void Base::complete_beg( FieldVec& fields ) const
{
    if( fields[0] == f_invalid ) {
        return;
    }
    bool begining = true;
    for( size_t i = 0; i < required_size(); i++ ) {
        if( begining && fields[i] != f_invalid ) {
            continue;
        }
        fields[i] = get_beg_field_value( fields, i );
        begining = false;
    }
}

void Base::complete_end( FieldVec& fields ) const
{
    if( fields[0] == f_invalid ) {
        return;
    }
    bool begining = true;
    for( size_t i = 0; i < required_size(); i++ ) {
        if( begining && fields[i] != f_invalid ) {
            continue;
        }
        fields[i] = get_end_field_value( fields, i );
        begining = false;
    }
}

BoolVec Base::mark_balanced_fields(
    const FieldVec& fbeg, const FieldVec& fend, const XIndexVec& rank_to_def, size_t size ) const
{
    BoolVec mask( m_fieldnames.size(), true );
    size_t rank_index = size - 1;
    while( rank_index > 0 ) {
        size_t def_index = rank_to_def[rank_index];
        Field beg = get_beg_field_value( fbeg, def_index );
        Field end = get_end_field_value( fend, def_index );
        if( fbeg[def_index] == beg && fend[def_index] == end ) {
            mask[def_index] = false;
        }
        else {
            break;
        }
        --rank_index;
    }
    return mask;
}

// End of src/glc/hicBase.cpp file
