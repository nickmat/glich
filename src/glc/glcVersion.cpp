/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        include/glc/glcVersion.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Library version number.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     17th August 2023
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

#include "glcVersion.h"

#define VERSION_NUM      "0.3.0"

#if defined(NDEBUG) || !defined(_DEBUG)
#define VERSION_CONFIG   ""
#else
#define VERSION_CONFIG   " debug"
#endif

using namespace glich;

/*! A string containing the current full version number.
 */
const char* glich::glc_version = VERSION_NUM VERSION_CONFIG;

/*****************************************************************************

 Version history.
 19sep13  v0.1.x  Library created and developed with The Family Pack project.

 11jan14  v0.2.0  Library modified for Programmable Calendric Calculator.

 25nov14  v0.2.1  Library renamed Cal library. Home project renamed HistoryCal. 

 26nov14  v0.2.2  Minor changes to source for gnu compile.

  6dec14  v0.2.3  Typos corrected. Keyword "today" added.

 16mar15  v0.2.4  Handling of UTF8 improved. Add French Republician calendar.
                  Add Hebrew calendar.

 24may15  v0.2.5  Add Islamic tabular calendars. Add Scottish hybrid and
                  regnal calendars. Improve format system.

 18nov15  v0.2.6  Add optional fields to records and allow field ranking.
                  Get liturgical calendars working.
                  Add "jce", "gce" and "ay" default calendars.
                  Add "if/endif" and "do/loop" statements to script language.

 26jun16  v0.2.7  Added ISO Week and Ordinal calendar schemes.
                  Improve "mark" and "clear" script statements
                  Replace the SchemeFormats struct with FormatInfo struct.
                  Script, the amount of quoting required has been reduced.
                  Script, cast operators improved.
                  Support for ISO:8601 and some EDTF formatting added.
                  Other changes, see docs/cal-lib-changes.txt

  2mar21  v0.2.8  Add Chinese calendar. Other changes, not released.

  active  v0.3.0  Renamed Glich with Hics extension. Major rewrite.

*****************************************************************************/

// End of src/cal/calversion.cpp file
