/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicJwn.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Jwn (Julian Week Number) calendar source.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     27th July 2023
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

#include "hicJwn.h"

#include "glcHelper.h"
#include "glcMath.h"

using namespace glich;
using std::string;


Jwn::Jwn( const StdStrVec& data )
    : m_offset( 0 ), Base( StdStrVec(), 2 )
{
    m_fieldnames = { "week", "day" };
    for( const string& word : data ) {
        cal_data( word );
    }
}

void Jwn::cal_data( const string& data )
{
    if( data == "monday" ) {
        m_offset = 0;
    }
    else if( data == "tuesday" ) {
        m_offset = 6;
    }
    else if( data == "wednesday" ) {
        m_offset = 5;
    }
    else if( data == "thursday" ) {
        m_offset = 4;
    }
    else if( data == "friday" ) {
        m_offset = 3;
    }
    else if( data == "saturday" ) {
        m_offset = 2;
    }
    else if( data == "sunday" ) {
        m_offset = 1;
    }
    else {
        Base::set_data( data );
    }
}

Field Jwn::get_jdn( const FieldVec& fields ) const
{
    return to_jdn( fields[0], fields[1], m_offset );
}

Field Jwn::get_end_field_value( const FieldVec& fields, size_t index ) const
{
    switch( index )
    {
    case 0: // last week?
        return f_maximum;
    case 1: // last day of week
        return 7;
    }
    return f_invalid;
}

FieldVec Jwn::get_fields( Field jdn ) const
{
    FieldVec fields( record_size(), f_invalid );
    from_jdn( &fields[0], &fields[1], jdn, m_offset );
    return fields;
}

Field Jwn::to_jdn( Field week, Field day, Field offset )
{
    return week * 7 + day - 1 - offset;
}

void Jwn::from_jdn( Field* week, Field* day, Field jdn, Field offset )
{
    *week = fdiv_e( jdn + offset, 7 );
    *day = fmod_e( jdn + offset, 7 ) + 1;
}

// End of src/glc/hicJwn.cpp file.
