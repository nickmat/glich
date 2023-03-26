/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicBase.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Hics Base class, used as base for all calendar calulations.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     17th March 2023
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

#ifndef GLC_HICBASE_H_GUARD
#define GLC_HICBASE_H_GUARD

#include <glc/hicDefs.h>
#include "hicHelper.h"

namespace glich {

    class FormatText;

    typedef std::map<int,XRefVec> XRefSet;
    typedef std::map<std::string,XRefSet> XRefMap;

    enum {
        YMD_year, YMD_month, YMD_day
    };
    enum OptFieldID {
        OFID_NULL,
        OFID_wday,      // 7 Day week Mon=1 (1 to 7)
        OFID_wsday,     // 7 Day week Sun=1 (1 to 7)
        OFID_jwn,       // Julian Week Number, week count from jdn 0.
        OFID_dayinyear, // Day in year (1 to about 366, dep. on scheme)
        OFID_unshift,   // Value before being shifted, (year or day)
        OFID_repeated,  // Where dates can occasionally be repeated ie Easter. (0 or 1).
        //
        OFID_MAX
    };

    class Base
    {
    public:
        Base( const std::string& data = std::string() );
        virtual ~Base();

        // Set initial data from string.
        virtual void set_data( const std::string& data );

        Format* get_format( const std::string& fcode ) const;
        bool attach_grammar( Grammar* gmr );

        // Return the number of Required Fields.
        virtual size_t required_size() const = 0;
        // Return the number of of all Fields including optional and calculated.
        size_t record_size() const { return m_fieldnames.size(); }
        // Get list of fieldnames in default order.
        StdStrVec get_fieldnames() const { return m_fieldnames; }
        std::string get_fieldname( size_t index ) const { return m_fieldnames[index]; }

        // Converts the Field's into a jdn and returns it.
        virtual Field get_jdn( const FieldVec& fields ) const = 0;

        // Converts the given jdn into the Records' Fields.
        virtual FieldVec get_fields( Field jdn ) const = 0;

    protected:
        StdStrVec m_fieldnames; // This is both required and optional.
        LocaleData m_locale;
        Grammar* m_grammar;
    };

}

#endif // GLC_HICBASE_H_GUARD
