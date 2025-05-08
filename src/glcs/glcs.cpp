/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glcs/glsc.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Glich Script interactive app glcs source file
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     8th February 2023
 * Copyright:   Copyright (c) 2023..2025, Nick Matthews.
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

#include <algorithm>
#include <fstream>
#include <iostream>
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
#define COPYRIGHT  "2023 Nick Matthews"

const char* g_version = VERSION;
const char* g_progName = PROGNAME;
const char* g_copyright = "Copyright (c) " COPYRIGHT "\n";

#if defined(NDEBUG) || !defined(_DEBUG)
const char* g_title = PROGNAME " - Version " VERSION "\n";
#else
const char* g_title = PROGNAME " - Version " VERSION " Debug\n";
#endif

enum class StmtType { semicolon, curlybracket, both, if_endif, do_loop };

struct StmtStatus {
    StmtType type;
    bool in_quote;
    bool in_mcomment; // Multiline comment
    int brace_cnt;
    int word_cnt;

    StmtStatus( StmtType t )
        : type( t ), in_quote( false ), in_mcomment( false ),
        brace_cnt( 0 ), word_cnt( 0 ) {}
};

hg::StdStrVec get_args( int argc, char* argv[] )
{
    hg::StdStrVec args;
    for( int i = 0; i < argc; i++ ) {
        args.push_back( argv[i] );
    }
    return args;
}

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
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

void do_usage()
{
    std::cout << "glcs" << g_title << g_copyright << "\n";
    std::cout << 
        "Usage:\n"
        "  " << "glcs" << " [options]\n"
        "\n"
        "Options:\n"
        "  -h    Show this help message and exit.\n"
        "  name  Run the file 'name' as a script.\n"
        "        Multiple files are run in the order that they appear.\n" 
        "\n"
    ;
}

void do_help( const string& option )
{
    if ( option == "usage" ) {
        do_usage();
        return;
    }
    std::cout <<
        "\n"
        "Welcome to the Glich interactive program.\n"
        "\n"
        "Briefly, a script consists of one or more statements. These statements are\n"
        "detailed in the online manual at http://historycal.org/man/script/. A single\n"
        "statements is entered, if necessary over a number of lines, and it will be\n"
        "executed as soon as the statement is completed.\n"
        "\n"
        "In addition to the statements, the following commands are available: 'help',\n"
        "'info', 'run' and 'end'. These are used as follows.\n"
        "\n"
        " help             Display this text.\n"
        " help usage       Display the command line details.\n"
        //" help <statement> Display help for the given statement.\n"
        " info             List all the currently defined schemes.\n"
        " info <scheme>    Display scheme details of the given scheme code.\n"
        " run <filename>   Import and run the script in the named file.\n"
        " end              Exit the program.\n"
        "\n"
        "If what you enter is neither a named statement or a command then, if it ends\n"
        "with a ';' character, it is treated as an assignment statement. Otherwise it is\n"
        "treated as an expression, wrapped within a 'write' statement and executed.\n"
        "\n"
    ;
}

// Remove comments and strings.
string compress_statement( const string& line, StmtStatus& status )
{
    string compressed;
    bool start_comment = false;
    bool end_comment = false;

    for( auto ch : line ) {
        if( start_comment ) {
            start_comment = false;
            if( ch == '/' ) {
                break; // Single line comment.
            }
            else if( ch == '*' ) {
                status.in_mcomment = true;
                continue;
            }
            else {
                compressed += string( "/" ) + ch;
            }
        }
        if( status.in_mcomment ) {
            if( end_comment ) {
                if( ch == '/' ) {
                    status.in_mcomment = false;
                }
            }
            end_comment = (ch == '*');
            continue;
        }

        if( status.in_quote ) {
            if( ch == '"' ) {
                status.in_quote = false;
            }
        }
        else if( ch == '"' ) {
            status.in_quote = true;
        }
        else if( ch == '/' ) {
            start_comment = true;
        }
        else {
            compressed += ch;
        }
    }
    return compressed;
}

bool terminated_semicolon( const string& stmt )
{
    if( stmt.empty() ) {
        return false;
    }
    string input = stmt;
    input.erase( std::remove( input.begin(), input.end(), ' ' ), input.end() );
    int ch = stmt.back();
    return (ch == ';');
}

