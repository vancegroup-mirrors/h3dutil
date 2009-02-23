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
/// \file Quaterniond.h
/// \brief Header file for Quaterniond.
///
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __QUATERNIOND_H__
#define __QUATERNIOND_H__

#include <H3DUtil/H3DUtil.h>
#include <H3DUtil/H3DBasicTypes.h>
#include <H3DUtil/H3DMath.h>
#include <H3DUtil/Vec3d.h>
#include <H3DUtil/Quaternion.h>

namespace H3DUtil {
  namespace ArithmeticTypes {
    // forward declarations.
    class Matrix3d;
	class Matrix3d;
    class Rotationd;

    /// Quaterniond describes an arbitrary rotation.
    /// \ingroup H3DUtilBasicTypes
    class H3DUTIL_API Quaterniond {
    public:
      /// Default constructor.
      Quaterniond(): v( 0, 0, 0 ), w( 0 ) {}

      /// Constructor. 
      Quaterniond( H3DDouble x,
                  H3DDouble y,
                  H3DDouble z,
                  H3DDouble _w ) : v( x, y, z ), w(_w) {}
      
      /// Constructor.
      Quaterniond( const Vec3d &_v, 
                  H3DDouble _w ) : v( _v ), w( _w ) {}
      
      /// Constructor. From Euler angles (yaw, pitch, roll ).
      explicit Quaterniond( const Vec3d &euler_angles );

      /// Constructor. From Rotationd object.
      Quaterniond( const Rotationd &r );
      
      /// Constructor. From Rotation object.
      Quaterniond( const Quaternion &r ) : v( r.v ), w( r.w ){}

      /// Constructor. From Matrix3d that is a rotation matrix.  Assumes
      /// the matrix is orthogonal.
      explicit Quaterniond( const Matrix3d &m );

      /// Returns the Quaterniond norm.
      inline H3DDouble norm() {
        return w*w + v.dotProduct( v );
      }

      /// Normalize the Quaterniond, i.e. scale it so that the magnitude
      /// is 1.
      inline void normalize() {
        H3DDouble n = norm();
        if (H3DAbs( n ) > Constants::f_epsilon ) {
          H3DDouble length = H3DSqrt( n );
          v = v / length;
          w = w / length;
        }
      }
      
      /// dotProduct() returns the cos(angle) between two quaternions
      inline H3DDouble dotProduct( const Quaterniond &q) const {
        return v.x*q.v.x + v.y*q.v.y + v.z*q.v.z + w*q.w;
      }
      
      /// Returns the conjugate of the Quaterniond.
      inline Quaterniond conjugate() {
        return Quaterniond( -v, w );
      }

      /// Returns the inverse of the Quaterniond.
      inline Quaterniond inverse();
      
      /// Get the euler angles( yaw, pitch, roll ) representation of 
      /// the Quaterniond.
      Vec3d toEulerAngles();

      /// Spherical linear interpolation between two Quaternionds.
      /// \param q Ending Quaterniond
      /// \param t Interpolation value between 0 and 1.
      Quaterniond slerp( const Quaterniond &q,
                        H3DDouble t ) const;

      /// The Quaterniond vector part.
      Vec3d v;
      /// The Quaterniond scalar part.
      H3DDouble w;
    };

    /// \defgroup QuaterniondOperators Quaterniond operators.
    /// \brief Operators on Quaterniond instances. See also the 
    /// \ref TemplateOperators "template operators" for more operators
    /// automatically defined from the explicit ones defined here.
    /// \ingroup H3DUtilBasicTypes
    /// \{

    /// Function for printing a Quaterniond to an ostream.
	  inline ostream& operator<<( ostream &os, const Quaterniond &q ) {
		  os << q.v << " " << q.w;
		  return os;
	  }

    /// Equality between two Quaterniond instances.
    inline bool operator==( const Quaterniond &q1, const Quaterniond &q2 ) {
      return q1.v == q2.v && q1.w == q2.w;
    }

    /// Multiplication of Quaternionds. If q1 and q2 are unit quaternions, 
    /// then return value will also be a unit quaternion.
    inline Quaterniond operator*( const Quaterniond &q1, 
                                 const Quaterniond &q2 ) {    
      return Quaterniond( q1.w * q2.v + q2.w * q1.v + q1.v % q2.v,
                         q1.w * q2.w - q1.v * q2.v );
    }

    /// Multiplacation by a double.
    inline Quaterniond operator*( const Quaterniond &q, 
                                 double d ) {    
      return Quaterniond( d *q.v, (H3DDouble)(d * q.w) );
    }

    /// Multiplacation by a float.
    inline Quaterniond operator*( const Quaterniond &q, 
                                 float d ) {    
      return Quaterniond( d *q.v, d * q.w );
    }

    /// Multiplacation by an int.
    inline Quaterniond operator*( const Quaterniond &q, 
                                 int d ) {    
      return Quaterniond( d *q.v, (H3DDouble) (d * q.w) );
    }

    /// Multiplacation by a long.
    inline Quaterniond operator*( const Quaterniond &q, 
                                 long d ) {    
      return Quaterniond( d *q.v, (H3DDouble) (d * q.w) );
    }

    /// Multiplication with float.
    inline Quaterniond operator*( const float &a, 
                                 const Quaterniond &b ) { 
      return b * a;
    }

    /// Multiplication with double.
    inline Quaterniond operator*( const double &a, 
                                 const Quaterniond &b ) { 
      return b * a;
    }

    /// Multiplication with int.
    inline Quaterniond operator*( const int &a, 
                                 const Quaterniond &b ) { 
      return b * a;
    }

    /// Multiplication with long.
    inline Quaterniond operator*( const long &a, 
                                 const Quaterniond &b ) { 
      return b * a;
    }

    /// Addition of Quaternionds. The result is not necessarily a unit 
    /// quaternion even if  and  are unit quaternion
    inline Quaterniond operator+( const Quaterniond &q1, 
                                 const Quaterniond &q2 ) {
      return Quaterniond( q1.v + q2.v,
                         q1.w + q2.w );
    }
    /// \}

    /// Unary minus.
    inline Quaterniond operator-( const Quaterniond &q ) { return q * -1; }
    
    /// Subtraction between two Quaternionds. The result is not necessarily a unit 
    /// quaternion even if a and b are unit quaternion.
    inline Quaterniond operator-( const Quaterniond &a, const Quaterniond &b ) { 
      return a + (-b); 
    }

    /// \}

    // Returns the inverse of the Quaterniond.
    inline Quaterniond Quaterniond::inverse() {
      H3DDouble n = norm();
      if ( H3DAbs(n ) < Constants::d_epsilon ) {
        return Quaterniond(0,0,0,0);
      } else {
        return conjugate() / n;
      }
    }
  }
}

#endif
