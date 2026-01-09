/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glcTokenStream.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Translate a character stream to a higs token stream.
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

#ifndef SRC_GLC_GLCTOKENSTREAM_H_GUARD
#define SRC_GLC_GLCTOKENSTREAM_H_GUARD

#include "glcValue.h"

#include <iostream>

namespace glich {

    class Glich;

    class SToken
    {
    public:
        enum class Type {
            Null, End, String, Name, Number, Field, Real,
            Equal, Plus, Minus, Divide, Star,
            PlusEq, MinusEq, DivideEq, StarEq,
            IntDivEq, ModEq,
            Percent, Backslash, DblBackslash, Tilde,
            Dot, Comma, Semicolon, Qmark,
            Vline, Ampersand, DblAmpersand, Exclamation, DblExclamation,
            Carrot, DblCarrot, At,
            Lbracket, Rbracket, LCbracket, RCbracket,
            LSbracket, RSbracket,
            Div, Mod, And, Or, Not,
            DotDot,      //  ".."
            NotEqual,    //  "<>"
            GtThan,      //  ">"
            GtThanEq,    //  ">="
            LessThan,    //  "<"
            LessThanEq,  //  "<="
            AtEq,        //  "@="
            RangeEq,     //  "..="
            UnionEq,     //  "|="
            IntersectEq, //  "&&="
            RelCompEq,   //  "\\="
            SymDiffEq,   //  "^^="
            // Alternative token names
            UNION = Vline,
            INTERSECTION = DblAmpersand,
            REL_COMPLEMENT = DblBackslash,
            SYM_DIFFERENCE = DblCarrot,
            COMPLEMENT = Exclamation
        };

        SToken() : m_type(Type::End) {}
        SToken( const SToken& token );

        void set_type( Type type ) { m_type = type; }
        void set_value_str( const std::string& str ) { m_value.set_str( str ); }
        void set_value_num( Num num ) { m_value.set_number( num ); }
        void set_value_field( Field fld ) { m_value.set_field( fld ); }
        void set_value_real( double real ) { m_value.set_float( real ); }

        Type type() const { return m_type; }
        SValue value() const { return m_value; }
        std::string get_str() const { return m_value.get_str(); }
        bool get_bool() const { return m_value.get_bool(); }
        Num get_number() const { return m_value.get_number(); }
        Field get_field() const { return m_value.get_field(); }
        Field get_real() const { return m_value.get_float(); }

    private:
        Type m_type;
        SValue m_value;
    };

    class STokenStream
    {
    public:
        STokenStream( std::istream& in, std::ostream& err )
            : m_in( &in ), m_err( &err ), m_line( 1 ), m_errors( 0 ) {}

        SToken& next();
        SToken& current() { return m_cur_token; }
        void skip_to( SToken::Type type );
        std::string read_until( const std::string& name, const std::string& esc );
        std::string read_do_block();
        bool skip_to_char( char end );

        bool error( const std::string& mess );
        bool error_value( const std::string& mess );
        int errors() const { return m_errors; }

        int get_line() const { return m_line; }
        void set_line( int line ) { m_line = line; }
        std::istream* reset_in( std::istream* in );

    private:
        void set_type( SToken::Type type ) { m_cur_token.set_type( type ); }
        void set_current( SToken::Type type, const std::string& str );

        std::istream* m_in;
        std::ostream* m_err;
        SToken        m_cur_token;
        int           m_line;
        int           m_errors;
    };
}

#endif // SRC_GLC_GLCTOKENSTREAM_H_GUARD
