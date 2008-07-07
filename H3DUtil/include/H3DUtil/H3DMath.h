//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004-2007, SenseGraphics AB
//
//    This file is part of H3DUtil.
//
//    H3DUtil is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    H3DUtil is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with H3DUtil; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//    A commercial license is also available. Please contact us at 
//    www.sensegraphics.com for more information.
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __H3DMATH_H__
#define __H3DMATH_H__

#include <H3DUtil/Exception.h>
#include <math.h>

namespace H3DUtil {

  /// Namespace containing various useful constants.
  namespace Constants {
    /// Pi.
    static const double pi = 3.1415926535897932384626433832795;
  }

  /// \ingroup H3DUtilClasses
  /// \defgroup H3DUtilMath Math functions
  /// Different functions used for mathematics.

  /// \ingroup H3DUtilMath
  /// Determines if the given integer is a power of two or not.
  inline bool isPowerOfTwo( unsigned int i ) {
    return (((i - 1) & i) == 0);
  }

  /// \ingroup H3DUtilMath
  /// If i is a power of two, i is returned. Otherwise the first
  /// power of two greater that i is returned.
  inline unsigned int nextPowerOfTwo( unsigned int i ) {
    i--;
    i |= i >> 16;
    i |= i >> 8;
    i |= i >> 4;
    i |= i >> 2;
    i |= i >> 1;
    i++;
    return i;
  }

  /// \ingroup H3DUtilMath
  /// Returns the absolute value of f. 
  template< class F >
  inline F H3DAbs( F f ) {
    return fabs( f );
  }

  /// \ingroup H3DUtilMath
  /// Returns the square root of f. 
  template< class F >
  inline F H3DSqrt( F f ) {
    return sqrt( f );
  }

  /// \ingroup H3DUtilMath
  /// Returns the exponential function of f. 
  template< class F >
  inline F H3DExp( F f ) {
    return exp( f );
  }

  /// \ingroup H3DUtilMath
  /// Returns the natural logarithm of f. 
  template< class F >
  inline F H3DLog( F f ) {
    return log( f );
  }

  /// \ingroup H3DUtilMath
  /// Returns the base 10 logarithm of f. 
  template< class F >
  inline F H3DLog10( F f ) {
    return log10( f );
  }

  /// \ingroup H3DUtilMath
  /// Returns f raised to the power of t. 
  template< class F, class T >
  inline F H3DPow( F f, T t ) {
    return pow( f, t );
  }

  /// \ingroup H3DUtilMath
  /// Returns f raised to the power of t. 
  inline double H3DPow( int f, int t ) {
    return pow( (double)f, (double)t );
  }
  
  /// \ingroup H3DUtilMath
  /// Returns the maximum value of a and b.
  template< class A >
  inline A H3DMax( const A &a, const A &b  ) {
    return max( a, b );
  }
    
  /// \ingroup H3DUtilMath
  /// Returns the minimum value of a and b.
  template< class A >
  inline A H3DMin( const A &a, const A &b  ) {
    return min( a, b );
  }

  /// \ingroup H3DUtilMath
  /// Returns the cosine of d.
  template< class F >
  inline F H3DCos( F d ) {
    return cos( d );
  }

  /// \ingroup H3DUtilMath
  /// Returns the hyperbolic cosine of d.
  template< class F >
  inline F H3DCosh( F d ) {
    return cosh( d );
  }

  /// \ingroup H3DUtilMath
  /// Returns the arccosine of d.  
  template< class F >
  inline F H3DAcos( F d ) {
    return acos( d );
  }

  /// \ingroup H3DUtilMath
  /// Returns the sine of d.
  template< class F >
  inline  F H3DSin( F d ) {
    return sin( d );
  }

  /// \ingroup H3DUtilMath
  /// Returns the hyperbolic sine of d.
  template< class F >
  inline F H3DSinh( F d ) {
    return sinh( d );
  }

  /// \ingroup H3DUtilMath
  /// Returns the arcsine of d.  
  template< class F >
  inline F H3DAsin( F d ) {
    return asin( d );
  }

  /// \ingroup H3DUtilMath
  /// Returns the tangent of d.  
  template< class F >
  inline F H3DTan( F d ) {
    return tan( d );
  }

  /// \ingroup H3DUtilMath
  /// Returns the hyperbolic tangent of d.  
  template< class F >
  inline F H3DTanh( F d ) {
    return tanh( d );
  }

  /// \ingroup H3DUtilMath
  /// Returns the arctangent of d in the range -pi/2 to pi/2 radians.
  /// If d is 0, o is returned.
  template< class F >
  inline F H3DAtan( F d ) {
    return atan( d );
  }

  /// \ingroup H3DUtilMath
  /// Returns the arctangent of y/x in the range -pi to pi radians.
  /// If both parameters of atan2 are 0, 0 is returned. H3DAtan2 is well
  /// defined for every point other than the origin, even if x equals
  /// 0 and y does not equal 0.
  template< class F >
  inline F H3DAtan2( F y, F x ) {
    return atan2( y, x );
  }

  /// \ingroup H3DUtilMath
  /// Returns the largest integer that is not greater than f.
  template< class F >
  inline F H3DFloor( F f ) {
    return floor( f );
  }

  /// \ingroup H3DUtilMath
  /// Returns the smallest integer greater than or equal to f.
  template< class F >
  inline F H3DCeil( F f ) {
    return ceil( f );
  }
}

#endif
