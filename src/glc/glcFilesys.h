/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glcFilesys.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Functions to interact with the host filesystem.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     12th December 2023
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

#ifndef SRC_GLC_GLCFILESYS_H_GUARD
#define SRC_GLC_GLCFILESYS_H_GUARD

#include <glc/glcDefs.h>

namespace glich {

    class SValue;

    SValue action_at_filesys( const std::string& cmnd );

}

#endif // SRC_GLC_GLCFILESYS_H_GUARD
