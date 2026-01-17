/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glcValue.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Class to hold all value types used by script.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     5th February 2023
 * Copyright:   Copyright (c) 2023..2026, Nick Matthews.
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

#include "glcValue.h"

#include <glc/glc.h>

#include "glcHelper.h"
#include "glcMath.h"
#include "glcObject.h"
#include "glcScript.h"

#include <cassert>

using namespace glich;
using std::string;


SValue::SValue( const SValue& value )
{
    m_type = value.m_type;
    m_data = value.m_data;
}

string SValue::as_string() const
{
    switch( m_type )
    {
    case Type::Null:
        return "null";
    case Type::String:
        return std::get<string>( m_data );
    case Type::Error:
        return std::get<string>( m_data ) + "\n";
    case Type::Bool:
        return bool_to_string( std::get<bool>( m_data ) );
    case Type::Number:
        return std::to_string( std::get<Num>( m_data ) );
    case Type::field:
        return get_special_field_string( Field( std::get<Num>( m_data ) ) );
    case Type::range:
        return get_special_range_string( std::get<Range>( m_data ) );
    case Type::rlist:
        return get_special_rlist_string( std::get<RList>( m_data ) );
    case Type::Float:
        return float_to_string( std::get<double>( m_data ) );
    case Type::Object:
        return object_to_string( std::get<SValueVec>( m_data ) );
    case Type::blob:
        return blob_to_string( std::get<Blob>( m_data ) );
    }
    return string();
}

std::string glich::SValue::object_to_string( const SValueVec& values ) const
{
    if( values.empty() ) {
        return "{}";
    }
    string result = "{";
    int i = 0;
    for( const auto& value : values ) {
        if( i == 0 && value.m_type == Type::String ) {
            string str = value.get_str();
            if( is_name( str ) ) {
                result += str;
            }
            else {
                result += "\"" + str + "\"";
            }
            i++;
            continue;
        }
        if( i != 1 ) {
            result += ",";
        }
        i++;
        result += " ";
        if( value.m_type == Type::String ) {
            result += "\"" + value.get_str() + "\"";
            continue;
        }
        result += value.as_string();

    }
    return result + "}";
}

string SValue::blob_to_string( const Blob& blob ) const
{
    return "blob:" + blob.type_str() + ":" + std::to_string( blob.size() );
}

/* static */
SValue SValue::create_error( const std::string& mess )
{
    SValue value;
    value.set_error( mess );
    return value;
}

void glich::SValue::set_range_demote( Range rng )
{
    if( rng.m_beg == f_invalid || rng.m_end == f_invalid ) {
        set_error( "Invalid range." );
    }
    else if( rng.m_beg == rng.m_end ) {
        set_field( rng.m_beg );
    }
    else {
        set_range( rng );
    }
}

void glich::SValue::set_rlist_demote( const RList& rlist )
{
    if( rlist.size() == 1 ) {
        set_range_demote( rlist[0] );
    }
    else {
        set_rlist( rlist );
    }
}

void SValue::set_error( const std::string& str )
{
    STokenStream* ts = Script::get_current_ts();
    m_type = Type::Error;
    if( ts ) {
        m_data = "Error " + ts->get_module() +
            " (" + std::to_string( ts->get_line() ) + "): " + str;
    }
    else {
        m_data = str;
    }
}

SValue* SValue::get_object_element( size_t index, size_t expand )
{
    if( std::holds_alternative<SValueVec>( m_data ) ) {
        SValueVec& vv = std::get<SValueVec>( m_data );
        if( index < 0 ) {
            return nullptr;
        }
        size_t sz = vv.size();
        if( index + 1 >= sz ) {
            if( index + 1 >= sz + expand ) {
                return nullptr;
            }
            vv.resize( index + 2 );
        }
        if( index >= 0 && index < vv.size() - 1 ) {
            return &vv[index + 1];
        }
    }
    return nullptr;
}

SValueVec* glich::SValue::get_object_values()
{
    if( std::holds_alternative<SValueVec>( m_data ) ) {
        return &std::get<SValueVec>( m_data );
    }
    return nullptr;
}

const SValueVec* glich::SValue::get_object_values() const
{
    if( std::holds_alternative<SValueVec>( m_data ) ) {
        return &std::get<SValueVec>( m_data );
    }
    return nullptr;
}

Object* SValue::get_object_ptr()
{
    string ocode = get_object_code();
    if( ocode.empty() ) {
        return nullptr;
    }
    return glc().get_object(ocode);
}

void glich::SValue::object_fill_store( Store* store )
{
    if( std::holds_alternative<SValueVec>( m_data ) ) {
        SValueVec& vv = std::get<SValueVec>( m_data );
        if( vv.size() == 0 ) {
            return;
        }
        string ocode = vv[0].get_str();
        Object* obj = glc().get_object(ocode);
        if( obj == nullptr ) {
            return;
        }
        const NameIndexMap& vnames = obj->get_vnames_map();
        for( const auto& name : vnames ) {
            bool success = store->create_local( name.first );
            if( !success ) {
                continue;
            }
            size_t index = name.second;
            if( index < vv.size() ) {
                store->update_local( name.first, vv[index] );
            }
        }
    }
}

