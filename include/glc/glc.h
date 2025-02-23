/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        include/glc/glc.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Glich language class header
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     8th February 2023
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

#ifndef INCLUDE_GLC_GLC_H_GUARD
#define INCLUDE_GLC_GLC_H_GUARD

#include <glc/glcGlich.h>

namespace glich {

    void init_glc( InOut* inout = nullptr, StdStrVec args = StdStrVec() );
    void init_glc( Glich* glc );
    void exit_glc();
    Glich& glc();

} // namespace glich

#endif // INCLUDE_GLC_GLC_H_GUARD
