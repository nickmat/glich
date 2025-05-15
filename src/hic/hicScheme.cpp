/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/hic/hicScheme.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Scheme class implimentation.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     17th March 2023
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

#include "hicScheme.h"

#include "glcFunction.h"
#include "glcScript.h"
#include "glcStore.h"
#include "hicChinese.h"
#include "hicEaster.h"
#include "hicFormatText.h"
#include "hicFrench.h"
#include "hicGrammar.h"
#include "hicGregorian.h"
#include "hicHebrew.h"
#include "hicIslamic.h"
#include "hicIsoOrdinal.h"
#include "hicIsoWeek.h"
#include "hicJdn.h"
#include "hicJulian.h"
#include "hicJwn.h"
#include "hicOptional.h"
#include "hicRecord.h"

#include <cassert>
#include <cstdlib>

using namespace glich;
using std::string;


Scheme::Scheme( const string& code, const Base& base )
    : m_def_visible(false), m_cur_visible(false), m_base(base), Object(code)
{
    set_value_names( base.get_fieldnames() );
}

Scheme::~Scheme()
{
    delete &m_base;
}

bool Scheme::reset()
{
    Grammar* gmr = m_base.get_grammar();
    if( gmr == nullptr ) {
        return false;
    }
    m_input_fcode = gmr->get_pref_input_fcode();
    m_output_fcode = gmr->get_pref_output_fcode();
    for( const auto& fun : gmr->get_function_map() ) {
        add_function( fun.second );
    }
    return true;
}

void Scheme::set_output_format( const string& fcode )
{
    if( !fcode.empty() ) {
        Format* fmt = m_base.get_format( fcode );
        if( fmt != nullptr ) {
            if( fmt->has_output() ) {
                m_output_fcode = fcode;
            }
        }
    }
}

void Scheme::set_input_format( const string& fcode )
{
    if( !fcode.empty() ) {
        Format* fmt = m_base.get_format( fcode );
        if( fmt != nullptr ) {
            if( fmt->has_input() ) {
                m_input_fcode = fcode;
            }
        }
    }
}

SValue Scheme::complete_object( Field jdn ) const
{
    const Base& base = get_base();
    Record record( base, jdn );
    return record.get_object( get_code() );
}

SValue Scheme::complete_object( const string& input, const string& fcode ) const
{
    Format* fmt = get_input_format( fcode );
    if( fmt == nullptr ) {
        return SValue();
    }
    return fmt->string_to_object( get_code(), m_base, input );
}

string Scheme::get_scode() const
{
    string scode = get_code();
    assert( scode.length() > 2 );
    return scode.substr( 2 );
}

StdStrVec Scheme::get_format_list() const
{
    Grammar* gmr = m_base.get_grammar();
    StdStrVec fcodes = gmr->get_format_code_list();
    return fcodes;
}

Format* Scheme::get_output_format( const string& fcode ) const
{
    if( fcode.empty() ) {
        return m_base.get_format( m_output_fcode );
    }
    return m_base.get_format( fcode );
}

Format* Scheme::get_input_format( const string& fcode ) const
{
    if( fcode.empty() ) {
        return m_base.get_format( m_input_fcode );
    }
    return m_base.get_format( fcode );
}

void Scheme::get_info( Scheme_info* info ) const
{
    info->name = m_name;
    info->code = get_scode();
    Grammar* gmr = m_base.get_grammar();
    if( gmr ) {
        info->grammar_code = gmr->get_code();
        info->lexicon_codes = gmr->get_lexicon_codes();
        info->lexicon_names = gmr->get_lexicon_names();
    }
    else {
        info->grammar_code = "";
    }
}

void Scheme::get_format_text_info( FormatText_info* info, const string& fcode ) const
{
    FormatText* fmt = dynamic_cast<FormatText*>(m_base.get_format( fcode ));
    if( fmt ) {
        fmt->get_info( &info->info );
        info->control_str = fmt->get_control_in();
    }
}

bool Scheme::is_leap_year( Field year ) const
{
    return m_base.is_leap_year( year );;
}

FieldVec Scheme::get_object_fields( const SValueVec& values ) const
{
    FieldVec fields( m_base.record_size(), f_invalid );
    if( values.size() <= 1 ) {
        return fields;
    }
    size_t size = std::min( m_base.record_size(), values.size() - 1 );
    bool success = false;
    for( size_t i = 0; i < size; i++ ) {
        Field fld = values[i + 1].get_field( success );
        if( success ) {
            fields[i] = fld;
        }
    }
    return fields;
}