string SValue::get_str() const
{
    if( std::holds_alternative<string>( m_data ) ) {
        return std::get<string>( m_data );
    }
    assert( false ); // Should only be called for string types.
    return string();
}

Num SValue::get_number() const
{
    if( std::holds_alternative<Num>( m_data ) && m_type == Type::Number ) {
        return std::get<Num>( m_data );
    }
    assert( false ); // Should only be called for Number type.
    return 0;
}

bool SValue::get_bool() const
{
    if( std::holds_alternative<bool>( m_data ) ) {
        return std::get<bool>( m_data );
    }
    assert( false ); // Should only be called for bool type.
    return false;
}

Field SValue::get_field() const
{
    if( std::holds_alternative<Num>( m_data ) && m_type == Type::field ) {
        return Field( std::get<Num>( m_data ) );
    }
    assert( false );
    return f_invalid;
}

Range SValue::get_range() const
{
    if( std::holds_alternative<Range>( m_data ) ) {
        return std::get<Range>( m_data );
    }
    assert( false );
    return Range( f_invalid, f_invalid );
}

RList SValue::get_rlist() const
{
    if( std::holds_alternative<RList>( m_data ) ) {
        return std::get<RList>( m_data );
    }
    assert( false );
    return RList( 0 );
}

double SValue::get_float() const
{
    if( std::holds_alternative<double>( m_data ) ) {
        return std::get<double>( m_data );
    }
    assert( false ); // Should only be called for Float type.
    return 0.0;
}

SValueVec SValue::get_object() const
{
    if( std::holds_alternative<SValueVec>( m_data ) ) {
        return std::get<SValueVec>( m_data );
    }
    assert( false ); // Should only be called for Object type.
    return SValueVec();
}

Blob SValue::get_blob() const
{
    if( std::holds_alternative<Blob>( m_data ) ) {
        return std::get<Blob>( m_data );
    }
    assert( false ); // Should only be called for Blob type.
    return Blob();
}

Field SValue::get_num_as_field() const
{
    if( std::holds_alternative<Num>( m_data ) && m_type == Type::Number ) {
        Num num = std::get<Num>( m_data );
        if( num >= f_maximum || num <= f_minimum ) {
            return f_invalid;
        }
        return Field( num );
    }
    assert( false );
    return Field();
}

double SValue::get_field_as_float() const
{
    if( std::holds_alternative<Num>( m_data ) && m_type == Type::field ) {
        Field fld = Field( std::get<Num>( m_data ) );
        switch( fld )
        {
        case f_invalid: return std::numeric_limits<double>::quiet_NaN();
        case f_minimum: return -std::numeric_limits<double>::infinity();
        case f_maximum: return std::numeric_limits<double>::infinity();
        }
        return static_cast<double>(fld);
    }
    assert( false );
    return std::numeric_limits<double>::quiet_NaN();
}

Field SValue::get_as_field() const
{
    if( std::holds_alternative<Num>( m_data ) && m_type == Type::field ) {
        return Field( std::get<Num>( m_data ) );
    }
    if( std::holds_alternative<Num>( m_data ) && m_type == Type::Number ) {
        Num num = std::get<Num>( m_data );
        if( num >= f_maximum || num <= f_minimum ) {
            return f_invalid;
        }
        return Field( num );
    }
    return f_invalid;
}

string SValue::get_str( bool& success ) const
{
    if( std::holds_alternative<string>( m_data ) ) {
        success = true;
        return std::get<string>( m_data );
    }
    success = false;
    return string();
}

Num glich::SValue::get_number( bool& success ) const
{
    if( std::holds_alternative<Num>( m_data ) && m_type == Type::Number ) {
        success = true;
        return std::get<Num>( m_data );
    }
    Field fld = get_field( success );
    if( success ) {
        return field_to_num( fld, success );
    }
    return 0;
}

bool SValue::get_bool( bool& success ) const
{
    if( std::holds_alternative<bool>( m_data ) ) {
        success = true;
        return std::get<bool>( m_data );
    }
    success = false;
    return false;
}

Field SValue::get_field( bool& success ) const
{
    success = true;
    if( std::holds_alternative<Num>( m_data ) && m_type == Type::field ) {
        return Field( std::get<Num>( m_data ) );
    }
    if( std::holds_alternative<Range>( m_data ) ) {
        Range rng = std::get<Range>( m_data );
        if( rng.m_beg == rng.m_end ) {
            return rng.m_beg;
        }
    }
    if( std::holds_alternative<RList>( m_data ) ) {
        RList rl = std::get<RList>( m_data );
        if( rl.size() == 1 && rl[0].m_beg == rl[0].m_end ) {
            return rl[0].m_beg;
        }
    }
    if( std::holds_alternative<Num>( m_data ) && m_type == Type::Number ) {
        Num num = std::get<Num>( m_data );
        return num_to_field( num, success );
    }
    success = false;
    return f_invalid;
}

