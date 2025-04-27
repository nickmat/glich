/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        test/glctest/glcGetfns.cpp
 * Project:     glctest: Command line Glich Test Script Program.
 * Purpose:     Read directory file listing.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     7th February 2023
 * Copyright:   Copyright (c) 2023..2025, Nick Matthews.
 * Licence:     GNU GPLv3
 *
 *  glctest is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  glctest is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with glctest.  If not, see <http://www.gnu.org/licenses/>.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

*/

#include "glcsTestmain.h"

#include <filesystem>

namespace fs = std::filesystem;

using std::vector;
using std::string;


CheckFile check_file( const string& name )
{
    fs::path path( name );
    if( fs::is_directory( path ) ) {
        return CheckFile::dir;  
    }
    if( fs::is_regular_file( path ) ) {
        return CheckFile::file;
    }
    return CheckFile::none; 
}

void get_filenames( vector<string>& vec, const string& path )
{
    fs::path dir( path );
    if( !fs::exists( dir ) || !fs::is_directory( dir ) ) {
        return;
    }
    for( const auto& entry : fs::directory_iterator( dir ) ) {
        if( entry.is_directory() ) {
            get_filenames( vec, entry.path().string() );
        }
        if( entry.is_regular_file() && entry.path().extension() == ".glcs" ) {
            vec.push_back( entry.path().string() );
        }
    }
}


// End of test/glctest/glcGetfns.cpp