//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004-2009, SenseGraphics AB
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
/// \file ReadWriteH3DTypes.h
/// \brief Header file which contains functions for reading and writing
/// H3DTypes to and from streams.
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __READWRITEH3DTYPES_H__
#define __READWRITEH3DTYPES_H__

#include <H3DUtil/LinAlgTypes.h>

namespace H3DUtil {

  /// \ingroup H3DUtilClasses
  /// \defgroup HelpFunctions Various help functions
  /// These functions might be of use for various tasks.

  /// \ingroup HelpFunctions
  /// Write a H3DInt32 binary to an ostream ( such as an ofstream ).
  /// \param output The ostream to write data to.
  /// \param data The data to write to stream.
  inline void writeH3DType( ostream &output, H3DInt32 data ) {
    output.write( (char *)&(data), sizeof( H3DInt32 ) );
  }

  /// \ingroup HelpFunctions
  /// Write a float binary to an ostream ( such as an ofstream ).
  /// The reason for not using H3DFloat is that H3DFloat and H3DDouble might
  /// both be defined to be double or both are float and in that case this
  /// code might not compile.
  /// \param output The ostream to write data to.
  /// \param data The data to write to stream.
  inline void writeH3DType( ostream &output, float data ) {
      output.write( (char *)&(data), sizeof( float ) );
  }

  /// \ingroup HelpFunctions
  /// Write a double binary to an ostream ( such as an ofstream ).
  /// The reason for not using H3DDouble is that H3DFloat and H3DDouble might
  /// both be defined to be double or both are float and in that case this
  /// code might not compile.
  /// \param output The ostream to write data to.
  /// \param data The data to write to stream.
  inline void writeH3DType( ostream &output, double data ) {
      output.write( (char *)&(data), sizeof( double ) );
  }

  /// \ingroup HelpFunctions
  /// Write a Vec2f binary to an ostream ( such as an ofstream ).
  /// \param output The ostream to write data to.
  /// \param data The data to write to stream.
  inline void writeH3DType( ostream &output, const Vec2f &data ) {
    writeH3DType( output, data.x );
    writeH3DType( output, data.y );
  }

  /// \ingroup HelpFunctions
  /// Write a Vec2d binary to an ostream ( such as an ofstream ).
  /// \param output The ostream to write data to.
  /// \param data The data to write to stream.
  inline void writeH3DType( ostream &output, const Vec2d &data ) {
    writeH3DType( output, data.x );
    writeH3DType( output, data.y );
  }

  /// \ingroup HelpFunctions
  /// Write a Vec3f binary to an ostream ( such as an ofstream ).
  /// \param output The ostream to write data to.
  /// \param data The data to write to stream.
  inline void writeH3DType( ostream &output, const Vec3f &data ) {
    writeH3DType( output, data.x );
    writeH3DType( output, data.y );
    writeH3DType( output, data.z );
  }

  /// \ingroup HelpFunctions
  /// Write a Vec3d binary to an ostream ( such as an ofstream ).
  /// \param output The ostream to write data to.
  /// \param data The data to write to stream.
  inline void writeH3DType( ostream &output, const Vec3d &data ) {
    writeH3DType( output, data.x );
    writeH3DType( output, data.y );
    writeH3DType( output, data.z );
  }

  /// \ingroup HelpFunctions
  /// Write a Vec4f binary to an ostream ( such as an ofstream ).
  /// \param output The ostream to write data to.
  /// \param data The data to write to stream.
  inline void writeH3DType( ostream &output, const Vec4f &data ) {
    writeH3DType( output, data.x );
    writeH3DType( output, data.y );
    writeH3DType( output, data.z );
    writeH3DType( output, data.w );
  }

  /// \ingroup HelpFunctions
  /// Write a Vec4d binary to an ostream ( such as an ofstream ).
  /// \param output The ostream to write data to.
  /// \param data The data to write to stream.
  inline void writeH3DType( ostream &output, const Vec4d &data ) {
    writeH3DType( output, data.x );
    writeH3DType( output, data.y );
    writeH3DType( output, data.z );
    writeH3DType( output, data.w );
  }

  /// \ingroup HelpFunctions
  /// Write a Rotation binary to an ostream ( such as an ofstream ).
  /// \param output The ostream to write data to.
  /// \param data The data to write to stream.
  inline void writeH3DType( ostream &output, const Rotation &data ) {
    writeH3DType( output, data.axis );
    writeH3DType( output, data.angle );
  }

