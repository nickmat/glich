/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        test/gu/guAPI.cpp
 * Project:     glcUnit: Unit test program for the Glich Hics library.
 * Purpose:     Test general Glich API functions.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     17th August 2023
 * Copyright:   Copyright (c) 2023..2026, Nick Matthews.
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

#include <catch2/catch.hpp>

#include "guMain.h"

using namespace glich;
using std::string;

TEST_CASE( "Test Version", "[Version]" )
{
    string ver = g_glc->version();
    string ver_num = ver.substr( 0, 5 );
    REQUIRE( ver_num == "0.3.0" );
}

TEST_CASE( "Test get_scheme_list", "[get_scheme_list]" )
{
    SchemeList schemes = g_glc->get_scheme_list( Visibility::all );
    size_t hidden_size = schemes.size();
    size_t index = 0;
    while( index < hidden_size && schemes[index].code != "g" ) index++;
    REQUIRE( index < hidden_size );
    SchemeData& data = schemes[index];
    REQUIRE( data.name == "Gregorian" );

    index = 0;
    while( index < hidden_size && schemes[index].code != "ja" ) index++;
    REQUIRE( index < hidden_size );
    data = schemes[index];
    REQUIRE( data.name == "Julian Annunciation Florence" );

    index = 0;
    while( index < hidden_size && schemes[index].code != "jwn" ) index++;
    REQUIRE( index < hidden_size );
    data = schemes[index];
    REQUIRE( data.name == "Julian Week Number" );

    schemes = g_glc->get_scheme_list( Visibility::visible );
    size_t default_size = schemes.size();
    REQUIRE( default_size < hidden_size );
    index = 0;
    while( index < default_size && schemes[index].code != "g" ) index++;
    REQUIRE( index < default_size );
    index = 0;
    while( index < default_size && schemes[index].code != "ja" ) index++;
    REQUIRE( index < default_size );
    index = 0;
    while( index < default_size && schemes[index].code != "jwn" ) index++;
    REQUIRE( index == default_size );
}

TEST_CASE( "Test date_phrase_to_rlist", "[date_phrase_to_rlist]" )
{
    RList expect = { { 2460176, 2460176 } };
    RList rlist = g_glc->date_phrase_to_rlist( "2460176", "jdn" );
    REQUIRE( rlist == expect );
    rlist = g_glc->date_phrase_to_rlist( "aug19,2023", "g:mdy");
    REQUIRE( rlist == expect );
    rlist = g_glc->date_phrase_to_rlist( "19aug2023" );
    REQUIRE( rlist == expect );
}

TEST_CASE( "Test date_phrase_to_text", "[date_phrase_to_text]" )
{
    string expect = "Aug 19, 2023";
    string text = g_glc->date_phrase_to_text( "2460176", "jdn", "g:mdy" );
    REQUIRE( text == expect );
    expect = "2460176";
    text = g_glc->date_phrase_to_text( "aug19,2023", ":mdy", "jdn" );
    REQUIRE( text == expect );
}

TEST_CASE( "Test rlist_to_text", "[rlist_to_text]" )
{
    RList rlist = { { 2460176, 2460176 } };
    string text = g_glc->rlist_to_text( rlist, "jdn" );
    REQUIRE( text == "2460176" );
    text = g_glc->rlist_to_text( rlist, "g:mdy" );
    REQUIRE( text == "Aug 19, 2023" );
    text = g_glc->rlist_to_text( rlist );
    REQUIRE( text == "19 Aug 2023" );
    text = g_glc->rlist_to_text( rlist, "jap:dmy" );
    REQUIRE( text == "6 Aug 2024/3" );
}

TEST_CASE( "Test range_to_text", "[range_to_text]" )
{
    Range range = { 2460176, 2460176 };
    string text = g_glc->range_to_text( range, "jdn" );
    REQUIRE( text == "2460176" );
    text = g_glc->range_to_text( range, "g:mdy" );
    REQUIRE( text == "Aug 19, 2023" );
    text = g_glc->range_to_text( range );
    REQUIRE( text == "19 Aug 2023" );
}

TEST_CASE( "Test field_to_text", "[field_to_text]" )
{
    Field field = 2460176;
    string text = g_glc->field_to_text( field, "jdn" );
    REQUIRE( text == "2460176" );
    text = g_glc->field_to_text( field, "g:mdy" );
    REQUIRE( text == "Aug 19, 2023" );
    text = g_glc->field_to_text( field );
    REQUIRE( text == "19 Aug 2023" );
}

TEST_CASE( "Test text_to_rlist", "[text_to_rlist]" )
{
    RList expect = { { 2460176, 2460176 } };
    RList result = g_glc->text_to_rlist( "2460176", "jdn" );
    REQUIRE( result.size() == 1 );
    REQUIRE( result[0] == expect[0] );
    result = g_glc->text_to_rlist( "aug19,2023", "g:mdy" );
    REQUIRE( result.size() == 1 );
    REQUIRE( result[0] == expect[0] );
    result = g_glc->text_to_rlist( "19aug2023" );
    REQUIRE( result.size() == 1 );
    REQUIRE( result[0] == expect[0] );
}

TEST_CASE( "Test text_to_range", "[text_to_range]" )
{
    Range expect = { 2460176, 2460176 };
    Range result = g_glc->text_to_range( "2460176", "jdn" );
    REQUIRE( result == expect );
    result = g_glc->text_to_range( "aug19,2023", "g:mdy" );
    REQUIRE( result == expect );
    result = g_glc->text_to_range( "19aug2023" );
    REQUIRE( result == expect );
}

TEST_CASE( "Test text_to_field", "[text_to_field]" )
{
    Field expect = 2460176;
    Field result = g_glc->text_to_field( "2460176", "jdn" );
    REQUIRE( result == expect );
    result = g_glc->text_to_field( "aug19,2023", "g:mdy" );
    REQUIRE( result == expect );
    result = g_glc->text_to_field( "19aug2023" );
    REQUIRE( result == expect );
}

TEST_CASE( "Test get_glc_data", "[get_glc_data]" )
{
    string script =
        "mark  test;\n"
        "let vnumber = 123;\n"
        "let vstring = \"text\";\n"
        ;
    g_glc->run_script( script, "get_glc_data" );
    GlcMarkDataVec marks = g_glc->get_glc_data();
    int cnt = 0;
    for( auto& mark : marks ) {
        if( mark.name == "test" ) {
            for( auto& item : mark.var ) {
                if( item.name == "vnumber" ) {
                    REQUIRE( item.value == "123" );
                    cnt++;
                }
                else if( item.name == "vstring" ) {
                    REQUIRE( item.value == "text" );
                    cnt++;
                }
            }
        }
    }
    REQUIRE( cnt == 2 );
}

// End of test/gu/guAPI.cpp file.
