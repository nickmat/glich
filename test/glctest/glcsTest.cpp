/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        test/glctest/glcsTest.cpp
 * Project:     glctest: Command line Glich Test Script Program.
 * Purpose:     Command line test program for Glich library.
 * Author:      Nick Matthews
 * Website:     http://historycal.org
 * Created:     9th February 2023
 * Copyright:   Copyright (c) 2023..2024, Nick Matthews.
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

#include <glc/glc.h>

#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <Windows.h>
#include <Wincon.h>
#endif

#define VERSION   "0.1.0"
#define PROGNAME  "GlichTest"
#define COPYRIGHT  "2023 Nick Matthews"

const char* g_version = VERSION;
const char* g_progName = PROGNAME;
const char* g_copyright = "Copyright (c) " COPYRIGHT "\n";

#if defined(NDEBUG) || !defined(_DEBUG)
const char* g_title = PROGNAME " - Version " VERSION "\n";
#else
const char* g_title = PROGNAME " - Version " VERSION " Debug\n";
#endif

namespace fs = std::filesystem;
using namespace glich;
using std::string;
using std::vector;

struct TestResults {
    TestResults() 
        : tests( 0 ), fails( 0 ), skips( 0 ), show_skips( false ), show_files( false ) {}
    int tests;
    int fails;
    int skips;
    bool show_skips;
    bool show_files;
};

string left_trim( const string& str )
{
    size_t pos = 0;
    while( str.find( ' ', pos ) == pos ) {
        pos++;
    }
    if( pos != string::npos ) {
        return str.substr( pos );
    }
    return "";
}

string get_first_word( const string& str, string* tail, char sep )
{
    size_t pos = str.find( sep );
    string result = str.substr( 0, pos );
    if( tail ) {
        *tail = ( pos == string::npos ) ? "" : left_trim( str.substr( pos + 1 ) );
    }
    return result;
}

string read_file( const string& name )
{
    std::ifstream in( name.c_str() );
    return std::string( std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() );
}

string run_test( TestResults* totals, Glich& glc, const string& filename )
{
    if( totals->show_files ) {
        std::cout << "\n" << filename << "  ";
    }
    string script = read_file( filename );
    size_t pos1 = script.find( "\n/*[SKIP]*/" );
    if ( pos1 != string::npos ) {
        totals->skips++;
        std::cout << "S";
        if ( totals->show_skips ) {
            return filename + " skipped.";
        }
        return "";
    }
    glc.run_script( "mark __:test:__;" ); // All test should start clean;
    string output = glc.run_script( script );
    totals->tests++;
    string expected;
    pos1 = script.find( "/*[OUTPUT]\n" );
    if( pos1 != string::npos ) {
        pos1 += 11;
        size_t pos2 = script.find( "\n[OUTPUT]*/", pos1 );
        if( pos2 != string::npos ) {
            expected = script.substr( pos1, pos2 - pos1 );
        }
    }
    string error;
    if( output.empty() ) {
        error = "No output";
    } else if( output != expected ) {
        error = "  Expected: " + expected + "\n"
            + "  Output: " + output;
    }
    string result;
    if( !error.empty() ) {
        result = filename + "\n" + error;
        std::cout << "F";
    } else {
        std::cout << ".";
    }
    return result;
}

bool ends_with( string const& str, string const& ending ) {
    if( str.length() >= ending.length() ) {
        return 0 == str.compare( str.length() - ending.length(), ending.length(), ending );
    }
    return false;
}

string run_full_test( TestResults* totals, Glich& glc, const string& path )
{

    string result;
    for( auto const& dir_entry : fs::directory_iterator { path } ) {
        if( !dir_entry.is_directory() ) {
            continue;
        }
        glc.run_script( "mark __user;" ); // Start directory clean;
        string dir = dir_entry.path().string();
        if( ends_with( dir, "hics-lib" ) ) {
            glc.load_hics_library();
        }
        vector<string> filenames;
        get_filenames( filenames, dir );
        for( size_t i = 0; i < filenames.size(); i++ ) {
            string error = run_test( totals, glc, filenames[i] );
            if( !error.empty() ) {
                totals->fails++;
                result += "\n\n" + error;
            }
        }
    }
    return result;
}

string run_test_script( TestResults* totals, Glich& glc, const string& filename )
{
    string script = read_file( filename );
    glc.run_script( "mark __user;" ); // Start clean;
    if( filename.find( "hics-lib" ) != string::npos ) {
        glc.load_hics_library();
    }
    string output = glc.run_script( script );
    string expected;
    size_t pos1 = script.find( "/*[OUTPUT]\n" );
    if( pos1 != string::npos ) {
        pos1 += 11;
        size_t pos2 = script.find( "\n[OUTPUT]*/", pos1 );
        if( pos2 != string::npos ) {
            expected = script.substr( pos1, pos2 - pos1 );
        }
    }
    string result = filename + "  ";
    if ( output.empty() ) {
        result += "No output\n";
        totals->fails++;
    } else if ( output == expected ) {
        result += "Pass: " + output + "\n\n";
    } else {
        result += "\n Expected: " + expected + "\n"
            + "   Output: " + output + "\n\n";
        totals->fails++;
    }
    totals->tests++;
    return result;
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

    std::cout << g_title << "\n";
    clock_t t = clock();
    init_glc( InitLibrary::None );
    Glich& glc = *get_glc();
    string result;
    TestResults totals;
    for ( int i = 1; i < argc; i++ ) {
        string arg = argv[i];
        if( arg == "-s" ) {
            totals.show_skips = true;
            continue;
        }
        if( arg == "-f" ) {
            totals.show_files = true;
            continue;
        }
        CheckFile cf = check_file( arg );
        if ( cf == CheckFile::file ) {
            result += run_test_script( &totals, glc, arg );
            continue;
        } else if ( cf == CheckFile::dir ) {
            result += run_full_test( &totals, glc, arg );
            continue;
        }
        result += "Unknown command line switch " + arg + "\n\n";
    }
    if( !totals.show_files ) {
        std::cout << result;
    }

    double dt = ((double) clock() - t) / CLOCKS_PER_SEC;

    std::cout << "\n\nRun (" + std::to_string( totals.tests ) + ") "
        "  fail (" + std::to_string( totals.fails ) + ")"
        "  skip (" + std::to_string( totals.skips ) + ")"
        "  Timed: " + std::to_string( dt ) + "s\n\n";

    exit_glc();
    return 0;
}

// End of test/glctest/glcsTest.cpp