Range SValue::get_range( bool& success ) const
{
    success = true;
    if( std::holds_alternative<Range>( m_data ) ) {
        return std::get<Range>( m_data );
    }
    if( std::holds_alternative<Num>( m_data ) && m_type == Type::field ) {
        Field fld = Field( std::get<Num>( m_data ) );
        return { fld, fld };
    }
    if( std::holds_alternative<Num>( m_data ) && m_type == Type::Number ) {
        Num num = std::get<Num>( m_data );
        if( num >= f_maximum || num <= f_minimum ) {
            return f_invalid;
        }
        Field fld = Field( num );
        return { fld, fld };
    }
    if( std::holds_alternative<RList>( m_data ) ) {
        RList rl = std::get<RList>( m_data );
        if( rl.size() == 1 ) {
            return rl[0];
        }
    }
    success = false;
    return Range( f_invalid, f_invalid );
}

RList SValue::get_rlist( bool& success ) const
{
    success = true;
    if( std::holds_alternative<RList>( m_data ) ) {
        return std::get<RList>( m_data );
    }
    if( std::holds_alternative<Range>( m_data ) ) {
        Range rng = std::get<Range>( m_data );
        return { rng };
    }
    if( std::holds_alternative<Num>( m_data ) ) {
        Field fld = Field( std::get<Num>( m_data ) );
        if( m_type == Type::Number ) {
            fld = get_num_as_field();
        }
        return { Range( fld, fld ) };
    }
    success = false;
    return RList( 0 );
}

double glich::SValue::get_float( bool& success ) const
{
    success = true;
    if( std::holds_alternative<double>( m_data ) ) {
        return std::get<double>( m_data );
    }
    success = false;
    return 0.0;
}

SValueVec glich::SValue::get_object( bool& success ) const
{
    success = true;
    if( std::holds_alternative<SValueVec>( m_data ) ) {
        return std::get<SValueVec>( m_data );
    }
    success = false;
    return SValueVec();
}

std::string glich::SValue::get_object_code() const
{
    if( std::holds_alternative<SValueVec>( m_data ) ) {
        SValueVec values = std::get<SValueVec>( m_data );
        if( values.size() > 0 && values[0].m_type == Type::String ) {
            return values[0].get_str();
        }
    }
    return std::string();
}

Blob SValue::get_blob( bool& success ) const
{
    success = true;
    if( std::holds_alternative<Blob>( m_data ) ) {
        return std::get<Blob>( m_data );
    }
    success = false;
    return Blob();
}

Field SValue::get_int_as_field( bool& success ) const
{
    success = true;
    if( type() == Type::Number ) {
        Num num = get_number();
        return num_to_field( num, success );
    }
    if( type() == Type::field ) {
        return get_field();
    }
    success = false;
    return f_invalid;
}

size_t SValue::get_int_as_size_t( bool& success ) const
{
    success = true;
    if( type() == Type::Number ) {
        Num num = get_number();
        if( num < 0 ) {
            success = false;
            num = 0;
        }
        return static_cast<size_t>(num);
    }
    if( type() == Type::field ) {
        Field fld = get_field();
        if( fld < 0 || fld >= f_maximum ) {
            success = false;
            fld = 0;
        }
        return static_cast<size_t>(fld);
    }
    success = false;
    return 0;
}

double glich::SValue::get_any_as_float( bool& success ) const
{
    success = true;
    switch( type() )
    {
    case Type::Float:
        return get_float();
    case Type::field:
        return field_to_double( get_field(), success );
    case Type::Number:
        return num_to_double( get_number(), success );
    default:
        success = false;
        return std::numeric_limits<double>::quiet_NaN();
    }
}

bool SValue::propagate_error( const SValue& value )
{
    if( is_error() ) {
        return true;
    }
    if( value.is_error() ) {
        m_type = Type::Error;
        m_data = value.m_data;
        return true;
    }
    return false;
}

bool SValue::obtain_rlists( RList& left, RList& right, const SValue& value )
{
    if( propagate_error( value ) ) {
        return false;
    }
    bool success;
    left = get_rlist( success );
    if( !success ) {
        set_error( "Cannot convert left hand to RList." );
        return false;
    }
    right = value.get_rlist( success );
    if( !success ) {
        set_error( "Cannot convert right hand to RList." );
        return false;
    }
    return true;
}

void SValue::logical_or( const SValue& value )
{
    if( propagate_error( value ) ) {
        return;
    }
    if( type() == Type::Bool && value.type() == Type::Bool ) {
        set_bool( get_bool() || value.get_bool() );
        return;
    }
    set_error( "Operator 'or' requires boolean values." );
}

