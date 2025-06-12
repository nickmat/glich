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

#include "glcMath.h"
#include "glcValue.h"
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

//FunctionData* HicScript::get_function_data( Scheme* sch, Format* fmt )
static FunctionData* get_function_data( Scheme* sch, Format* fmt, std::ostream& outs )
{
    if( fmt->has_use_function() ) {
        string ocode = sch->get_code();
        Object* obj = glc().get_object( ocode );
        string funcode = fmt->get_from_text_funcode();
        Function* fun = obj->get_function( funcode );
        FunctionData* fundata = new FunctionData( *fun, outs );
        fundata->ocode = ocode;
        return fundata;
    }
    return nullptr;
}

static SValue str_to_date( Scheme* sch, string& text, const string& fcode, std::ostream& outs )
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
    FunctionData* fundata = get_function_data( sch, fmt, outs );
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

static SValue complete_object( Scheme* sch, Field jdn, std::ostream& outs )
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
        StdStrVec qual;
        SValueVec args;
        value = fun->run( &value, qual, args, outs );
    }
    return value;
}

static SValue complete_object( Scheme* sch, Range rng, const std::string& fcode )
{
    assert( sch != nullptr );
    const Base& base = sch->get_base();
    Record beg( base, rng.m_beg ), end( base, rng.m_end );
    Format* fmt = sch->get_input_format( fcode );
    assert( fmt != nullptr );
    BoolVec reveal = fmt->get_reveal( beg, end );
    FieldVec beg_fields = beg.get_reveald_fields( reveal );
    FieldVec end_fields = end.get_reveald_fields( reveal );
    for( size_t i = 0; i < beg_fields.size(); i++ ) {
        if( beg_fields[i] != end_fields[i] ) {
            return SValue::create_error( "Can not complete object." );
        }
    }
    beg.set_fields( beg_fields );
    return beg.get_object( sch->get_code() );
}

static SValue complete_object( Scheme* sch, const string& input, const string& fcode, std::ostream& outs )
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
            StdStrVec qual;
            SValueVec args;
            value = fun->run( &value, qual, args, outs );
        }
        mask.set_object( value );
    }
    return mask.get_object( ocode );
}


SValue glich::hic_at_date( const StdStrVec& quals, const SValueVec& args, std::ostream& outs )
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
        return str_to_date( sch, text, fcode, outs );
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


