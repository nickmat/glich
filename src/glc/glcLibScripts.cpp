/* ../../src/glc/glcLibScripts.cpp - File created by file2cpp 1.0.0 */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        lib/glich/glcScripts.f2c
 * Project:     Glich: Extendable Script Language.
 * Purpose:     file2cpp template file for glich library scripts.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     23rd May 2023
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

 This template is used to create the src/glc/glcLibScripts.cpp file.

*/

#include "glcLibScripts.h"

glich::ScriptModule glich::glc_builtin_scripts[] = {
    { "glc_builtin",  /* glc_builtin.glcs */
 "object : {};\n" },
};

size_t glich::glc_builtin_scripts_size =
    sizeof(glich::glc_builtin_scripts) / sizeof(glich::ScriptModule);

// End of lib/glich/glcScripts.f2c file
