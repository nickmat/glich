/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/hic/hic.cpp
 * Project:     glc: Glich script hic extension library.
 * Purpose:     Global access to the HicGlich derived class.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     22nd October 2024
 * Copyright:   Copyright (c) 2024..2026, Nick Matthews.
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

#include <glc/glc.h>

#include <cassert>

using namespace glich;


static HicGlich* s_hic = nullptr;

#if !defined(NDEBUG) || defined(_DEBUG)
Glich* g_hic_test = nullptr; // To allow Visual Studio watch to see it
#endif

void glich::init_hic( InitLibrary lib, InOut* inout, StdStrVec args )
{
    if( s_hic ) {
        delete s_hic;
    }
    s_hic = new HicGlich( inout );
#if !defined(NDEBUG) || defined(_DEBUG)
    g_hic_test = s_hic;
#endif
    s_hic->init();
    init_glc( s_hic );

    s_hic->load_builtin_library( args );
    switch( lib )
    {
    case InitLibrary::None:
        break;
    case InitLibrary::Hics:
        s_hic->load_hics_library();
        break;
    }
    // Mark the start of user definitions.
    s_hic->run_script( "mark __user;", "hics:" );

}

void glich::exit_hic()
{
    delete s_hic;
}

HicGlich& glich::hic()
{
    assert( s_hic != nullptr );
    return *s_hic;
}

// End of src/hic/hic.cpp