SValue glich::hic_at_scheme( const StdStrVec& quals, const SValueVec& args, std::ostream& outs )
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
        return complete_object( sch, jdn, outs);
    }
    Range range = value.get_range( success );
    if( success ) {
        if( sch == nullptr ) {
            sch = hic().get_oscheme();
            if( sch == nullptr ) {
                return SValue::create_error( no_default_mess );
            }
        }
        return complete_object( sch, range, fcode );
    }
    if( value.type() == SValue::Type::String ) {
        if( sch == nullptr ) {
            sch = hic().get_ischeme();
            if( sch == nullptr ) {
                return SValue::create_error( no_default_mess );
            }
        }
        string text = value.get_str();
        return complete_object( sch, text, fcode, outs );
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
        value.set_field( ele.get_converted_field( value.get_str() ) );
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

SValue glich::hic_at_age( const StdStrVec& quals, const SValueVec& args, std::ostream& outs )
{
    if( args.size() < 2 ) {
        return SValue::create_error( "@age requires birth date and an 'age at' date." );
    }
    SValue birth_value( args[0] );
    bool success = false;
    if( birth_value.type() == SValue::Type::String || birth_value.type() == SValue::Type::Object ) {
        birth_value = hic_at_date( quals, args, outs );
    }
    Field birth = birth_value.get_field( success );
    if( !success ) {
        return SValue::create_error( "@age birth date not valid." );
    }

    SValue date_value( args[1] );
    if( date_value.type() == SValue::Type::String || date_value.type() == SValue::Type::Object ) {
        SValueVec arg = { date_value };
        date_value = hic_at_date( quals, arg, outs );
    }
    Field date = date_value.get_field( success );
    if( !success ) {
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

SValue glich::hic_at_dob( const StdStrVec& quals, const SValueVec& args, std::ostream& outs )
{
    if( args.size() < 2 ) {
        return SValue::create_error( "@dob requires date and age." );
    }
    bool success = false;
    SValue date_value( args[0] );
    if( date_value.type() == SValue::Type::Object || date_value.type() == SValue::Type::String ) {
        date_value = hic_at_date( quals, args, outs );
    }
    if( date_value.type() == SValue::Type::range ) {
        const char* mess = "@dob date range not valid.";
        Range range = date_value.get_range();
        SValueVec args2 = args;
        args2[0] = SValue( range.m_beg, SValue::Type::field );
        SValue beg_value = hic_at_dob( quals, args2, outs );
        args2[0] = SValue( range.m_end, SValue::Type::field );
        SValue end_value = hic_at_dob( quals, args2, outs );
        Range beg_range = beg_value.get_range( success );
        if( !success ) {
            return SValue::create_error( mess );
        }
        Range end_range = end_value.get_range( success );
        if( !success ) {
            return SValue::create_error( mess );
        }
        Range result = Range( beg_range.m_beg -1, end_range.m_end );
        if( !result.is_valid() ) {
            return SValue::create_error( mess );
        }
        return SValue( result );
    }
    if( date_value.type() == SValue::Type::rlist ) {
        RList rlist = date_value.get_rlist();
        SValueVec args2 = args;
        SValue result;
        for( const auto& range : rlist ) {
            SValue range_value( range );
            args2[0] = range_value;
            SValue dob_value = hic_at_dob( quals, args2, outs );
            if( result.type() == SValue::Type::Null ) {
                result = dob_value;
            }
            else {
                result.rlist_union( dob_value );
            }
        }
        return result;
    }
    Field date = date_value.get_as_field();
    if( date == f_invalid ) {
        return SValue::create_error( "@dob date not valid." );
    }
    SValue age = args[1];
    SValueVec age_obj;
    Field age_value = age.get_field( success );
    if( success ) {
        age_obj.push_back( "age:" );
        age_obj.push_back( age_value );
    }
    else if( age.type() == SValue::Type::Object ) {
        age_obj = age.get_object( success );
        if( !success || age_obj.size() < 2 ) {
            return SValue::create_error( "@dob age object not valid." );
        }
        age_value = age_obj[1].get_field( success );
        if( !success || age_value == f_invalid ) {
            return SValue::create_error( "@dob age value not valid." );
        }
    }
    else {
        return SValue::create_error( "@dob age type not recognised." );
    }

    string age_unit;
    if( age_obj.size() >= 3 ) {
        age_unit = age_obj[2].get_str( success );    
    }
    if( age_unit.empty() ) {
        age_unit = "year";  // Default unit is year
    }
    
    if( age_unit == "day" ) {
        return SValue( date - age_value );
    }
    if( age_unit == "week" ) {
        Field start = date - (age_value + 1) * 7 + 1;
        return SValue( Range( start, start + 6 ) );
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
    Record  record( base, date );
    Record beg( record );
    Record end( record );
    int y_index = base.get_fieldname_index( "year" );
    if( y_index < 0 ) {
        return SValue::create_error( "@dob scheme does not have a year field." );
    }
    if( age_unit == "year" ) {
        Field y_value = record.get_field( y_index );
        beg.set_field( y_value - age_value -1 , y_index);
        end.set_field( y_value - age_value, y_index );
    }
    else if( age_unit == "month" ) {
        Field m_index = base.get_fieldname_index( "month" );
        if( m_index < 0 ) {
            return SValue::create_error( "@dob scheme does not have a month field." );
        }
        Field m_value = record.get_field( m_index ) - age_value - 1;
        Field y_value = record.get_field( y_index );
        if( m_value < 1 ) {
            y_value--;
            m_value += 12;
        }
        beg.set_field( m_value, m_index );
        beg.set_field( y_value, y_index );
        m_value++;
        if( m_value > 12 ) {
            m_value = 1;
            y_value++;
        }
        end.set_field( m_value, m_index );
        end.set_field( y_value, y_index );
    }
    else {
        return SValue::create_error( "@dob age unit not recognised." );
    }

    Range range( beg.calc_jdn() + 1, end.calc_jdn() );
    if( !range.is_valid() ) {
        return SValue::create_error( "@dob range not valid." );
    }
    return SValue( range );
}

SValue glich::hic_sch_at_pseudo_in( const Scheme* sch, const SValueVec& args )
{
    Format* fmt = nullptr;
    if( !args.empty() && args[0].type() == SValue::Type::String ) {
        string fcode = args[0].get_str();
        fmt = sch->get_input_format( fcode );
    }
    if( fmt == nullptr ) {
        return SValue::create_error( "Format not found." );
    }
    return SValue( fmt->get_input_str() );
}

SValue glich::hic_sch_at_pseudo_out( const Scheme* sch, const SValueVec& args )
{
    Format* fmt = nullptr;
    if( !args.empty() && args[0].type() == SValue::Type::String ) {
        string fcode = args[0].get_str();
        fmt = sch->get_output_format( fcode );
    }
    if( fmt == nullptr ) {
        return SValue::create_error( "Format not found." );
    }
    return SValue( fmt->get_output_str() );
}

SValue glich::hic_sch_at_normalise( const Scheme* sch, const StdStrVec& qual, const SValue& left )
{
    Norm norm = Norm::expand;
    if( !qual.empty() ) {
        norm = norm_from_string( qual[0] );
    }
    Record record( sch->get_base(), left );
    FieldVec& fields = record.get_field_vec();
    const Base& base = sch->get_base();
    if( base.normalise( fields, norm ) ) {
        record.calc_jdn();
    }
    return record.get_object( left.get_object_code() );
}


// End of src/hic/hicAtFunction.cpp file