void SValue::logical_and( const SValue& value )
{
    if( propagate_error( value ) ) {
        return;
    }
    if( type() == Type::Bool && value.type() == Type::Bool ) {
        set_bool( get_bool() && value.get_bool() );
        return;
    }
    set_error( "Operator 'and' requires boolean values." );
}

void SValue::equal( const SValue& value )
{
    if( propagate_error( value ) ) {
        return;
    }
    // Check mixed Integer.
    if( (type() == Type::Number && value.type() == Type::field) ||
        (type() == Type::field && value.type() == Type::Number) ) {
        bool success1, success2;
        Field left = get_int_as_field( success1 );
        Field right = value.get_int_as_field( success2 );
        if( !success1 || !success2 ) {
            set_error( "Could not convert to Field type." );
            return;
        }
        set_bool( left == right );
        return;
    }
    if( m_type == value.type() ) {
        bool result = false;
        switch( m_type )
        {
        case Type::String:
            result = (get_str() == value.get_str());
            break;
        case Type::Bool:
            result = (get_bool() == value.get_bool());
            break;
        case Type::Number:
            result = (get_number() == value.get_number());
            break;
        case Type::field:
            result = (get_field() == value.get_field());
            break;
        case Type::range:
            result = (get_range() == value.get_range());
            break;
        case Type::rlist:
            result = (get_rlist() == value.get_rlist());
            break;
        case Type::Float:
            result = (get_float() == value.get_float());
            break;
        case Type::Null:
            result = true;
            break;
        default:
            set_error( "Type cannot be compared." );
            return;
        }
        set_bool( result );
        return;
    }
    if( m_type == Type::Null || value.type() == Type::Null ) {
        set_bool( false );
        return;
    }
    set_error( "Must compare same types." );
}

void SValue::greater_than( const SValue& value )
{
    if( propagate_error( value ) ) {
        return;
    }
    // Check mixed Integer.
    if( (type() == Type::Number && value.type() == Type::field) ||
        (type() == Type::field && value.type() == Type::Number) ) {
        bool success1, success2;
        Field left = get_int_as_field( success1 );
        Field right = value.get_int_as_field( success2 );
        if( !success1 || !success2 ) {
            set_error( "Could not convert to Field type." );
            return;
        }
        if( left == f_invalid || right == f_invalid ) {
            set_error( "Cannot compare invalid fields." );
            return;
        }
        set_bool( left > right );
        return;
    }
    if( m_type == value.type() ) {
        bool result = false;
        switch( m_type )
        {
        case Type::String:
            result = (get_str().compare( value.get_str() ) > 0);
            break;
        case Type::Bool:
            set_error( "Can only compare bool for equal or not equal." );
            return;
        case Type::Number:
            result = (get_number() > value.get_number());
            break;
        case Type::field:
            {
                Field left = get_field();
                Field right = value.get_field();
                if( left == f_invalid || right == f_invalid ) {
                    set_error( "Cannot compare invalid fields." );
                    return;
                }
                result = (left > right);
            }
            break;
        case Type::Float:
            result = (get_float() > value.get_float());
            break;
        case Type::Null:
            set_error( "Can only compare a null value for equal or not equal." );
            return;
        default:
            set_error( "Type cannot be compared." );
            return;
        }
        set_bool( result );
        return;
    }
    set_error( "Must compare same types." );
}

void SValue::less_than( const SValue& value )
{
    if( propagate_error( value ) ) {
        return;
    }
    SValue right( value );
    right.greater_than( *this );
    *this = right;
}

