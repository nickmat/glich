/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glcTokenStream.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Translate a character stream to a Glich token stream.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     5th February 2023
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

#include "glcTokenStream.h"

#include <glc/glc.h>
#include "glcHelper.h"

#include <string>
#include <vector>

#include <cassert>

using namespace glich;
using std::string;
using std::vector;

#define MAX_ALLOWED_ERRORS 5


SToken::SToken( const SToken& token )
{
    m_type = token.m_type;
    m_value = token.m_value;
}

SToken& STokenStream::next()
{
    char ch;
    SToken::Type str_token = SToken::Type::String;
    bool lcomment = false, mcomment = false, inquote = false;

    do { // Skip whitespace.
        if( !m_in->get( ch ) ) {
            set_type( SToken::Type::End );
            return m_cur_token;
        }
        if( ch == '"' ) {
            inquote = !inquote;
        }
        if( !inquote ) {
            if( ch == '/' && m_in->peek() == '*' ) {
                mcomment = true;
            }
            if( ch == '*' && m_in->peek() == '/' ) {
                m_in->get( ch );
                m_in->get( ch );
                mcomment = false;
            }
            if( ch == '/' && m_in->peek() == '/' ) {
                lcomment = true;
            }
        }
        if( ch == '\n' ) {
            m_line++;
            lcomment = false;
        }
    } while( u8_isspace( ch ) || lcomment || mcomment );

    if( isdigit( ch ) ) {
        string text;
        do {
            text += ch;
        } while( m_in->get( ch ) && isdigit( ch ) );
        if( ch == 'f' || ch == 'n' || ch == '.' ) {
            int ch2 = m_in->peek();
            if( (ch != '.' || ch2 != '.') ) {
                if( ch == '.' ) {
                    text += ch;
                    ch = m_in->get();
                    if( isdigit( ch ) ) {
                        do {
                            text += ch;
                        } while( m_in->get( ch ) && isdigit( ch ) );
                    }
                    m_in->putback(ch);
                    set_current( SToken::Type::Real, text );
                    return m_cur_token;
                }
                else if( !isalpha( ch2 ) ) {
                    SToken::Type type = SToken::Type::Number;
                    if( ch == 'f' ) {
                        type = SToken::Type::Field;
                    }
                    set_current( type, text );
                    return m_cur_token;
                }
            }
        }
        m_in->putback( ch );
        SToken::Type type = SToken::Type::Number;
        Integer integer = glc().get_integer();
        if( integer == Integer::field ) {
            type = SToken::Type::Field;
        }
        set_current( type, text );
        return m_cur_token;
    }

    // TODO: This should recognise a utf-8 alpha codepoint
    string str;
    if( isascii( ch ) && (isalpha( ch ) || ch == '_' || ch == ':') ) {
        str += ch;
        while( m_in->get( ch ) ) {
            if( isascii( ch ) && (isalnum( ch ) || ch == '_' || ch == ':') )
            {
                str += ch;
            }
            else {
                break;
            }
        }
        m_in->putback( ch );
        if( str == "or" ) {
            set_type( SToken::Type::Or );
        }
        else if( str == "and" ) {
            set_type( SToken::Type::And );
        }
        else if( str == "not" ) {
            set_type( SToken::Type::Not );
        }
        else if( str == "div" ) {
            set_type( SToken::Type::Div );
        }
        else if( str == "mod" ) {
            set_type( SToken::Type::Mod );
        }
        else {
            set_current( SToken::Type::Name, str );
        }
        return m_cur_token;
    }

    if( ch == '"' ) {
        string text;
        while( m_in->get( ch ) ) {
            if( ch == '"' ) {
                if( m_in->peek() == '"' ) {
                    m_in->get( ch );
                }
                else {
                    break;
                }
            }
            if( ch == '\n' ) {
                // We are allowing new lines in comments at the moment.
                m_line++;
            }
            text += ch;
        }
        set_current( str_token, text );
        return m_cur_token;
    }

    switch( ch )
    {
    case '=': set_type( SToken::Type::Equal ); break;
    case '+':
        switch( m_in->peek() ) {
        case '=': m_in->get( ch ); set_type( SToken::Type::PlusEq ); break;
        default: set_type( SToken::Type::Plus ); break;
        } break;
    case '-':
        switch( m_in->peek() ) {
        case '=': m_in->get( ch ); set_type( SToken::Type::MinusEq ); break;
        default: set_type( SToken::Type::Minus ); break;
        } break;
    case '/':
        switch( m_in->peek() ) {
        case '=': m_in->get( ch ); set_type( SToken::Type::DivideEq ); break;
        default: set_type( SToken::Type::Divide ); break;
        } break;
    case '*':
        switch( m_in->peek() ) {
        case '=': m_in->get( ch ); set_type( SToken::Type::StarEq ); break;
        default: set_type( SToken::Type::Star ); break;
        } break;
    case '%':
        switch( m_in->peek() )
        {
        case '/': m_in->get( ch ); set_type( SToken::Type::Div ); break;
        case '%': m_in->get( ch ); set_type( SToken::Type::Mod ); break;
        default: set_type( SToken::Type::Percent ); break;
        }
        break;
    case '<':
        switch( m_in->peek() ) {
        case '>': m_in->get( ch ); set_type( SToken::Type::NotEqual ); break;
        case '=': m_in->get( ch ); set_type( SToken::Type::LessThanEq ); break;
        default: set_type( SToken::Type::LessThan ); break;
        } break;
    case '>':
        switch( m_in->peek() ) {
        case '=': m_in->get( ch ); set_type( SToken::Type::GtThanEq ); break;
        default: set_type( SToken::Type::GtThan ); break;
        } break;
    case '(': set_type( SToken::Type::Lbracket ); break;
    case ')': set_type( SToken::Type::Rbracket ); break;
    case '{': set_type( SToken::Type::LCbracket ); break;
    case '}': set_type( SToken::Type::RCbracket ); break;
    case '[': set_type( SToken::Type::LSbracket ); break;
    case ']': set_type( SToken::Type::RSbracket ); break;
    case ';': set_type( SToken::Type::Semicolon ); break;
    case ',': set_type( SToken::Type::Comma ); break;
    case '.':
        switch( m_in->peek() ) {
        case '.': m_in->get( ch ); set_type( SToken::Type::DotDot ); break;
        default: set_type( SToken::Type::Dot ); break;
        } break;
    case '~': set_type( SToken::Type::Tilde ); break;
    case '|': set_type( SToken::Type::Vline ); break;
    case '&':
        switch( m_in->peek() ) {
        case '&': m_in->get( ch ); set_type( SToken::Type::DblAmpersand ); break;
        default: set_type( SToken::Type::Ampersand ); break;
        } break;        
    case '!':
        switch( m_in->peek() ) {
        case '!': m_in->get( ch ); set_type( SToken::Type::DblExclamation ); break;
        default: set_type( SToken::Type::Exclamation ); break;
        } break;
    case '^':
        switch( m_in->peek() ) {
        case '^': m_in->get( ch ); set_type( SToken::Type::DblCarrot ); break;
        default: set_type( SToken::Type::Carrot ); break;
        } break;
    case '\\':
        switch( m_in->peek() ) {
        case '\\': m_in->get( ch ); set_type( SToken::Type::DblBackslash ); break;
        default: set_type( SToken::Type::Backslash ); break;
        } break;
    case '?': set_type( SToken::Type::Qmark ); break;
    case '@':
        switch( m_in->peek() ) {
        case '=': m_in->get( ch ); set_type( SToken::Type::AtEq ); break;
        default: set_type( SToken::Type::At ); break;
        } break;
    default:
        error( "Unrecognised token." );
        set_type( SToken::Type::End );
        break; // Error
    }
    return m_cur_token;
}

