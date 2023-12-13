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

#include <filesystem>

using namespace glich;
using std::string;
namespace fs = std::filesystem;


static SValue create_file_list()
{
    fs::path dir = fs::current_path();
    SValueVec values = { ":", dir.u8string() };

    SValue value;
    for( const auto& entry : fs::directory_iterator( dir ) ) {
        string prefix = entry.is_directory() ? "D: " : "F: ";
        string filename = prefix + entry.path().filename().u8string();
        value.set_str( filename );
        values.push_back( value );
    }

    return SValue( values );
}

SValue glich::action_at_filesys( const StdStrVec& quals, const SValueVec& args )
{
    string cmnd, arg0;
    if( !quals.empty() ) {
        cmnd = quals[0];
    }
    if( !args.empty() ) {
        arg0 = args[0].as_string();
    }

    if( cmnd == "cd" ) {
        if( args.empty() ) {
            return SValue::create_error( "@filesys command \"cd\" requires argument." );
        }
        std::error_code ec;
        fs::current_path( fs::path( arg0 ), ec );
        if( ec ) {
            if( ec == std::errc::no_such_file_or_directory ) {
                return SValue::create_error( "Directory \"" + arg0 + "\" does not exist." );
            }
            return SValue::create_error( ec.message() );
        }
    }
    else if( cmnd == "dir" ) {
        return create_file_list();
    }
    else if( !cmnd.empty() ) {
        return SValue::create_error( "@filesys command \"" + cmnd + "\" not recognised." );
    }
    return SValue( fs::current_path().u8string() );
}

// End of src/glcs/glsFile.cpp file
