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

namespace {

    Base* do_base_hybrid( Script& script, const string& hscode )
    {
        SToken token = script.next_token();
        if( token.type() != SToken::Type::LCbracket ) {
            script.error( "'{' expected." );
            return NULL;
        }
        StdStrVec fieldnames;
        HybridData data;
        data.start = f_minimum;
        std::vector<HybridData> data_vec;
        for( ;;) {
            SToken token = script.next_token();
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
                    fieldnames = script.get_string_list( GetToken::next );
                }
                else if( token.get_str() == "scheme" ) {
                    string scode = hscode + ":" + script.get_name_or_primary( GetToken::next );
                    Scheme* sch = do_create_scheme( script, scode );
                    if( sch == nullptr ) {
                        script.error( "Unable to create scheme " + scode );
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
                        script.error( "Hybrid data not complete." );
                    }
                }
                else if( token.get_str() == "change" ) {
                    data.start = script.expr( GetToken::next ).get_as_field();
                    if( data.start == f_invalid ) {
                        script.error( "Change start must be Field." );
                    }
                }
                else {
                    script.error( "Unrecognised statement." );
                }
            }
        }
        return Scheme::create_base_hybrid( fieldnames, data_vec );
    }

    Base* do_base( Script& script, const string& code )
    {
        Scheme::BaseName bs = Scheme::BaseName::null;
        string data;
        SToken token = script.next_token();
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
            else if( name == "jwn" ) {
                bs = Scheme::BaseName::jwn;
            }
            else if( name == "jdn" ) {
                bs = Scheme::BaseName::jdn;
            }
            else if( name == "hybrid" ) {
                return do_base_hybrid( script, code );
            }
            else {
                script.error( "Base scheme not recognised." );
                return nullptr;
            }
            token = script.next_token();
            if( token.type() != SToken::Type::Semicolon ) {
                data = script.get_name_or_primary( GetToken::current );
            }
        }
        else {
            script.error( "Base name expected." );
        }
        return Scheme::create_base( bs, data );
    }

} // namespace

Scheme* glich::do_create_scheme( Script& script, const std::string& code )
{
    SToken token = script.current_token();
    if( token.type() != SToken::Type::LCbracket ) {
        script.error( "'{' expected." );
        return nullptr;
    }
    Base* base = nullptr;
    string name, gmr_code;
    Field epoch = f_invalid;
    Grammar* gmr = nullptr;
    SchemeStyle style = SchemeStyle::Default;
    for( ;;) {
        token = script.next_token();
        if( token.type() == SToken::Type::RCbracket ||
            token.type() == SToken::Type::End ) {
            break;
        }
        if( token.type() == SToken::Type::Semicolon ) {
            continue;
        }
        if( token.type() == SToken::Type::Name ) {
            if( token.get_str() == "name" ) {
                name = script.expr( GetToken::next ).as_string();
            }
            else if( token.get_str() == "base" ) {
                base = do_base( script, code );
            }
            else if( token.get_str() == "epoch" ) {
                epoch = script.expr( GetToken::next ).get_as_field();
            }
            else if( token.get_str() == "grammar" ) {
                token = script.next_token();
                if( token.type() == SToken::Type::LCbracket ) {
                    gmr = do_create_grammar( script, "", base );
                    if( gmr == nullptr ) {
                        script.error( "Unable to create grammar." );
                    }
                }
                else {
                    gmr_code = script.get_name_or_primary( GetToken::current );
                    gmr = script.get_glich()->get_grammar( gmr_code );
                }
            }
            else if( token.get_str() == "style" ) {
                string str = script.get_name_or_primary( GetToken::next );
                if( str == "hidden" ) {
                    style = SchemeStyle::Hidden;
                }
                else if( str == "selected" ) {
                    style = SchemeStyle::Selected;
                }
                else if( str != "default" ) {
                    script.error( "Style name expected." );
                }
            }
            else {
                script.error( "Scheme sub-statement expected." );
            }
        }
    }
    if( base == nullptr ) {
        if( gmr_code.empty() ) {
            delete gmr;
        }
        script.error( "Scheme \"" + code + "\" not created." );
        return nullptr;
    }
    if( gmr == nullptr ) {
        gmr = Grammar::create_default_grammar( base, script.get_glich() );
    }
    if( !base->attach_grammar( gmr ) ) {
        if( gmr_code.empty() ) {
            delete gmr;
        }
        delete base;
        script.error( "Unable to attach grammar." );
        return nullptr;
    }
    if( epoch != f_invalid && !base->set_epoch( epoch ) ) {
        script.error( "Unable to set epoch." );
    }
    Scheme* sch = new Scheme( "s:" + code, *base);
    sch->reset();
    sch->set_name( name );
    sch->set_style( style );
    return sch;
}

