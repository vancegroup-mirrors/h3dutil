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
/// \file Rotationd.h
/// \brief Header file for Rotationd.
///
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __ROTATIOND_H__
#define __ROTATIOND_H__

#include <H3DUtil/H3DUtil.h>
#include <H3DUtil/H3DBasicTypes.h>
#include <H3DUtil/H3DMath.h>
#include <H3DUtil/Vec3d.h>
#include <H3DUtil/Vec3f.h>
#include <H3DUtil/Rotation.h>
#include <H3DUtil/TemplateOperators.h>

namespace H3DUtil {
  namespace ArithmeticTypes {
    // forward declarations.
    class Matrix3f;
    class Matrix3d;
    class Quaterniond;

    /// Rotationd describes an arbitrary rotation. It specifies an axis to
    /// rotate around and the angle to rotate.
    /// \ingroup H3DUtilBasicTypes
    class H3DUTIL_API Rotationd {
    public:
      /// Default constructor.
      Rotationd() : axis( 0,0,0 ), angle( 0 ) {}

      /// Constructor. x, y, z is the vector to rotate around and a is the
      /// angle.
      Rotationd( H3DDouble x,
                H3DDouble y,
                H3DDouble z,
                H3DDouble a ) : axis( x, y, z ), angle(a) {}

      /// Constructor.
      /// \param _axis The axis of the rotation.
      /// \param _angle The angle of the rotation.
      Rotationd( const Vec3d &_axis, 
                 H3DDouble _angle ) : axis( _axis ), angle( _angle ) {}

      /// Constructor.
      /// Constructs the shortest rotation the goes from n1 to n2.
      /// Both n1 and n2 must be unit vectors.
      Rotationd( const Vec3d &n1, const Vec3d &n2 );

      /// Constructor. From Euler angles (yaw, pitch, roll ).
      explicit Rotationd( const Vec3d &euler_angles );

      /// Constructor. From Quaterniond object.
      Rotationd( const Quaterniond &r );

      /// Constructor. From Rotation object.
      Rotationd( const Rotation &r ) : axis( r.axis ), angle( r.angle ){}
      
      /// Constructor. From Matrix3d that is a rotation matrix. Assumes
      /// the matrix is orthogonal.
      explicit Rotationd( const Matrix3d &m );
      
      /// Get the euler angles( yaw, pitch, roll ) representation of 
      /// the Rotationd. 
      Vec3d toEulerAngles(); 

      /// Spherical linear interpolation between two Rotationds.
      /// \param r Ending Rotationd
      /// \param t Interpolation value between 0 and 1.
      Rotationd slerp( const Rotationd &r, 
                       H3DDouble t ) const;

      /// The axis the rotation is around.
      Vec3d axis;

      /// The angle of the rotation. 
      H3DDouble angle;
    };

    /// \defgroup RotationdOperators Rotationd operators.
    /// \brief Operators on Rotationd instances. See also the 
    /// \ref TemplateOperators "template operators" for more operators
    /// automatically defined from the explicit ones defined here.
    /// \ingroup H3DUtilBasicTypes
    /// \{

    /// Test two Rotationd instances for equality.
    inline bool operator==( const Rotationd &r1, const Rotationd &r2 ) {
      return r1.axis == r2.axis && r1.angle == r2.angle;
    }
		
    /// Negation of a Rotationd is the Rotationd around the same axis
    /// but in the other direction.
    ///
    inline Rotationd operator-( const Rotationd &r ) {
      return Rotationd( r.axis, -r.angle );
    }

    /// Multiplacation by a double.
    inline Rotationd operator*( const Rotationd &r, 
                               double d ) {    
      return Rotationd( r.axis, (r.angle * d) );
    }

    /// Multiplacation by a float.
    inline Rotationd operator*( const Rotationd &r, 
                                float f ) {    
      return Rotationd( r.axis, (r.angle * f) );
    }

    /// Multiplacation by a int.
    inline Rotationd operator*( const Rotationd &r, 
                               int i ) {    
      return Rotationd( r.axis, r.angle * i );
    }

    /// Multiplacation by a long.
    inline Rotationd operator*( const Rotationd &r, 
                               long i ) {    
      return Rotationd( r.axis, r.angle * i );
    }

    /// Multiplication with float.
    inline Rotationd operator*( const float &a, 
                                const Rotationd &b ) { 
      return b * a;
    }

    /// Multiplication with double.
    inline Rotationd operator*( const double &a, 
                                const Rotationd &b ) { 
      return b * a;
    }

    /// Multiplication with int.
    inline Rotationd operator*( const int &a, 
                                const Rotationd &b ) { 
      return b * a;
    }

    /// Multiplication with long.
    inline Rotationd operator*( const long &a, 
                                const Rotationd &b ) { 
      return b * a;
    }

    /// Function for printing a Rotationd to an ostream.
	  inline ostream& operator<<( ostream &os, const Rotationd &r ) {
		  os << r.axis << " " << r.angle;
		  return os;
	  }



    /// Multiplication of Rotationd is the composition of the rotations.
    Rotationd H3DUTIL_API operator*( const Rotationd &r1 ,const Rotationd &r2 );
   }
}

#endif
