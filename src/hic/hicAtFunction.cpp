/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/hic/hicAtFunction.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Impliment built-in hics functions.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     11th May 2025
 * Copyright:   Copyright (c) 2025, Nick Matthews.
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

#include "hicAtFunction.h" 

#include <glc/hic.h>

#include "hicCalendars.h"
#include "hicElement.h"
#include "hicFormat.h"
#include "hicHelper.h"
#include "hicRecord.h"
#include "hicScheme.h"
#include "hicScript.h"

#include <cassert>

using namespace glich;
using std::string;

static SValue hic_object_to_date( Scheme* sch, SValue value )
{
    assert( sch != nullptr );
    const Base& base = sch->get_base();
    Record record( base, value );
    Field jdn = record.calc_jdn();
    if( jdn != f_invalid ) {
        return SValue( jdn, SValue::Type::field );
    }
    Record rec_end( record );
    record.complete_fields_as_beg();
    rec_end.complete_fields_as_end();
    Range range(record.get_jdn(), rec_end.get_jdn());
    if( range.is_valid() ) {
        return SValue( range );
    }
    return SValue::create_error( "Cannot convert object." );
}


SValue glich::hic_at_date( HicScript& script, const StdStrVec& quals, const SValueVec& args )
{
    string sig, scode, fcode;
    if( !quals.empty() ) {
        sig = quals[0];
    }
    split_code( &scode, &fcode, sig );
    Scheme* sch = hic().get_scheme( scode );
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
        return hic_object_to_date( sch, value );
    }
    if( value.type() == SValue::Type::String ) {
        if( sch == nullptr ) {
            sch = hic().get_ischeme();
            if( sch == nullptr ) {
                return SValue::create_error( "No default scheme set." );
            }
        }
        string text = value.get_str();
        return script.str_to_date( sch, text, fcode );
    }
    return SValue::create_error( "Expected an object or string type." );
}

