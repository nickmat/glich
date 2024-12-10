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

B1  1 Blk4  58  1st Week of Advent
    2       59  2nd Week of Advent
    3       60  3rd Week of Advent
    4       61  4th Week of Advent
B2  5 Blk5  62  1st Week of Christmas      Sunday between 25 Dec and 31 Dec (incl.)
      ----------- Year Change ------------
    6      * 1  2nd Week of Christmas      Sunday between 1 Jan and 5 Jan (incl.)
B3  7 Blk1 * 2  Epiphany                   Only if 6 Jan is Sunday
B4  8 Blk2 * 3  1st Week of Epiphany       1st Sunday after 6 Jan
    9      * 4  2nd Week of Epiphany
   10      * 5  3rd Week of Epiphany
   11      * 6  4th Week of Epiphany
   12      * 7  5th Week of Epiphany
   13      * 8  6th Week of Epiphany
B5 14 Blk3   9  Septuagesima
   15       10  Sexagesima
   16       11  Quinquagesima
   17       12  Quadragesima
   18       13  2nd Week of Lent
   19       14  3rd Week of Lent
   20       15  4th Week of Lent
   21       16  5th (Passion)
   22       17  6th (Holy Week) (Palm)
   23       18  Easter Week
   24       19  Quasimodo
   25       20  2nd Week after Easter
   26       21  3rd Week after Easter
   27       22  4th Week after Easter
   28       23  Rogation
   29       24  1st Week after Ascension
   30       25  Whitsun
   31       26  Trinity
   32       27  1st Week after Trinity
   33       28  2nd Week after Trinity
   34       29  3rd Week after Trinity
   35       30  4th Week after Trinity
   36       31  5th Week after Trinity
   37       32  6th Week after Trinity
   38       33  7th Week after Trinity
   39       34  8th Week after Trinity
   40       35  9th Week after Trinity
   41       36  10th Week after Trinity
   42       37  11th Week after Trinity
   43       38  12th Week after Trinity
   44       39  13th Week after Trinity
   45       40  14th Week after Trinity
   46       41  15th Week after Trinity
   47       42  16th Week after Trinity
   48       43  17th Week after Trinity
   49       44  18th Week after Trinity
   50       45  19th Week after Trinity
   51       46  20th Week after Trinity
   52       47  21st Week after Trinity
   53       48  22nd Week after Trinity
   54      *49  23rd Week after Trinity
   55      *50  24th Week after Trinity
   56      *51  25th Week after Trinity
   57      *52  26th Week after Trinity
   58      *53  27th Week after Trinity
   59      *54  4th Week before Advent
   60      *55  3rd Week before Advent
   61      *56  2nd Week before Advent
   62      *57  1st Week before Advent

   ------- Next Liturgical Year --------
B1  1 Blk4  58  1st Week of Advent
    2       59  2nd Week of Advent
    3       60  3rd Week of Advent
    4       61  4th Week of Advent
B2  5 Blk5  62  1st Week of Christmas      Sunday between 25 Dec and 31 Dec (incl.)
   ----------- Year Change ------------
    6      * 1  2nd Week of Christmas      Sunday between 1 Jan and 5 Jan (incl.)

    * Optional weeks.

    Block numbers
B1  Blk4  (Advent 1)  Nearest Sunday to [base: year 11 30]
B2  Blk5  (Xmas I)    First Sunday on or after 25th Dec
B3  Blk1  (Epiph)     If [base: year 1 6] is a Sunday
B4  Blk2  (Epiph 1)   First sunday after [base: year 1 6]
B5  Blk3  (Septuag)   Ninth Sunday before Easter [base: Easter(year) - 63]
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
    m_fieldnames = { "lyear", "lweek", "wsday", "year", "month", "day"};
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
    Field lyear = fields[0];
    Field lweek = fields[1];
    Field wsday = fields[2];
    Field jdn = f_invalid;
    // First check the week starting the end of the previous year
    // and deal with it if necessary.
    if( lweek <= Block::B2 ) {
        Field year = lyear - 1;
        if( lweek == Block::B2 ) {
            jdn = xmas1( year );
        }
        else {
            jdn = advent( year ) + (lweek - Block::B1) * 7;
        }
    }
    else if( lweek < Block::B3 ) {
        jdn = xmas2( lyear );
    }
    else if( lweek == Block::B3 ) {
        jdn = epiph( lyear );
    }
    else if( lweek < Block::B5 ) {
        jdn = epiph1( lyear ) + (lweek - Block::B4) * 7;
    }
    else {
        jdn = septuag( lyear ) + (lweek - Block::B5) * 7;
    }
    return jdn + wsday - 1;
}

Field Liturgical::get_end_field_value( const FieldVec& fields, size_t index ) const
{
    return Field();
}

void Liturgical::update_input( FieldVec& fields ) const
{
    Field jdn = get_jdn( fields );
    if( ( fields[3] == f_invalid || fields[4] == f_invalid || fields[5] == f_invalid) &&
        jdn != f_invalid )
    {
        Field year = f_invalid, month = f_invalid, day = f_invalid;
        if( m_basetype == BaseType::julian ) {
            julian_from_jdn( &year, &month, &day, jdn );
        }
        else {
            gregorian_from_jdn( &year, &month, &day, jdn );
        }
        fields[3] = year;
        fields[4] = month;
        fields[5] = day;
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

    Field lyear = year, lweek = f_invalid;
    Field jdn_sunday = kday_on_or_before( WDAY_Sunday, jdn );
    Field jdn_advent = advent( year );
    if( jdn_sunday >= jdn_advent ) {
        lyear++;
        Field jdn_xmas1 = xmas1( year );
        if( jdn_sunday == jdn_xmas1 ) {
            lweek = Block::B2;
        }
        else {
            lweek = Block::B1 + (jdn_sunday - jdn_advent) / 7;
        }
    }
    else if( jdn_sunday == xmas2( year ) ) {
        lweek = Block::B2 + 1;
    }
    else if( jdn_sunday == epiph( year ) ) {
        lweek = Block::B3;
    }
    else {
        Field b5 = septuag( year );
        if( jdn_sunday < b5 ) {
            Field b4 = epiph1( year );
            lweek = Block::B4 + (jdn_sunday - b4) / 7;
        }
        else {
            lweek = Block::B5 + (jdn_sunday - b5) / 7;
        }
    }
    FieldVec fields( record_size(), f_invalid );
    fields[0] = lyear;
    fields[1] = lweek;
    fields[2] = jdn - jdn_sunday + 1;
    fields[3] = year;
    fields[4] = month;
    fields[5] = day;
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



// End of src/glc/hicLiturgical.cpp file
