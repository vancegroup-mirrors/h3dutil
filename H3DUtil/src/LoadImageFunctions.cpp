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
/// \brief cpp file for functions to load images
///
//
//////////////////////////////////////////////////////////////////////////////
#include <H3DUtil/LoadImageFunctions.h>
#ifdef HAVE_ZLIB
#include <zlib.h>
#endif

#ifdef HAVE_FREEIMAGE
#include <H3DUtil/FreeImageImage.h>
#include <FreeImage.h>
#endif
#include <H3DUtil/PixelImage.h>
#include <fstream>

using namespace H3DUtil;

#ifdef HAVE_FREEIMAGE
Image *H3DUtil::loadFreeImage( const string &url ) {
  FREE_IMAGE_FORMAT format = FreeImage_GetFileType( url.c_str() );
  if( format == FIF_UNKNOWN ) {
    format = FreeImage_GetFIFFromFilename( url.c_str() );
  }

  if( format != FIF_UNKNOWN && FreeImage_FIFSupportsReading( format ) ) { 
    FIBITMAP *bm = FreeImage_Load( format, url.c_str() );

    if( bm ) {
      // Take care of the case of 32 bit RGB images( alpha ignored ) that seems to
      // happen once in a while with png images
      if( FreeImage_GetColorType( bm ) == FIC_RGB && FreeImage_GetBPP( bm ) == 32 ) {
        FIBITMAP *old = bm;
        bm = FreeImage_ConvertTo24Bits( bm );
        FreeImage_Unload( old );
      }
      return new FreeImageImage( bm );
    } 
  }
  return NULL;
}
#endif

Image *H3DUtil::loadRawImage( const string &url,
                              RawImageInfo &raw_image_info ) {
  Image::PixelType pixel_type;
  if( raw_image_info.pixel_type_string == "LUMINANCE" )
    pixel_type = Image::LUMINANCE;
  else if( raw_image_info.pixel_type_string == "LUMINANCE_ALPHA" )
    pixel_type = Image::LUMINANCE_ALPHA;
  else if( raw_image_info.pixel_type_string == "RGB" ) 
    pixel_type = Image::RGB;
  else if( raw_image_info.pixel_type_string == "RGBA" ) 
    pixel_type = Image::RGBA;
  else if( raw_image_info.pixel_type_string == "BGR" ) 
    pixel_type = Image::BGR;
  else if( raw_image_info.pixel_type_string == "BGRA" ) 
    pixel_type = Image::BGRA;
  else if( raw_image_info.pixel_type_string == "VEC3" ) 
    pixel_type = Image::VEC3;
  else {
    Console(3) << "Warning: Invalid pixelType value \"" << raw_image_info.pixel_type_string
               << "\" in  RawImageLoader. " << endl;
	  return NULL;
  }

  Image::PixelComponentType pixel_component_type;
  if( raw_image_info.pixel_component_type_string == "SIGNED" ) 
    pixel_component_type = Image::SIGNED; 
  else if( raw_image_info.pixel_component_type_string == "UNSIGNED" )
    pixel_component_type = Image::UNSIGNED;
  else if( raw_image_info.pixel_component_type_string == "RATIONAL" )
    pixel_component_type = Image::RATIONAL;
  else {
    Console(3) << "Warning: Invalid pixelComponentType value \"" 
               << raw_image_info.pixel_component_type_string
               << "\" in  RawImageLoader. " << endl;
	  return NULL;
  }
    
  unsigned int expected_size = 
    raw_image_info.width * raw_image_info.height * raw_image_info.depth *
    raw_image_info.bits_per_pixel / 8;

  unsigned char * data = new unsigned char[expected_size];
  
  ifstream is( url.c_str(), ios::in | ios::binary );
  if( !is.good() ) {
    return NULL;
  }
  
  is.read( (char *)data, expected_size );
  int actual_size = is.gcount();
  is.close();
  
#ifdef HAVE_ZLIB
  
  if( actual_size < (int)expected_size ){
    
    unsigned char * data2 = new unsigned char[expected_size];
    unsigned long int uncompressed_size = expected_size;
    
    int err;
    z_stream strm = {
      data, actual_size, 0,
      data2, expected_size, 0
    };
    
    err = inflateInit2(&strm,47);
    
    if( err == Z_MEM_ERROR ){
      Console(3) << "Warning: zlib memory error." << endl;
      delete[] data, data2;
      return NULL;
    }
    if( err == Z_VERSION_ERROR ){
      Console(3) << "Warning: zlib version error." << endl;
      delete[] data, data2;
      return NULL;
    }
    
    err = inflate(&strm,Z_FINISH);
    
    if( err == Z_DATA_ERROR ){
      Console(3) << "Warning: zlib unrecognizable data error." << endl;
      delete[] data, data2;
      return NULL;
    }
    if( err == Z_STREAM_ERROR ){
      Console(3) << "Warning: zlib stream error." << endl;
      delete[] data, data2;
      return NULL;
    }
    if( err == Z_BUF_ERROR ){
      Console(3) << "Warning: zlib out of memory error." << endl;
      delete[] data, data2;
      return NULL;
    }
    
    err = inflateEnd(&strm);
    
    if( err == Z_STREAM_ERROR ){
      Console(3) << "Warning: zlib stream error." << endl;
      delete[] data, data2;
      return NULL;
    }
    
    Console(2) << "Inflated compressed raw file." << endl;
    delete[] data;
    data = data2;
  }
  
#endif
  
  return new PixelImage( raw_image_info.width,
                         raw_image_info.height,
                         raw_image_info.depth,
                         raw_image_info.bits_per_pixel,
                         pixel_type,
                         pixel_component_type,
                         data,
                         false,
                         raw_image_info.pixel_size );
}
