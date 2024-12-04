/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicLiturgical.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Liturgical calendar source.
 * Author:      Nick Matthews
 * Website:     http://historycal.org
 * Created:     18th August 2024
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

    Day numbers (enum Weekday) 0 = Monday ... 6 = Sunday

    Week numbers

  Blk5   62  1st Week of Christmas      1st Sunday on or after 25 Dec
    ----------- Year Change ------------
        * 1  2nd Week of Christmas      Sunday between 1 Jan and 5 Jan (incl.)
  Blk1  * 2  Epiphany                   Only if 6 Jan is Sunday
  Blk2    3  1st Week of Epiphany       1st Sunday after 6 Jan
        * 4  2nd Week of Epiphany
        * 5  3rd Week of Epiphany
        * 6  4th Week of Epiphany
        * 7  5th Week of Epiphany
        * 8  6th Week of Epiphany
  Blk3    9  Septuagesima
         10  Sexagesima
         11  Quinquagesima
         12  Quadragesima
         13  2nd Week of Lent
         14  3rd Week of Lent
         15  4th Week of Lent
         16  5th (Passion)
         17  6th (Holy Week) (Palm)
         18  Easter Week
         19  Quasimodo
         20  2nd Week after Easter
         21  3rd Week after Easter
         22  4th Week after Easter
         23  Rogation
         24  1st Week after Ascension
         25  Whitsun
         26  Trinity
         27  1st Week after Trinity
         28  2nd Week after Trinity
         29  3rd Week after Trinity
         30  4th Week after Trinity
         31  5th Week after Trinity
         32  6th Week after Trinity
         33  7th Week after Trinity
         34  8th Week after Trinity
         35  9th Week after Trinity
         36  10th Week after Trinity
         37  11th Week after Trinity
         38  12th Week after Trinity
         39  13th Week after Trinity
         40  14th Week after Trinity
         41  15th Week after Trinity
         42  16th Week after Trinity
         43  17th Week after Trinity
         44  18th Week after Trinity
         45  19th Week after Trinity
         46  20th Week after Trinity
         47  21st Week after Trinity
         48  22nd Week after Trinity
        *49  23rd Week after Trinity
        *50  24th Week after Trinity
        *51  25th Week after Trinity
        *52  26th Week after Trinity
        *53  27th Week after Trinity
        *54  4th Week before Advent
        *55  3rd Week before Advent
        *56  2nd Week before Advent
        *57  1st Week before Advent
  Blk4   58  1st Week of Advent
         59  2nd Week of Advent
         60  3rd Week of Advent
         61  4th Week of Advent
  Blk5   62  1st Week of Christmas

    ------- Next Liturgical Year --------
        * 1  2nd Week of Christmas
  Blk1  * 2  Epiphany
  Blk2    3  1st Week of Epiphany

    * Optional weeks.

    Block numbers
    Blk1  (Epiph)     If [base: year 1 6] is a Sunday
    Blk2  (Epiph 1)   First sunday after [base: year 1 6]
    Blk3  (Septuag)   Ninth Sunday before Easter [base: Easter(year) - 63]
    Blk4  (Advent 1)  Nearest Sunday to [base: year 11 30]
*/

#include "hicLiturgical.h"

#include "hicGregorian.h"
#include "hicMath.h"
#include "hicOptional.h"
#include "hicRecord.h"

#include <cassert>

using namespace glich;
using std::string;


Liturgical::Liturgical( const StdStrVec& data )
    : m_basetype( BaseType::gregorian ), m_base( nullptr ), Base( StdStrVec(), 4 )
{
    m_fieldnames = { "year", "lweek", "wsday", "month", "day" };
    m_allow_shorthand = false;
    for( const string& word : data ) {
        cal_data( word );
    }
    if( m_basetype == BaseType::julian ) {
        m_base = new Julian( StdStrVec() );
    }
    else {
        m_base = new Gregorian( StdStrVec() );
    }
}

void Liturgical::cal_data( const string& data )
{
    if( data == "julian" ) {
        m_basetype = BaseType::julian;
    }
    else if( data == "gregorian" ) {
        m_basetype = BaseType::gregorian;
    }
    else {
        set_data( data );
    }
}

