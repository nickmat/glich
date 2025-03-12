/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glcBlob.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Object class to handle script blobs.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     11th March 2025
 * Copyright:   Copyright (c) 2025, Nick Matthews.
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

#include "glcBlob.h"

#include <filesystem>
#include <fstream>

using namespace glich;
using std::string;
using std::vector;

bool Blob::load( const string& filename, BlobType type )
{
    std::ifstream file( filename, std::ios::binary | std::ios::ate );
    if( !file.is_open() ) {
        return false;
    }

    std::streamsize size = file.tellg();
    file.seekg( 0, std::ios::beg );

    m_data.resize( size );
    if( file.read( reinterpret_cast<char*>(m_data.data()), size ) ) {
        return false;
    }
    if( type == BlobType::unknown ) {
        string ext = std::filesystem::path( filename ).extension().string();
        if( ext == ".mp3" ) {
            type = BlobType::mp3;
        }
        else if( ext == ".mp4" ) {
            type = BlobType::mp4;
        }
        else if( ext == ".jpeg" || ext == ".jpg" ) {
            type = BlobType::jpeg;
        }
        else if( ext == ".png" ) {
            type = BlobType::png;
        }
    }
    m_type = type;
    return true;
}

bool Blob::save( const string& filename ) const
{
    std::ofstream file( filename, std::ios::binary );
    if( !file.is_open() ) {
        return false;
    }
    file.write( reinterpret_cast<const char*>(m_data.data()), m_data.size() );
    return true;
}

string Blob::type_str() const
{
    switch( m_type ) {
    case BlobType::file: return "file";
    case BlobType::mp3: return "mp3";
    case BlobType::mp4: return "mp4";
    case BlobType::jpeg: return "jpeg";
    case BlobType::png: return "png";
    default: return "unknown";
    }
}

// End of src/glc/glcBlob.cpp
