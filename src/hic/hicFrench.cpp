/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicFrench.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     French Republican (fr) calendar source.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     13th August 2023
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

#include "hicFrench.h"

#include "hicAstro.h"
#include "hicMath.h"
#include "hicRecord.h"

#include <cmath>
#include <cassert>

using namespace glich;
using std::string;


#define BASEDATE_French 2375840L  // 22 Sep 1792 (G)

namespace {

    const double paris_longitude = 2.3375;

    double midnight_in_paris( Field jdn )
    {
        double jdnp1 = jdn + 1;
        double universalfromlocal = jdnp1 - paris_longitude / 360;
        double localfromapparent = jdnp1 - equation_of_time( universalfromlocal );
        return localfromapparent - paris_longitude / 360;
    }

    bool fnyoob_min_func( Field jdn, const void* data )
    {
        double constant = *static_cast<const double*>(data);
        return constant <= solar_longitude( midnight_in_paris( jdn ) );
    }

    Field french_new_year_on_or_before( Field jdn )
    {
        double approx =
            estimate_prior_solar_longitude( autumn, midnight_in_paris( jdn ) );
        double c = autumn;
        return min_search( (Field) approx - 1, fnyoob_min_func, &c );
    }

    Field french_last_day_in_month( Field year, Field month )
    {
        const French base;
        if( month == 13 ) {
            FieldVec r = { year+1, 1, 1 };
            Record rec( base, r );
            Field jdn = rec.get_jdn();
            rec.set_jdn( jdn-1 );
            return rec.get_field( 2 );
        } else {
            return 30;
        }
    }

    // CC3 p242
    Field french_to_jdn( Field year, Field month, Field day )
    {
        Field approx = BASEDATE_French + 180 + (Field) ( mean_tropical_year * ( year-1 ) );

        Field newyear = french_new_year_on_or_before( approx );

        return newyear - 1 + 30 * (month-1) + day;
    }

    // CC3 p242
    bool french_from_jdn( Field* year, Field* month, Field* day, Field jdn )
    {
        Field newyear = french_new_year_on_or_before( jdn );

        double y = ( (double) (newyear - BASEDATE_French) ) / mean_tropical_year;
        *year = (Field) floor( y + 1.5 );

        *month = ( jdn - newyear ) / 30 + 1;

        *day = ( jdn - newyear ) % 30 + 1;

        return true;
    }

} // namespace


French::French( const StdStrVec& data )
    : Base( data, 3 )
{
    m_fieldnames = { "year", "month", "day" };
}


Field French::get_jdn( const FieldVec& fields ) const
{
    if( fields.size() < 3 || fields[0] == f_invalid || fields[1] == f_invalid || fields[2] == f_invalid ) {
        return f_invalid;
    }
    return french_to_jdn( fields[0], fields[1], fields[2] );
}

Field French::get_end_field_value( const FieldVec& fields, size_t index ) const
{
    switch( index )
    {
    case 1: // Last month of year
        return 13;
    case 2: // Last day of month
        return french_last_day_in_month( fields[0], fields[1] );
    }
    return f_invalid;
}

FieldVec French::get_fields( Field jdn ) const
{
    FieldVec fields( record_size(), f_invalid );
    french_from_jdn( &fields[0], &fields[1], &fields[2], jdn );
    return fields;
}

/* static */
bool French::leap_year( Field year )
{
    return french_last_day_in_month( year, 13 ) == 6;
}

// End of src/glc/hicFrench.cpp