  /// \ingroup HelpFunctions
  /// Write a Rotationd binary to an ostream ( such as an ofstream ).
  /// \param output The ostream to write data to.
  /// \param data The data to write to stream.
  inline void writeH3DType( ostream &output, const Rotationd &data ) {
    writeH3DType( output, data.axis );
    writeH3DType( output, data.angle );
  }

  /// \ingroup HelpFunctions
  /// Write a Matrix3f binary to an ostream ( such as an ofstream ).
  /// \param output The ostream to write data to.
  /// \param data The data to write to stream.
  inline void writeH3DType( ostream &output, const Matrix3f &data ) {
    writeH3DType( output, data[0][0] );
    writeH3DType( output, data[0][1] );
    writeH3DType( output, data[0][2] );
    writeH3DType( output, data[1][0] );
    writeH3DType( output, data[1][1] );
    writeH3DType( output, data[1][2] );
    writeH3DType( output, data[2][0] );
    writeH3DType( output, data[2][1] );
    writeH3DType( output, data[2][2] );
  }

  /// \ingroup HelpFunctions
  /// Write a Matrix3d binary to an ostream ( such as an ofstream ).
  /// \param output The ostream to write data to.
  /// \param data The data to write to stream.
  inline void writeH3DType( ostream &output, const Matrix3d &data ) {
    writeH3DType( output, data[0][0] );
    writeH3DType( output, data[0][1] );
    writeH3DType( output, data[0][2] );
    writeH3DType( output, data[1][0] );
    writeH3DType( output, data[1][1] );
    writeH3DType( output, data[1][2] );
    writeH3DType( output, data[2][0] );
    writeH3DType( output, data[2][1] );
    writeH3DType( output, data[2][2] );
  }

  /// \ingroup HelpFunctions
  /// Write a Matrix4f binary to an ostream ( such as an ofstream ).
  /// \param output The ostream to write data to.
  /// \param data The data to write to stream.
  inline void writeH3DType( ostream &output, const Matrix4f &data ) {
    writeH3DType( output, data[0][0] );
    writeH3DType( output, data[0][1] );
    writeH3DType( output, data[0][2] );
    writeH3DType( output, data[0][3] );
    writeH3DType( output, data[1][0] );
    writeH3DType( output, data[1][1] );
    writeH3DType( output, data[1][2] );
    writeH3DType( output, data[1][3] );
    writeH3DType( output, data[2][0] );
    writeH3DType( output, data[2][1] );
    writeH3DType( output, data[2][2] );
    writeH3DType( output, data[2][3] );
    writeH3DType( output, data[3][0] );
    writeH3DType( output, data[3][1] );
    writeH3DType( output, data[3][2] );
    writeH3DType( output, data[3][3] );
  }

  /// \ingroup HelpFunctions
  /// Write a Matrix4d binary to an ostream ( such as an ofstream ).
  /// \param output The ostream to write data to.
  /// \param data The data to write to stream.
  inline void writeH3DType( ostream &output, const Matrix4d &data ) {
    writeH3DType( output, data[0][0] );
    writeH3DType( output, data[0][1] );
    writeH3DType( output, data[0][2] );
    writeH3DType( output, data[0][3] );
    writeH3DType( output, data[1][0] );
    writeH3DType( output, data[1][1] );
    writeH3DType( output, data[1][2] );
    writeH3DType( output, data[1][3] );
    writeH3DType( output, data[2][0] );
    writeH3DType( output, data[2][1] );
    writeH3DType( output, data[2][2] );
    writeH3DType( output, data[2][3] );
    writeH3DType( output, data[3][0] );
    writeH3DType( output, data[3][1] );
    writeH3DType( output, data[3][2] );
    writeH3DType( output, data[3][3] );
  }

  /// \ingroup HelpFunctions
  /// Write a Quaternion binary to an ostream ( such as an ofstream ).
  /// \param output The ostream to write data to.
  /// \param data The data to write to stream.
  inline void writeH3DType( ostream &output, const Quaternion &data ) {
    writeH3DType( output, data.v );
    writeH3DType( output, data.w );
  }

  /// \ingroup HelpFunctions
  /// Write a Quaterniond binary to an ostream ( such as an ofstream ).
  /// \param output The ostream to write data to.
  /// \param data The data to write to stream.
  inline void writeH3DType( ostream &output, const Quaterniond &data ) {
    writeH3DType( output, data.v );
    writeH3DType( output, data.w );
  }

