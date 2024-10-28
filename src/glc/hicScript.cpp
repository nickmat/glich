/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicScript.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Create Scheme and associated class implimentations.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
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

#include "hicScript.h"

#include "glcFunction.h"
#include "glcHelper.h"
#include "glcMath.h"
#include "glcScript.h"
#include "hicBase.h"
#include "hicCalendars.h"
#include "hicDatePhrase.h"
#include "hicElement.h"
#include "hicGrammar.h"
#include "hicFormatIso.h"
#include "hicFormatText.h"
#include "hicFormatUnit.h"
#include "hicLexicon.h"
#include "hicRecord.h"
#include "hicScheme.h"

#include <cassert>
#include <sstream>

using namespace glich;
using std::string;


bool HicScript::statement()
{
    SToken token = current_token();
    if( token.type() == SToken::Type::Name ) {
        string name = token.get_str();
        if( name == "scheme" ) return do_scheme();
        if( name == "lexicon" ) return do_lexicon();
        if( name == "grammar" ) return do_grammar();
        if( name == "format" ) return do_format();
    }
    return Script::statement();
}

SValue HicScript::builtin_function_call( bool& success, const string& name )
{
    enum f {
        f_date, f_text, f_record, f_scheme, f_element, f_phrase, f_leapyear, f_first, f_last,
        f_fmt_object, f_sch_object, f_easter, f_sch_list
    };
    const static std::map<string, f> fmap = {
        { "date", f_date }, { "text", f_text }, { "record", f_record }, { "scheme", f_scheme }, { "element", f_element },
        { "phrase", f_phrase }, { "leapyear", f_leapyear }, { "first", f_first }, { "last", f_last },
        { "fmt:object", f_fmt_object }, { "sch:object", f_sch_object }, { "easter", f_easter },
        { "sch:list", f_sch_list }
    };

    SValue value = Script::builtin_function_call( success, name );
    if( success ) {
        return value;
    }

    auto fnum = fmap.find( name );
    if( fnum != fmap.end() ) {
        success = true;
        switch( fnum->second )
        {
        case f_date: return at_date();
        case f_text: return at_text( *this );
        case f_record: [[fallthrough]];
        case f_scheme: return at_scheme( *this );
        case f_element: return at_element( *this );
        case f_phrase: return at_phrase( *this );
        case f_leapyear: return at_leapyear( *this );
        case f_first: return at_last( *this );
        case f_last: return at_first( *this );
        case f_fmt_object: return at_fmt_object( *this );
        case f_sch_object: return at_sch_object( *this );
        case f_easter: return at_easter( *this );
        case f_sch_list: return at_sch_list( *this );
        }
        return SValue::create_error( "Built-in function error." );
    }
    success = false;
    return SValue();
}

bool HicScript::do_scheme()
{
    string code = get_name_or_primary( GetToken::next );
    if( code.empty() ) {
        error( "Scheme code missing." );
        return false;
    }
    DefinedStatus status = glc().get_scheme_status( code );
    if( status == DefinedStatus::defined ) {
        error( "Scheme \"" + code + "\" already exists." );
        return false;
    }
    Scheme* sch = do_create_scheme( code );
    return glc().add_scheme( sch, code );
}

bool HicScript::do_lexicon()
{
    string code = get_name_or_primary( GetToken::next );
    if( code.empty() ) {
        error( "Lexicon code missing." );
        return false;
    }
    DefinedStatus status = glc().get_lexicon_status( code );
    if( status == DefinedStatus::defined ) {
        error( "Lexicon \"" + code + "\" already exists." );
        return false;
    }
    Lexicon* lex = do_create_lexicon( code );
    return glc().add_lexicon( lex, code );
}

bool HicScript::do_grammar()
{
    string code = get_name_or_primary( GetToken::next );
    if( code.empty() ) {
        error( "Grammar code missing." );
        return false;
    }
    DefinedStatus status = glc().get_grammar_status( code );
    if( status == DefinedStatus::defined ) {
        error( "Grammar \"" + code + "\" already exists." );
        return false;
    }
    Grammar* gmr = do_create_grammar( code, nullptr );
    return glc().add_grammar( gmr, code );
}

// If parsing the format in the global space then gmr is a nullptr and
// the Grammar to be used should be encoded into the Format code,
// as in "gmr:fmt"
// If parsing the format within a grammar then gmr is not a nullptr
// and the Format code just contains just the Format code, as in "fmt".
bool HicScript::do_format( Grammar* gmr )
{
    string code = get_name_or_primary( GetToken::next );
    if( code.empty() ) {
        error( "Format code missing." );
        return false;
    }
    return do_create_format( code, gmr );
}

SValue HicScript::do_object_at( bool& success, Object* obj, const string& fcode, const SValue& left )
{
    Scheme* sch = dynamic_cast<Scheme*>(obj);
    if( sch == nullptr ) {
       return Script::do_object_at( success, obj, fcode, left );
    }
    SValueVec args = get_args( GetToken::current );
    SValue value;
    if( !args.empty() ) {
        value = args[0];
    }
    success = true;
    if( fcode == "pseudo_in" && value.type() == SValue::Type::String ) {
        Format* fmt = sch->get_input_format( value.get_str() );
        if( fmt ) {
            return SValue( fmt->get_input_str() );
        }
    }
    if( fcode == "pseudo_out" && value.type() == SValue::Type::String ) {
        Format* fmt = sch->get_output_format( value.get_str() );
        if( fmt ) {
            return SValue( fmt->get_output_str() );
        }
    }
    if( fcode == "name" ) {
        return SValue( sch->get_name() );
    }
    success = false;
    return SValue();
}