string Scheme::jdn_to_str( Field jdn, const string& fcode ) const
{
    Format* fmt = get_output_format( fcode );
    if( fmt == nullptr ) {
        return string();
    }
    return fmt->jdn_to_string( *this, jdn );
}

string Scheme::range_to_str( const Range& rng, const string& fcode ) const
{
    Format* fmt = get_output_format( fcode );
    if( fmt == nullptr ) {
        return string();
    }
    return fmt->range_to_string( *this, rng );
}

string Scheme::rlist_to_str( const RList& rlist, const string& fcode ) const
{
    Format* fmt = get_output_format( fcode );
    if( fmt == nullptr ) {
        return string();
    }
    return fmt->rlist_to_string( *this, rlist );
}

string Scheme::object_to_str( const SValue& ovalue, const string& fcode ) const
{
    const Base& base = get_base();
    Record record( base, ovalue );
    Format* fmt = get_input_format( fcode );
    if( fmt == nullptr ) {
        return string();
    }
    return fmt->get_text_output( record );
}

bool Scheme::set_epoch( Base* base, Field epoch, int line )
{
    Easter* ebase = dynamic_cast<Easter*>(base);
    if( ebase != nullptr ) {
        return ebase->set_epoch( epoch );
    }
    return create_epoch_functions( epoch, line );
}

/* static */
Base* Scheme::create_base( BaseName bs, const StdStrVec& data )
{
    switch( bs )
    {
    case BaseName::gregorian:
        return new Gregorian( data );
    case BaseName::julian:
        return new Julian( data );
    case BaseName::isoweek:
        return new IsoWeek( data );
    case BaseName::ordinal:
        return new IsoOrdinal( data );
    case BaseName::hebrew:
        return new Hebrew( data );
    case BaseName::french:
        return new French( data );
    case BaseName::islamic:
        return new Islamic( data );
    case BaseName::chinese:
        return new Chinese( data );
    case BaseName::easter:
        return new Easter( data );
    case BaseName::jdn:
        return new Jdn( data );
    case BaseName::jwn:
        return new Jwn( data );
    case BaseName::null:
        return nullptr;
    }
    assert( false ); // Should not reach here.
    return nullptr;
}

Base* Scheme::create_base_hybrid( const StdStrVec& fieldnames, const std::vector<HybridData>& data )
{
    Base* base = new Hybrid( fieldnames, data );
    if( base->is_ok() ) {
        return base;
    }
    delete base;
    return nullptr;
}

bool Scheme::create_epoch_functions( Field epoch, int line )
{
    int index = m_base.get_fieldname_index( "cyear" );
    if( index != 3 ) {
        return false;
    }
    FieldVec f = m_base.get_fields( epoch );
    string f0 = std::to_string( f[0] );
    string f1 = std::to_string( f[1] );
    string f2 = std::to_string( f[2] );
    Field std_epoch = m_base.get_jdn( { 1, 1, 1 } );
    string adj_year_true, adj_year_false, adj_cyear_true, adj_cyear_false;
    if( epoch > std_epoch ) {
        string adj = std::to_string( f[0] );
        adj_year_true = "-" + adj;
        adj_cyear_true = "+" + adj;
    }
    else if( epoch < std_epoch ) {
        string adj = std::to_string( 1 - f[0] );
        adj_year_false = "+" + adj;
        adj_cyear_false = "-" + adj;
    }
    string script =
        " year = @if( month<" + f1 + " or (month=" + f1 + " and day<" + f2 + "),"
        " cyear" + adj_cyear_true + ", cyear" + adj_cyear_false + ");"
        " result = this;";


    SpFunction fun = SpFunction( new Function( "calc_year" ) );
    if( fun == nullptr ) {
        return false;
    }
    fun->set_line( line );
    fun->set_script( script );
    add_function( fun );

    script =
        " cyear = @if( month<" + f1 + " or (month=" + f1 + " and day<" + f2 + "),"
        " year" + adj_year_true + ", year" + adj_year_false + ");"
        " result = this;";

    fun = SpFunction( new Function( "calc_cyear" ) );
    if( fun == nullptr ) {
        return false;
    }
    fun->set_line( line );
    fun->set_script( script );
    add_function( fun );

    return true;
}

// End of src/hic/hicScheme.cpp file