  /// \ingroup HelpFunctions
  /// Write a string binary to an ostream ( such as an ofstream ).
  /// Not really an H3D type but it is useful anyways.
  /// \param output The ostream to write data to.
  /// \param data The data to write to stream.
  inline void writeH3DType( ostream &output, const string &data ) {
    if( data.length() > 0 )
      output.write( data.c_str(), sizeof( char ) * data.length() );
    char null_char = '\0';
    output.write( &null_char, sizeof(char) );
  }

  /// \ingroup HelpFunctions
  /// Read a H3DInt32 binary from an istream ( such as an ifstream ).
  /// \param input The istream to read data from.
  /// \param data The variable in which to put the read data.
  inline void readH3DType( istream &input, H3DInt32 &data ) {
    input.read( (char *)&(data), sizeof( H3DInt32 ) );
  }

  /// \ingroup HelpFunctions
  /// Read a float binary from an istream ( such as an ifstream ).
  /// The reason for not using H3DFloat is that H3DFloat and H3DDouble might
  /// both be defined to be double or both are float and in that case this
  /// code might not compile.
  /// \param input The istream to read data from.
  /// \param data The variable in which to put the read data.
  inline void readH3DType( istream &input, float &data ) {
      input.read( (char *)&(data), sizeof( float ) );
  }

  /// \ingroup HelpFunctions
  /// Read a double binary from an istream ( such as an ifstream ).
  /// The reason for not using H3DDouble is that H3DFloat and H3DDouble might
  /// both be defined to be double or both are float and in that case this
  /// code might not compile.
  /// \param input The istream to read data from.
  /// \param data The variable in which to put the read data.
  inline void readH3DType( istream &input, double &data ) {
      input.read( (char *)&(data), sizeof( double ) );
  }

  /// \ingroup HelpFunctions
  /// Read a Vec2f binary from an istream ( such as an ifstream ).
  /// \param input The istream to read data from.
  /// \param data The variable in which to put the read data.
  inline void readH3DType( istream &input, Vec2f &data ) {
    readH3DType( input, data.x );
    readH3DType( input, data.y );
  }

  /// \ingroup HelpFunctions
  /// Read a Vec2d binary from an istream ( such as an ifstream ).
  /// \param input The istream to read data from.
  /// \param data The variable in which to put the read data.
  inline void readH3DType( istream &input, Vec2d &data ) {
    readH3DType( input, data.x );
    readH3DType( input, data.y );
  }

  /// \ingroup HelpFunctions
  /// Read a Vec3f binary from an istream ( such as an ifstream ).
  /// \param input The istream to read data from.
  /// \param data The variable in which to put the read data.
  inline void readH3DType( istream &input, Vec3f &data ) {
    readH3DType( input, data.x );
    readH3DType( input, data.y );
    readH3DType( input, data.z );
  }

  /// \ingroup HelpFunctions
  /// Read a Vec3d binary from an istream ( such as an ifstream ).
  /// \param input The istream to read data from.
  /// \param data The variable in which to put the read data.
  inline void readH3DType( istream &input, Vec3d &data ) {
    readH3DType( input, data.x );
    readH3DType( input, data.y );
    readH3DType( input, data.z );
  }

  /// \ingroup HelpFunctions
  /// Read a Vec4f binary from an istream ( such as an ifstream ).
  /// \param input The istream to read data from.
  /// \param data The variable in which to put the read data.
  inline void readH3DType( istream &input, Vec4f &data ) {
    readH3DType( input, data.x );
    readH3DType( input, data.y );
    readH3DType( input, data.z );
    readH3DType( input, data.w );
  }

  /// \ingroup HelpFunctions
  /// Read a Vec4d binary from an istream ( such as an ifstream ).
  /// \param input The istream to read data from.
  /// \param data The variable in which to put the read data.
  inline void readH3DType( istream &input, Vec4d &data ) {
    readH3DType( input, data.x );
    readH3DType( input, data.y );
    readH3DType( input, data.z );
    readH3DType( input, data.w );
  }

  /// \ingroup HelpFunctions
  /// Read a Rotation binary from an istream ( such as an ifstream ).
  /// \param input The istream to read data from.
  /// \param data The variable in which to put the read data.
  inline void readH3DType( istream &input, Rotation &data ) {
    readH3DType( input, data.axis );
    readH3DType( input, data.angle );
  }

  /// \ingroup HelpFunctions
  /// Read a Rotationd binary from an istream ( such as an ifstream ).
  /// \param input The istream to read data from.
  /// \param data The variable in which to put the read data.
  inline void readH3DType( istream &input, Rotationd &data ) {
    readH3DType( input, data.axis );
    readH3DType( input, data.angle );
  }

