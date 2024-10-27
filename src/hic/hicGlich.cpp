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
#include "hicScript.h"

#include <iostream>
#include <sstream>

using namespace glich;
using std::string;
using std::vector;


SValue HicGlich::evaluate( const string& expression )
{
    std::istringstream iss( expression );
    std::ostringstream oss;
    HicScript scr( iss, oss );
    return scr.evaluate();
}

bool glich::HicGlich::run( std::istream& in, std::ostream& out, int line )
{
    HicScript scr( in, out );
    scr.set_line( line );
    return scr.run();
}


// End of src/hic/hicGlich.cpp
