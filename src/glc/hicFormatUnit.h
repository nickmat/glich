/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicFormatUnit.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     FormatUnit class to read and write as units.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     24th March 2023
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

#ifndef SRC_GLC_HICFORMATUNIT_H_GUARD
#define SRC_GLC_HICFORMATUNIT_H_GUARD

#include "hicFormat.h"

namespace glich {

    class Record;

    class FormatUnit : public Format
    {
    public:
        FormatUnit( const std::string& code, Grammar& gmr );

        FormatType get_format_type() const override { return FormatType::unit; };
        std::string get_text_output( Record& rec ) const override;
        RList string_to_rlist( const Base& base, const std::string& input, FunctionData* fdata = nullptr ) const override;
        bool set_input( Record& record, const std::string& input, Boundary rb ) const override;

    private:
        void set_field_by_unit( Record& record, const std::string& value, const std::string& unit ) const;
    };

}

#endif // SRC_GLC_HICFORMATUNIT_H_GUARD