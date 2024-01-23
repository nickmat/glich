/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicRecord.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Source for Record class to hold date values.
 * Author:      Nick Matthews
 * Website:     http://historycal.org
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

#include "hicRecord.h"

#include <glc/glc.h>
#include "glcHelper.h"
#include "glcStore.h"
#include "hicBase.h"
#include "hicFormat.h"
#include "hicGregorian.h"
#include "hicOptional.h"

#include <algorithm>
#include <cassert>

using namespace glich;
using std::string;

Record::Record( const Base& base )
    : m_base( base ), m_jdn( f_invalid ), m_f( base.record_size(), f_invalid )
{
}

Record::Record( const Base& base, Field jdn )
    : m_base(base), m_jdn(jdn)
{
    if( jdn == f_minimum || jdn == f_maximum || jdn == f_invalid ) {
        m_f = FieldVec( base.record_size(), f_invalid );
        m_f[0] = jdn;
        return;
    }
    m_f = m_base.get_fields( jdn );
}

Record::Record( const Base& base, const string& str, const Format& fmt, Boundary rb )
    : m_base( base ), m_jdn( f_invalid ), m_f( base.record_size() )
{
    set_str( str, fmt, rb );
}

Record::Record( const Base& base, const FieldVec& fields )
    : m_base( base ), m_jdn( f_invalid ), m_f( base.record_size(), f_invalid )
{
    set_fields( fields );
}

Record::Record( const Base& base, const SValue& ovalue )
    : m_base( base ), m_jdn( f_invalid ), m_f( base.record_size() )
{
    set_object( ovalue );
}

void Record::set_jdn( Field jdn )
{
    m_jdn = jdn;
    if( jdn == f_minimum || jdn == f_maximum || jdn == f_invalid ) {
        clear_fields();
        m_f[0] = jdn;
        return;
    }
    m_f = m_base.get_fields( jdn );
}

Field Record::calc_jdn()
{
    if( m_f[0] == f_minimum || m_f[0] == f_maximum ) {
        m_jdn = m_f[0];
    }
    else {
        // Base should check or correct for valid content
        m_jdn = m_base.get_jdn( m_f );
    }
    return m_jdn;
}

void Record::set_str( const string& str, const Format& fmt, Boundary rb )
{
    clear_fields();
    string in = full_trim( str );
    if( in == "past" ) {
        m_jdn = m_f[0] = f_minimum;
        return;
    }
    if( in == "future" ) {
        m_jdn = m_f[0] = f_maximum;
        return;
    }
    if( in == "today" ) {
        set_jdn( Gregorian::today() );
        return;
    }
    if( !fmt.set_input( *this, in, rb ) ) {
        clear_fields();
    }
}

void glich::Record::set_fields( const FieldVec fields )
{
    size_t size = std::min( fields.size(), m_f.size() );
    for( size_t i = 0; i < size; i++ ) {
        m_f[i] = fields[i];
    }
    calc_jdn();
}

void Record::set_object( const SValue& ovalue )
{
    clear_fields();
    const SValueVec* values = ovalue.get_object_values();
    if( values->size() <= 1 ) {
        return;
    }
    size_t size = std::min( m_base.record_size(), values->size() - 1 );
    for( size_t i = 0; i < size; i++ ) {
        Field fld = values->at( i + 1 ).get_as_field();
        m_f[i] = fld;
    }
    calc_jdn();
}

// Complete all invalid required fields by setting them to the first valid value.
// If the first field is invalid, set it to past.
Field Record::complete_fields_as_beg()
{
    bool begining = true;
    for( size_t i = 0; i < m_base.required_size(); i++ ) {
        if( begining && m_f[i] != f_invalid ) {
            continue;
        }
        m_f[i] = m_base.get_beg_field_value( m_f, i );
        begining = false;
    }
    return calc_jdn();
}


// Complete all invalid required fields by setting them to the last valid value.
// If the first field is invalid, set it to future.
Field Record::complete_fields_as_end()
{
    bool begining = true;
    for( size_t i = 0; i < m_base.required_size(); i++ ) {
        if( begining && m_f[i] != f_invalid ) {
            continue;
        }
        m_f[i] = m_base.get_end_field_value( m_f, i );
        begining = false;
    }
    return calc_jdn();
}