namespace {

    bool do_lexicon_tokens( Script& script, Lexicon* voc )
    {
        SToken token = script.next_token();
        if( token.type() != SToken::Type::LCbracket ) {
            script.error( "'{' expected." );
            return false;
        }
        while( script.next_token().type() != SToken::Type::RCbracket &&
            script.current_token().type() != SToken::Type::End )
        {
            bool success = false;
            Field number = script.expr( GetToken::current ).get_field( success );
            if( !success ) {
                script.error( "Number expected." );
                return false;
            }
            if( script.current_token().type() != SToken::Type::Comma ) {
                script.error( "',' expected." );
                return false;
            }
            StdStrVec names = script.get_string_list( GetToken::next );
            string name, abbrev;
            if( names.size() > 0 ) {
                name = names[0];
            }
            if( names.size() > 1 ) {
                abbrev = names[1];
            }
            voc->add_token( number, name, abbrev );
        }
        return true;
    }

    void do_grammar_lexicons( Script& script, Grammar* gmr )
    {
        StdStrVec lexicons = script.get_string_list( GetToken::next );
        for( size_t i = 0; i < lexicons.size(); i++ ) {
            Lexicon* lex = script.get_glich()->get_lexicon( lexicons[i] );
            if( lex == nullptr ) {
                gmr->create_error( "lexicon " + lexicons[i] + " not found." );
            }
            else {
                gmr->add_lexicon( lex );
            }
        }
    }

    bool do_grammar_alias( Script& script, Grammar* gmr )
    {
        string alias = script.get_name_or_primary( GetToken::next );
        if( script.current_token().type() != SToken::Type::LCbracket ) {
            script.error( "'{' expected." );
            return false;
        }
        StdStrVec pairs;
        for( ;;) {
            // Look ahead for '}'
            script.next_token();
            if( script.current_token().type() == SToken::Type::RCbracket ||
                script.current_token().type() == SToken::Type::End )
            {
                break; // All done.
            }
            StdStrVec pair = script.get_string_list( GetToken::current );
            if( pair.size() != 2 ) {
                script.error( "Name or String pair expected." );
                return false;
            }
            if( script.current_token().type() != SToken::Type::Semicolon ) {
                script.error( "';' expected." );
                return false;
            }
            pairs.push_back( pair[0] );
            pairs.push_back( pair[1] );
        }
        gmr->add_alias( alias, pairs );
        return true;
    }

    bool do_grammar_calculate( Script& script, Grammar* gmr )
    {
        SToken token = script.next_token();
        if( token.type() != SToken::Type::LCbracket ) {
            script.error( "'{' expected." );
            return false;
        }
        string input, output, first, last;
        for( ;; ) {
            token = script.next_token();
            if( token.type() == SToken::Type::RCbracket ||
                token.type() == SToken::Type::End )
            {
                break; // All done.
            }
            if( token.type() == SToken::Type::Name ) {
                string sub = token.get_str();
                if( sub == "output" ) {
                    output = script.read_until( ";" );
                }
                else if( sub == "input" ) {
                    input = script.read_until( ";" );
                }
                else if( sub == "first" ) {
                    first = script.read_until( ";" );
                }
                else if( sub == "last" ) {
                    last = script.read_until( ";" );
                }
                else {
                    script.error( "Grammar calculate sub-statement expected." );
                }
            }
        }
        gmr->set_calculate( input, output, first, last );
        return true;
    }

    bool do_grammar_function( Script& script, Grammar* gmr )
    {
        string code = script.get_name_or_primary( GetToken::next );
        if( code.empty() ) {
            script.error( "Function name missing." );
            return false;
        }
        if( gmr->get_function( code ) != nullptr ) {
            script.error( "The function \"" + code + "\" already exists." );
            return false;
        }
        SpFunction fun = script.create_function( code );
        if( fun == nullptr ) {
            return false;
        }
        return gmr->add_function( fun );
    }

} // namespace

