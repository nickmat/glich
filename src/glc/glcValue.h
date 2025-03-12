/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glcValue.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Class to hold all value types used by script.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     8th February 2023
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

#ifndef SRC_GLC_GLCVALUE_H_GUARD
#define SRC_GLC_GLCVALUE_H_GUARD

#include "glc/glcDefs.h"

#include "glcBlob.h"

#include <variant>

namespace glich {

    class Glich;
    class Store;
    class SValue;
    class Object;
    using SValueVec = std::vector<SValue>;

    class SValue
    {
    public:
        enum class Type {
            Null, Error, String, Bool, Number, field, range, rlist, Float, Object, blob
        };
        static constexpr const char* s_typename[] = {
            "null", "error", "string", "bool", "number", "field", "range", "rlist", "float",
            "object", "blob"
        };
        SValue() : m_type( Type::Null ) {}
        SValue( const SValue& value );
        SValue( const std::string& str ) : m_type( Type::String ), m_data( str ) {}
        SValue( const char* str, Type type = Type::String ) : m_type( type ), m_data( std::string( str ) ) {}
        SValue( Num num, Type type ) : m_type( type ), m_data( num ) {}
        SValue( int num ) : m_type( Type::Number ), m_data( static_cast<Num>(num) ) {}
        SValue( bool b ) : m_type( Type::Bool ), m_data( b ) {}
        SValue( Range r ) : m_type( Type::range ), m_data( r ) {}
        SValue( const RList& rl ) : m_type( Type::rlist ), m_data( rl ) {}
        SValue( double real ) : m_type( Type::Float ), m_data( real) {}
        SValue( SValueVec obj ) : m_type( Type::Object ), m_data( obj ) {}
        SValue( Blob& blob ) : m_type( Type::blob ), m_data( blob ) {}

        static SValue create_error( const std::string& mess );

        void set_str( const std::string& str ) { m_type = Type::String; m_data = str; }
        void set_bool( bool b ) { m_type = Type::Bool; m_data = b; }
        void set_number( Num num ) { m_type = Type::Number; m_data = num; }
        void set_field( Field fld ) { m_type = Type::field; m_data = static_cast<Num>(fld); }
        void set_range( Range rng ) { m_type = Type::range; m_data = rng; }
        void set_rlist( const RList& rlist ) { m_type = Type::rlist; m_data = rlist; }
        void set_float( double real ) { m_type = Type::Float; m_data = real; }
        void set_object( SValueVec obj ) { m_type = Type::Object; m_data = obj; }
        void set_blob( Blob& blob ) { m_type = Type::blob; m_data = blob; }

        void set_range_demote( Range rng );
        void set_rlist_demote( const RList& rlist );

        void set_error( const std::string& str );

        SValue* get_object_element( size_t index, size_t expand = 10 );
        SValueVec* get_object_values();
        const SValueVec* get_object_values() const;
        Object* get_object_ptr();
        void object_fill_store( Store* store );

        std::string as_string() const;
        std::string object_to_string( const SValueVec& values ) const;
        std::string blob_to_string( const Blob& blob ) const;

        std::string get_str() const;
        Num get_number() const;
        bool get_bool() const;
        Field get_field() const;
        Range get_range() const;
        RList get_rlist() const;
        double get_float() const;
        SValueVec get_object() const;
        Blob get_blob() const;
        Field get_num_as_field() const;
        double get_field_as_float() const;

        Field get_as_field() const;

        std::string get_str( bool& success ) const;
        Num get_number( bool& success ) const;
        bool get_bool( bool& success ) const;
        Field get_field( bool& success ) const; // Demote if possible.
        Range get_range( bool& success ) const; // Promote or demote if possible.
        RList get_rlist( bool& success ) const; // Promote if possible.
        double get_float( bool& success ) const;
        SValueVec get_object( bool& success ) const;
        std::string get_object_code() const;
        Blob get_blob( bool& success ) const;

        Field get_int_as_field( bool& success ) const; // Num or Field as Field
        size_t get_int_as_size_t( bool& success ) const; // Num or Field as size_t
        double get_any_as_float( bool& success ) const; // Num, Field or double as float.

        bool is_error() const { return m_type == Type::Error; }
        bool propagate_error( const SValue& value );
        bool obtain_rlists( RList& left, RList& right, const SValue& value );

        void logical_or( const SValue& value );
        void logical_and( const SValue& value );
        void equal( const SValue& value );
        void greater_than( const SValue& value );
        void less_than( const SValue& value );
        void plus( const SValue& value );
        void minus( const SValue& value );
        void multiply( const SValue& value );
        void divide( const SValue& value );
        void int_div( const SValue& value );
        void modulus( const SValue& value );

        void rlist_union( const SValue& value );
        void intersection( const SValue& value );
        void rel_complement( const SValue& value );
        void sym_difference( const SValue& value );

        void range_op( const SValue& value );
        void property_op( const SValue& value );
        void mask_op( const SValue& value );

        void negate(); // Unitary minus
        void logical_not();
        void compliment();

        Type type() const { return m_type; }
        const char* type_str() const { return s_typename[static_cast<size_t>(m_type)]; }
        bool is_integer() const { return (m_type == Type::Number || m_type == Type::field); }

    private:
        static std::string get_special_field_string( Field fld );
        static std::string get_special_range_string( Range rng );
        static std::string get_special_rlist_string( RList rlist );

        Type m_type;
        std::variant<bool, Num, std::string, Range, RList, double, SValueVec, Blob> m_data;
    };

    using SValueVec = std::vector<SValue>;


}

#endif // SRC_GLC_GLCVALUE_H_GUARD