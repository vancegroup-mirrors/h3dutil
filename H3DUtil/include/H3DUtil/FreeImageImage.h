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
/// \file FreeImageImage.h
/// \brief Image class encapsulating a FreeImage bitmap.
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __FREEIMAGEIMAGE_H__
#define __FREEIMAGEIMAGE_H__

#include <H3DUtil/Image.h>
#include <H3DUtil/Exception.h>

#ifdef HAVE_ZLIB
#include <zlib.h>

#define ZIP_MAGIC_NR 0x8B1F

#endif

#ifdef HAVE_FREEIMAGE


struct FIBITMAP;

namespace H3DUtil {


  /// This Image class is an encapsulation of a FreeImage FIBITMAP *,
  /// providing all nesessary Image functions to use it in e.g. ImageTexture
  /// 
  class H3DUTIL_API FreeImageImage: public Image {
  public:
    /// Constructor. 
    /// \param _bitmap The FreeImage bitmap we want to encapsulate.
    ///
    FreeImageImage( FIBITMAP *_bitmap ):
      bitmap( _bitmap ){
      byte_alignment = 4;
    }
    
    /// Destructor.
    virtual  ~FreeImageImage();

    /// Returns the width of the image in pixels.
    virtual unsigned int width();

    /// Returns the height of the image in pixels.
    virtual unsigned int height();

    /// Returns the depth of the image in pixels.
    virtual unsigned int depth() {
      return 1;
    }

    /// Returns the number of bits used for each pixel in the image.
    virtual unsigned int bitsPerPixel();

    /// Returns the PixelType of the image.
    virtual PixelType pixelType();
        
    /// Returns the PixelComponentType of the image.
    virtual PixelComponentType pixelComponentType();
        
    /// Returns a pointer to the raw image data. 
    virtual void *getImageData();

  protected:
    FIBITMAP * bitmap;
  };

    
}

#endif // HAVE_FREEIMAGE
#endif


