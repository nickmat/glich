/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        test/glctest/glcsTestmain.h
 * Project:     glctest: Command line Glich Test Script Program.
 * Purpose:     Common header for glctest.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     7th February 2023
 * Copyright:   Copyright (c) 2023, Nick Matthews.
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

#ifndef TEST_GLCTEST_GLCSTESTMAIN_H_GUARD
#define TEST_GLCTEST_GLCSTESTMAIN_H_GUARD

#include <string>
#include <vector>

enum class CheckFile { none, file, dir };
CheckFile check_file( const std::string& name );

void get_filenames( std::vector<std::string>& vec, const std::string& path );
std::string get_module_name( const std::string& filename );

#endif // TEST_GLCTEST_GLCSTESTMAIN_H_GUARD