Scheme* HicScript::do_create_scheme( const string& code )
{
    SToken token = current_token();
    if( token.type() != SToken::Type::LCbracket ) {
        error( "'{' expected." );
        return nullptr;
    }
    bool error_ret = false;
    Base* base = nullptr;
    string name, gmr_code;
    Field epoch = f_invalid;
    int epoch_line = 0;
    Grammar* gmr = nullptr;
    bool visible = true;
    for( ;;) {
        token = next_token();
        if( token.type() == SToken::Type::RCbracket ||
            token.type() == SToken::Type::End ) {
            break;
        }
        if( token.type() == SToken::Type::Semicolon ) {
            continue;
        }
        if( token.type() == SToken::Type::Name ) {
            if( token.get_str() == "name" ) {
                name = expr( GetToken::next ).as_string();
            }
            else if( token.get_str() == "base" ) {
                base = do_base( code );
            }
            else if( token.get_str() == "epoch" ) {
                epoch = expr( GetToken::next ).get_as_field();
                epoch_line = get_line();
            }
            else if( token.get_str() == "grammar" ) {
                token = next_token();
                if( token.type() == SToken::Type::LCbracket ) {
                    gmr = do_create_grammar( "", base );
                    if( gmr == nullptr ) {
                        error( "Unable to create grammar." );
                        error_ret = true;
                    }
                }
                else {
                    gmr_code = get_name_or_primary( GetToken::current );
                    gmr = glc().get_grammar( gmr_code );
                    if( gmr == nullptr && !gmr_code.empty() ) {
                        error( "Grammar \"" + gmr_code + "\" not found." );
                        error_ret = true;
                    }
                }
            }
            else if( token.get_str() == "visible" ) {
                string str = get_name_or_primary( GetToken::next );
                if( str == "no" ) {
                    visible = false;
                }
                else if( str != "yes" ) {
                    error( "Visible yes or no expected." );
                    error_ret = true;
                }
            }
            else {
                error( "Scheme sub-statement expected." );
                error_ret = true;
            }
        }
    }
    if( base == nullptr ) {
        if( gmr_code.empty() ) {
            delete gmr;
        }
        error( "Scheme \"" + code + "\" not created." );
        return nullptr;
    }
    if( gmr == nullptr ) {
        gmr = Grammar::create_default_grammar( base, &glc() );
    }
    if( !base->attach_grammar( gmr ) ) {
        if( gmr_code.empty() ) {
            delete gmr;
        }
        delete base;
        error( "Unable to attach grammar." );
        return nullptr;
    }
    Scheme* sch = new Scheme( "s:" + code, *base );
    sch->reset();
    sch->set_name( name );
    sch->set_def_visible( visible );
    sch->set_cur_visible( visible );
    if( epoch != f_invalid ) {
        if( !sch->set_epoch( base, epoch, epoch_line ) ) {
            error( "Unable to set epoch." );
            error_ret = true;
        }
    }
    if( error_ret ) {
        delete sch;
        return nullptr;
    }
    return sch;
}

Lexicon* HicScript::do_create_lexicon( const string& code )
{
    SToken token = current_token();
    if( token.type() != SToken::Type::LCbracket ) {
        error( "'{' expected." );
        return nullptr;
    }
    Lexicon* lex = new Lexicon( code );
    string str;
    for( ;;) {
        token = next_token();
        if( token.type() == SToken::Type::RCbracket ||
            token.type() == SToken::Type::End ) {
            break; // All done.
        }
        else if( token.type() == SToken::Type::Name ) {
            string name = token.get_str();
            if( name == "name" ) {
                str = expr( GetToken::next ).as_string();
                lex->set_name( str );
            }
            else if( name == "inherit" ) {
                str = get_name_or_primary( GetToken::next );
                lex->set_inherit( str );
            }
            else if( name == "fieldname" ) {
                str = get_name_or_primary( GetToken::next );
                lex->set_fieldname( str );
            }
            else if( name == "lang" ) {
                str = get_name_or_primary( GetToken::next );
                lex->set_lang( str );
            }
            else if( name == "pseudo" ) {
                StdStrVec pseudos = get_string_list( GetToken::next );
                lex->set_pseudo_names( pseudos );
            }
            else if( name == "tokens" ) {
                do_lexicon_tokens( lex );
            }
            else {
                error( "Unknown lexicon subcommand." );
            }
        }
    }
    return lex;
}

