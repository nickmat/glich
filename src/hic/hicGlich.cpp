/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/hic/hicGlich.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     HicGlich hic extension language class.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     22nd October 2024
 * Copyright:   Copyright (c) 2024, Nick Matthews.
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

#include <glc/hicGlich.h>

#include "glcValue.h"
#include "hicDatePhrase.h"
#include "hicGrammar.h"
#include "hicLexicon.h"
#include "hicScheme.h"
#include "hicScript.h"
#include "hicMark.h"

#include <cassert>
#include <iostream>
#include <sstream>

using namespace glich;
using std::string;
using std::vector;


HicGlich::~HicGlich()
{
    for( auto pair : m_lexicons ) {
        delete pair.second;
    }
    m_lexicons.clear();
}

void HicGlich::init()
{
    m_marks.push_back( new HicMark( "", nullptr ) );
}

void HicGlich::load_hics_library()
{
    run_module( "hics:hic_lib" );
}

SValue HicGlich::evaluate( const string& expression )
{
    std::istringstream iss( expression );
    std::ostringstream oss;
    HicScript scr( iss, oss );
    return scr.evaluate();
}

bool HicGlich::run( std::istream& in, std::ostream& out, int line )
{
    HicScript scr( in, out );
    scr.set_line( line );
    return scr.run();
}

bool HicGlich::add_module_def( const ModuleDef& def, const std::string& code )
{
    if( def.m_definition == "lexicon" ) {
        for( auto& lex : def.m_codes ) {
            if( !add_lexicon( nullptr, lex ) ) {
                return false;
            }
            m_lexicon_mods[lex] = code;
        }
        return true;
    }
    if( def.m_definition == "grammar" ) {
        for( auto& gmr : def.m_codes ) {
            if( !add_grammar( nullptr, gmr ) ) {
                return false;
            }
            m_grammar_mods[gmr] = code;
        }
        return true;
    }
    if( def.m_definition == "scheme" ) {
        for( auto& sch : def.m_codes ) {
            if( !add_scheme( nullptr, sch ) ) {
                return false;
            }
            string scode = "s:" + sch;
            m_object_mods[scode] = code;
        }
        return true;
    }
    return Glich::add_module_def( def, code );
}

SchemeList HicGlich::get_scheme_list( Visibility vis )
{
    SchemeList slist;
    SchemeData sdata;

    for( const auto& object : m_objects ) {
        string prefix, code;
        split_string( prefix, code, object.first );
        if( prefix != "s" ) {
            continue;
        }
        Scheme* sch = get_scheme( code );
        if( sch == nullptr ) {
            continue;
        }
        if( vis == Visibility::visible && !sch->get_cur_visible() ) {
            continue;
        }
        if( vis == Visibility::hidden && sch->get_cur_visible() ) {
            continue;
        }
        sdata.code = code;
        sdata.name = sch->get_name();
        slist.push_back( sdata );
    }
    return slist;
}

void HicGlich::get_scheme_info( Scheme_info* info, const string& scode )
{
    Scheme* sch = get_scheme( scode );
    if( sch != nullptr ) {
        sch->get_info( info );
    }
}

void HicGlich::get_input_info( SchemeFormatInfo* info, const string& scode )
{
    Scheme* sch = get_scheme( scode );
    if( sch != nullptr ) {
        string incode = sch->get_input_format_code();
        const Grammar* gmr = sch->get_grammar();
        gmr->get_input_formats( info, incode );
    }
}

void HicGlich::get_output_info( SchemeFormatInfo* info, const string& scode )
{
    Scheme* sch = get_scheme( scode );
    if( sch != nullptr ) {
        string outcode = sch->get_output_format_code();
        const Grammar* gmr = sch->get_grammar();
        gmr->get_output_formats( info, outcode );
    }
}