  /// \ingroup HelpFunctions
  /// Read a Matrix3f binary from an istream ( such as an ifstream ).
  /// \param input The istream to read data from.
  /// \param data The variable in which to put the read data.
  inline void readH3DType( istream &input, Matrix3f &data ) {
    readH3DType( input, data[0][0] );
    readH3DType( input, data[0][1] );
    readH3DType( input, data[0][2] );
    readH3DType( input, data[1][0] );
    readH3DType( input, data[1][1] );
    readH3DType( input, data[1][2] );
    readH3DType( input, data[2][0] );
    readH3DType( input, data[2][1] );
    readH3DType( input, data[2][2] );
  }

  /// \ingroup HelpFunctions
  /// Read a Matrix3d binary from an istream ( such as an ifstream ).
  /// \param input The istream to read data from.
  /// \param data The variable in which to put the read data.
  inline void readH3DType( istream &input, Matrix3d &data ) {
    readH3DType( input, data[0][0] );
    readH3DType( input, data[0][1] );
    readH3DType( input, data[0][2] );
    readH3DType( input, data[1][0] );
    readH3DType( input, data[1][1] );
    readH3DType( input, data[1][2] );
    readH3DType( input, data[2][0] );
    readH3DType( input, data[2][1] );
    readH3DType( input, data[2][2] );
  }

  /// \ingroup HelpFunctions
  /// Read a Matrix4f binary from an istream ( such as an ifstream ).
  /// \param input The istream to read data from.
  /// \param data The variable in which to put the read data.
  inline void readH3DType( istream &input, Matrix4f &data ) {
    readH3DType( input, data[0][0] );
    readH3DType( input, data[0][1] );
    readH3DType( input, data[0][2] );
    readH3DType( input, data[0][3] );
    readH3DType( input, data[1][0] );
    readH3DType( input, data[1][1] );
    readH3DType( input, data[1][2] );
    readH3DType( input, data[1][3] );
    readH3DType( input, data[2][0] );
    readH3DType( input, data[2][1] );
    readH3DType( input, data[2][2] );
    readH3DType( input, data[2][3] );
    readH3DType( input, data[3][0] );
    readH3DType( input, data[3][1] );
    readH3DType( input, data[3][2] );
    readH3DType( input, data[3][3] );
  }

  /// \ingroup HelpFunctions
  /// Read a Matrix4d binary from an istream ( such as an ifstream ).
  /// \param input The istream to read data from.
  /// \param data The variable in which to put the read data.
  inline void readH3DType( istream &input, Matrix4d &data ) {
    readH3DType( input, data[0][0] );
    readH3DType( input, data[0][1] );
    readH3DType( input, data[0][2] );
    readH3DType( input, data[0][3] );
    readH3DType( input, data[1][0] );
    readH3DType( input, data[1][1] );
    readH3DType( input, data[1][2] );
    readH3DType( input, data[1][3] );
    readH3DType( input, data[2][0] );
    readH3DType( input, data[2][1] );
    readH3DType( input, data[2][2] );
    readH3DType( input, data[2][3] );
    readH3DType( input, data[3][0] );
    readH3DType( input, data[3][1] );
    readH3DType( input, data[3][2] );
    readH3DType( input, data[3][3] );
  }

  /// \ingroup HelpFunctions
  /// Read a Quaternion binary from an istream ( such as an ifstream ).
  /// \param input The istream to read data from.
  /// \param data The variable in which to put the read data.
  inline void readH3DType( istream &input, Quaternion &data ) {
    readH3DType( input, data.v );
    readH3DType( input, data.w );
  }

  /// \ingroup HelpFunctions
  /// Read a Quaterniond binary from an istream ( such as an ifstream ).
  /// \param input The istream to read data from.
  /// \param data The variable in which to put the read data.
  inline void readH3DType( istream &input, Quaterniond &data ) {
    readH3DType( input, data.v );
    readH3DType( input, data.w );
  }

  /// \ingroup HelpFunctions
  /// Read a string binary from an istream ( such as an ifstream ).
  /// Not really an H3D type but it is useful anyways.
  /// \param input The istream to read data from.
  /// \param data The variable in which to put the read data.
  inline void readH3DType( istream &input, string &data ) {
    while( !input.eof() ) {
      char tmp_chr;
      input.read( &tmp_chr, sizeof( char ) );
      if( tmp_chr == '\0' ) {
        break;
      }
      data = string( data + tmp_chr );
    }
  }
}

#endif