void STokenStream::skip_to( SToken::Type type )
{
    while( m_cur_token.type() != type && m_cur_token.type() != SToken::Type::End ) {
        next();
    }
}

std::string STokenStream::read_until( const std::string& name, const std::string& esc )
{
    string code;
    char ch;
    int count = 0;
    bool braces = (name == "}");
    bool terminal = (name == ";");
    for( ;;) {
        string word;
        while( m_in->get( ch ) && isalnum( ch ) ) {
            word += ch;
        }
        code += word;
        if( ch == '\n' ) {
            m_line++;
        }
        if( braces ) {
            if( ch == '{' ) {
                count++;
            }
            if( ch == '}' ) {
                if( count == 0 ) {
                    break;
                }
                --count;
            }
        }
        else if( terminal && ch == ';' ) {
            break;
        }
        else if( !word.empty() ) {
            if( word == esc ) {
                count++;
            }
            if( word == name ) {
                if( count == 0 ) {
                    break;
                }
                --count;
            }
        }
        if( ch == '/' ) {
            if( m_in->peek() == '*' ) { // Multiline comments
                m_in->get(); // Step over '*'
                while( m_in->get( ch ) ) {
                    if( ch == '\n' ) {
                        m_line++;
                        code += '\n'; // Keep line numbers in sync.
                    }
                    if( ch == '*' && m_in->peek() == '/' ) {
                        m_in->get(); // Step over '/'
                        ch = ' '; // Replace comment with a space
                        break;
                    }
                }
            }
            if( m_in->peek() == '/' ) { // Singleline comments
                m_in->get(); // Step over '/'
                while( m_in->get( ch ) ) {
                    if( ch == '\n' ) {
                        m_line++;
                        break;
                    }
                }
            }
        }
        if( ch == '"' ) {
            code += ch;
            while( m_in->get( ch ) && ch != '"' ) {
                if( ch == '\n' ) {
                    m_line++;
                }
                code += ch;
            }
        }
        code += ch;
        if( m_in->eof() ) { // End of stream before name found.
            return "";
        }
    }
    return code;
}