SValue glich::hic_at_text( const StdStrVec& quals, const SValueVec& args )
{
    string sig, scode, fcode;
    if( !quals.empty() ) {
        sig = quals[0];
    }
    split_code( &scode, &fcode, sig );
    Scheme* sch = hic().get_scheme( scode );
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
        value = hic_object_to_date( rec_sch, value );
    }
    if( sch == nullptr ) {
        if( rec_sch != nullptr ) {
            sch = rec_sch;
        }
        else {
            sch = hic().get_oscheme();
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


SValue glich::hic_at_scheme( HicScript& script, const StdStrVec& quals, const SValueVec& args )
{
    const char* no_default_mess = "No default scheme set.";
    if( args.empty() ) {
        return SValue::create_error( "One argument required." );
    }
    SValue value( args[0] );
    string sig, scode, fcode;
    if( !quals.empty() ) {
        sig = quals[0];
    }
    split_code( &scode, &fcode, sig );
    Scheme* sch = hic().get_scheme( scode );
    bool success = false;
    Field jdn = value.get_field( success );
    if( success ) {
        if( sch == nullptr ) {
            sch = hic().get_oscheme();
            if( sch == nullptr ) {
                return SValue::create_error( no_default_mess );
            }
        }
        return script.complete_object( sch, jdn );
    }
    Range range = value.get_range( success );
    if( success ) {
        if( sch == nullptr ) {
            sch = hic().get_oscheme();
            if( sch == nullptr ) {
                return SValue::create_error( no_default_mess );
            }
        }
        return script.complete_object( sch, range, fcode );
    }
    if( value.type() == SValue::Type::String ) {
        if( sch == nullptr ) {
            sch = hic().get_ischeme();
            if( sch == nullptr ) {
                return SValue::create_error( no_default_mess );
            }
        }
        string text = value.get_str();
        return script.complete_object( sch, text, fcode );
    }
    return SValue::create_error( "Expected a field or string type." );
}

SValue glich::hic_at_element( const StdStrVec& quals, const SValueVec& args )
{
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
        value.set_field( ele.get_converted_field( &glc(), value.get_str() ) );
    }
    else {
        value.set_error( "Element requires field like or string type." );
    }
    return value;
}

SValue glich::hic_at_phrase( const StdStrVec& quals, const SValueVec& args )
{
    const char* no_default_mess = "No default scheme set.";
    if( args.size() != 1 || args[0].type() != SValue::Type::String ) {
        return SValue::create_error( "@phrase requires 1 string argument." );
    }
    string sig;
    if( !quals.empty() ) {
        sig = quals[0];
    }
    RList rlist = hic().date_phrase_to_rlist( args[0].get_str(), sig );
    SValue value;
    value.set_rlist_demote( rlist );
    return value;
}

SValue glich::hic_at_leapyear( const StdStrVec& quals, const SValueVec& args )
{
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

SValue glich::hic_at_easter( const StdStrVec& quals, const SValueVec& args )
{
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

SValue glich::hic_at_sch_list()
{
    SValueVec obj;
    obj.push_back( ":" );
    StdStrVec scodes = hic().get_scheme_list();
    for( const auto& scode : scodes ) {
        obj.push_back( scode );
    }
    return SValue( obj );
}

SValue glich::hic_at_sch_object( const StdStrVec& quals )
{
    if( quals.empty() ) {
        return SValue::create_error( "@sch:object requires scheme code." );
    }
    string scode = quals[0];
    Scheme* sch = hic().get_scheme( scode );
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

SValue glich::hic_at_fmt_object( const StdStrVec& quals )
{
    string sig, scode, fcode;
    if( quals.empty() ) {
        return SValue::create_error( "@fmt:object requires format signiture." );
    }
    sig = quals[0];
    split_code( &scode, &fcode, sig );
    Scheme* sch = hic().get_scheme( scode );
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

SValue glich::hic_at_age( const StdStrVec& quals, const SValueVec& args )
{
    if( args.size() < 2 ) {
        return SValue::create_error( "@age requires from and to date." );
    }
    Field birth = args[0].get_as_field();
    if( birth == f_invalid ) {
        return SValue::create_error( "@age birth date not valid." );
    }
    Field date = args[1].get_as_field();
    if( date == f_invalid ) {
        return SValue::create_error( "@age date not valid." );
    }
    int direction = 1;
    if( date < birth ) {
        Field temp = birth;
        birth = date;
        date = temp;
        direction = -1;
    }
    string scode;
    if( !quals.empty() ) {
        scode = quals[0];
    }
    Scheme* sch = hic().get_scheme( scode );
    if( sch == nullptr ) {
        sch = hic().get_oscheme();
        if( sch == nullptr ) {
            return SValue::create_error( "No default scheme found." );
        }
    }
    const Base& base = sch->get_base();
    Record r_birth( base, birth );
    Record r_date( base, date );
    int y_index = base.get_fieldname_index( "year" );
    if( y_index < 0 ) {
        return SValue::create_error( "@age scheme does not have a year field." );
    }
    int m_index = base.get_fieldname_index( "month" );
    if( m_index < 0 ) {
        return SValue::create_error( "@age scheme does not have a month field." );
    }
    int d_index = base.get_fieldname_index( "day" );
    if( d_index < 0 ) {
        return SValue::create_error( "@age scheme does not have a day field." );
    }
    Field y1 = r_birth.get_field( y_index );
    Field y2 = r_date.get_field( y_index );
    Field m1 = r_birth.get_field( m_index );
    Field m2 = r_date.get_field( m_index );
    Field d1 = r_birth.get_field( d_index );
    Field d2 = r_date.get_field( d_index );
    Field age = y2 - y1;
    if( m2 < m1 || (m2 == m1 && d2 < d1) ) {
        age--;
    }
    SValueVec obj;
    obj.push_back( "age:" );
    if( age > 0 ) {
        obj.push_back( age * direction );
        obj.push_back( "year" );
        return SValue( obj );
    }
    age = m2 - m1;
    if( d2 < d1 ) {
        age--;
    }
    if( age < 0 ) {
        age += 12;
    }
    if( age > 0 ) {
        obj.push_back( age * direction );
        obj.push_back( "month" );
        return SValue( obj );
    }
    Field days = date - birth;
    age = days / 7;
    if( age > 0 ) {
        obj.push_back( age * direction );
        obj.push_back( "week" );
        return SValue( obj );
    }
    obj.push_back( days * direction );
    obj.push_back( "day" );
    return SValue( obj );
}

// End of src/hic/hicAtFunction.cpp file
