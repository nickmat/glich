/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/hic/hic.cpp
 * Project:     glc: Glich script hic extension library.
 * Purpose:     Global access to the HicGlich derived class.
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

#include <glc/hic.h>

#include <cassert>

using namespace glich;


static HicGlich* s_glc = nullptr;

#if !defined(NDEBUG) || defined(_DEBUG)
Glich* g_glc_test = nullptr; // To allow Visual Studio watch to see it
#endif

void glich::init_glc( InitLibrary lib, InOut* inout )
{
    if( s_glc ) {
        delete s_glc;
    }
    s_glc = new HicGlich( inout );
#if !defined(NDEBUG) || defined(_DEBUG)
    g_glc_test = s_glc;
#endif

    s_glc->load_builtin_library();
    switch( lib )
    {
    case InitLibrary::None:
        break;
    case InitLibrary::Hics:
        s_glc->load_hics_library();
        break;
    }
    // Mark the start of user definitions.
    s_glc->run_script( "mark __user;" );

}

void glich::exit_glc()
{
    delete s_glc;
}

HicGlich& glich::glc()
{
    assert( s_glc != nullptr );
    return *s_glc;
}

// End of src/hic/hic.cpp
