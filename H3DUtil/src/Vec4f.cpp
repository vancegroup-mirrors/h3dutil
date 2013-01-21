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
/// \file Vec4f.cpp
/// \brief .cpp file for Vec4f class.
///
//
//////////////////////////////////////////////////////////////////////////////

#include <H3DUtil/Vec4f.h>
#include <H3DUtil/Vec4d.h>

H3DUtil::ArithmeticTypes::Vec4f::Vec4f( const Vec4d &v ): x( (H3DFloat)v.x ), 
                                                      y( (H3DFloat)v.y ), 
                                                      z( (H3DFloat)v.z ),
                                                      w( (H3DFloat)v.w ){}