Grammar* HicScript::do_create_grammar( const string& code, const Base* base )
{
    SToken token = current_token();
    if( token.type() != SToken::Type::LCbracket ) {
        error( "'{' expected." );
        return nullptr;
    }
    Grammar* gmr = new Grammar( code, &glc() );
    StdStrVec basefields;
    if( base != nullptr ) {
        for( size_t i = 0; i < base->required_size(); i++ ) {
            basefields.push_back( base->get_fieldname( i ) );
        }
        gmr->set_base_fieldnames( basefields );
    }
    string str;
    for( ;;) {
        token = next_token();
        if( token.type() == SToken::Type::LCbracket ) {
            continue;
        }
        else if( token.type() == SToken::Type::RCbracket ||
            token.type() == SToken::Type::End ) {
            break; // All done.
        }
        else if( token.type() == SToken::Type::Name ) {
            string name = token.get_str();
            if( name == "name" ) {
                str = expr( GetToken::next ).as_string();
                gmr->set_name( str );
            }
            else if( name == "lexicons" ) {
                do_grammar_lexicons( gmr );
            }
            else if( name == "format" ) {
                do_format( gmr );
            }
            else if( name == "preferred" ) {
                str = get_name_or_primary( GetToken::next );
                gmr->set_preferred( str );
            }
            else if( name == "alias" ) {
                do_grammar_alias( gmr );
            }
            else if( name == "inherit" ) {
                StdStrVec inherit = get_string_list( GetToken::next );
                gmr->set_inherit( inherit );
            }
            else if( name == "fields" ) {
                basefields = get_string_list( GetToken::next );
                gmr->set_base_fieldnames( basefields );
            }
            else if( name == "optional" ) {
                StdStrVec optfields = get_string_list( GetToken::next );
                gmr->set_opt_fieldnames( optfields );
            }
            else if( name == "calculated" ) {
                StdStrVec calcfields = get_string_list( GetToken::next );
                gmr->set_calc_fieldnames( calcfields );
            }
            else if( name == "rank" ) {
                StdStrVec rankfields = get_string_list( GetToken::next );
                gmr->set_rank_fieldnames( rankfields );
            }
            else if( name == "function" ) {
                do_grammar_function( gmr );
            }
            else if( name == "use" ) {
                do_grammar_use( gmr );
            }
        }
        else {
            error( "Grammar sub-statement expected." );
        }
    }
    gmr->constuct();
    if( !gmr->is_ok() ) {
        string gmr_error = gmr->get_error_string();
        delete gmr;
        gmr = nullptr;
        if( gmr_error.empty() ) {
            gmr_error = "Unable to construct grammar \"" + code + "\".";
        }
        error( gmr_error );
    }
    return gmr;
}

// If gmr == nullptr then this is a standalone format.
bool HicScript::do_create_format( const string& code, Grammar* gmr )
{
    bool in_gmr = bool( gmr );
    string gcode, fcode;
    if( code.find( ':' ) == string::npos ) {
        assert( in_gmr );
        fcode = code;
    }
    else {
        assert( !in_gmr );
        split_code( &gcode, &fcode, code );
        gmr = glc().get_grammar( gcode );
        if( gmr == nullptr ) {
            error( "Grammar not found." );
            return false;
        }
    }
    assert( gmr != nullptr );

    string format_in, format_out, instring, outstring, separators, infun;
    StdStrVec rankfields, rankoutfields, rules;
    bool visible = true;
    if( current_token().type() == SToken::Type::LCbracket ) {
        for( ;;) {
            SToken token = next_token();
            if( token.type() == SToken::Type::RCbracket ||
                token.type() == SToken::Type::End ) {
                break;
            }
            if( token.type() == SToken::Type::Semicolon ) {
                continue;
            }
            if( token.type() == SToken::Type::Name ) {
                string name = token.get_str();
                if( name == "output" ) {
                    format_out = expr( GetToken::next ).as_string();
                    continue;
                }
                if( name == "inout" ) {
                    format_out = expr( GetToken::next ).as_string();
                    format_in = format_out;
                    continue;
                }
                if( name == "input" ) {
                    format_in = expr( GetToken::next ).as_string();
                    continue;
                }
                if( name == "separators" ) {
                    separators = expr( GetToken::next ).as_string();
                    continue;
                }
                if( name == "rank" ) {
                    rankfields = get_string_list( GetToken::next );
                    continue;
                }
                if( name == "rankout" ) {
                    rankoutfields = get_string_list( GetToken::next );
                    continue;
                }
                if( name == "visible" ) {
                    string str = get_name_or_primary( GetToken::next );
                    if( str == "no" ) {
                        visible = false;
                    }
                    else if( str != "yes" ) {
                        error( "Visiblity yes or no expected." );
                    }
                    continue;
                }
                if( name == "pseudo:in" ) {
                    instring = expr( GetToken::next ).as_string();
                    continue;
                }
                if( name == "pseudo:out" ) {
                    outstring = expr( GetToken::next ).as_string();
                    continue;
                }
                if( name == "rules" ) {
                    rules = get_string_list( GetToken::next );
                    continue;
                }
                if( name == "use:in" ) {
                    infun = get_name_or_primary( GetToken::next );
                    continue;
                }
                error( "Expected format sub-statement." );
            }
        }
    }
    else {
        if( current_token().type() == SToken::Type::Comma ) {
            next_token();
        }
        format_out = expr( GetToken::current ).as_string();
        if( format_out.empty() ) {
            error( "Format missing." );
            return false;
        }
        format_in = format_out;
        if( current_token().type() != SToken::Type::Semicolon ) {
            error( "';' expected." );
            return false;
        }
    }

    Format* fmt = nullptr;
    if( rules.empty() || rules[0] == "text" ) {
        if( format_in.empty() && format_out.empty() ) {
            error( "Format string not found." );
            return false;
        }
        FormatText* fmtt = new FormatText( fcode, *gmr ); // create_format_text( script.get_glich(), code, gmr );
        if( fmtt == nullptr ) {
            error( "Unable to create format." );
            return false;
        }
        if( separators.size() ) {
            fmtt->set_separators( separators );
        }
        if( rankfields.size() ) {
            fmtt->set_rank_fieldnames( rankfields );
        }
        if( rankoutfields.size() ) {
            fmtt->set_rankout_fieldnames( rankoutfields );
        }
        if( !format_out.empty() ) {
            fmtt->set_control_out( format_out );
        }
        if( !format_in.empty() ) {
            fmtt->set_control_in( format_in );
        }
        if( !instring.empty() ) {
            fmtt->set_user_input_str( instring );
        }
        if( !outstring.empty() ) {
            fmtt->set_user_output_str( outstring );
        }
        if( !infun.empty() ) {
            fmtt->set_from_text_function( infun );
        }
        fmt = fmtt;
    }
    else if( rules[0] == "iso8601" ) {
        fmt = new FormatIso( fcode, *gmr, rules ); // create_format_iso( script.get_glich(), code, gmr, rules );
        if( fmt == nullptr ) {
            error( "Unable to create ISO format." );
            return false;
        }
    }
    else if( rules[0] == "units" ) {
        fmt = new FormatUnit( fcode, *gmr ); // create_format_unit( script.get_glich(), code, gmr );
        if( fmt == nullptr ) {
            error( "Unable to create Units format." );
            return false;
        }
    }
    else {
        error( "Unknown rules statement." );
        return false;
    }
    assert( fmt != nullptr );
    fmt->set_visible( visible );
    fmt->construct();

    if( !gmr->add_format( fmt ) ) {
        error( "Unable to add format to grammar." );
        return false;
    }
    if( !in_gmr ) {
        glc().add_format( code );
    }
    return true;
}

