/* ../../src/glc/hicLibScripts.cpp - File created by file2cpp 1.0.0 */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        lib/hics/hicScripts.f2c
 * Project:     Glich: Extendable Script Language.
 * Purpose:     file2cpp template file for hics library scripts.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     1st April 2023
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

 This template is used to create the src/glc/hicLibScripts.cpp file.

*/

#include "hicLibScripts.h"

glich::ScriptModule glich::hics_default_scripts[] = {
    { "hicLib",  /* hicLib.glcs */
 "mark \"__hics_lib\";\n"
 "set context hics;\n" },
    { "jdn",  /* jdn.glcs */
 "grammar d {\n"
 "name \"Day Number\";\n"
 "fields day;\n"
 "alias pseudo { ddddd day; }\n"
 "format d \"{day}\";\n"
 "preferred d;\n"
 "}\n"
 "scheme jdn {\n"
 "name \"Julian Day Number\";\n"
 "base jdn;\n"
 "grammar d;\n"
 "}\n" },
    { "week",  /* week.glcs */
 "lexicon w {\n"
 "name \"Weekday names Monday start\";\n"
 "fieldname wday;\n"
 "lang en;\n"
 "pseudo Weekday, Wday;\n"
 "tokens {\n"
 "1, \"Monday\", \"Mon\";\n"
 "2, \"Tuesday\", \"Tue\";\n"
 "3, \"Wednesday\", \"Wed\";\n"
 "4, \"Thursday\", \"Thur\";\n"
 "5, \"Friday\", \"Fri\";\n"
 "6, \"Saturday\", \"Sat\";\n"
 "7, \"Sunday\", \"Sun\";\n"
 "}\n"
 "}\n"
 "grammar jwn {\n"
 "name \"Julian Week Number (Monday)\";\n"
 "fields week day;\n"
 "lexicons w;\n"
 "alias lexicon { wday day; }\n"
 "alias pseudo { wwwww week; d day; }\n"
 "alias unit { d day; w week; }\n"
 "format wd \"{week}| {day}\";\n"
 "format wday \"{week}| {day:w:a}\";\n"
 "format \"wday+\", \"{week}| {day:w}\";\n"
 "preferred wday;\n"
 "}\n"
 "scheme jwn {\n"
 "name \"Julian Week Number\";\n"
 "base jwn;\n"
 "grammar jwn;\n"
 "style hidden;\n"
 "}\n"
 "lexicon ws {\n"
 "name \"Weekday names Sunday start\";\n"
 "fieldname wsday;\n"
 "lang en;\n"
 "pseudo Weekday, Wday;\n"
 "tokens {\n"
 "1, \"Sunday\", \"Sun\";\n"
 "2, \"Monday\", \"Mon\";\n"
 "3, \"Tuesday\", \"Tue\";\n"
 "4, \"Wednesday\", \"Wed\";\n"
 "5, \"Thursday\", \"Thur\";\n"
 "6, \"Friday\", \"Fri\";\n"
 "7, \"Saturday\", \"Sat\";\n"
 "}\n"
 "}\n"
 "grammar jwsn {\n"
 "name \"Julian Week Number (Sunday)\";\n"
 "fields week day;\n"
 "lexicons ws;\n"
 "alias lexicon { wsday day; }\n"
 "alias pseudo { wwwww week; d day; }\n"
 "alias unit { d day; w week; }\n"
 "format wd \"{week}| {day}\";\n"
 "format wday \"{week}| {day:ws:a}\";\n"
 "format \"wday+\", \"{week}| {day:ws}\";\n"
 "preferred wday;\n"
 "}\n"
 "scheme jwsn {\n"
 "name \"Julian Week (Sunday) Number\";\n"
 "base jwn sunday;\n"
 "grammar jwsn;\n"
 "style hidden;\n"
 "}\n" },
    { "julian",  /* julian.glcs */
 "lexicon m {\n"
 "name \"Month names\";\n"
 "fieldname month;\n"
 "lang en;\n"
 "pseudo Month, Mon;\n"
 "tokens {\n"
 "1, \"January\", \"Jan\";\n"
 "2, \"February\", \"Feb\";\n"
 "3, \"March\", \"Mar\";\n"
 "4, \"April\", \"Apr\";\n"
 "5, \"May\";\n"
 "6, \"June\", \"Jun\";\n"
 "7, \"July\", \"Jul\";\n"
 "8, \"August\", \"Aug\";\n"
 "9, \"September\", \"Sep\";\n"
 "10, \"October\", \"Oct\";\n"
 "11, \"November\", \"Nov\";\n"
 "12, \"December\", \"Dec\";\n"
 "}\n"
 "}\n"
 "grammar j {\n"
 "name \"Julian\";\n"
 "fields year month day;\n"
 "optional wday;\n"
 "lexicons m, w;\n"
 "alias pseudo { w wday; dd day; mm month; yyyy year; }\n"
 "alias unit { d day; m month; y year; }\n"
 "format dmy \"{day} |{month:m:a} |{year}\";\n"
 "format \"dmy+\" \"{day} |{month:m} |{year}\";\n"
 "format wdmy \"{wday:w:a} |{day} |{month:m:a} |{year}\";\n"
 "format \"wdmy+\" \"{wday:w} |{day} |{month:m} |{year}\";\n"
 "format out { output \"{day::os} |{month:m} |{year}\"; }\n"
 "format full { output \"{wday:w} |{day::os} |{month:m} |{year}\"; }\n"
 "format mdy \"{month:m:a} |{day}, |{year}\";\n"
 "format \"mdy+\" \"{month:m} |{day}, |{year}\";\n"
 "format wmdy \"{wday:w:a}, |{month:m:a} |{day}, |{year}\";\n"
 "format \"wmdy+\" \"{wday:w}, |{month:m} |{day}, |{year}\";\n"
 "format ymd \"{year}|:{month:m:a}|:{day}\";\n"
 "preferred dmy;\n"
 "}\n"
 "scheme j {\n"
 "name \"Julian\";\n"
 "base julian;\n"
 "grammar j;\n"
 "}\n" },
    { "julian_shift",  /* julian_shift.glcs */
 "grammar j_sh {\n"
 "name \"Julian shifted epoch\";\n"
 "fields year month day;\n"
 "calculated cyear;\n"
 "optional wday;\n"
 "rank cyear month day;\n"
 "lexicons m w;\n"
 "alias pseudo { w wday; dd day; mm month; yyyy cyear; h year; }\n"
 "alias unit { d day; m month; hy year; cy cyear; }\n"
 "use epoch;\n"
 "format dmy \"{day} |{month:m:a} |{cyear/year}\";\n"
 "format \"dmy+\" \"{day} |{month:m} |{cyear/year}\";\n"
 "format wdmy \"{wday:w:a} |{day} |{month:m:a} |{cyear/year}\";\n"
 "format \"wdmy+\" \"{wday:w} |{day} |{month:m} |{cyear/year}\";\n"
 "format out { output \"{day::os} |{month:m} |{cyear/year}\"; }\n"
 "format full { output \"{wday:w} |{day::os} |{month:m} |{cyear/year}\"; }\n"
 "format mdy \"{month:m:a} |{day}, |{cyear/year}\";\n"
 "format \"mdy+\" \"{month:m} |{day}, |{cyear/year}\";\n"
 "format wmdy \"{wday:w:a}, |{month:m:a} |{day}, |{cyear/year}\";\n"
 "format \"wmdy+\" \"{wday:w}, |{month:m} |{day}, |{cyear/year}\";\n"
 "format cdef {\n"
 "inout \"{cyear}| {month}| {day}\";\n"
 "style hidden;\n"
 "}\n"
 "preferred dmy;\n"
 "}\n"
 "scheme jn {\n"
 "name \"Julian Nativity\";\n"
 "base julian;\n"
 "epoch 1721417;\n"
 "grammar j_sh;\n"
 "}\n"
 "scheme ja {\n"
 "name \"Julian Annunciation Florence\";\n"
 "base julian;\n"
 "epoch 1721507;\n"
 "grammar j_sh;\n"
 "}\n"
 "scheme jap {\n"
 "name \"Julian Annunciation Pisa\";\n"
 "base julian;\n"
 "epoch 1721142;\n"
 "grammar j_sh;\n"
 "}\n" },
    { "eh",  /* eh.glcs */
 "scheme eh {\n"
 "name \"Julian Era Hispanica\";\n"
 "base julian \"year:-38\";\n"
 "grammar j;\n"
 "}\n" },
    { "gregorian",  /* gregorian.glcs */
 "grammar g {\n"
 "name \"Gregorian\";\n"
 "inherit j;\n"
 "format iso { rules iso8601 minus; }\n"
 "format iso_set { rules iso8601 dateset minus; }\n"
 "preferred dmy;\n"
 "}\n"
 "scheme g {\n"
 "name \"Gregorian\";\n"
 "base gregorian;\n"
 "grammar g;\n"
 "}\n"
 "set inout g;\n"
 "grammar gw {\n"
 "name \"Year Week Day\";\n"
 "fields year week day;\n"
 "lexicons w;\n"
 "alias field { wday week; }\n"
 "alias pseudo { d day; ww week; yyyy year; }\n"
 "alias unit { d day; w week; y year; }\n"
 "format ywd \"{year}| {week}| {day:w:a}\";\n"
 "format \"ywd+\", \"{year}| {week}| {day:w}\";\n"
 "format iso { rules iso8601 week minus; }\n"
 "preferred ywd;\n"
 "}\n"
 "scheme gw {\n"
 "name \"Gregorian ISO Week\";\n"
 "base isoweek;\n"
 "grammar gw;\n"
 "}\n"
 "grammar go {\n"
 "name \"Year Day\";\n"
 "fields year day;\n"
 "alias pseudo { ddd day; yyyy year; }\n"
 "alias unit { d day; y year; }\n"
 "format yd \"{year}| {day}\";\n"
 "format iso { rules iso8601 ordinal minus; }\n"
 "preferred yd;\n"
 "}\n"
 "scheme go {\n"
 "name \"Gregorian Ordinal\";\n"
 "base ordinal;\n"
 "grammar go;\n"
 "}\n" },
    { "jce",  /* jce.glcs */
 "lexicon ce {\n"
 "name \"Historic Era\";\n"
 "fieldname ce;\n"
 "lang en;\n"
 "pseudo CommonEra, CE;\n"
 "tokens {\n"
 "0, \"Before Common Era\", \"BCE\";\n"
 "1, \"Common Era\", \"CE\";\n"
 "}\n"
 "}\n"
 "grammar jce {\n"
 "name \"Julian Common Era\";\n"
 "fields year month day;\n"
 "calculated ce ceyear;\n"
 "optional wday;\n"
 "rank ce ceyear month day;\n"
 "lexicons m w ce;\n"
 "alias pseudo { w wday; dd day; mm month; yyyy ceyear; ce ce; }\n"
 "alias unit { d day; m month; y year; cy ceyear; }\n"
 "function calc_ceyear {\n"
 "ce = @if( year<1, 0, 1 );\n"
 "ceyear = @if( year<1, -year+1, year );\n"
 "result = this;\n"
 "}\n"
 "function calc_year {\n"
 "year = @if( ce=1, ceyear, -ceyear+1 );\n"
 "result = this;\n"
 "}\n"
 "use { calculate calc_ceyear; from:text calc_year; }\n"
 "format dmye \"{day} |{month:m:a} |{ceyear} |{ce:ce:a}\";\n"
 "format wdmye \"{wday:w:a} |{day} |{month:m:a} |{ceyear} |{ce:ce:a}\";\n"
 "format mdye \"{month:m:a} |{day}, |{ceyear} |{ce:ce:a}\";\n"
 "format wmdye \"{wday:w:a} |{month:m:a} |{day}, |{ceyear} |{ce:ce:a}\";\n"
 "format \"dmye+\" \"{day} |{month:m} |{ceyear} |{ce:ce}\";\n"
 "format \"wdmye+\" \"{wday:w} |{day} |{month:m} |{ceyear} |{ce:ce}\";\n"
 "format out { output \"{day::os} |{month:m} |{ceyear} |{ce:ce}\"; }\n"
 "format full { output \"{wday:w} |{day::os} |{month:m} |{ceyear} |{ce:ce}\"; }\n"
 "preferred dmye;\n"
 "}\n"
 "scheme jce {\n"
 "name \"Julian Common Era\";\n"
 "base julian;\n"
 "grammar jce;\n"
 "}\n" },
    { "ay",  /* ay.glcs */
 "lexicon jg {\n"
 "name \"Hybrid Julian and Gregorian scheme names\";\n"
 "fieldname scheme;\n"
 "lang en;\n"
 "pseudo Scheme sch;\n"
 "tokens {\n"
 "0, \"Julian\", \"j\";\n"
 "1, \"Gregorian\", \"g\";\n"
 "}\n"
 "}\n"
 "grammar jg {\n"
 "name \"Hybrid Julian and Gregorian\";\n"
 "fields scheme year month day;\n"
 "rank year month day;\n"
 "lexicons m jg;\n"
 "alias pseudo { dd day; mm month; yyyy year; s scheme; }\n"
 "alias unit { d day; m month; y year; s scheme; }\n"
 "format dmy \"{day} |{month:m:a} |{year}\";\n"
 "format \"dmy+\" \"{day} |{month:m} |{year}\";\n"
 "format mdy \"{month:m:a} |{day}, |{year}\";\n"
 "format \"mdy+\" \"{month:m} |{day}, |{year}\";\n"
 "format dmys \"{day} |{month:m:a} |{year} |{scheme:jg:a}\";\n"
 "format \"dmys+\" \"{day} |{month:m} |{year} |{scheme:jg}\";\n"
 "format mdys \"{month:m:a} |{day}, |{year} |{scheme:jg:a}\";\n"
 "format \"mdys+\" \"{month:m} |{day}, |{year} |{scheme:jg}\";\n"
 "preferred dmy;\n"
 "}\n"
 "scheme ay {\n"
 "name \"Astronomical Year\";\n"
 "base hybrid {\n"
 "fields scheme year month day;\n"
 "scheme 0 { base julian; }\n"
 "change 2299161;\n"
 "scheme 1 { base gregorian; }\n"
 "}\n"
 "grammar jg;\n"
 "}\n" },
    { "hebrew",  /* hebrew.glcs */
 "lexicon hm {\n"
 "name \"Month names\";\n"
 "fieldname month;\n"
 "lang en;\n"
 "pseudo Month, Mon;\n"
 "tokens {\n"
 "1, \"Nissan\", \"Nsn\", \"Nisan\";\n"
 "2, \"Iyar\", \"Iyr\", \"Iyyar\";\n"
 "3, \"Sivan\", \"Svn\";\n"
 "4, \"Tammuz\", \"Tmz\", \"Tamaz\";\n"
 "5, \"Av\", \"Aav\";\n"
 "6, \"Elul\", \"Ell\";\n"
 "7, \"Tishrei\", \"Tsh\", \"Tishri\";\n"
 "8, \"Cheshvan\", \"Csh\", \"Marheshvan\", \"Heshvan\";\n"
 "9, \"Kislev\", \"Ksl\";\n"
 "10, \"Tevet\", \"Tvt\";\n"
 "11, \"Shevat\", \"Shv\", \"Shvat\";\n"
 "12, \"Adar\", \"Adr\";\n"
 "13, \"Adar II\", \"Ads\";\n"
 "}\n"
 "}\n"
 "lexicon hcm {\n"
 "name \"Complete Month names\";\n"
 "fieldname cmonth;\n"
 "inherit hm;\n"
 "tokens {\n"
 "14, \"Adar I\", \"Adf\";\n"
 "}\n"
 "}\n"
 "lexicon hw {\n"
 "name \"Weekday names\";\n"
 "fieldname wsday;\n"
 "lang en;\n"
 "pseudo Weekday, WDay;\n"
 "tokens {\n"
 "1, \"Yom Rishon\", \"Rish\";\n"
 "2, \"Yom Sheni\", \"Shen\";\n"
 "3, \"Yom Shelishi\", \"Shel\";\n"
 "4, \"Yom Revi'i\", \"Revi\";\n"
 "5, \"Yom Hamishi\", \"Hami\";\n"
 "6, \"Yom Shishi\", \"Shis\";\n"
 "7, \"Yom Shabbat\", \"Shab\";\n"
 "}\n"
 "}\n"
 "grammar h {\n"
 "name \"Hebrew\";\n"
 "fields year month day;\n"
 "calculated cmonth;\n"
 "optional wsday;\n"
 "rank year cmonth day;\n"
 "lexicons hm, hcm, hw;\n"
 "alias pseudo { w wday; dd day; mm month; yyyy year; }\n"
 "alias unit { d day; m month; y year; }\n"
 "function calc_cmonth {\n"
 "cmonth = @if( month=12 and @leapyear.hebrew(year), 14, month );\n"
 "result = this;\n"
 "}\n"
 "function calc_month {\n"
 "month = @if( cmonth=14, 12, cmonth );\n"
 "result = this;\n"
 "}\n"
 "use { calculate calc_cmonth; from:text calc_month; }\n"
 "format dmy_a \"{day} |{cmonth:hcm:a} |{year}\";\n"
 "format dmy \"{day} |{cmonth:hcm} |{year}\";\n"
 "format \"dmy-\" {\n"
 "inout \"{day} |{month} |{year}\";\n"
 "rank year month day;\n"
 "use:in calc_cmonth;\n"
 "}\n"
 "format wdmy \"{wsday:hw} |{day} |{cmonth:hcm} |{year}\";\n"
 "format ymd \"{year}| {cmonth:hcm}| {day}\";\n"
 "format \"ymd-\" {\n"
 "inout \"{year}| {month}| {day}\";\n"
 "rank year month day;\n"
 "use:in calc_cmonth;\n"
 "}\n"
 "format gedcom {\n"
 "inout \"{day} |{month:hm:a:u} |{year}\";\n"
 "rank year month day;\n"
 "use:in calc_cmonth;\n"
 "}\n"
 "preferred dmy;\n"
 "}\n"
 "scheme h {\n"
 "name \"Hebrew\";\n"
 "base hebrew;\n"
 "grammar h;\n"
 "}\n" },
};

size_t glich::hics_default_scripts_size =
    sizeof(glich::hics_default_scripts) / sizeof(glich::ScriptModule);

// End of lib/hics/hicScripts.f2c file
