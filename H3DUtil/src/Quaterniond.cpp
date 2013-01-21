//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004-2013, SenseGraphics AB
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
/// \file Quaterniond.cpp
/// \brief .cpp file for Quaterniond.
///
//
//////////////////////////////////////////////////////////////////////////////

#include <H3DUtil/Quaterniond.h>
#include <H3DUtil/Matrix3d.h>
#include <H3DUtil/Rotationd.h>
#include <H3DUtil/Matrix4d.h>
#include <H3DUtil/Vec3d.h>

using namespace H3DUtil;
using namespace ArithmeticTypes;

Quaterniond::Quaterniond( const Vec3d &euler ) {
  H3DDouble roll = euler.x;
  H3DDouble pitch = euler.y;
  H3DDouble yaw   = euler.z;

  H3DDouble cr = H3DCos(roll/2);
  H3DDouble cp = H3DCos(pitch/2);
  H3DDouble cy = H3DCos(yaw/2);
  
  H3DDouble sr = H3DSin(roll/2);
  H3DDouble sp = H3DSin(pitch/2);
  H3DDouble sy = H3DSin(yaw/2);
  
  H3DDouble cpcy = cp * cy;
  H3DDouble spsy = sp * sy;
  
  w = cr * cpcy + sr * spsy;
  v.x = sr * cpcy - cr * spsy;
  v.y = cr * sp * cy + sr * cp * sy;
  v.z = cr * cp * sy - sr * sp * cy; 
}

/// Constructor. From Rotationd object.
Quaterniond::Quaterniond( const Rotationd &r ) {
  H3DDouble half_angle = r.angle / (H3DDouble) 2.0;
  v = r.axis * H3DSin( half_angle );
  w = H3DCos( half_angle );
}
      

// Constructor. From Matrix3d that is a rotation matrix. 
Quaterniond::Quaterniond( const Matrix3d &m ) {
  H3DDouble trace = m[0][0]+m[1][1]+m[2][2];
  
  if ( trace > 0.0 ) {
    H3DDouble s = (H3DDouble) 0.5 / H3DSqrt( trace + 1 );
    v.x = ( m[2][1] - m[1][2] ) * s;
    v.y = ( m[0][2] - m[2][0] ) * s;
    v.z = ( m[1][0] - m[0][1] ) * s;
    w = (H3DDouble)0.25 / s;
  } else {
    double  s, q[4];
    int    i, j, k;
    int nxt[3] = {1, 2, 0};

    i = 0;
    if (m[1][1] > m[0][0]) i = 1;
    if (m[2][2] > m[i][i]) i = 2;
    j = nxt[i];
    k = nxt[j];
    s = H3DSqrt ((m[i][i] - (m[j][j] + m[k][k])) + (H3DDouble)1.0);
    q[i] = s * (H3DDouble)0.5;
    if (s != 0.0) s = (H3DDouble)0.5 / s;
    q[3] = (m[k][j] - m[j][k]) * s;
    q[j] = (m[i][j] + m[j][i]) * s;
    q[k] = (m[i][k] + m[k][i]) * s;

    v.x = q[0];
    v.y = q[1];
    v.z = q[2];
    w = q[3];
  }
  normalize();
    /*
  else if ( m[0][0] > m[1][1] && m[0][0] > m[2][2] ) {
    H3DDouble s = 2.0 * H3DSqrt( 1.0 + m[0][0] - m[1][1] - m[2][2]);
    x = 0.25 * s;
    y = (m[0][1] + m[1][0] ) / s;
    z = (m[0][2] + m[2][0] ) / s;
    w = (m[1][2] - m[2][1] ) / s;
  } else if (m[1][1] > m[2][2]) {
      H3DDouble s = 2.0 * H3DSqrt( 1.0 + m[1][1] - m[0][0] - m[2][2]);
    x = (m[0][1] + m[1][0] ) / s;
    y = 0.25f * s;
    z = (m[1][2] + m[2][1] ) / s;
    w = (m[0][2] - m[2][0] ) / s;
  } else {
    H3DDouble s = 2.0 * H3DSqrt( 1.0f + m[2][2] - m[0][0] - m[1][1] );
    x = (m[0][2] + m[2][0] ) / s;
    y = (m[1][2] + m[2][1] ) / s;
    z = 0.25f * s;
    w = (m[0][1] - m[1][0] ) / s;
    }*/
  
}

Vec3d Quaterniond::toEulerAngles() {
  return Matrix3d( *this ).toEulerAngles();
}

// SLERP interpolation between two quaternions
Quaterniond Quaterniond::slerp( const Quaterniond &b, 
                              H3DDouble frac ) const {
  Quaterniond a = *this;
  H3DDouble alpha = a.dotProduct(b);
  
  if ( alpha < 0 ) {
    a = -a;
    alpha = -alpha;
  }
  
  H3DDouble scale;
  H3DDouble invscale;
  
  if ( ( 1 - alpha ) >= Constants::f_epsilon) {  
    // spherical interpolation
    H3DDouble theta = acos( alpha );
    H3DDouble sintheta = 1 / sin( theta );
    scale = sin( theta * (1-frac) ) * sintheta;
    invscale = sin( theta * frac ) * sintheta;
  }
  else { 
    // linear interploation
    scale = 1 - frac;
    invscale = frac;
  }
  
  return ( a * scale) + ( b * invscale);
}    

