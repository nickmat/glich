/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/hicAtFunction.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Impliment built-in hics functions.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     11th May 2025
 * Copyright:   Copyright (c) 2025, Nick Matthews.
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

#ifndef SRC_HIC_HICATFUNCTION_H
#define SRC_HIC_HICATFUNCTION_H

#include "glcValue.h"

namespace glich {

    class Scheme;

    SValue hic_at_date( const StdStrVec& quals, const SValueVec& args, std::ostream& outs );
    SValue hic_at_text( const StdStrVec& quals, const SValueVec& args );
    SValue hic_at_scheme( const StdStrVec& quals, const SValueVec& args, std::ostream& outs );
    SValue hic_at_element( const StdStrVec& quals, const SValueVec& args );
    SValue hic_at_phrase( const StdStrVec& quals, const SValueVec& args );
    SValue hic_at_leapyear( const StdStrVec& quals, const SValueVec& args );
    SValue hic_at_easter( const StdStrVec& quals, const SValueVec& args );
    SValue hic_at_sch_list();
    SValue hic_at_sch_object( const StdStrVec& quals );
    SValue hic_at_fmt_object( const StdStrVec& quals );
    SValue hic_at_age( const StdStrVec& quals, const SValueVec& args, std::ostream& outs );
    SValue hic_at_dob( const StdStrVec& quals, const SValueVec& args, std::ostream& outs );

    // Scheme object functions.
    SValue hic_sch_at_pseudo_in( const Scheme* sch, const SValueVec& args );
    SValue hic_sch_at_pseudo_out( const Scheme* sch, const SValueVec& args );
    SValue hic_sch_at_normalise( const Scheme* sch, const StdStrVec& qual, const SValue& left );
}

#endif // SRC_HIC_HICATFUNCTION_H   