void SValue::plus( const SValue& value )
{
    if( propagate_error( value ) ) {
        return;
    }
    if( type() == Type::String || value.type() == Type::String ) {
        string str1 = as_string();
        string str2 = value.as_string();
        set_str( str1 + str2 );
        return;
    }
    bool success = true;
    const char* invalid_range_err = "Invalid range.";
    const char* invalid_rlist_err = "Invalid rlist.";
    switch( type() )
    {
    case Type::Number:
    {
        Field fld = get_num_as_field();
        switch( value.type() )
        {
        case Type::Number:
            set_number( get_number() + value.get_number() );
            return;
        case Type::field:
            set_field( add_field( fld, value.get_field() ) );
            return;
        case Type::Float:
            set_float( static_cast<double>(get_number()) + value.get_float() );
            return;
        case Type::range:
            set_range_demote( add_range( value.get_range(), fld, success ) );
            if( !success ) {
                set_error( invalid_range_err );
            }
            return;
        case Type::rlist:
            set_rlist( add_rlist( value.get_rlist(), fld, success ) );
            if( !success ) {
                set_error( invalid_rlist_err );
            }
            return;
        default:
            break;
        }
        break;
    }
    case Type::field:
        switch( value.type() )
        {
        case Type::Number:
            set_field( add_field( get_field(), value.get_num_as_field() ) );
            return;
        case Type::field:
            set_field( add_field( get_field(), value.get_field() ) );
            return;
        case Type::Float:
            set_float( add_float_field( value.get_float(), get_field() ) );
            return;
        case Type::range:
            set_range_demote( add_range( value.get_range(), get_field(), success ) );
            if( !success ) {
                set_error( invalid_range_err );
            }
            return;
        case Type::rlist:
            set_rlist( add_rlist( value.get_rlist(), get_field(), success ) );
            if( !success ) {
                set_error( invalid_rlist_err );
            }
            return;
        default:
            break;
        }
        break;
    case Type::Float:
        switch( value.type() )
        {
        case Type::Number:
            set_float( get_float() + static_cast<double>(value.get_number()) );
            return;
        case Type::field:
            set_float( add_float_field( get_float(), value.get_field() ) );
            return;
        case Type::Float:
            set_float( get_float() + value.get_float() );
            return;
        default:
            break;
        }
        break;
    case Type::range:
        switch( value.type() )
        {
        case Type::Number:
            set_range_demote( add_range( get_range(), value.get_num_as_field(), success ) );
            if( !success ) {
                set_error( invalid_range_err );
            }
            return;
        case Type::field:
            set_range_demote( add_range( get_range(), value.get_field(), success ) );
            if( !success ) {
                set_error( invalid_range_err );
            }
            return;
        default:
            break;
        }
        break;
    case Type::rlist:
        switch( value.type() )
        {
        case Type::Number:
            set_rlist( add_rlist( get_rlist(), value.get_num_as_field(), success ) );
            if( !success ) {
                set_error( invalid_rlist_err );
            }
            return;
        case Type::field:
            set_rlist( add_rlist( get_rlist(), value.get_field(), success ) );
            if( !success ) {
                set_error( invalid_rlist_err );
            }
            return;
        case Type::rlist:
            set_error( "Unable to add or subtract rlists." );
            return;
        default:
            break;
        }
        break;
    default:
        break;
    }
    set_error( "Unable to add or subtract types." );
}

void SValue::minus( const SValue& value )
{
    if( propagate_error( value ) ) {
        return;
    }
    if( ( m_type == Type::Number || m_type == Type::Float ||
        m_type == Type::field || m_type == Type::range ||
        m_type == Type::rlist ) &&
        ( value.m_type == Type::Number || value.m_type == Type::Float ||
            value.m_type == Type::field || value.m_type == Type::range ||
            value.m_type == Type::rlist )
        ) {
        SValue v( value );
        v.negate();
        plus( v );
        return;
    }
    set_error( "Unable to subtract types." );
}

void SValue::multiply( const SValue& value )
{
    if( propagate_error( value ) ) {
        return;
    }
    const char* only_ints_err = "Can only multiply fields and numbers.";
    switch( type() )
    {
    case Type::Number:
        switch( value.type() )
        {
        case Type::Number:
            set_number( get_number() * value.get_number() );
            return;
        case Type::field:
            set_field( mult_field( get_num_as_field(), value.get_field() ) );
            break;
        case Type::Float:
            set_float( static_cast<double>(get_number()) * value.get_float() );
            return;
        default:
            break;
        }
        break;
    case Type::field:
        switch( value.type() )
        {
        case Type::Number:
            set_field( mult_field( get_field(), value.get_num_as_field() ) );
            break;
        case Type::field:
            set_field( mult_field( get_field(), value.get_field() ) );
            break;
        case Type::Float:
            set_float( mult_float_field( value.get_float(), get_field() ) );
            return;
        default:
            break;
        }
        break;
    case Type::Float:
        switch( value.type() )
        {
        case Type::Number:
            set_float( get_float() * static_cast<double>(value.get_number()) );
            return;
        case Type::field:
            set_float( mult_float_field( get_float(), value.get_field() ) );
            return;
        case Type::Float:
            set_float( get_float() * value.get_float() );
            return;
        default:
            break;  
        }
        break;
    default:
        break;
    }
    if( type() == Type::field ) {
        if( get_field() == f_invalid ) {
            set_error( "Multiply has invalid result." );
        }
        return;
    }
    set_error( "Can only multiply fields and numbers." );
}