Base* HicScript::do_base( const string& code )
{
    Scheme::BaseName bs = Scheme::BaseName::null;
    StdStrVec data;
    SToken token =next_token();
    if( token.type() == SToken::Type::Name ) {
        string name = token.get_str();
        if( name == "julian" ) {
            bs = Scheme::BaseName::julian;
        }
        else if( name == "gregorian" ) {
            bs = Scheme::BaseName::gregorian;
        }
        else if( name == "isoweek" ) {
            bs = Scheme::BaseName::isoweek;
        }
        else if( name == "ordinal" ) {
            bs = Scheme::BaseName::ordinal;
        }
        else if( name == "hebrew" ) {
            bs = Scheme::BaseName::hebrew;
        }
        else if( name == "french" ) {
            bs = Scheme::BaseName::french;
        }
        else if( name == "islamic" ) {
            bs = Scheme::BaseName::islamic;
        }
        else if( name == "chinese" ) {
            bs = Scheme::BaseName::chinese;
        }
        else if( name == "easter" ) {
            bs = Scheme::BaseName::easter;
        }
        else if( name == "liturgical" ) {
            bs = Scheme::BaseName::liturgical;
        }
        else if( name == "jwn" ) {
            bs = Scheme::BaseName::jwn;
        }
        else if( name == "jdn" ) {
            bs = Scheme::BaseName::jdn;
        }
        else if( name == "hybrid" ) {
            return do_base_hybrid( code );
        }
        else {
            error( "Base scheme not recognised." );
            return nullptr;
        }
        token = next_token();
        if( token.type() != SToken::Type::Semicolon ) {
            data = get_string_list( GetToken::current );
        }
    }
    else {
        error( "Base name expected." );
    }
    return Scheme::create_base( bs, data );
}

Base* HicScript::do_base_hybrid( const string& hscode )
{
    SToken token = next_token();
    if( token.type() != SToken::Type::LCbracket ) {
        error( "'{' expected." );
        return NULL;
    }
    StdStrVec fieldnames;
    HybridData data;
    data.start = f_minimum;
    std::vector<HybridData> data_vec;
    for( ;;) {
        SToken token = next_token();
        if( token.type() == SToken::Type::RCbracket ||
            token.type() == SToken::Type::End ) {
            break;
        }
        if( token.type() == SToken::Type::Semicolon ) {
            continue;
        }
        SValue value;
        if( token.type() == SToken::Type::Name ) {
            if( token.get_str() == "fields" ) {
                fieldnames = get_string_list( GetToken::next );
            }
            else if( token.get_str() == "scheme" ) {
                string scode = hscode + ":" + get_name_or_primary( GetToken::next );
                Scheme* sch = do_create_scheme( scode );
                if( sch == nullptr ) {
                    error( "Unable to create scheme " + scode );
                    continue;
                }
                data.base = &sch->get_base();
                data.scheme = sch;
                if( data.ok() ) {
                    data_vec.push_back( data );
                    data = HybridData();
                }
                else {
                    delete data.scheme;
                    error( "Hybrid data not complete." );
                }
            }
            else if( token.get_str() == "change" ) {
                data.start = expr( GetToken::next ).get_as_field();
                if( data.start == f_invalid ) {
                    error( "Change start must be Field." );
                }
                size_t datasize = data_vec.size();
                if( datasize > 0 ) {
                    data_vec[datasize - 1].end = data.start;
                }
            }
            else {
                error( "Unrecognised statement." );
            }
        }
    }
    return Scheme::create_base_hybrid( fieldnames, data_vec );
}