Lexicon* glich::do_create_lexicon( Script& script, const std::string& code )
{
    SToken token = script.current_token();
    if( token.type() != SToken::Type::LCbracket ) {
        script.error( "'{' expected." );
        return nullptr;
    }
    Lexicon* lex = new Lexicon( code );
    string str;
    for( ;;) {
        token = script.next_token();
        if( token.type() == SToken::Type::RCbracket ||
            token.type() == SToken::Type::End ) {
            break; // All done.
        }
        else if( token.type() == SToken::Type::Name ) {
            string name = token.get_str();
            if( name == "name" ) {
                str = script.expr( GetToken::next ).as_string();
                lex->set_name( str );
            }
            else if( name == "inherit" ) {
                str = script.get_name_or_primary( GetToken::next );
                lex->set_inherit( str );
            }
            else if( name == "fieldname" ) {
                str = script.get_name_or_primary( GetToken::next );
                lex->set_fieldname( str );
            }
            else if( name == "lang" ) {
                str = script.get_name_or_primary( GetToken::next );
                lex->set_lang( str );
            }
            else if( name == "pseudo" ) {
                StdStrVec pseudos = script.get_string_list( GetToken::next );
                lex->set_pseudo_names( pseudos );
            }
            else if( name == "tokens" ) {
                do_lexicon_tokens( script, lex );
            }
            else {
                script.error( "Unknown lexicon subcommand." );
            }
        }
    }
    return lex;

}

Grammar* glich::do_create_grammar( Script& script, const std::string& code, const Base* base )
{
    SToken token = script.current_token();
    if( token.type() != SToken::Type::LCbracket ) {
        script.error( "'{' expected." );
        return nullptr;
    }
    Grammar* gmr = new Grammar( code, script.get_glich() );
    StdStrVec basefields;
    if( base != nullptr ) {
        for( size_t i = 0; i < base->required_size(); i++ ) {
            basefields.push_back( base->get_fieldname( i ) );
        }
        gmr->set_base_fieldnames( basefields );
    }
    string str;
    for( ;;) {
        token = script.next_token();
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
                str = script.expr( GetToken::next ).as_string();
                gmr->set_name( str );
            }
            else if( name == "lexicons" ) {
                do_grammar_lexicons( script, gmr );
            }
            else if( name == "format" ) {
                script.do_format( gmr );
            }
            else if( name == "preferred" ) {
                str = script.get_name_or_primary( GetToken::next );
                gmr->set_preferred( str );
            }
            else if( name == "calculate" ) {
                do_grammar_calculate( script, gmr );
            }
            else if( name == "alias" ) {
                do_grammar_alias( script, gmr );
            }
            else if( name == "inherit" ) {
                StdStrVec inherit = script.get_string_list( GetToken::next );
                gmr->set_inherit( inherit );
            }
            else if( name == "fields" ) {
                basefields = script.get_string_list( GetToken::next );
                gmr->set_base_fieldnames( basefields );
            }
            else if( name == "optional" ) {
                StdStrVec optfields = script.get_string_list( GetToken::next );
                gmr->set_opt_fieldnames( optfields );
            }
            else if( name == "calculated" ) {
                StdStrVec calcfields = script.get_string_list( GetToken::next );
                gmr->set_calc_fieldnames( calcfields );
            }
            else if( name == "rank" ) {
                StdStrVec rankfields = script.get_string_list( GetToken::next );
                gmr->set_rank_fieldnames( rankfields );
            }
            else if( name == "function" ) {
                do_grammar_function( script, gmr );
            }
            else if( name == "use:jdn" ) {
                str = script.get_name_or_primary( GetToken::next );
                gmr->set_use_jdn( str );
            }
        }
        else {
            script.error( "Grammar sub-statement expected." );
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
        script.error( gmr_error );
    }
    return gmr;
}

namespace {