Field Liturgical::get_jdn( const FieldVec& fields ) const
{
    assert( fields.size() >= 3 );
    Field year = fields[0];
    Field week = fields[1];
    Field day = fields[2];
    Field jdn = f_invalid;
    // First check the week starting the end of the previous year
    // and deal with it if necessary.
    if( week == WEEK_Blk5 ) {
        Field yearstart = base_jdn( year, 1, 1 );
        jdn = xmas1( year - 1 );
        if( jdn >= yearstart ) {
            return jdn;
        }
    }
    if( week < WEEK_Blk1 ) {
        jdn = xmas2( year ) + (week - 1) * 7;
    }
    else if( week < WEEK_Blk2 ) {
        jdn = epiph( year ) + (week - WEEK_Blk1) * 7;
    }
    else if( week < WEEK_Blk3 ) {
        jdn = epiph1( year ) + (week - WEEK_Blk2) * 7;
    }
    else if( week < WEEK_Blk4 ) {
        jdn = septuag( year ) + (week - WEEK_Blk3) * 7;
    }
    else { // must be block 4 or 5
        jdn = advent( year ) + (week - WEEK_Blk4) * 7;
    }
    jdn += day - 1;
    return jdn;
}

Field Liturgical::get_end_field_value( const FieldVec& fields, size_t index ) const
{
    return Field();
}

void Liturgical::update_input( FieldVec& fields ) const
{
    Field jdn = get_jdn( fields );
    if( ( fields[3] == f_invalid || fields[4] == f_invalid ) && jdn != f_invalid ) {
        Field year = f_invalid, month = f_invalid, day = f_invalid;
        if( m_basetype == BaseType::julian ) {
            julian_from_jdn( &year, &month, &day, jdn );
        }
        else {
            gregorian_from_jdn( &year, &month, &day, jdn );
        }
        fields[3] = month;
        fields[4] = day;
    }
}

FieldVec Liturgical::get_fields( Field jdn ) const
{
    Field year = f_invalid, month = f_invalid, day = f_invalid;
    if( m_basetype == BaseType::julian ) {
        julian_from_jdn( &year, &month, &day, jdn );
    }
    else {
        gregorian_from_jdn( &year, &month, &day, jdn );
    }
    FieldVec fields( record_size(), f_invalid );
    fields[0] = year;
    fields[1] = get_litweek( jdn );
    fields[2] = GetOptionalField( "wsday", jdn );
    fields[3] = month;
    fields[4] = day;
    return fields;
}

Field Liturgical::base_jdn( Field year, Field month, Field day ) const
{
    if( m_basetype == BaseType::julian ) {
        return julian_to_jdn( year, month, day );
    }
    return gregorian_to_jdn( year, month, day );
}

Field Liturgical::xmas1( Field year ) const
{
    return kday_on_or_after( WDAY_Sunday, base_jdn( year, 12, 25 ) );
}

Field Liturgical::xmas2( Field year ) const
{
    return kday_on_or_after( WDAY_Sunday, base_jdn( year, 1, 1 ) );
}

Field Liturgical::epiph( Field year ) const
{
    return kday_on_or_after( WDAY_Sunday, base_jdn( year, 1, 6 ) );
}

Field Liturgical::epiph1( Field year ) const
{
    return kday_after( WDAY_Sunday, base_jdn( year, 1, 6 ) );
}

Field Liturgical::septuag( Field year ) const
{
    if( m_basetype == BaseType::julian ) {
        return Julian::easter( year ) - 63;
    }
    return Gregorian::easter( year ) - 63;
}

Field Liturgical::advent( Field year ) const
{
    return kday_nearest( WDAY_Sunday, base_jdn( year, 11, 30 ) );
}

Field Liturgical::get_litweek( Field jdn ) const
{
    Field sunday = kday_on_or_before( WDAY_Sunday, jdn );
    Record rec( *m_base, sunday );
    Field year = rec.get_field( 0 );
    Field b1 = base_jdn( year, 1, 6 );
    if( sunday < b1 ) {
        return 1;
    }
    if( sunday == b1 ) {
        return 2;
    }
    Field b2 = epiph1( year );
    Field b3 = septuag( year );
    if( sunday < b3 ) {
        return WEEK_Blk2 + (sunday - b2) / 7;
    }
    Field b4 = advent( year );
    if( sunday < b4 ) {
        return WEEK_Blk3 + (sunday - b3) / 7;
    }
    return WEEK_Blk4 + (sunday - b4) / 7;
}


// End of src/glc/hicLiturgical.cpp file