bool HicScript::do_lexicon_tokens( Lexicon* lex )
{
    SToken token = next_token();
    if( token.type() != SToken::Type::LCbracket ) {
        error( "'{' expected." );
        return false;
    }
    while( next_token().type() != SToken::Type::RCbracket &&
        current_token().type() != SToken::Type::End )
    {
        bool success = false;
        Field number = expr( GetToken::current ).get_field( success );
        if( !success ) {
            error( "Number expected." );
            return false;
        }
        if( current_token().type() != SToken::Type::Comma ) {
            error( "',' expected." );
            return false;
        }
        StdStrVec names = get_string_list( GetToken::next );
        string name, abbrev;
        if( names.size() > 0 ) {
            name = names[0];
        }
        if( names.size() > 1 ) {
            abbrev = names[1];
        }
        lex->add_token( number, name, abbrev );
    }
    return true;
}

void HicScript::do_grammar_lexicons( Grammar* gmr )
{
    StdStrVec lexicons = get_string_list( GetToken::next );
    for( size_t i = 0; i < lexicons.size(); i++ ) {
        Lexicon* lex = glc().get_lexicon( lexicons[i] );
        if( lex == nullptr ) {
            gmr->create_error( "lexicon " + lexicons[i] + " not found." );
        }
        else {
            gmr->add_lexicon( lex );
        }
    }
}

bool HicScript::do_grammar_alias( Grammar* gmr )
{
    string alias = get_name_or_primary( GetToken::next );
    if( current_token().type() != SToken::Type::LCbracket ) {
        error( "'{' expected." );
        return false;
    }
    StdStrVec pairs;
    for( ;;) {
        // Look ahead for '}'
        next_token();
        if( current_token().type() == SToken::Type::RCbracket ||
            current_token().type() == SToken::Type::End )
        {
            break; // All done.
        }
        StdStrVec pair = get_string_list( GetToken::current );
        if( pair.size() != 2 ) {
            error( "Name or String pair expected." );
            return false;
        }
        if( current_token().type() != SToken::Type::Semicolon ) {
            error( "';' expected." );
            return false;
        }
        pairs.push_back( pair[0] );
        pairs.push_back( pair[1] );
    }
    gmr->add_alias( alias, pairs );
    return true;
}

bool HicScript::do_grammar_function( Grammar* gmr )
{
    string code = get_name_or_primary( GetToken::next );
    if( code.empty() ) {
        error( "Function name missing." );
        return false;
    }
    if( gmr->get_function( code ) != nullptr ) {
        error( "The function \"" + code + "\" already exists." );
        return false;
    }
    SpFunction fun =create_function( code );
    if( fun == nullptr ) {
        return false;
    }
    return gmr->add_function( fun );
}

bool HicScript::do_grammar_use( Grammar* gmr )
{
    StdStrMap usemap;
    SToken token = next_token();
    if( current_token().type() == SToken::Type::Name ) {
        string sub = token.get_str();
        if( sub == "epoch" ) {
            usemap = {
                { "calculate", "calc_cyear" },
                { "to:text", "calc_cyear" },
                { "from:text", "calc_year" }
            };
            token = next_token();
            if( token.type() != SToken::Type::Semicolon ) {
                error( "';' expected." );
                return false;
            }
            gmr->set_use_function( usemap );
            return true;
        }
        else {
            error( "'{' expected." );
            return false;
        }
    }
    if( current_token().type() != SToken::Type::LCbracket ) {
        error( "'{' expected." );
        return false;
    }
    for( ;;) {
        // Look ahead for '}'
        next_token();
        if( current_token().type() == SToken::Type::RCbracket ||
            current_token().type() == SToken::Type::End )
        {
            break; // All done.
        }
        StdStrVec pair = get_string_list( GetToken::current );
        if( pair.size() != 2 ) {
            error( "Name or String pair expected." );
            return false;
        }
        if( current_token().type() != SToken::Type::Semicolon ) {
            error( "';' expected." );
            return false;
        }
        usemap[pair[0]] = pair[1];
    }
    gmr->set_use_function( usemap );
    return true;
}

SValue HicScript::object_to_date( Scheme* sch, SValue value )
{
    assert( sch != nullptr );
    const Base& base = sch->get_base();
    Record record( base, value );
    record.calc_jdn();
    return SValue( record.get_jdn(), SValue::Type::field );
}

FunctionData* HicScript::get_function_data( Scheme* sch, Format* fmt )
{
    if( fmt->has_use_function() ) {
        string ocode = sch->get_code();
        Object* obj = glc().get_object( ocode );
        string funcode = fmt->get_from_text_funcode();
        Function* fun = obj->get_function( funcode );
        FunctionData* fundata = new FunctionData( *fun, get_out_stream() );
        fundata->ocode = ocode;
        return fundata;
    }
    return nullptr;
}

