/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicLibScripts.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Header for completed file2cpp template file.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     1st April 2023
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

#ifndef SRC_GLC_HICLIBSCRIPTS_H_GUARD
#define SRC_GLC_HICLIBSCRIPTS_H_GUARD


#include <glc/glcDefs.h>
#include "glcLibScripts.h"

#include <cstddef>

namespace glich {

    extern ScriptModule hics_builtin_scripts[];
    extern std::size_t hics_builtin_scripts_size;

    extern StdStrMap hics_default_scripts;

}

#endif // SRC_GLC_HICLIBSCRIPTS_H_GUARD