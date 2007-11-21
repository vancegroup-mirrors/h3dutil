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
/// \brief Header file for the LoadImageFunctions struct which contains
/// functions for loading image files.
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __LoadImageFunctions_H__
#define __LoadImageFunctions_H__

#include <H3DUtil/Image.h>

namespace H3DUtil {
  /// LoadImageFunctions is a struct containing static functions
  /// used to load image files. Most of the common image formats
  /// is loaded by the loadFreeImage() function and raw-files can
  /// be loaded by the loadRawImage() function.
  struct H3DUTIL_API LoadImageFunctions {

#ifdef HAVE_FREEIMAGE
    /// Loads an image using FreeImage.
    static Image *loadFreeImage( const string &url );
#endif

    struct H3DUTIL_API RawImageInfo {
    public:
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

        int width;
        int height;
        int depth;
        std::string pixel_type_string;
        std::string pixel_component_type_string;
        int bits_per_pixel;
        Vec3f pixel_size;
    };
    /// Read the data from the file pointed to by the parameter url
    /// and creates and returns a PixelImage containing this data.
    /// How to interpret the data is specified by the raw_image_info parameter.
    static Image *loadRawImage( const string &url,
                                RawImageInfo &raw_image_info );
  };
}

#endif