SValue HicScript::str_to_date( Scheme* sch, string& text, const string& fcode )
{
    assert( sch != nullptr );
    Format* fmt = sch->get_input_format( fcode );
    if( fmt == nullptr ) {
        return SValue::create_error( "Unable to find \"" + fcode + "\" format." );
    }
    StringPairVec pairs = fmt->string_to_stringpair( text );
    RList rlist;
    Range range;
    const Base& base = sch->get_base();
    FunctionData* fundata = get_function_data( sch, fmt );
    for( auto& pair : pairs ) {
        range = fmt->string_to_range( base, pair.first, fundata );
        if( !pair.second.empty() ) {
            Range range2 = fmt->string_to_range( base, pair.second, fundata );
            range.m_end = range2.m_end;
        }
        if( range.is_valid() ) {
            rlist.push_back( range );
        }
    }
    delete fundata;
    SValue value;
    value.set_rlist_demote( op_set_well_order( rlist ) );
    return value;
}

SValue glich::HicScript::at_date()
{
    StdStrVec quals = get_qualifiers( GetToken::next );
    SValueVec args = get_args( GetToken::current );
    string sig, scode, fcode;
    if( !quals.empty() ) {
        sig = quals[0];
    }
    split_code( &scode, &fcode, sig );
    Scheme* sch = glc().get_scheme( scode );
    if( sch == nullptr && !scode.empty() ) {
        return SValue::create_error( "Scheme \"" + scode + "\" not found." );
    }
    if( args.empty() ) {
        return SValue::create_error( "One argument required." );
    }
    SValue value( args[0] );
    if( value.type() == SValue::Type::Object ) {
        Object* obj = value.get_object_ptr();
        if( obj == nullptr ) {
            return SValue::create_error( "Object type not recognised." );
        }
        // We ignore any suffix scheme setting
        sch = dynamic_cast<Scheme*>(obj);
        if( sch == nullptr ) {
            return SValue::create_error( "Object is not a scheme." );
        }
        return object_to_date( sch, value );
    }
    if( value.type() == SValue::Type::String ) {
        if( sch == nullptr ) {
            sch = glc().get_ischeme();
            if( sch == nullptr ) {
                return SValue::create_error( "No default scheme set." );
            }
        }
        string text = value.get_str();
        return str_to_date( sch, text, fcode );
    }
    return SValue::create_error( "Expected an object or string type." );
}

SValue glich::at_text( Script& script )
{
    StdStrVec quals = script.get_qualifiers( GetToken::next );
    SValueVec args = script.get_args( GetToken::current );
    string sig, scode, fcode;
    if( !quals.empty() ) {
        sig = quals[0];
    }
    split_code( &scode, &fcode, sig );
    Scheme* sch = glc().get_scheme( scode );
    Scheme* rec_sch = nullptr;
    if( args.empty() ) {
        return SValue::create_error( "One argument required." );
    }
    SValue value( args[0] );
    if( value.type() == SValue::Type::Object ) {
        Object* obj = value.get_object_ptr();
        if( obj == nullptr ) {
            return SValue::create_error( "Object type not recognised." );
        }
        // We ignore any suffix scheme setting
        rec_sch = dynamic_cast<Scheme*>(obj);
        if( rec_sch == nullptr ) {
            return SValue::create_error( "Object is not a scheme." );
        }
        value = rec_sch->object_to_demoted_rlist( value );
    }
    if( sch == nullptr ) {
        if( rec_sch != nullptr ) {
            sch = rec_sch;
        }
        else {
            sch = glc().get_oscheme();
        }
        if( sch == nullptr ) {
            if( !scode.empty() ) {
                return SValue::create_error( "Scheme \"" + scode + "\" not found." );
            }
            return SValue::create_error( "No default scheme set." );
        }
    }
    bool success = false;
    Field jdn = value.get_field( success );
    if( success ) {
        string text = sch->jdn_to_str( jdn, fcode );
        return SValue( text );
    }
    Range range = value.get_range( success );
    if( success ) {
        string text = sch->range_to_str( range, fcode );
        return SValue( text );
    }
    RList rlist = value.get_rlist( success );
    if( success ) {
        string text = sch->rlist_to_str( rlist, fcode );
        return SValue( text );
    }
    return SValue::create_error( "Expected field, range, rlist or record type." );
}


namespace {

    SValue complete_object( Script& script, Scheme* sch, Field jdn )
    {
        assert( sch != nullptr );
        const Base& base = sch->get_base();
        Record record( base, jdn );
        SValue value = record.get_object( sch->get_code() );
        const Grammar* gmr = sch->get_grammar();
        assert( gmr != nullptr );
        string funcode = gmr->get_calculate();
        Function* fun = sch->get_function( funcode );
        if( fun != nullptr ) {
            value = fun->run( &value, StdStrVec(), SValueVec(), script.get_out_stream() );
        }
        return value;
    }