    FormatText* create_format_text( Glich* glc, const string& code, Grammar* gmr )
    {
        size_t pos = code.find( ':' );
        if( pos == string::npos ) {
            if( gmr == nullptr ) {
                return nullptr;
            }
            return gmr->create_format_text( code );
        }
        if( gmr != nullptr ) {
            return nullptr;
        }
        string gcode = code.substr( 0, pos );
        string fcode = code.substr( pos + 1 );
        gmr = glc->get_grammar( gcode );
        if( gmr == nullptr || gmr->get_format( fcode ) != nullptr ) {
            return nullptr;
        }
        FormatText* fmt = new FormatText( fcode, *gmr );
        if( !gmr->add_format( fmt ) ) {
            delete fmt;
            return nullptr;
        }
        glc->add_format( fmt, code );
        return fmt;
    }

    FormatIso* create_format_iso( Glich* glc, const string& code, Grammar* gmr, const StdStrVec& rules )
    {
        size_t pos = code.find( ':' );
        if( pos == string::npos ) {
            if( gmr == nullptr ) {
                return nullptr;
            }
            return gmr->create_format_iso( code, rules );
        }
        if( gmr != nullptr ) {
            return nullptr;
        }
        string gcode = code.substr( 0, pos );
        string fcode = code.substr( pos + 1 );
        gmr = glc->get_grammar( gcode );
        if( gmr == nullptr || gmr->get_format( fcode ) != nullptr ) {
            return nullptr;
        }
        FormatIso* fmt = new FormatIso( fcode, *gmr, rules );
        if( !gmr->add_format( fmt ) ) {
            delete fmt;
            return nullptr;
        }
        glc->add_format( fmt, code );
        return fmt;
    }

    FormatUnit* create_format_unit( Glich* glc, const std::string& code, Grammar* gmr )
    {
        size_t pos = code.find( ':' );
        if( pos == string::npos ) {
            if( gmr == nullptr ) {
                return nullptr;
            }
            return gmr->create_format_unit( code );
        }
        if( gmr != nullptr ) {
            return nullptr;
        }
        string gcode = code.substr( 0, pos );
        string fcode = code.substr( pos + 1 );
        gmr = glc->get_grammar( gcode );
        if( gmr == nullptr || gmr->get_format( fcode ) != nullptr ) {
            return nullptr;
        }
        FormatUnit* fmt = new FormatUnit( fcode, *gmr );
        if( !gmr->add_format( fmt ) ) {
            delete fmt;
            return nullptr;
        }
        glc->add_format( fmt, code );
        return fmt;
    }
}

// If gmr == nullptr then this is a standalone format.
bool glich::do_create_format( Script& script, const string& code, Grammar* gmr )
{
    string format_in, format_out, instring, outstring, separators, infun, outfun;
    StdStrVec rankfields, rankoutfields, rules;
    FormatStyle style = FormatStyle::Default;

    if( script.current_token().type() == SToken::Type::LCbracket ) {
        for( ;;) {
            SToken token = script.next_token();
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
                    format_out = script.expr( GetToken::next ).as_string();
                    continue;
                }
                if( name == "inout" ) {
                    format_out = script.expr( GetToken::next ).as_string();
                    format_in = format_out;
                    continue;
                }
                if( name == "input" ) {
                    format_in = script.expr( GetToken::next ).as_string();
                    continue;
                }
                if( name == "separators" ) {
                    separators = script.expr( GetToken::next ).as_string();
                    continue;
                }
                if( name == "rank" ) {
                    rankfields = script.get_string_list( GetToken::next );
                    continue;
                }
                if( name == "rankout" ) {
                    rankoutfields = script.get_string_list( GetToken::next );
                    continue;
                }
                if( name == "style" ) {
                    string str = script.get_name_or_primary( GetToken::next );
                    if( str == "hidden" ) {
                        style = FormatStyle::Hidden;
                    }
                    else if( str != "default" ) {
                        script.error( "Style name expected." );
                    }
                    continue;
                }
                if( name == "pseudo:in" ) {
                    instring = script.expr( GetToken::next ).as_string();
                    continue;
                }
                if( name == "pseudo:out" ) {
                    outstring = script.expr( GetToken::next ).as_string();
                    continue;
                }
                if( name == "rules" ) {
                    rules = script.get_string_list( GetToken::next );
                    continue;
                }
                if( name == "use:in" ) {
                    infun = script.get_name_or_primary( GetToken::next );
                    continue;
                }
                if( name == "use:out" ) {
                    outfun = script.get_name_or_primary( GetToken::next );
                    continue;
                }
                script.error( "Expected format sub-statement." );
            }
        }
    }
    else {
        if( script.current_token().type() == SToken::Type::Comma ) {
            script.next_token();
        }
        format_out = script.expr( GetToken::current ).as_string();
        if( format_out.empty() ) {
            script.error( "Format missing." );
            return false;
        }
        format_in = format_out;
        if( script.current_token().type() != SToken::Type::Semicolon ) {
            script.error( "';' expected." );
            return false;
        }
    }

    Format* fmt = nullptr;
    if( rules.empty() || rules[0] == "text" ) {
        if( format_in.empty() && format_out.empty() ) {
            script.error( "Format string not found." );
            return false;
        }
        FormatText* fmtt = create_format_text( script.get_glich(), code, gmr );
        if( fmtt == nullptr ) {
            script.error( "Unable to create format." );
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
            fmtt->set_input_function( infun );
        }
        if( !outfun.empty() ) {
            fmtt->set_output_function( outfun );
        }
        fmt = fmtt;
    }
    else if( rules[0] == "iso8601" ) {
        fmt = create_format_iso( script.get_glich(), code, gmr, rules );
        if( fmt == nullptr ) {
            script.error( "Unable to create ISO format." );
            return false;
        }
    }
    else if( rules[0] == "units" ) {
        fmt = create_format_unit( script.get_glich(), code, gmr );
        if( fmt == nullptr ) {
            script.error( "Unable to create Units format." );
            return false;
        }
    }
    else {
        script.error( "Unknown rules statement." );
        return false;
    }
    fmt->set_style( style );
    if( gmr == nullptr ) {
        fmt->construct();
    }
    return true;
}

