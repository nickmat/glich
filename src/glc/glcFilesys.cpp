/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glcFilesys.cpp
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

#include "glcFilesys.h"

#include "glcValue.h"

#include <filesystem>

using namespace glich;
using std::string;
namespace fs = std::filesystem;

SValue glich::action_at_filesys( const std::string& cmnd )
{
    fs::path cpath = fs::current_path();
    if( cmnd.empty() ) {
        return SValue( cpath.u8string() );
    }
    return SValue::create_error( "@filesys command \"" + cmnd + "\" not recognised." );
}

// End of src/glcs/glsFile.cpp file