    SValue complete_object( Script& script, Scheme* sch, const string& input, const string& fcode )
    {
        assert( sch != nullptr );
        Format* fmt = sch->get_input_format( fcode );
        if( fmt == nullptr ) {
            return SValue();
        }
        const Base& base = sch->get_base();
        Record mask( base, input, *fmt );
        string ocode = sch->get_code();
        SValue value = mask.get_object( ocode );
        if( fmt->has_use_function() ) {
            string funcode = fmt->get_from_text_funcode();
            Object* obj = glc().get_object( ocode );
            Function* fun = obj->get_function( funcode );
            if( fun != nullptr ) {
                value = fun->run( &value, StdStrVec(), SValueVec(), script.get_out_stream() );
            }
            mask.set_object( value );
        }
        return mask.get_object( ocode );
    }

} // namespace

SValue glich::at_scheme( Script& script )
{
    const char* no_default_mess = "No default scheme set.";
    StdStrVec quals = script.get_qualifiers( GetToken::next );
    SValueVec args = script.get_args( GetToken::current );
    if( args.empty() ) {
        return SValue::create_error( "One argument required." );
    }
    SValue value( args[0] );
    string sig, scode, fcode;
    if( !quals.empty() ) {
        sig = quals[0];
    }
    split_code( &scode, &fcode, sig );
    Scheme* sch = glc().get_scheme( scode );
    bool success = false;
    Field jdn = value.get_field( success );
    if( success ) {
        if( sch == nullptr ) {
            sch = glc().get_oscheme();
            if( sch == nullptr ) {
                return SValue::create_error( no_default_mess );
            }
        }
        return complete_object( script, sch, jdn );
    }
    if( value.type() == SValue::Type::String ) {
        if( sch == nullptr ) {
            sch = glc().get_ischeme();
            if( sch == nullptr ) {
                return SValue::create_error( no_default_mess );
            }
        }
        string text = value.get_str();
        return complete_object( script, sch, text, fcode );
    }
    return SValue::create_error( "Expected a field or string type." );
}

SValue glich::at_element( Script& script )
{
    StdStrVec quals = script.get_qualifiers( GetToken::next );
    SValueVec args = script.get_args( GetToken::current );
    string sig;
    if( !quals.empty() ) {
        sig = quals[0];
    }
    Element ele;
    if( !sig.empty() ) {
        ele.add_char( ':' );
        ele.add_string( sig );
    }
    SValue value( args[0] );
    bool success = false;
    Field fld = value.get_field( success );
    if( success ) {
        string element = ele.get_formatted_element( fld );
        if( element.empty() ) {
            return SValue::create_error( "Element not found." );
        }
        value.set_str( element );
    }
    else if( value.type() == SValue::Type::String ) {
        value.set_field( ele.get_converted_field( &glc(), value.get_str()));
    }
    else {
        value.set_error( "Element requires field like or string type." );
    }
    return value;
}

SValue glich::at_phrase( Script& script )
{
    const char* no_default_mess = "No default scheme set.";
    StdStrVec quals = script.get_qualifiers( GetToken::next );
    SValueVec args = script.get_args( GetToken::current );
    if( args.size() != 1 || args[0].type() != SValue::Type::String ) {
        return SValue::create_error( "@phrase requires 1 string argument." );
    }
    string sig;
    if( !quals.empty() ) {
        sig = quals[0];
    }
    RList rlist = glc().date_phrase_to_rlist( args[0].get_str(), sig );
    SValue value;
    value.set_rlist_demote( rlist );
    return value;
}

SValue glich::at_leapyear( Script& script )
{
    StdStrVec quals = script.get_qualifiers( GetToken::next );
    SValueVec args = script.get_args( GetToken::current );
    if( quals.empty() ) {
        return SValue::create_error( "@leapyear requires a qualifier." );
    }
    string mess = "@leapyear requires integer argument.";
    if( args.empty() ) {
        return SValue::create_error( mess );
    }
    bool success = false;
    Field year = args[0].get_field( success );
    if( !success ) {
        return SValue::create_error( mess );
    }

    string calendar = quals[0];
    if( calendar == "julian" ) {
        return Julian::leap_year( year );
    }
    if( calendar == "gregorian" ) {
        return Gregorian::leap_year( year );
    }
    if( calendar == "hebrew" ) {
        return Hebrew::leap_year( year );
    }
    if( calendar == "ordinal" ) {
        return IsoOrdinal::leap_year( year );
    }
    if( calendar == "isoweek" ) {
        return IsoWeek::leap_year( year );
    }
    return false;
}

SValue glich::at_easter( Script& script )
{
    StdStrVec quals = script.get_qualifiers( GetToken::next );
    SValueVec args = script.get_args( GetToken::current );
    if( quals.empty() ) {
        return SValue::create_error( "@easter requires a qualifier." );
    }
    string mess = "@easter requires integer argument.";
    if( args.empty() ) {
        return SValue::create_error( mess );
    }
    bool success = false;
    Field year = args[0].get_field( success );
    if( !success ) {
        return SValue::create_error( mess );
    }

    string calendar = quals[0];
    Field result = f_invalid;
    if( calendar == "julian" ) {
        result = Julian::easter( year );
    }
    if( calendar == "gregorian" ) {
        result = Gregorian::easter( year );
    }
    return SValue( result, SValue::Type::field );
}

