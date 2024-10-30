/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glc.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Global access to the latest Glich derived class.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     8th February 2023
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

#include <glc/glc.h>

#include <cassert>

using namespace glich;

// Initialise.
//   glc()  is the generic name of the system.
//   glcs() is the name of the base Glich Script system.
//   hics() is the name of the hics (HistoryCal Script) calendar extension.
//   hils() is the name of the hils (HistoryLoc Script) future location extension.
//   higs() is the name of the hics (HistoryGen Script) future genealogy extension.

static Glich* s_glc = nullptr;

#if !defined(NDEBUG) || defined(_DEBUG)
Glich* g_glc_test = nullptr; // To allow Visual Studio watch to see it
#endif

void glich::init_glc( InOut* inout )
{
    if( s_glc ) {
        delete s_glc;
    }
    s_glc = new Glich( inout );
#if !defined(NDEBUG) || defined(_DEBUG)
    g_glc_test = s_glc;
#endif
    s_glc->init();

    s_glc->load_builtin_library();
    // Mark the start of user definitions.
    s_glc->run_script( "mark __user;" );

}

void glich::init_glc( Glich* glc )
{
    s_glc = glc;
#if !defined(NDEBUG) || defined(_DEBUG)
    g_glc_test = s_glc;
#endif
}

void glich::exit_glc()
{
    delete s_glc;
}

Glich& glich::glc()
{
    assert( s_glc != nullptr );
    return *s_glc;
}

// End of src/glc/glc.cpp