// This is a tempory method to read a do...loop or do {...}; block.
// When the do ... loop construct is removed this method can be
// replaced with read_until( "}", "{" );
std::string STokenStream::read_do_block()
{
    string code;
    string name = "loop";
    string esc = "do";
    char ch;
    int count = 0;
    bool braces = false; // (name == "}");
    bool started = false;
    for( ;;) {
        string word;
        while( m_in->get( ch ) && isalnum( ch ) ) {
            word += ch;
        }
        code += word;
        if( !started ) {
            if( ch == '{' ) {
                braces = true;
                name = "}";
                esc = "{";
                started = true;
                continue;
            }
            if( !word.empty() ) {
                started = true;
            }
        }
        if( ch == '\n' ) {
            m_line++;
        }
        if( braces ) {
            if( ch == '{' ) {
                count++;
            }
            if( ch == '}' ) {
                if( count == 0 ) {
                    code += ch;
                    break;
                }
                --count;
            }
        }
        else if( !word.empty() ) {
            if( word == esc ) {
                count++;
            }
            if( word == name ) {
                if( count == 0 ) {
                    code += ch;
                    break;
                }
                --count;
            }
        }
        if( ch == '/' ) {
            if( m_in->peek() == '*' ) { // Multiline comments
                m_in->get(); // Step over '*'
                while( m_in->get( ch ) ) {
                    if( ch == '\n' ) {
                        m_line++;
                        code += '\n'; // Keep line numbers in sync.
                    }
                    if( ch == '*' && m_in->peek() == '/' ) {
                        m_in->get(); // Step over '/'
                        ch = ' '; // Replace comment with a space
                        break;
                    }
                }
            }
            if( m_in->peek() == '/' ) { // Singleline comments
                m_in->get(); // Step over '/'
                while( m_in->get( ch ) ) {
                    if( ch == '\n' ) {
                        m_line++;
                        break;
                    }
                }
            }
        }
        if( ch == '"' ) {
            code += ch;
            while( m_in->get( ch ) && ch != '"' ) {
                if( ch == '\n' ) {
                    m_line++;
                }
                code += ch;
            }
        }
        code += ch;
        if( m_in->eof() ) { // End of stream before name found.
            return "";
        }
    }
    return code;
}

bool STokenStream::skip_to_char( char end )
{
    char ch;
    for( ;;) {
        if( !m_in->get( ch ) ) {
            return false;
        }
        if( ch == end ) {
            return true;
        }
        // Skip comments
        if( ch == '/' ) {
            if( m_in->peek() == '*' ) { // Multiline comments
                m_in->get(); // Step over '*'
                while( m_in->get( ch ) ) {
                    if( ch == '\n' ) {
                        m_line++;
                    }
                    if( ch == '*' && m_in->peek() == '/' ) {
                        m_in->get(); // Step over '/'
                        ch = ' '; // Replace comment with a space
                        break;
                    }
                }
            }
            if( m_in->peek() == '/' ) { // Singleline comments
                m_in->get(); // Step over '/'
                while( m_in->get( ch ) ) {
                    if( ch == '\n' ) {
                        m_line++;
                        break;
                    }
                }
            }
        }
        // Skip strings
        if( ch == '"' ) {
            while( m_in->get( ch ) && ch != '"' ) {
                if( ch == '\n' ) {
                    m_line++;
                }
            }
        }
        // skip blocks
        if( ch == '{' ) {
            int nested = 1;
            while( nested > 0 && m_in->get( ch ) ) {
                if( ch == '{' ) {
                    nested++;
                }
                else if( ch == '}' ) {
                    nested--;
                }
                else if( ch == '\n' ) {
                    m_line++;
                }
            }
        }
        if( ch == '\n' ) {
            m_line++;
        }
    }
}

bool STokenStream::error( const std::string& mess )
{
    *m_err << "Error (" << m_line << "): " << mess << "\n";
    return (++m_errors > MAX_ALLOWED_ERRORS);
}

bool STokenStream::error_value( const std::string& mess )
{
    *m_err << mess;
    return (++m_errors > MAX_ALLOWED_ERRORS);
}

std::istream* STokenStream::reset_in( std::istream* in )
{
    std::istream* prev = m_in;
    m_in = in;
    return prev;
}

void STokenStream::set_current( SToken::Type type, const std::string& str )
{
    m_cur_token.set_type( type );
    switch( type )
    {
    case SToken::Type::Real:
        m_cur_token.set_value_real( str_to_float( str ) );
        break;
    case SToken::Type::Number:
        m_cur_token.set_value_num( str_to_num( str ) );
        break;
    case SToken::Type::Field:
        m_cur_token.set_value_field( str_to_field( str ) );
        break;
    default:
        m_cur_token.set_value_str( str );
        break;
    }
}


// End of src/glc/glcTokenStream.cpp