RList HicGlich::date_phrase_to_rlist( const string& phrase, const string& sig )
{
    string scode, fcode;
    Scheme* sch = nullptr;
    Scheme* prev_sch = nullptr;
    string prev_fcode;
    if( !sig.empty() ) {
        split_code( &scode, &fcode, sig );
        prev_sch = get_ischeme();
        if( !scode.empty() ) {
            sch = get_scheme( scode );
            set_ischeme( sch );
        }
        if( sch == nullptr ) {
            sch = get_ischeme();
            if( sch == nullptr ) {
                return RList();
            }
        }
        if( !fcode.empty() ) {
            prev_fcode = sch->get_input_format_code();
            if( prev_fcode == fcode ) { // No change.
                prev_fcode.clear();
            }
            else {
                sch->set_input_format( fcode );
            }
        }
    }

    string script = parse_date_phrase( phrase );
    SValue value = evaluate( script );
    bool success = false;
    RList rlist = value.get_rlist( success );

    if( !prev_fcode.empty() ) {
        sch->set_input_format( prev_fcode );
    }
    if( prev_sch != nullptr ) {
        set_ischeme( prev_sch );
    }
    return rlist;
}

string HicGlich::date_phrase_to_text( const string& phrase, const string& sig_in, const string& sig_out )
{
    RList rlist = date_phrase_to_rlist( phrase, sig_in );
    return rlist_to_text( rlist, sig_out );
}

string HicGlich::rlist_to_text( RList rlist, const string& sig )
{
    string scode, fcode;
    Scheme* sch = nullptr;
    if( !sig.empty() ) {
        split_code( &scode, &fcode, sig );
        if( !scode.empty() ) {
            sch = get_scheme( scode );
        }
    }
    if( sch == nullptr ) {
        sch = get_oscheme();
        if( sch == nullptr ) {
            return string();
        }
    }
    return sch->rlist_to_str( rlist, fcode );
}

string HicGlich::range_to_text( Range range, const string& sig )
{
    string scode, fcode;
    Scheme* sch = nullptr;
    if( !sig.empty() ) {
        split_code( &scode, &fcode, sig );
        if( !scode.empty() ) {
            sch = get_scheme( scode );
        }
    }
    if( sch == nullptr ) {
        sch = get_oscheme();
        if( sch == nullptr ) {
            return string();
        }
    }
    return sch->range_to_str( range, fcode );
}

string HicGlich::field_to_text( Field field, const string& sig )
{
    string qual;
    if( !sig.empty() ) {
        qual = ".\"" + sig + "\"";
    }
    string function = "@text" + qual + "(" + field_to_string( field ) + ")";
    SValue value = evaluate( function );
    bool success;
    return value.get_str( success );
}

RList HicGlich::text_to_rlist( const string& text, const string& sig )
{
    string qual;
    if( !sig.empty() ) {
        qual = ".\"" + sig + "\"";
    }
    string function = "@date" + qual + "(\"" + text + "\")";
    SValue value = evaluate( function );
    bool success;
    return value.get_rlist( success );
}

Range HicGlich::text_to_range( const string& text, const string& sig )
{
    string qual;
    if( !sig.empty() ) {
        qual = ".\"" + sig + "\"";
    }
    string function = "@date" + qual + "(\"" + text + "\")";
    SValue value = evaluate( function );
    bool success;
    return value.get_range( success );
}

Field HicGlich::text_to_field( const string& text, const string& sig )
{
    string qual;
    if( !sig.empty() ) {
        qual = ".\"" + sig + "\"";
    }
    string function = "@date" + qual + "(\"" + text + "\")";
    SValue value = evaluate( function );
    bool success;
    return value.get_field( success );
}

HicMarkDataVec HicGlich::get_hic_data() const
{
    HicMarkDataVec hicdata;
    for( auto& mark : m_marks ) {
        HicMark* hicmark = dynamic_cast<HicMark*>(mark);
        if( hicmark ) {
            HicMarkData data;
            hicmark->get_mark_hic_data( data );
            hicdata.push_back( data );
        }
    }
    return hicdata;
}

