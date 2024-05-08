/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicHybrid.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Hybrid Calendar Schemes Source.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     15th June 2023
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

#include "hicHybrid.h"

//#include "cal/calendars.h"
//#include "calformattext.h"
//#include "calparse.h"
#include "hicScheme.h"
#include "hicRecord.h"

#include <cassert>

using namespace glich;
using namespace std;

Hybrid::Hybrid( const StdStrVec& fields, const std::vector<HybridData>& data )
    : m_data( data ), m_rec_size( fields.size() ), Base( std::string(), fields.size() )
{
    m_fieldnames = fields;
    XRefVec xref( fields.size() );
    for( size_t i = 0; i < data.size(); i++ ) {
        for( size_t j = 0; j < xref.size(); j++ ) {
            xref[j] = data[i].base->get_fieldname_index( fields[j] );
        }
        m_xref_fields.push_back( xref );
    }
}

Hybrid::~Hybrid()
{
    for( size_t i = 0 ; i < m_data.size() ; i++ ) {
        delete m_data[i].scheme;
    }
}

bool Hybrid::is_ok() const
{
    return
        m_fieldnames.size() && 
        m_data.size() > 1 && 
        m_data.size() == m_xref_fields.size();
}

Field Hybrid::get_jdn( const FieldVec& fields ) const
{
    Field sch = fields[0];
    if( sch < 0 || sch >= m_data.size() ) {
        return f_invalid;
    }
    FieldVec fs = get_xref( fields, sch );
    return m_data[sch].base->get_jdn( fs );
}

Field Hybrid::get_end_field_value( const FieldVec& fields, size_t index ) const
{
    return f_invalid;
}

void Hybrid::update_input( FieldVec& fields ) const
{
    if( fields[0] != f_invalid ) {
        return;
    }
    Field result = f_invalid;
    for( size_t i = 0; i < m_data.size(); i++ ) {
        FieldVec fs = get_xref( fields, i );
        Field jdn = m_data[i].base->get_jdn( fs );
        if( jdn >= m_data[i].start ) {
            result = i;
        }
        else {
            break;
        }
    }
    fields[0] = result;
}

FieldVec Hybrid::get_fields( Field jdn ) const
{
    Field sch = find_scheme( jdn );
    FieldVec f( record_size(), f_invalid );
    f[0] = sch;
    XRefVec xref = m_xref_fields[sch];
    Record rec( *m_data[sch].base, jdn );
    for( size_t i = 0; i < xref.size(); i++ ) {
        if( xref[i] >= 0 ) {
            f[i] = rec.get_field( xref[i] );
        }
    }
    return f;
}

FieldVec Hybrid::get_xref( const FieldVec& fields, Field sch ) const
{
    FieldVec result( m_rec_size - 1, f_invalid );
    if( sch >= (Field) m_xref_fields.size() || sch < 0 ) {
        return result;
    }
    FieldVec xref = m_xref_fields[sch];
    for( size_t i = 1 ; i < xref.size() ; i++ ) {
        int x = xref[i];
        if ( x < 0 || x >= int( m_fieldnames.size() ) ) {
            continue;
        }
        result[x] = fields[i];
    }
    return result;
}

bool Hybrid::is_in_scheme( Field jdn, Field scheme ) const
{
    if ( jdn == f_invalid || scheme < 0 ) {
        return false;
    }
    if ( jdn < m_data[scheme].start ) {
        return false;
    }
    if ( scheme < int(m_data.size()) - 1 && jdn >= m_data[scheme+1].start ) {
        return false;
    }
    return true;
}

size_t Hybrid::find_scheme( Field jdn ) const
{
    for( size_t i = 1 ; i < m_data.size() ; i++ ) {
        if( jdn < m_data[i].start ) {
            return i-1;
        }
    }
    return m_data.size() - 1;
}

void Hybrid::set_hybrid_fields( Field* fields, const Field* mask, Field sch ) const
{
    fields[m_rec_size-1] = sch;
    FieldVec xref = m_xref_fields[sch];
    for( size_t i = 0 ; i < m_data[sch].base->record_size() ; i++ ) {
        if( xref[i] < 0 ) {
            continue;
        }
        fields[i] = mask[xref[i]];
    }
}

// End of src/cal/calhybrid.cpp
