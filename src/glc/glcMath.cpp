/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/glc/glcMath.cpp
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Miscellaneous math functions.
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     6th February 2023
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

#include "glcMath.h"

#include <cassert>
#include <cmath>

using namespace glich;

// Use our own value of pi for consistancy
double const glich::cal_pi = 3.14159265358979323846;

// Integer function to return floor( a / b )
Num glich::div_f( Num a, Num b )
{
    assert( b != 0 );
    Num q = a / b;
    if( ( a >= 0 && b > 0 ) || ( a < 0 && b < 0 ) || a % b == 0 ) {
        return q;
    }
    return q - 1;
}

// Integer function to return (a modulo b) that has the same sign as b.  
// Function based on Floored division.
Num glich::mod_f( Num a, Num b )
{
    assert( b != 0 );
    Num r = a % b;
    if( ( r > 0 && b < 0 ) || ( r < 0 && b > 0 ) ) {
        r += b;
    }
    return r;
}

// Integer function to return Euclidean division
Num glich::div_e( Num a, Num b )
{
    assert( b != 0 );
    Num q = a / b;
    if( a % b < 0 ) {
        if( b > 0 ) {
            q -= 1;
        }
        else {
            q += 1;
        }
    }
    return q;
}

// Field integer function to return Euclidean division
Field glich::fdiv_e( Field a, Field b )
{
    assert( b != 0 );
    assert( a >= f_minimum || a <= f_maximum );
    assert( b >= f_minimum || b <= f_maximum );
    Field q = a / b;
    if( a % b < 0 ) {
        if( b > 0 ) {
            q -= 1;
        }
        else {
            q += 1;
        }
    }
    return q;
}

// Integer function to return positive value for (a modulo b).
// Function based on Euclidean division.
Num glich::mod_e( Num a, Num b )
{
    assert( b != 0 );
    Num r = a % b;
    if( r < 0 ) {
        if( b > 0 ) {
            r += b;
        }
        else {
            r -= b;
        }
    }
    return r;
}

// Field integer function to return positive value for (a modulo b).
// Function based on Euclidean division.
Field glich::fmod_e( Field a, Field b )
{
    assert( b != 0 );
    assert( a >= f_minimum || a <= f_maximum );
    assert( b >= f_minimum || b <= f_maximum );
    Field r = a % b;
    if( r < 0 ) {
        if( b > 0 ) {
            r += b;
        }
        else {
            r -= b;
        }
    }
    return r;
}

Num glich::round_f( double r )
{
    return Num( r > 0.0 ? r + 0.5 : r - 0.5 );
}

Num glich::floor_f( double r )
{
    return Num( floor( r ) );
}


int glich::cal_signum( double n )
{
    if( n > 0 ) return 1;
    if( n < 0 ) return -1;
    return 0;
}

double glich::fmod_f( double x, double y )
{
    return x - y * floor( x / y ); 
} 

double glich::fmod_r( double x, double y )
{
    double f = floor( x / y );
    double r = x - y * f;
    if( r > y / 2 ) {
        return r - y;
    }
    return r;
}


RList glich::set_operation( SetOp op, const RList& left, const RList& right )
{
    bool lon = false, ron = false;
    Field lpos = f_maximum, rpos = f_maximum, apos = f_minimum;
    size_t li = 0, ri = 0;
    bool aon = false, paon = false;

    if( left.size() ) {
        lpos = f_minimum;
        lon = (lpos == left[0].m_beg);
    }
    if( right.size() ) {
        rpos = f_minimum;
        ron = (rpos == right[0].m_beg);
    }

    RList answer;
    Range range;
    while( apos != f_end && apos != f_invalid2 ) {
        apos = std::min( lpos, rpos ); // Next position.
        // We want to know if the left, right ranges are on or off
        // at the current answer position. If the left or right position
        // is the same as the answer postion, we can just use the on/off flag.
        // Otherwise, we want the previous setting, which is the opposite
        // of the current on/off setting.
        bool lval = (apos == lpos) ? lon : !lon;
        bool rval = (apos == rpos) ? ron : !ron;
        switch( op )
        {
        case SetOp::Union:
            aon = (lval || rval);
            break;
        case SetOp::Inter:
            aon = (lval && rval);
            break;
        case SetOp::RelComp:
            aon = (lval && !rval);
            break;
        case SetOp::SymDif:
            aon = (lval != rval);
            break;
        default:
            assert( false ); // How did we get here?
        }
        if( aon != paon ) {         // we need make an answer entry
            if( aon ) {            // start of a new range
                range.m_beg = apos;
            }
            else {               // completes a range
                range.m_end = apos - 1;
                answer.push_back( range );
            }
            paon = aon;
        }
        // move on left
        if( lpos == apos && lpos != f_invalid2 ) {
            if( li < left.size() ) {
                if( lon ) {
                    lpos = left[li].m_end;
                    assert( lpos != f_end );
                    lpos++;
                    li++;
                }
                else { // lon == false
                    lpos = left[li].m_beg;
                }
            }
            else {
                lpos = f_invalid2;
            }
            lon = !lon;
        }
        // move on right
        if( rpos == apos && rpos != f_invalid2 ) {
            if( ri < right.size() ) {
                if( ron ) {
                    rpos = right[ri].m_end;
                    assert( rpos != f_end );
                    rpos++;
                    ri++;
                }
                else { // ron == false
                    rpos = right[ri].m_beg;
                }
            }
            else {
                rpos = f_invalid2;
            }
            ron = !ron;
        }
    }
    return answer;
}

