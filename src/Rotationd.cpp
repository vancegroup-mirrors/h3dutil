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
/// \file Rotationd.cpp
/// \brief .cpp file for Rotationd.
///
//
//////////////////////////////////////////////////////////////////////////////

#include <H3DUtil/Rotationd.h>
#include <H3DUtil/Matrix3d.h>
#include <H3DUtil/Quaterniond.h>

using namespace H3DUtil;
using namespace ArithmeticTypes;

Rotationd H3DUtil::ArithmeticTypes::operator*( const Rotationd &r1 ,const Rotationd &r2 ) {
  return (Quaterniond) r1 * (Quaterniond) r2;
}
/// Constructor. From Euler angles (yaw, pitch, roll ).
Rotationd::Rotationd( const Vec3d &euler_angles ) {
  *this = Quaterniond( euler_angles );
}


/// Constructor. From Quaterniond object.
Rotationd::Rotationd( const Quaterniond &q ) {
  H3DDouble v2 = q.v * q.v;
  if( v2 <= 0 ) {// Constants::d_epsilon ) { 
    axis = Vec3d(1,0,0);
    angle = 0;
  } else {
    axis = q.v / H3DSqrt(v2);
    if( q.w > 1 ) 
      angle = 0;
    else if ( q.w < -1 ) 
      angle = (H3DDouble) Constants::pi;
    else
      angle = 2 * H3DAcos( q.w );
  }
}

// Constructor. From Matrix3d that is a rotation matrix. 
Rotationd::Rotationd( const Matrix3d &m ) {
  Quaterniond q(m);
  *this = Rotationd::Rotationd( q );
}

// Get the euler angles( yaw, pitch, roll ) representation of 
// the Rotationd. 
Vec3d Rotationd::toEulerAngles() {
  return Matrix3d( *this ).toEulerAngles();
}

/*Rotationd Rotationd::slerp( const Rotationd &r, 
                          H3DDouble t ) const {
  Quaterniond q = *this;
  return q.slerp( r, t );
}*/

Rotationd::Rotationd( const Vec3d &n1, const Vec3d &n2 ) {
  H3DDouble dot_product = n1 * n2;

  bool use_perpendicular_axis = false;
  if( dot_product < -1.0 + Constants::d_epsilon ) {
    angle = (H3DDouble)Constants::pi;
    use_perpendicular_axis = true;
  } else if( dot_product > 1.0 - Constants::d_epsilon ) {
    angle = 0;
    use_perpendicular_axis = true;
  } else {
    angle = H3DAcos( dot_product );
  }

  if( use_perpendicular_axis ) {
    if( H3DAbs( n1.x ) > 0.5 ) axis = Vec3d( -n1.z, 0, n1.x );
    else if( H3DAbs( n1.y ) > 0.5 ) axis = Vec3d( n1.y, -n1.x, 0 );
    else axis = Vec3d( 0, n1.z, -n1.y );
  } else {
    axis = n1 % n2;
    if( axis * axis < Constants::d_epsilon )
      axis = Vec3d( 1, 0, 0 );
  }

  axis.normalize();
}