void HicGlich::get_format_text_info( FormatText_info* info, const string& scode, const string& fcode )
{
    Scheme* sch = get_scheme( scode );
    if( sch != nullptr ) {
        sch->get_format_text_info( info, fcode );
    }
}

bool HicGlich::get_lexicon_info( Lexicon_info* info, const string& code )
{
    Lexicon* lex = get_lexicon( code );
    if( lex == nullptr ) {
        return false;
    }
    lex->get_info( info );
    return true;
}

bool HicGlich::add_lexicon( Lexicon* lex, const string& code )
{
    DefinedStatus status = get_lexicon_status( code );
    if( status == DefinedStatus::defined ) {
        delete lex;
        return false;
    }
    if( status == DefinedStatus::none ) {
        HicMark* mark = dynamic_cast<HicMark*>(m_marks[m_marks.size() - 1]);
        assert( mark != nullptr );
        mark->add_lexicon( code );
    }
    m_lexicons[code] = lex;
    return true;
}

void HicGlich::remove_lexicon( const string& code )
{
    if( m_lexicons.count( code ) == 0 ) {
        return;
    }
    delete m_lexicons.find( code )->second;
    m_lexicons.erase( code );
}

Lexicon* HicGlich::get_lexicon( const string& code )
{
    if( m_lexicons.count( code ) > 0 ) {
        Lexicon* lex = m_lexicons.find( code )->second;
        if( lex == nullptr && m_lexicon_mods.count( code ) == 1 ) {
            string mod = m_lexicon_mods.find( code )->second;
            string mess = run_module( mod );
            lex = m_lexicons.find( code )->second;
        }
        return lex;
    }
    return nullptr;
}

DefinedStatus HicGlich::get_lexicon_status( const string& code ) const
{
    if( m_lexicons.count( code ) == 0 ) {
        return DefinedStatus::none;
    }
    Lexicon* lex = m_lexicons.find( code )->second;
    return lex ? DefinedStatus::defined : DefinedStatus::module;
}

bool HicGlich::add_grammar( Grammar* gmr, const string& code )
{
    DefinedStatus status = get_grammar_status( code );
    if( status == DefinedStatus::defined ) {
        delete gmr;
        return false;
    }
    if( status == DefinedStatus::none ) {
        HicMark* mark = dynamic_cast<HicMark*>(m_marks[m_marks.size() - 1]);
        assert( mark != nullptr );
        mark->add_grammar( code );
    }
    m_grammars[code] = gmr;
    return true;
}

void HicGlich::remove_grammar( const string& code )
{
    if( m_grammars.count( code ) == 0 ) {
        return;
    }
    delete m_grammars.find( code )->second;
    m_grammars.erase( code );
}

Grammar* HicGlich::get_grammar( const string& code )
{
    if( m_grammars.count( code ) > 0 ) {
        Grammar* gmr = m_grammars.find( code )->second;
        if( gmr == nullptr && m_grammar_mods.count( code ) == 1 ) {
            string mod = m_grammar_mods.find( code )->second;
            string mess = run_module( mod );
            gmr = m_grammars.find( code )->second;
        }
        return gmr;
    }
    return nullptr;
}

DefinedStatus HicGlich::get_grammar_status( const string& code ) const
{
    if( m_grammars.count( code ) == 0 ) {
        return DefinedStatus::none;
    }
    Grammar* gmr = m_grammars.find( code )->second;
    return gmr ? DefinedStatus::defined : DefinedStatus::module;
}

bool HicGlich::add_format( const string& code )
{
    assert( m_marks.size() > 0 );
    HicMark* mark = dynamic_cast<HicMark*>(m_marks[m_marks.size() - 1]);
    assert( mark != nullptr );
    mark->add_format( code );
    return true;
}

bool HicGlich::add_scheme( Scheme* sch, const string& scode )
{
    string code = "s:" + scode;
    DefinedStatus status = get_object_status( code );
    if( status == DefinedStatus::defined ) {
        delete sch;
        return false;
    }
    if( status == DefinedStatus::none ) {
        HicMark* mark = dynamic_cast<HicMark*>(m_marks[m_marks.size() - 1]);
        assert( mark != nullptr );
        mark->add_scheme( scode );
    }
    m_objects[code] = sch;
    return true;
}

