/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glcFile.h
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

#ifndef SRC_GLC_GLCFILE_H_GUARD
#define SRC_GLC_GLCFILE_H_GUARD

#include <fstream>

namespace glich {

    class File {
    public:
        enum FileType { FT_null, FT_read, FT_write, FT_append };

        File ( const std::string& code ) : m_code( code ), m_filetype( FT_write ) {}
        
        void set_filename( const std::string& filename ) { m_filename = filename; }
        void set_filetype( FileType filetype ) { m_filetype = filetype; }

        std::string get_code() const { return m_code; }

        bool open();
        std::ostream* get_out() { return &m_file; }
        std::istream* get_input() { return &m_file; }

    private:
        std::string  m_code;
        std::string  m_filename;
        FileType     m_filetype;
        std::fstream m_file;
    };

}

#endif // SRC_GLC_GLCFILE_H_GUARD