void SValue::divide( const SValue& value )
{
    if( propagate_error( value ) ) {
        return;
    }
    const char* only_ints_err = "Can only divide numeric values.";
    double left = 0.0;
    double right = 0.0;
    switch( type() )
    {
    case Type::Number:
        switch( value.type() )
        {
        case Type::Number: 
            left = static_cast<double>(get_number());
            right = static_cast<double>(value.get_number());
            break;
        case Type::field:
            left = static_cast<double>(get_number());
            right = value.get_field_as_float();
            break;
        case Type::Float:
            left = static_cast<double>(get_number());
            right = value.get_float();
            break;
        default:
            set_error( only_ints_err );
            return;
        }
        break;
    case Type::field:
        switch( value.type() )
        {
        case Type::Number:
            left = get_field_as_float();
            right = static_cast<double>(value.get_number());
            break;
        case Type::field:
            left = get_field_as_float();
            right = value.get_field_as_float();
            break;
        case Type::Float:
            left = get_field_as_float();
            right = value.get_float();
            break;
        default:
            set_error( only_ints_err );
            return;
        }
        break;
    case Type::Float:
        switch( value.type() )
        {
        case Type::Number:
            left = get_float();
            right = static_cast<double>(value.get_number());
            break;
        case Type::field:
            left = get_float();
            right = value.get_field_as_float();
            break;
        case Type::Float:
            left = get_float();
            right = value.get_float();
            break;
        default:
            set_error( only_ints_err );
            return;
        }
        break;
    default:
        set_error( only_ints_err );
        return;
    }
    if( std::isnan( right ) ) {
        set_error( "Division by nan." );
        return;
    }
    if( std::isinf( right ) ) {
        string sign = (right > 0) ? "+" : "-";
        set_error( "Division by " + sign + "inf." );
        return;
    }
    if( is_zero( right ) ) {
        set_error( "Division by zero." );
        return;
    }
    if( std::isnan( left ) ) {
        set_error( "Cannot divide nan." );
        return;
    }
    set_float( left / right );
}

void glich::SValue::int_div( const SValue& value )
{
    if( propagate_error( value ) ) {
        return;
    }
    const char* only_ints_err = "Can only divide fields and numbers.";
    const char* divide_zero_err = "Division by zero.";
    Field left = f_invalid;
    Field right = f_invalid;
    switch( type() )
    {
    case Type::Number:
        switch( value.type() )
        {
        case Type::Number:
        {
            Num num1 = get_number();
            Num num2 = value.get_number();
            if( num2 == 0 ) {
                set_error( divide_zero_err );
                return;
            }
            set_number( div_e( num1, num2 ) );
        }
        return;
        case Type::field:
            left = get_num_as_field();
            right = value.get_field();
            break;
        default:
            set_error( only_ints_err );
            return;
        }
        break;
    case Type::field:
        switch( value.type() )
        {
        case Type::Number:
            left = get_field();
            right = value.get_num_as_field();
            break;
        case Type::field:
            left = get_field();
            right = value.get_field();
            break;
        default:
            set_error( only_ints_err );
            return;
        }
        break;
    default:
        set_error( only_ints_err );
        return;
    }
    switch( right )
    {
    case 0:
        set_error( divide_zero_err );
        return;
    case f_invalid:
        set_error( "Division by invalid." );
        return;
    case f_maximum:
        set_error( "Division by +infinity." );
        return;
    case f_minimum:
        set_error( "Division by -infinity." );
        return;
    }
    switch( left )
    {
    case f_invalid:
        set_error( "Cannot divide invalid." );
        return;
    case f_maximum:
    case f_minimum:
        set_field( left );
        return;
    }
    set_field( fdiv_e( left, right ) );
}

void SValue::modulus( const SValue& value )
{
    if( propagate_error( value ) ) {
        return;
    }
    const char* only_ints_err = "Can only use modulus with numbers.";
    const char* modulo_zero_err = "Modulo by zero.";
    Field left = f_invalid;
    Field right = f_invalid;
    switch( type() )
    {
    case Type::Number:
        switch( value.type() )
        {
        case Type::Number:
        {
            Num num1 = get_number();
            Num num2 = value.get_number();
            if( num2 == 0 ) {
                set_error( modulo_zero_err );
                return;
            }
            set_number( mod_e( num1, num2 ) );
        }
        return;
        case Type::field:
            left = get_num_as_field();
            right = value.get_field();
            break;
        default:
            set_error( only_ints_err );
            return;
        }
        break;
    case Type::field:
        switch( value.type() )
        {
        case Type::Number:
            left = get_field();
            right = value.get_num_as_field();
            break;
        case Type::field:
            left = get_field();
            right = value.get_field();
            break;
        default:
            set_error( only_ints_err );
            return;
        }
        break;
    default:
        set_error( only_ints_err );
        return;
    }
    switch( right )
    {
    case 0:
        set_error( modulo_zero_err );
        return;
    case f_invalid:
        set_error( "Modulo by invalid." );
        return;
    case f_maximum:
        set_error( "Modulo by +infinity." );
        return;
    case f_minimum:
        set_error( "Modulo by -infinity." );
        return;
    }
    switch( left )
    {
    case f_invalid:
        set_error( "Cannot modulo invalid." );
        return;
    case f_maximum:
    case f_minimum:
        set_field( 0 );
        return;
    }
    set_field( fmod_e( left, right ) );
}

