/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/hic/hicJdn.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Jdn (Julian Day Number) calendar source.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     6th November 2024
 * Copyright:   Copyright (c) 2024, Nick Matthews.
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

#include "hicJdn.h"

using namespace glich;
using std::string;


Jdn::Jdn( StdStrVec data )
    : m_day_offset( 0 ), Base( data, 1 )
{
    m_fieldnames = { "day" };
    for( const string& word : data ) {
        cal_data( word );
    }
}

void glich::Jdn::cal_data( const std::string& word )
{
    string code, tail;
    split_code( &code, &tail, word );
    if( code == "epoch" ) {
        m_day_offset = str_to_field( tail );
    }
    else {
        set_data( word );
    }
}

// End of src/hic/hicJdn.cpp file
