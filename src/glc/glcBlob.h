/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glcBlob.h
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

#ifndef SRC_GLC_GLCBLOB_H_GUARD
#define SRC_GLC_GLCBLOB_H_GUARD

#include <glc/glcDefs.h>

namespace glich {

    enum class BlobType { unknown, file, mp3, mp4, jpeg, png };

    class Blob
    {
    public:
        Blob() {}

        bool load( const std::string& filename, BlobType type = BlobType::unknown );
        bool save( const std::string& filename ) const;

        std::string type_str() const;
        size_t size() const { return m_data.size(); }

    private:
        BlobType m_type = BlobType::unknown;
        std::vector<unsigned char> m_data;
    };
}

#endif // SRC_GLC_GLCBLOB_H_GUARD