void SValue::rlist_union( const SValue& value )
{
    RList left, right;
    if( obtain_rlists( left, right, value ) ) {
        set_rlist_demote( set_operation( SetOp::Union, left, right ) );
    }
}

void SValue::intersection( const SValue& value )
{
    RList left, right;
    if( obtain_rlists( left, right, value ) ) {
        set_rlist_demote( set_operation( SetOp::Inter, left, right ) );
    }
}

void SValue::rel_complement( const SValue& value )
{
    RList left, right;
    if( obtain_rlists( left, right, value ) ) {
        set_rlist_demote( set_operation( SetOp::RelComp, left, right ) );
    }
}

void SValue::sym_difference( const SValue& value )
{
    RList left, right;
    if( obtain_rlists( left, right, value ) ) {
        set_rlist_demote( set_operation( SetOp::SymDif, left, right ) );
    }
}

void SValue::range_op( const SValue& value )
{
    if( propagate_error( value ) ) {
        return;
    }
    Range lhs, rhs;
    const char* type_err_mess = "Range operator requires number types.";
    const char* empty_err_mess = "Range operator cannot use empty rlist.";
    const char* invalid_err_mess = "Range invalid.";
    switch( m_type )
    {
    case Type::rlist:
        if( get_rlist().empty() ) {
            set_error( empty_err_mess );
            return;
        }
        lhs = enclosing_range( get_rlist() );
        break;
    case Type::Number:
        lhs.m_beg = lhs.m_end = get_num_as_field();
        break;
    case Type::field:
        lhs.m_beg = lhs.m_end = get_field();
        break;
    case Type::range:
        lhs = get_range();
        break;
    default:
        set_error( type_err_mess );
        return;
    }
    switch( value.m_type )
    {
    case Type::rlist:
        if( value.get_rlist().empty() ) {
            set_error( empty_err_mess );
            return;
        }
        rhs = enclosing_range( value.get_rlist() );
        break;
    case Type::Number:
        rhs.m_beg = rhs.m_end = value.get_num_as_field();
        break;
    case Type::field:
        rhs.m_beg = rhs.m_end = value.get_field();
        break;
    case Type::range:
        rhs = value.get_range();
        break;
    default:
        set_error( type_err_mess );
        return;
    }
    if( lhs.m_beg > rhs.m_beg ) {
        lhs.m_beg = rhs.m_beg;
    }
    if( lhs.m_end < rhs.m_end ) {
        lhs.m_end = rhs.m_end;
    }
    if( lhs.m_beg == f_invalid ) {
        set_error( invalid_err_mess );
        return;
    }
    set_range( lhs );
}

void SValue::property_op( const SValue& value )
{
    // Note: property [.type] can operate on an error,
    // so don't just propagate it.
    if( value.is_error() ) {
        m_type = Type::Error;
        m_data = value.as_string();
        return;
    }
    if( value.m_type != Type::String ) {
        set_error( "Subscript property must be string type." );
        return;
    }
    string property = value.get_str();
    const char* type_err_mess = "Property not available or this type.";
    const char* empty_rlist_err_mess = "Empty rlist.";
    if( property == "type" ) {
        switch( m_type )
        {
        case Type::Number: set_str( "number" ); return;
        case Type::field:  set_str( "field" );  return;
        case Type::Float:  set_str( "float" );   return;
        case Type::range:  set_str( "range" );  return;
        case Type::rlist:  set_str( "rlist" );  return;
        case Type::String: set_str( "string" ); return;
        case Type::Object: set_str( "object" ); return;
        case Type::Bool:   set_str( "bool" );   return;
        case Type::Null:   set_str( "null" );   return;
        case Type::Error:  set_str( "error" );  return;
        default:           set_str( "unknown" ); return;
        }
    }
    if( m_type == Type::Error ) {
        return;
    }
    if( property == "low" ) {
        switch( m_type )
        {
        case Type::field:
        case Type::Number:
        case Type::Float:
            return;
        case Type::range:
            set_field( get_range().m_beg );
            return;
        case Type::rlist: {
            RList rlist = get_rlist();
            if( rlist.empty() ) {
                break;
            }
            set_field( rlist[0].m_beg );
            return;
        }
        default:
            break;
        }
        set_field( f_invalid );
        return;
    }
    if( property == "high" ) {
        switch( m_type )
        {
        case Type::field:
        case Type::Number:
        case Type::Float:
            return;
        case Type::range:
            set_field( get_range().m_end );
            return;
        case Type::rlist: {
            RList rlist = get_rlist();
            if( rlist.empty() ) {
                break;
            }
            set_field( rlist[rlist.size() - 1].m_end );
            return;
        }
        default:
            break;
        }
        set_field( f_invalid );
        return;
    }
    if( property == "span" ) {
        switch( m_type )
        {
        case Type::Number:
        case Type::field:
            set_field( 1 );
            return;
        case Type::range:
            if( get_range().m_beg <= f_minimum || get_range().m_end >= f_maximum ) {
                set_field( f_invalid );
            }
            else {
                set_field( get_range().m_end - get_range().m_beg + 1 );
            }
            return;
        case Type::rlist: {
            RList rlist = get_rlist();
            if( rlist.empty() ) {
                set_field( f_invalid );
                return;
            }
            Field fld1 = rlist[0].m_beg;
            Field fld2 = rlist[rlist.size() - 1].m_end;
            if( fld1 <= f_minimum || fld2 >= f_maximum ) {
                set_field( f_invalid );
                return;
            }
            set_field( fld2 - fld1 + 1 );
            return;
        }
        default:
            break;
        }
        set_field( f_invalid );
        return;
    }
    if( property == "size" ) {
        switch( m_type )
        {
        case Type::Object:
            set_number( get_object().size() - 1 );
            return;
        case Type::rlist:
            set_number( get_rlist().size() );
            return;
        case Type::String:
            set_number( get_str().size() );
            return;
        default:
            set_number( 0 );
            return;
        }
    }
    if( property == "envelope" ) {
        switch( m_type )
        {
        case Type::rlist: {
            RList rlist = get_rlist();
            if( rlist.empty() ) {
                break;
            }
            Range rng;
            rng.m_beg = rlist[0].m_beg;
            rng.m_end = rlist[rlist.size() - 1].m_end;
            set_range( rng );
            return;
        }
        default:
            break;
        }
        set_field( f_invalid );
        return;
    }
    if( property == "object" ) {
        set_str( get_object_code() );
        return;
    }
    set_error( "Property not recognised." );
}

