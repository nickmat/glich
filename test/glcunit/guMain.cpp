/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        test/gu/guMain.cpp
 * Project:     glcUnit: Unit test program for the Glich Hics library.
 * Purpose:     Program main function source.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     17th August 2023
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

#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#include "guMain.h"

using namespace glich;
using std::string;

HicGlich* g_glc = nullptr;

int main( int argc, char* argv[] )
{
    init_hic( InitLibrary::Hics );
    g_glc = &hic();

    int result = Catch::Session().run( argc, argv );

    exit_hic();

    return result;
}

// End of test/gu/guMain.cpp file.