void Record::calculate_expression( const string& script_expr )
{
    Store* store = new Store;
    StdStrVec names = m_base.get_fieldnames();
    assert( names.size() >= m_f.size() );
    SValueVec rec_values( m_f.size() + 1 );
    rec_values[0] = ":";
    for( size_t i = 0; i < m_f.size();i++ ) {
        store->create_local( names[i] );
        if( m_f[i] != f_invalid ) {
            SValue value( m_f[i] );
            store->update_local( names[i], value );
            rec_values[i + 1] = value;
        }
    }
    SValue calc = glc().evaluate(script_expr, store);
    SValue rec( rec_values );
    rec.mask_op( calc );
    const SValueVec* values_ptr = rec.get_object_values();
    bool success = false;
    for( size_t i = 0; i < m_f.size(); i++ ) {
        m_f[i] = (*values_ptr)[i + 1].get_int_as_field( success );
    }
}

void Record::update_input()
{
    if( m_base.has_calc_input() ) {
        calculate_expression( m_base.get_calc_input() );
    }
    m_base.update_input( m_f );
}

void Record::update_output()
{
    if( m_base.has_calc_output() ) {
        calculate_expression( m_base.get_calc_output() );
    }
    m_base.update_output( m_f );
}

void Record::set_field( Field value, size_t index )
{
    assert( index < m_f.size() );
    if( index < m_f.size() ) {
        m_f[index] = value;
    }
}

// Treat the record as a mask and generate rlist
// Mask is of the form "val, ..., ?, ..."
RList Record::get_rlist_from_mask() const
{
    RList rlist;
    Range range;
    if( m_base.is_complete( get_field_vec() ) ) {
        range = { m_jdn, m_jdn };
        rlist.push_back( range );
        return rlist;
    }
    if( m_f[0] == f_invalid ) {
        return rlist;
    }
    Record beg( *this ), end(*this);
    range.m_beg = beg.complete_fields_as_beg();
    range.m_end = end.complete_fields_as_end();
    rlist.push_back( range );
    return rlist;
}

SValue Record::get_object( const string& ocode ) const
{
    SValueVec values( m_base.object_size() + 1);
    values[0] = { ocode };
    for( size_t i = 0; i < m_f.size(); i++ ) {
        if( m_f[i] == f_invalid ) {
            continue;
        }
        values[i + 1].set_field( m_f[i] );
    }
    for( size_t i = m_f.size(); i < m_base.object_size(); i++ ) {
        values[i + 1] = GetOptional( m_base.get_fieldname( i ), m_jdn );
    }
    return { values };
}

BoolVec Record::mark_balanced_fields( Record& rec, const XIndexVec& rank_to_def, size_t size )
{
    assert( size > 0 && rank_to_def.size() >= size );
    BoolVec mask( m_f.size(), true );
    // Both must have the same Base and not be identical.
    if( &m_base != &rec.m_base || m_jdn == rec.get_jdn() ) {
        return mask;
    }
    size_t rank_index = size - 1;
    while( rank_index > 0 ) {
        size_t def_index = rank_to_def[rank_index];
        if( m_f[def_index] == m_base.get_beg_field_value( m_f, def_index ) &&
            rec.m_f[def_index] == m_base.get_end_field_value( rec.m_f, def_index ) )
        {
            mask[def_index] = false;
        }
        else {
            break;
        }
        --rank_index;
    }
    return mask;
}

Field Record::get_field( size_t index ) const
{
    if( index < m_base.record_size() ) {
        return m_f[index];
    }
    if( index < m_base.object_size() )
    {
        return GetOptionalField( m_base.get_fieldname( index ), m_jdn );
    }
    return f_invalid;
}

Field glich::Record::get_field( size_t index, const BoolVec* mask ) const
{
    if( index >= 0 && (!mask || (*mask)[index]) &&
        index < (int) m_base.record_size() ) {
        return m_f[index];
    }
    return f_invalid;
}

// End of src/glc/hicRecord.cpp file