SValue glich::at_last( Script& script )
{
    StdStrVec quals = script.get_qualifiers( GetToken::next );
    SValueVec args = script.get_args( GetToken::current );
    if( quals.empty() ) {
        return SValue::create_error( "@last requires a qualifier." );
    }
    string mess = "@last requires integer arguments.";
    if( args.empty() ) {
        return SValue::create_error( mess );
    }
    bool success = false;
    FieldVec fields;
    for( auto& value : args ) {
        Field field = value.get_field( success );
        if( !success ) {
            if( value.type() != SValue::Type::Null ) {
                return SValue::create_error( mess );
            }
            field = f_invalid;
        }
        fields.push_back( field );
    }

    string sig, calendar, fname;
    if( !quals.empty() ) {
        sig = quals[0];
    }
    split_code( &calendar, &fname, sig );
    
    if( calendar == "hebrew" ) {
        int index = Hebrew::fieldname_index( fname );
        if( index >= 0 ) {
            return Hebrew::end_field_value( fields, index );
        }
    }
    return f_invalid;
}

SValue glich::at_first( Script& script )
{
    StdStrVec quals = script.get_qualifiers( GetToken::next );
    SValueVec args = script.get_args( GetToken::current );
    if( quals.empty() ) {
        return SValue::create_error( "@first requires a qualifier." );
    }
    string mess = "@first requires integer arguments.";
    if( args.empty() ) {
        return SValue::create_error( mess );
    }
    bool success = false;
    FieldVec fields;
    for( auto& value : args ) {
        Field field = value.get_field( success );
        if( !success ) {
            if( value.type() != SValue::Type::Null ) {
                return SValue::create_error( mess );
            }
            field = f_invalid;
        }
        fields.push_back( field );
    }

    string sig, calendar, fname;
    if( !quals.empty() ) {
        sig = quals[0];
    }
    split_code( &calendar, &fname, sig );

    if( calendar == "hebrew" ) {
        int index = Hebrew::fieldname_index( fname );
        if( index >= 0 ) {
            return Hebrew::beg_field_value( fields, index );
        }
    }
    return f_invalid;
}


SValue glich::at_fmt_object( Script& script )
{
    StdStrVec quals = script.get_qualifiers( GetToken::next );
    SValueVec args = script.get_args( GetToken::current );
    string sig, scode, fcode;
    if( quals.empty() ) {
        return SValue::create_error( "@fmt:object requires format signiture." );
    }
    sig = quals[0];
    split_code( &scode, &fcode, sig );
    Scheme* sch = glc().get_scheme( scode );
    if( sch == nullptr ) {
        return SValue::create_error( "@fmt:object scheme not found." );
    }
    Format* fmt = sch->get_grammar()->get_format( fcode );
    if( fmt == nullptr ) {
        return SValue::create_error( "@fmt:object format not found." );
    }
    SValueVec obj;
    obj.push_back( "fmt:" );
    obj.push_back( fcode );
    obj.push_back( fmt->get_grammar().get_code() );
    obj.push_back( glich::get_fmt_rules_text( fmt->get_rules() ) );
    obj.push_back( fmt->is_visible() );
    obj.push_back( fmt->get_input_str() );
    obj.push_back( fmt->get_output_str() );
    obj.push_back( fmt->allow_shorthand() );
    return SValue( obj );
}

SValue glich::at_sch_object( Script& script )
{
    StdStrVec quals = script.get_qualifiers( GetToken::next );
    SValueVec args = script.get_args( GetToken::current );
    if( quals.empty() ) {
        return SValue::create_error( "@sch:object requires scheme code." );
    }
    string scode = quals[0];
    Scheme* sch = glc().get_scheme( scode );
    if( sch == nullptr ) {
        return SValue::create_error( "@sch:object scheme not found." );
    }
    SValueVec obj;
    obj.push_back( "sch:" );
    obj.push_back( scode );
    obj.push_back( sch->get_name() );
    obj.push_back( sch->get_base().basename() );

    SValueVec fnobj;
    fnobj.push_back( ":" );
    StdStrVec fnames = sch->get_base().get_fieldnames();
    for( const auto& fname : fnames ) {
        fnobj.push_back( fname );
    }
    obj.push_back( fnobj );

    obj.push_back( sch->get_grammar()->get_code() );

    SValueVec fobj;
    fobj.push_back( ":" );
    StdStrVec fcodes = sch->get_format_list();
    for( const auto& fcode : fcodes ) {
        fobj.push_back( fcode );
    }
    obj.push_back( fobj );

    obj.push_back( sch->get_grammar()->get_pref_input_fcode() );
    obj.push_back( sch->get_grammar()->get_pref_output_fcode() );
    obj.push_back( sch->get_cur_visible() );
    return SValue( obj );
}

SValue glich::at_sch_list( Script& script )
{
    StdStrVec quals = script.get_qualifiers( GetToken::next );
    SValueVec args = script.get_args( GetToken::current );

    SValueVec obj;
    obj.push_back( ":" );
    StdStrVec scodes = glc().get_scheme_list();
    for( const auto& scode : scodes ) {
        obj.push_back( scode );
    }
    return SValue( obj );
}

// End of src/glc/hicCreateSch.cpp file
