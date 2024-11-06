/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicAstro.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Functions for astrological calculations.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     13th August 2023
 * Copyright:   Copyright (c) 2023, Nick Matthews.
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

#ifndef SRC_GLC_HICASTRO_H_GUARD
#define SRC_GLC_HICASTRO_H_GUARD

namespace glich {

    extern const double mean_tropical_year; // 365.242189 days
    extern const double mean_synodic_month; // 29.530588853;

    extern const double spring; // 0 degrees
    extern const double summer; // 90 degrees
    extern const double autumn; // 180 degrees
    extern const double winter; // 270 degrees

    extern const double j2000;  // 1 Jan 2000 (G) plus 12 hours

    // CC3 p175
    inline double zone_from_longitude( double longitude )
    {
        return longitude / 360;
    }

    // CC3 p175
    inline double universal_from_local( double moment, double longitude )
    {
        return moment - zone_from_longitude( longitude );
    }

    // CC3 p175
    inline double local_from_universal( double moment, double longitude )
    {
        return moment + zone_from_longitude( longitude );
    }

    // CC3 p177
    inline double standard_from_universal( double moment, double zone )
    {
        return moment + zone;
    }

    // CC3 p177
    inline double universal_from_standard( double moment, double zone )
    {
        return moment - zone;
    }

    double equation_of_time( double moment );

    double solar_longitude( double moment );
    double solar_longitude_after( double season, double moment );

    double estimate_prior_solar_longitude( double season, double moment );

    double new_moon_before( double moment );
    double new_moon_at_or_after( double moment );

}

#endif // SRC_GLC_HICASTRO_H_GUARD
