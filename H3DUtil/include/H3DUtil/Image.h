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
/// \file Image.h
/// \brief Header file for Image, the base class for all images.
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <H3DUtil/H3DUtil.h>
#include <H3DUtil/LinAlgTypes.h>
#include <H3DUtil/RefCountedClass.h>
#include <assert.h>
#include <string.h>

namespace H3DUtil {
  /// Virtual base class for all images containing virtual functions that
  /// all Image classes must define.
  /// For functions that load images check ImageLoaderFunctions.h
  ///
  class H3DUTIL_API Image: public RefCountedClass {
  public:
    /// Constructor.
    Image():
      byte_alignment( 1 ) {}

    /// Type that defines what format each pixel in the image is
    /// on.
    typedef enum {
      LUMINANCE, 
      LUMINANCE_ALPHA,
      RGB,
      RGBA,
      BGR,
      BGRA,
      VEC3
      // Color table bitmap??
    } PixelType;
        
    /// Type of each component in the pixel. 
    typedef enum {
      SIGNED,
      UNSIGNED,
      RATIONAL
    } PixelComponentType;

    /// Filter type.
    typedef enum {
      NEAREST,
      LINEAR
    } FilterType;

    /// Returns the width of the image in pixels.
    virtual unsigned int width() = 0;
    /// Returns the height of the image in pixels.
    virtual unsigned int height() = 0;
    /// Returns the depth of the image in pixels.
    virtual unsigned int depth() = 0;
    /// Returns the number of bits used for each pixel in the image.
    virtual unsigned int bitsPerPixel() = 0;
    /// Returns the PixelType of the image.
    virtual PixelType pixelType() = 0;
    /// Returns the PixelComponentType of the image.
    virtual PixelComponentType pixelComponentType() = 0;
    /// Returns the size of the pixel in x, y and z direction in metres.
    /// By default all pixels have the size Vec3f( 1, 1, 1 ).
    virtual Vec3f pixelSize() {
      return Vec3f( 1, 1, 1 );
    }

    /// Returns the number of components each pixel contains. E.g. LUMINANCE 
    /// image contains 1 component and RGB image contains 3.
    unsigned int nrPixelComponents();

    /// Returns a pointer to the raw image data. The ownership of the 
    /// pointer is held by the Image class, if any memory allocations
    /// are made for the pointer the Image class is responsible for 
    /// deallocating that memory.
    virtual void *getImageData() = 0;

    /// Sample the image at a given normalized position(texture coordinate), 
    /// i.e. coordinates between 0 and 1. Pixel data will be trilinearly
    /// interpolated to  calculate the result.
    ///
    /// \param value Where to put the return value.
    /// \param x The position in x(width) to sample(0-1).
    /// \param y The position in y(height) to sample(0-1).
    /// \param z The position in z(depth) to sample(0-1).
    /// \param filter_type Determines the sample should be interpolated.
    void getSample( void *value, 
                    H3DFloat x = 0, 
                    H3DFloat y = 0, 
                    H3DFloat z = 0,
                    FilterType filter_type = LINEAR );

    /// Sample the image at a given normalized position(texture coordinate), 
    /// i.e. coordinates between 0 and 1. Pixel data will be trilinearly
    /// interpolated to  calculate the result.
    ///
    /// \param x The position in x(width) to sample(0-1).
    /// \param y The position in y(height) to sample(0-1).
    /// \param z The position in z(depth) to sample(0-1).
    /// \param filter_type Determines the sample should be interpolated.
    inline H3DUtil::RGBA getSample( H3DFloat x = 0, 
                                    H3DFloat y = 0, 
                                    H3DFloat z = 0,
                                    FilterType filter_type = LINEAR ) {
      unsigned int byte_rem = bitsPerPixel() % 8;
      unsigned int bytes_per_pixel = bitsPerPixel() / 8;
      
      assert( byte_rem == 0 );
      
      H3DUtil::RGBA rgba;

      // avoid allocating memory from the heap since this is very
      // slow. We only do it if the size of a pixel is larger than
      // the size of a long.
      if( bytes_per_pixel <= sizeof( long ) ) {
        long pixel_data;
        getSample( &pixel_data, x, y, z, filter_type );
        rgba = imageValueToRGBA( &pixel_data );
      } else {
        char *pixel_data = new char[bytes_per_pixel];
        getSample( pixel_data, x, y, z, filter_type );
        rgba = imageValueToRGBA( pixel_data );
        delete [] pixel_data;
      }
      return rgba;
    }
 