SValue glich::hics_at( Script& script, bool& success, Object* obj, const std::string& fcode, const SValue& left )
{
    Scheme* sch = dynamic_cast<Scheme*>(obj);
    if( sch == nullptr ) {
        return SValue();
    }
    SValueVec args = script.get_args( GetToken::current );
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

namespace {

    string jdn_to_str( Script& script, Scheme* sch, Field jdn, const string& fcode )
    {
        assert( sch != nullptr );
        const Base& base = sch->get_base();
        Record record( base, jdn );
        SValue value = record.get_object( sch->get_code() );
        const Grammar* gmr = sch->get_grammar();
        assert( gmr != nullptr );
        Function* fun = gmr->get_function( gmr->get_use_jdn() );
        if( fun != nullptr ) {
            value = fun->run( &value, StdStrVec(), SValueVec(), script.get_out_stream() );
        }
        record.set_object( value );
        Format* fmt = sch->get_input_format( fcode );
        if( fmt == nullptr ) {
            return string();
        }
        return fmt->get_text_output( record );
    }

} // namespace

SValue glich::at_text( Script& script )
{
    StdStrVec quals = script.get_qualifiers( GetToken::next );
    SValueVec args = script.get_args( GetToken::current );
    string sig, scode, fcode;
    if( !quals.empty() ) {
        sig = quals[0];
    }
    split_code( &scode, &fcode, sig );
    Glich* glc = script.get_glich();
    Scheme* sch = glc->get_scheme( scode );
    Scheme* rec_sch = nullptr;
    if( args.empty() ) {
        return SValue( "One argument required.", SValue::Type::Error );
    }
    SValue value( args[0] );
    if( value.type() == SValue::Type::Object ) {
        Object* obj = value.get_object_ptr();
        if( obj == nullptr ) {
            return SValue( "Object type not recognised.", SValue::Type::Error );
        }
        // We ignore any suffix scheme setting
        rec_sch = dynamic_cast<Scheme*>(obj);
        if( rec_sch == nullptr ) {
            return SValue( "Object is not a scheme.", SValue::Type::Error );
        }
        value = rec_sch->object_to_demoted_rlist( value );
    }
    if( sch == nullptr ) {
        if( rec_sch != nullptr ) {
            sch = rec_sch;
        }
        else {
            sch = glc->get_oscheme();
        }
        if( sch == nullptr ) {
            return SValue( "No default scheme set.", SValue::Type::Error );
        }
    }
    bool success = false;
    Field jdn = value.get_field( success );
    if( success ) {
        string text = jdn_to_str( script, sch, jdn, fcode );
        return SValue( text );
    }
    Range rng = value.get_range( success );
    if( success ) {
        value.set_str( sch->range_to_str( rng, fcode ) );
        return value;
    }
    RList rlist = value.get_rlist( success );
    if( !success ) {
        return SValue( "Expected field, range, rlist or record type.", SValue::Type::Error );
    }
    value.set_str( sch->rlist_to_str( rlist, fcode ) );
    return value;
}

namespace {


    SValue object_to_date( Script& script, Scheme* sch, SValue value )
    {
        assert( sch != nullptr );
        const Base& base = sch->get_base();
        Record record( base, value );
        record.calc_jdn();
        return SValue( record.get_jdn(), SValue::Type::field );
    }

    SValue str_to_date( Script& script, Scheme* sch, const string& text, const string& fcode )
    {
        assert( sch != nullptr );
        const Base& base = sch->get_base();
        Format* fmt = sch->get_input_format( fcode );
        if( fmt == nullptr ) {
            return SValue();
        }
        RList rlist;
        if( fmt->has_use_function() ) {
            string ocode = sch->get_code();
            Object* obj = glc().get_object( ocode );
            string funcode = fmt->get_input_funcode();
            Function* fun = obj->get_function( funcode );
            FunctionData fundata( *fun, script.get_out_stream() );
            fundata.ocode = ocode;
            rlist = fmt->string_to_rlist( base, text, &fundata );
        }
        else {
            rlist = fmt->string_to_rlist( base, text );
        }
        return SValue( rlist );
    }

} // namespace

SValue glich::at_date( Script& script )
{
    StdStrVec quals = script.get_qualifiers( GetToken::next );
    SValueVec args = script.get_args( GetToken::current );
    string sig, scode, fcode;
    if( !quals.empty() ) {
        sig = quals[0];
    }
    split_code( &scode, &fcode, sig );
    Glich* glc = script.get_glich();
    Scheme* sch = glc->get_scheme( scode );
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
        return object_to_date( script, sch, value );
    }
    if( value.type() == SValue::Type::String ) {
        if( sch == nullptr ) {
            sch = glc->get_ischeme();
            if( sch == nullptr ) {
                return SValue::create_error( "No default scheme set." );
            }
        }
        string text = value.get_str();
        return str_to_date( script, sch, text, fcode );
    }
    return SValue::create_error( "Expected an object or string type." );
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
        Function* fun = gmr->get_function( gmr->get_use_jdn() );
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
            string funcode = fmt->get_input_funcode();
            Object* obj = glc().get_object( ocode );
            Function* fun = obj->get_function( funcode );
            if( fun != nullptr ) {
                value = fun->run( &value, StdStrVec(), SValueVec(), script.get_out_stream() );
            }
        }
        return value;
    }

} // namespace

SValue glich::at_record( Script& script )
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
    Glich* glc = script.get_glich();
    Scheme* sch = glc->get_scheme( scode );
    bool success = false;
    Field jdn = value.get_field( success );
    if( success ) {
        if( sch == nullptr ) {
            sch = glc->get_oscheme();
            if( sch == nullptr ) {
                return SValue::create_error( no_default_mess );
            }
        }
        return complete_object( script, sch, jdn );
    }
    if( value.type() == SValue::Type::String ) {
        if( sch == nullptr ) {
            sch = glc->get_ischeme();
            if( sch == nullptr ) {
                return SValue::create_error( no_default_mess );
            }
        }
        return complete_object( script, sch, value.get_str(), fcode );
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
    Glich* glc = script.get_glich();
    SValue value( args[0] );
    bool success = false;
    Field fld = value.get_field( success );
    if( success ) {
        value.set_str( ele.get_formatted_element( *glc, fld ) );
    }
    else if( value.type() == SValue::Type::String ) {
        value.set_field( ele.get_converted_field( glc, value.get_str() ) );
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
    Glich* glc = script.get_glich();
    RList rlist = glc->date_phrase_to_rlist( args[0].get_str(), sig );
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

// End of src/glc/hicCreateSch.cpp file
