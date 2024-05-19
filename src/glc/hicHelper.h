/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        glc/glc/hicHelper.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Helpful functions and strucs for hics
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     17th February 2023
 * Copyright:   Copyright (c) 2023..2024, Nick Matthews.
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

#ifndef SRC_GLC_HICHELPER_H_GUARD
#define SRC_GLC_HICHELPER_H_GUARD

#include <glc/glcDefs.h>

namespace glich {

    Field str_to_dual2( Field dual1, const std::string& str );

    std::string make_key( const std::string& str );
    bool split_code( std::string* scheme, std::string* format, const std::string& str );

    enum InputFieldType {
        IFT_null, IFT_quest, IFT_number, IFT_dual1, IFT_dual2, IFT_lexicon
    };

    class Lexicon;

    struct InputField
    {
        InputField() : value( f_invalid ), type( IFT_null ), lexicon( nullptr ) {}

        Field value;
        InputFieldType type;
        Lexicon* lexicon;
    };

    typedef std::vector<InputField> InputFieldVec;

    enum class Boundary { None, Begin, End };

    struct LocaleData {
        LocaleData() : lat( 51.4772 ), lon( 0.0 ) {} // Default to Greenwich, England.

        double lat; // Degrees latitude from equator, positive North.
        double lon; // Degrees longtitude from Greenwich Meridian, positive East.
    };

    template < class T, class V >
    int find_in_vec( const T& item, const V& vec ) {
        for( size_t i = 0; i < vec.size(); i++ ) {
            if( item == vec[i] ) {
                return int( i );
            }
        }
        return -1;
    }
}

#endif // SRC_GLC_HICHELPER_H_GUARD