Scheme* HicGlich::get_scheme( const string& scode )
{
    string ocode = "s:" + scode;
    DefinedStatus status = get_object_status( ocode );
    if( status == DefinedStatus::module ) {
        run_module( m_object_mods[ocode] );
    }
    return dynamic_cast<Scheme*>(get_object( ocode ));
}

DefinedStatus HicGlich::get_scheme_status( const string& code ) const
{
    return get_object_status( "s:" + code );
}

StdStrVec HicGlich::get_scheme_list() const
{
    StdStrVec list;
    for( auto& pair : m_objects ) {
        string prefix, code;
        split_string( prefix, code, pair.first );
        if( prefix == "s" ) {
            list.push_back( code );
        }
    }
    return list;
}

Mark* HicGlich::create_mark( const string& name, Mark* prev )
{
    return new HicMark( name, dynamic_cast<HicMark*>(prev) );
}

bool HicGlich::set_property( const string& property, const string& value )
{
    int i = int( m_marks.size() ) - 1;
    if( i < 0 ) {
        return false;
    }
    HicMark* mark = dynamic_cast<HicMark*>(m_marks[i]);
    if( property == "context" ) {
        Context ct;
        if( value == "glich" ) {
            ct = Context::glich;
        }
        else if( value == "hics" ) {
            ct = Context::hics;
        }
        else {
            return false;
        }
        mark->set_context( ct );
        return true;
    }
    string scode, fcode;
    split_code( &scode, &fcode, value );
    Scheme* sch = get_scheme( scode );
    if( sch != nullptr ) {
        if( property == "input" ) {
            mark->set_ischeme( sch );
            sch->set_input_format( fcode );
            return true;
        }
        if( property == "output" ) {
            mark->set_oscheme( sch );
            sch->set_output_format( fcode );
            return true;
        }
        if( property == "inout" ) {
            mark->set_ischeme( sch );
            sch->set_input_format( fcode );
            mark->set_oscheme( sch );
            sch->set_output_format( fcode );
            return true;
        }
    }
    return Glich::set_property( property, value );
}

void HicGlich::set_ischeme( Scheme* sch )
{
    int i = int( m_marks.size() ) - 1;
    if( i >= 0 ) {
        dynamic_cast<HicMark*>(m_marks[i])->set_ischeme( sch );
    }
}

void HicGlich::set_oscheme( Scheme* sch )
{
    int i = int( m_marks.size() ) - 1;
    if( i >= 0 ) {
        dynamic_cast<HicMark*>(m_marks[i])->set_oscheme( sch );
    }
}

Context HicGlich::get_context() const
{
    int i = int( m_marks.size() ) - 1;
    if( i >= 0 ) {
        return dynamic_cast<HicMark*>(m_marks[i])->get_context();
    }
    return Context::glich;
}

Scheme* HicGlich::get_ischeme() const
{
    int i = int( m_marks.size() ) - 1;
    if( i >= 0 ) {
        return dynamic_cast<HicMark*>(m_marks[i])->get_ischeme();
    }
    return nullptr;
}

Scheme* HicGlich::get_oscheme() const
{
    int i = int( m_marks.size() ) - 1;
    if( i >= 0 ) {
        return dynamic_cast<HicMark*>(m_marks[i])->get_oscheme();
    }
    return nullptr;
}

string HicGlich::get_special_value_string( SpecialValue val )
{
    Context ctx = hic().get_context();
    switch( val )
    {
    case SpecialValue::plus_inf:
        return (ctx == Context::glich) ? "+infinity" : "future";
    case SpecialValue::minus_inf:
        return (ctx == Context::glich) ? "-infinity" : "past";
    case SpecialValue::unknown:
        return "?";
    }
    return std::string();
}

// End of src/hic/hicGlich.cpp