void SValue::mask_op( const SValue& value )
{
    if( propagate_error( value ) ) {
        return;
    }
    if( type() != Type::Object || value.type() != Type::Object ) {
        set_error( "Operator 'mask' requires object types." );
        return;
    }
    SValueVec* vleft = get_object_values();
    const SValueVec* vright = value.get_object_values();
    assert( vleft != nullptr && vright != nullptr );
    const SValue* right_array = vright->data();
    for( size_t i = 1; i < vright->size(); i++ ) {
        if( i >= vleft->size() ) {
            vleft->push_back( vright->at( i ) );
            continue;
        }
        if( (*vleft)[i].type() == Type::Null ) {
            (*vleft)[i] = right_array[i];
        }
    }
}

void SValue::negate()
{
    switch( m_type )
    {
    case Type::Error:
        return;
    case Type::Float:
        if( !std::isnan( get_float() ) ) {
            set_float( -get_float() );
        }
        return;
    case Type::Number:
        set_number( -get_number() );
        return;
    case Type::field:
        if( get_field() != f_invalid ) {
            set_field( -get_field() );
        }
        return;
    case Type::range: {
        Range rng = get_range();
        if( rng.m_beg != f_invalid && rng.m_end != f_invalid ) {
            Field end = -rng.m_beg;
            rng.m_beg = -rng.m_end;
            rng.m_end = end;
            set_range( rng );
        }
        return;
    }
    case Type::rlist: {
        RList rlist = get_rlist();
        for( Range& rng : rlist ) {
            if( rng.m_beg != f_invalid && rng.m_end != f_invalid ) {
                Field end = -rng.m_beg;
                rng.m_beg = -rng.m_end;
                rng.m_end = end;
            }
        }
        std::reverse( rlist.begin(), rlist.end() );
        set_rlist( rlist );
        return;
    }
    default:
        set_error( "Can only negate number types." );
        return;
    }
}

void SValue::logical_not()
{
    switch( m_type )
    {
    case Type::Error:
        return;
    case Type::Bool:
        set_bool( !get_bool() );
        return;
    default:
        set_error( "Logical 'not' only operates on bools" );
        return;
    }
}

void SValue::compliment()
{
    if( is_error() ) {
        return;
    }
    bool success;
    RList rlist = get_rlist( success );
    if( success ) {
        set_rlist( op_set_complement( rlist ) );
        return;
    }
    set_error( "Cannot convert to RList." );
}

string SValue::get_special_field_string( Field fld )
{
    switch( fld )
    {
    case f_maximum:
        return glc().get_special_value_string( SpecialValue::plus_inf );
    case f_minimum:
        return glc().get_special_value_string( SpecialValue::minus_inf );
    case f_invalid:
        return glc().get_special_value_string( SpecialValue::unknown );
    }
    return std::to_string( fld );
}

string SValue::get_special_range_string( Range rng )
{
    if( rng.m_beg == rng.m_end ) {
        return get_special_field_string( rng.m_beg );
    }
    return
        get_special_field_string( rng.m_beg ) + ".." +
        get_special_field_string( rng.m_end );
}

string SValue::get_special_rlist_string( RList rlist )
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
        str += get_special_range_string( rng );
        first = false;
    }
    return str;
}


// End of src/glc/glcValue.cpp