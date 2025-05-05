/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        test/gs/gsMain.h
 * Project:     glcSoak: Soak (extended unit) test program for Hics library.
 * Purpose:     Program main function header.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     4th September 2023
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

#ifndef TEST_GLCSOAK_GSMAIN_H
#define TEST_GLCSOAK_GSMAIN_H

#include <glc/hic.h>

#include <sstream>

extern glich::HicGlich* g_glc;

// Only one of these should defined at any one time.
//#define GLCTEST_SHORT
//#define GLCTEST_LONG
#define GLCTEST_SOAK

enum class Depth { Short, Long, Soak };

extern Depth g_depth;

inline std::string cvt_fields(
    glich::Field f1, glich::Field f2, glich::Field f3, glich::Field f4 )
{
    std::stringstream tst;
    tst << f1 << " " << f2 << " " << f3 << " " << f4;
    return tst.str();
}

inline std::string cvt_fields(
    glich::Field f1, glich::Field f2, glich::Field f3 )
{
    std::stringstream tst;
    tst << f1 << " " << f2 << " " << f3;
    return tst.str();
}

inline std::string cvt_fields( glich::Field f1, glich::Field f2 )
{
    std::stringstream tst;
    tst << f1 << " " << f2;
    return tst.str();
}

inline std::string cvt_fields( glich::Field f1 )
{
    std::stringstream tst;
    tst << f1;
    return tst.str();
}

constexpr const char* invalid = "";


#endif // TEST_GLCSOAK_GSMAIN_H
