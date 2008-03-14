//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004-2007, SenseGraphics AB
//
//    This file is part of H3D API.
//
//    H3D API is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    H3D API is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with H3D API; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//    A commercial license is also available. Please contact us at 
//    www.sensegraphics.com for more information.
//
//
/// \file LoadImageFunctions.h
/// \brief Header file for the which contains functions for loading
/// image files.
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __LOADIMAGEFUNCTIONS_H__
#define __LOADIMAGEFUNCTIONS_H__

#include <H3DUtil/Image.h>

namespace H3DUtil {

  /// \ingroup H3DUtilClasses
  /// \defgroup ImageLoaderFunctions Image loader functions
  /// These functions can be used to load an image of a certain type.
 
#ifdef HAVE_FREEIMAGE
  /// \ingroup ImageLoaderFunctions
  /// Loads an image using FreeImage.
  H3DUTIL_API Image *loadFreeImage( const string &url );
#endif

  /// Contains information needed by the loadRawImage function
  /// to correctly load the raw file.
  struct H3DUTIL_API RawImageInfo {
  public:
    /// Constructor
    RawImageInfo( int _width,
                  int _height,
                  int _depth,
                  std::string _pixel_type_string,
                  std::string _pixel_component_type_string,
                  int _bits_per_pixel,
                  Vec3f _pixel_size ) :
      width( _width ),
      height( _height ),
      depth( _depth ),
      pixel_type_string( _pixel_type_string ),
      pixel_component_type_string( _pixel_component_type_string ),
      bits_per_pixel( _bits_per_pixel ),
      pixel_size( _pixel_size ) {}

      /// Width of image.
      int width;
      /// Height of image
      int height;
      /// Depth of image.
      int depth;

      /// Contains a string describing the pixel_type.
      /// Should be a string that looks like one of Image::PixelType.
      std::string pixel_type_string;

      /// Contains a string describing the pixel_component_type. Should be
      /// a string that looks like one of Image::PixelComponentType.
      std::string pixel_component_type_string;

      /// The number of bits per pixel.
      int bits_per_pixel;

      /// The size of the pixel in x, y and z direction in metres.
      Vec3f pixel_size;
  };

  /// \ingroup ImageLoaderFunctions
  /// Read the data from the file pointed to by the parameter url
  /// and creates and returns a PixelImage containing this data.
  /// How to interpret the data is specified by the raw_image_info parameter.
  H3DUTIL_API Image *loadRawImage( const string &url,
                                   RawImageInfo &raw_image_info );
}

#endif