bool terminated_word( const string& line, StmtStatus& status )
{
    string beg, end;
    if( status.type == StmtType::if_endif ) {
        beg = "if";
        end = "endif";
    }
    else if( status.type == StmtType::do_loop ) {
        beg = "do";
        end = "loop";
    }
    else {
        return true; // End search.
    }
    string stmt = compress_statement( line, status ) + '\n';

    string word;
    bool in_word = false;
    bool is_funct = false;
    for( auto ch : stmt ) {
        if( in_word ) {
            if( isalnum( ch ) || ch == '_' || ch == ':' ) {
                word += ch;
            }
            else {
                if( is_funct ) {
                    is_funct = false;
                }
                else if( word == beg ) {
                    status.word_cnt++;
                }
                else if( word == end ) {
                    if( status.word_cnt > 1 ) {
                        --status.word_cnt;
                    }
                    else {
                        return true;
                    }
                }
                in_word = false;
            }
        }
        else if( isascii( ch ) && (isalpha( ch ) || ch == '_' || ch == ':') ) {
            in_word = true;
            word = ch;
        }
        else if( ch == '@' ) {
            is_funct = true;
        }
    }
    return false;
}

bool terminated( const string& stmt, StmtStatus& status )
{
    bool start_comment = false;
    bool end_comment = false;

    for( auto ch : stmt ) {
        if( start_comment ) {
            start_comment = false;
            if( ch == '/' ) {
                break; // Single line comment.
            }
            else if( ch == '*' ) {
                status.in_mcomment = true;
                continue;
            }
        }
        if( status.in_mcomment ) {
            if( end_comment ) {
                if( ch == '/' ) {
                    status.in_mcomment = false;
                }
            }
            end_comment = (ch == '*');
            continue;
        }

        if( status.in_quote ) {
            if( ch == '"' ) {
                status.in_quote = false;
            }
        }
        else if( ch == '}' ) {
            status.brace_cnt--;
            if( status.type == StmtType::curlybracket && status.brace_cnt == 0 ) {
                return true;
            }
        }
        else if( ch == ';' ) {
            if( status.type == StmtType::semicolon || status.type == StmtType::both ) {
                return true;
            }
        }
        else if( ch == '"' ) {
            status.in_quote = true;
        }
        else if( ch == '/' ) {
            start_comment = true;
        }
        else if( ch == '{' ) {
            status.brace_cnt++;
            if( status.type == StmtType::both ) {
                status.type = StmtType::curlybracket;
            }
        }
    }
    return false;
}

string get_statement( const string& start, StmtType type )
{
    StmtStatus status( type );
    string line = start;
    string statement;
    for( int lnum = 2;; lnum++ ) {
        line = left_trim( line );
        statement += line + "\n";
        if( line.empty() ) {
            break;
        }
        if( type == StmtType::if_endif || type == StmtType::do_loop ) {
            if( terminated_word( line, status ) ) {
                break;
            }
        }
        else {
            if( terminated( line, status ) ) {
                break;
            }
        }
        string prompt = std::to_string( lnum );
        while( prompt.size() < 4 ) {
            prompt += ".";
        }
        prompt += ":";
        std::cout << prompt << " ";
        std::getline( std::cin, line );
    }
    return statement;
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

    std::cout << "glcs" << g_title << g_copyright <<
        "Enter 'help' for more information.\n"
        "Enter 'end' to exit program.\n"
        "\n";


    // Run script files if given.
    for( size_t i = 0; i < filenames.size(); i++ ) {
        string script = read_file( filenames[i] );
        if( script.empty() ) {
            continue;
        }
        string response = hg::hic().run_script( script );
        if( !response.empty() ) {
            std::cout << response << "\n";
        }
    }

    for( ;;) {
        std::cout << "glcs: ";
        string cmnd;
        std::getline( std::cin, cmnd );
        string word, tail;
        word = get_first_word( cmnd, &tail, ' ' );

        if( word == "end" ) {
            break;
        }
        else if( word == "help" ) {
            do_help( tail );
            continue;
        }
        else if( word == "run" ) {
            cmnd = read_file( tail );
        }
        else if(
            word == "let" || word == "set" || word == "write"
            || word == "writeln" || word == "mark" || word == "clear"
            || word == "call" )
        {
            cmnd = get_statement( cmnd, StmtType::semicolon );
        }
        else if(
            word == "function" || word == "command" || word == "object" ||
            word == "scheme" || word == "grammar" || word == "lexicon" )
        {
            cmnd = get_statement( cmnd, StmtType::curlybracket );
        }
        else if( word == "format" ) {
            cmnd = get_statement( cmnd, StmtType::both );
        }
        else if( word == "if" ) {
            cmnd = get_statement( cmnd, StmtType::if_endif );
        }
        else if( word == "do" ) {
            cmnd = get_statement( cmnd, StmtType::do_loop );
        }
        else {
            if( !terminated_semicolon( cmnd ) && !cmnd.empty() ) {
                cmnd = "let answer = " + cmnd + "; write answer//*/\n;";
            }
        }
        string output = hg::hic().run_script( cmnd );
        if( output.size() ) {
            std::cout << output << "\n";
        }
    }
    
    hg::exit_hic();
    return 0;
}


// End of src/glcs/glsc.cpp
