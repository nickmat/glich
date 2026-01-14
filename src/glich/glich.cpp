/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glich/glich.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Glich Script interactive app glcs source file
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     23rd February 2025
 * Copyright:   Copyright (c) 2025..2026, Nick Matthews.
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

#include <glc/hic.h>

#include "glcsresource.h"

#include <iostream>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <Windows.h>
#include <Wincon.h>
#endif

namespace hg = glich;
using std::string;
using std::vector;


#define VERSION   "0.1.0"
#define PROGNAME  " (Glich Script)"
#define COPYRIGHT  "2023..2025 Nick Matthews"

const char* g_version = VERSION;
const char* g_progName = PROGNAME;
const char* g_copyright = "Copyright (c) " COPYRIGHT "\n";

#if defined(NDEBUG) || !defined(_DEBUG)
const char* g_title = PROGNAME " - Version " VERSION "\n";
#else
const char* g_title = PROGNAME " - Version " VERSION " Debug\n";
#endif


hg::StdStrVec get_args( int argc, char* argv[] )
{
    hg::StdStrVec args;
    for( int i = 0; i < argc; i++ ) {
        args.push_back( argv[i] );
    }
    return args;
}

void do_usage()
{
    std::cout << "glich" << g_title << g_copyright << "\n";
    std::cout <<
        "Usage:\n"
        "  glich [options]\n"
        "\n"
        "Options:\n"
        "  -h    Show this help message and exit.\n"
        "  name  Run the file 'name' as a script.\n"
        "        Multiple files are run in the order that they appear.\n"
        "\n"
        ;
}

string read_file( const string& name )
{
    std::ifstream in( name.c_str() );
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

int main( int argc, char* argv[] )
{
#ifdef _WIN32
    // Set Windows console in/out to use utf-8.
    SetConsoleCP( CP_UTF8 );
    setvbuf( stdin, nullptr, _IOFBF, 1000 );
    SetConsoleOutputCP( CP_UTF8 );
    setvbuf( stdout, nullptr, _IOFBF, 1000 );
#endif
    hg::StdStrVec args = get_args( argc, argv );
    hg::init_hic( hg::InitLibrary::Hics, nullptr, args );
    hg::StdStrVec filenames;

    std::string prog = args[0];
    for( int i = 1; i < args.size(); i++ ) {
        if( args[i][0] == '-' ) {
            switch( args[i][1] )
            {
            case 'h': // Help
                do_usage();
                hg::exit_hic();
                return 0;
            default:
                std::cout << "Command line flag not recognised.\n";
            }
        }
        else if( args[i].substr( args[i].find_last_of( "." ) + 1 ) == "glcs" ) {
            filenames.push_back( args[i] );
        }
    }
    // Run script files if given.
    for( size_t i = 0; i < filenames.size(); i++ ) {
        string script = read_file( filenames[i] );
        if( script.empty() ) {
            continue;
        }
        string response = hg::hic().run_script( script, "file:" + filenames[i]);
        if( !response.empty() ) {
            std::cout << response << "\n";
        }
    }

    hg::exit_hic();
    return 0;
}


// End of src/glich/glich.cpp