RList glich::op_set_complement( const RList& input )
{
    // Note, we assume the complement of the range infinite is -infinite and vice versa.
    // This may not be correct (it should be invalid?) but it should be safe.
    RList answer;
    Range range;

    // Check for empty set.
    if( input.empty() ) {
        range.m_beg = f_minimum;
        range.m_end = f_maximum;
        answer.push_back( range );
        return answer; // Return the full set.
    }

    int i = 0;
    if( input[0].m_beg != f_minimum ) {
        range.m_beg = f_minimum;
        range.m_end = (input[0].m_end == f_maximum) ? f_minimum : input[0].m_beg - 1;
        answer.push_back( range );
    }
    while( i < (int) input.size() - 1 ) {
        range.m_beg = input[i].m_end + 1;
        i++;
        range.m_end = input[i].m_beg - 1;
        answer.push_back( range );
    }
    if( input[i].m_end != f_maximum ) {
        range.m_beg = (input[i].m_beg == f_minimum) ? f_maximum : input[i].m_end + 1;
        range.m_end = f_maximum;
        answer.push_back( range );
    }
    return answer;
}

RList glich::op_set_well_order( const RList& rlist )
{
    RList result;
    if( rlist.size() > 0 ) {
        result.push_back( rlist[0] );
        op_normalise_range( &result[0] );
        if( rlist.size() > 1 ) {
            RList right( 1 );
            for( size_t i = 1; i < rlist.size(); i++ ) {
                right[0] = rlist[i];
                op_normalise_range( &right[0] );
                result = set_operation( SetOp::Union, result, right );
            }
        }
    }
    return result;
}

Range glich::enclosing_range( const RList& rlist )
{
    if( rlist.empty() ) {
        return Range( f_invalid, f_invalid );
    }
    Range range;
    range.m_beg = rlist[0].m_beg;
    range.m_end = rlist[rlist.size() - 1].m_end;
    return range;
}

double glich::add_float_field( double dbl, Field fld )
{
    if( fld > f_maximum || fld < f_minimum || isnan( dbl ) ) {
        // This incudes f_invalid.
        return std::numeric_limits<double>::quiet_NaN();
    }
    if( fld == f_maximum ) {
        if( -dbl == std::numeric_limits<double>::infinity() ) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        else {
            return std::numeric_limits<double>::infinity();
        }
    }
    if( fld == f_minimum ) {
        if( dbl == std::numeric_limits<double>::infinity() ) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        else {
            return -std::numeric_limits<double>::infinity();
        }
    }
    return dbl + static_cast<double>(fld);
}

Field glich::add_field( Field left, Field right )
{
    if( left == f_invalid || right == f_invalid ) {
        return f_invalid;
    }
    if( left == f_minimum ) {
        return (right == f_maximum) ? f_invalid : f_minimum;
    }
    if( left == f_maximum ) {
        return (right == f_minimum) ? f_invalid : f_maximum;
    }
    if( right == f_maximum || right == f_minimum ) {
        return right;
    }
    LongField lf = static_cast<LongField>(left) + static_cast<LongField>(right);
    if( lf <= static_cast<LongField>(f_minimum) ) {
        return f_invalid;
    }
    if( lf >= static_cast<LongField>(f_maximum) ) {
        return f_invalid;
    }
    return static_cast<Field>(lf);
}

Range glich::add_range( Range rng, Field fld, bool& success )
{
    success = true;
    Field beg = add_field( rng.m_beg, fld );
    Field end = add_field( rng.m_end, fld );
    if( beg == f_invalid || end == f_invalid ) {
        success = false;
    }
    return Range( beg, end );
}

RList glich::add_rlist( const RList& rlist, Field fld, bool& success )
{
    success = true;
    bool step = true;
    RList result;
    for( Range range : rlist ) {
        Range r = add_range( range, fld, step );
        if( !step ) {
            success = false;
        }
        result.push_back( r );
    }
    return result;
}

double glich::mult_float_field( double dbl, Field fld )
{
    if( fld > f_maximum || fld < f_minimum || isnan( dbl ) ) {
        // This incudes f_invalid.
        return std::numeric_limits<double>::quiet_NaN();
    }
    if( fld == f_maximum ) {
        if( -dbl == std::numeric_limits<double>::infinity() ) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        else {
            return std::numeric_limits<double>::infinity();
        }
    }
    if( fld == f_minimum ) {
        if( dbl == std::numeric_limits<double>::infinity() ) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        else {
            return -std::numeric_limits<double>::infinity();
        }
    }
    return dbl * static_cast<double>(fld);
}

Field glich::mult_field( Field left, Field right )
{
    // Checks for f_invalid
    if( left == f_invalid || right == f_invalid ) {
        return f_invalid;
    }
    if( left == f_minimum || right == f_minimum ) {
        if( left == f_maximum || right == f_maximum ) {
            return f_invalid; // +infinity * -infinity = invalid
        }
        if( left == 0 || right == 0 ) {
            return f_invalid; // -infinity * 0 = invalid
        }
        return f_minimum;
    }
    if( left == f_maximum || right == f_maximum ) {
        if( left == 0 || right == 0 ) {
            return f_invalid; // +infinity * 0 = invalid
        }
        return f_maximum;
    }

    // Checks for overflow.
    LongField lf = static_cast<LongField>(left) * static_cast<LongField>(right);
    if( lf <= static_cast<LongField>(f_minimum) ) {
        return f_invalid;
    }
    if( lf >= static_cast<LongField>(f_maximum) ) {
        return f_invalid;
    }
    return static_cast<Field>(lf);
}

// End of src/glc/glcMath.cpp