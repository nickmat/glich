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
#include "glcFunction.h"
#include "glcHelper.h"
#include "glcStore.h"
#include "hicBase.h"
#include "hicFormat.h"
#include "hicGregorian.h"
#include "hicOptional.h"
#include "hicScheme.h"

#include <algorithm>
#include <cassert>
#include <sstream>

using namespace glich;
using std::string;

Record::Record( const Base& base )
    : m_base( base ), m_jdn( f_invalid ), m_f( base.record_size(), f_invalid )
{
}

Record::Record( const Base& base, Field jdn )
    : m_base(base), m_jdn(jdn), m_f( base.record_size(), f_invalid )
{
    if( jdn == f_minimum || jdn == f_maximum || jdn == f_invalid ) {
        m_f[0] = jdn;
        return;
    }
    FieldVec fields = m_base.get_fields( jdn );
    for( size_t i = 0; i < fields.size(); i++ ) {
        m_f[i] = fields[i];
    }
}

Record::Record( const Scheme& sch, Field jdn )
    : m_base( sch.get_base() ), m_jdn( jdn ), m_f( m_base.record_size(), f_invalid )
{
    if( jdn == f_minimum || jdn == f_maximum || jdn == f_invalid ) {
        m_f[0] = jdn;
        return;
    }
    FieldVec fields = m_base.get_fields( jdn );
    for( size_t i = 0; i < fields.size(); i++ ) {
        m_f[i] = fields[i];
    }
    const Grammar* gmr = sch.get_grammar();
    Function* fun = sch.get_function( gmr->get_calculate() );
    if( fun != nullptr ) {
        SValue value = get_object( sch.get_code() );
        std::ostringstream outstm;
        StdStrVec qual;
        SValueVec args;
        value = fun->run( &value, qual, args, outstm );
        set_object( value );
    }
}

Record::Record( const Base& base, const string& str, const Format& fmt )
    : m_base( base ), m_jdn( f_invalid ), m_f( base.record_size(), f_invalid )
{
    set_str( str, fmt );
}

Record::Record( const Base& base, const string& str, const Format& fmt, Boundary rb )
    : m_base( base ), m_jdn( f_invalid ), m_f( base.record_size(), f_invalid )
{
    set_str( str, fmt, rb );
}

Record::Record( const Base& base, const FieldVec& fields )
    : m_base( base ), m_jdn( f_invalid ), m_f( base.record_size(), f_invalid )
{
    set_fields( fields );
}

Record::Record( const Base& base, const SValue& ovalue )
    : m_base( base ), m_jdn( f_invalid ), m_f( base.record_size(), f_invalid )
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

void Record::set_str( const string& str, const Format& fmt )
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
    if( fmt.set_input( *this, in ) ) {
        calc_jdn();
    }
    else {
        clear_fields();
    }
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
    if( fmt.set_input( *this, in, rb ) ) {
        calc_jdn();
    }
    else {
        clear_fields();
    }
}

void Record::set_fields( const FieldVec fields )
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
    if( values == nullptr || values->size() <= 1 ) {
        return;
    }
    size_t size = std::min( m_f.size(), values->size() - 1);
    for( size_t i = 0; i < size; i++ ) {
        Field fld = values->at( i + 1 ).get_as_field();
        m_f[i] = fld;
    }
    calc_jdn();
}

void Record::update_input()
{
    m_base.update_input( m_f );
}

void Record::update_output()
{
    m_base.update_output( m_f );
}

void Record::set_field( Field value, size_t index )
{
    assert( index < m_f.size() );
    if( index < m_f.size() ) {
        m_f[index] = value;
    }
}

Range Record::get_range_from_mask() const
{
    Range range;
    if( m_base.is_complete( get_field_vec() ) ) {
        range = { m_jdn, m_jdn };
        return range;
    }
    Record beg( *this ), end(*this);
    range.m_beg = beg.complete_fields_as_beg();
    range.m_end = end.complete_fields_as_end();
    return range;
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
    // Both must have the same Base and not be identical.
    if( &m_base != &rec.m_base || m_jdn == rec.get_jdn() ) {
        return BoolVec( m_f.size(), true );
    }
    return m_base.mark_balanced_fields( get_field_vec(), rec.get_field_vec(), rank_to_def, size );
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

Field Record::get_field( size_t index, const BoolVec* mask ) const
{
    if( index >= 0 && (!mask || (*mask)[index]) &&
        index < (int) m_base.record_size() ) {
        return m_f[index];
    }
    return f_invalid;
}

Field Record::get_revealed_field( size_t index, const BoolVec& reveal ) const
{
    assert( m_f.size() == reveal.size() );
    if( index < m_f.size() && reveal[index] ) {
        return m_f[index];
    }
    return f_invalid;
}

// End of src/glc/hicRecord.cpp file