    /// Set the pixel at a given position given an RGBA struct. 
    /// If an LUMINANCE image, the R component is used as value. 
    ///
    /// \param value The value to set the pixel to. One or more components
    /// may be used, this depends on the type of image.
    /// \param x The position in x(width).
    /// \param y The position in y(height).
    /// \param z The position in z(depth).
    void setPixel( const H3DUtil::RGBA &value, int x = 0, int y = 0, int z = 0 );

    /// Get the pixel at a given position as an RGBA struct. 
    /// If an LUMINANCE image, the value is copied to RGB channels and 
    /// alpha set to 1.
    /// \param x The position in x(width).
    /// \param y The position in y(height).
    /// \param z The position in z(depth).
    H3DUtil::RGBA getPixel( int x = 0, int y = 0, int z = 0 );

    /// Convert a RGBA value to a value of the same type as the image data.
    void RGBAToImageValue( const H3DUtil::RGBA &rgba, void *value );

    /// Convert an image value to an RGBA value.
    H3DUtil::RGBA imageValueToRGBA( void *value );

    /// Get the value of a pixel/voxel. The size of data written in value
    /// as output depends on the type of the image.
    ///
    /// \param value Where to put the return value.
    /// \param x The position in x(width) to sample.
    /// \param y The position in y(height) to sample.
    /// \param z The position in z(depth) to sample.
    virtual void getElement( void *value, int x = 0, int y = 0, int z = 0 ) {
      unsigned int byte_rem = bitsPerPixel() % 8;
      unsigned int bytes_per_pixel = bitsPerPixel() / 8;
      assert( byte_rem == 0 );

      if( byte_rem != 0 ) {
        bytes_per_pixel++;
      }
      
      unsigned char *data = (unsigned char *) getImageData();

      memcpy( value, 
              &data[ ( ( z * height() + y ) * width() + x ) * bytes_per_pixel ],
              bytes_per_pixel );
    }
    
    /// Set the value of a pixel/voxel. The size of data read from value
    /// depends on the type of the image.
    virtual void setElement( void *value, int x = 0, int y = 0, int z = 0 ) {
      unsigned int byte_rem = bitsPerPixel() % 8;
      unsigned int bytes_per_pixel = bitsPerPixel() / 8;
      assert( byte_rem == 0 );
      if( byte_rem != 0 )
        bytes_per_pixel++;

      unsigned char *data = (unsigned char *)getImageData();
      memcpy( &data[ ( ( z * height() + y ) * width() + x ) * bytes_per_pixel ],
              value, 
              bytes_per_pixel );
    }

    /// Gets the byte alignment for the start of each pixel row in memory.
    /// Valid values are 1, 2, 4 and 8.
    virtual int byteAlignment() {
      return byte_alignment;
    }

    /// Returns the image data as a float array with the same number of elements
    /// as getImageData but with the values normalized to the range [0,1] with
    /// the lowest value that can be held by the data type used in the image maps
    /// to 0 and the highest to 1. 
    ///
    /// This function only works on images with pixel component type SIGNED or
    /// UNSIGNED.
    /// 
    /// It is the responsibility of the caller to free the memory of the returned
    /// pointer when it is finished with it.
    float *convertToNormalizedFloatData();

    /// Returns the image data as a double array with the same number of elements
    /// as getImageData but with the values normalized to the range [0,1] with
    /// the lowest value that can be held by the data type used in the image maps
    /// to 0 and the highest to 1. 
    ///
    /// This function only works on images with pixel component type SIGNED or
    /// UNSIGNED.
    /// 
    /// It is the responsibility of the caller to free the memory of the returned
    /// pointer when it is finished with it.
    double *convertToNormalizedDoubleData();
  protected:
    int byte_alignment;
  };
}

#endif


