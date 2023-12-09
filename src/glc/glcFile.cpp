/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glcs/glsFile.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     File class to handle script files.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     6th February 2023
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

#include "glcFile.h"

using namespace glich;
using std::string;

bool File::open()
{
    if ( m_file.is_open() ) {
        return false;
    }
    std::fstream::openmode mode = std::fstream::out;
    switch ( m_filetype )
    {
    case FT_read:
        mode = std::fstream::in;
        break;
    case FT_write:
        mode |= std::fstream::trunc;
        break;
    case FT_append:
        mode |= std::fstream::app;
        break;
    default:
        return false;
    }
    m_file.open( m_filename, mode );
    if ( !m_file.is_open() ) {
        return false;
    }
    return true;
}

// End of src/glcs/glsFile.cpp file
