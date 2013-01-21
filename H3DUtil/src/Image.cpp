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
/// \file Image.cpp
/// \brief .cpp file for Image.
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include <H3DUtil/Image.h>
#ifdef WIN32
#undef max
#endif
#include <limits>

using namespace H3DUtil;


void Image::getSample( void *value, 
                       H3DFloat x, 
                       H3DFloat y, 
                       H3DFloat z,
                       FilterType filter_type ) {

  if( filter_type == NEAREST ) {
    int xp, yp, zp;

    if( x <= 0 ) xp = 0;
    else if( x >= 1 ) xp = width() - 1;
    else xp = (int) H3DFloor( width() * x );

    if( y <= 0 ) yp = 0;
    else if( y >= 1 ) yp = height() - 1;
    else yp = (int) H3DFloor( height() * y );

    if( z <= 0 ) zp = 0;
    else if( z >= 1 ) zp = depth() - 1;
    else zp = (int) H3DFloor( depth() * z );

    getElement( value, xp, yp, zp );
  } else {

    H3DFloat px = x * width() - 0.5f;
    H3DFloat py = y * height()- 0.5f;
    H3DFloat pz = z * depth() - 0.5f;
    
    if( px < 0 ) px = 0;
    if( py < 0 ) py = 0;
    if( pz < 0 ) pz = 0;
    
    H3DFloat fx = H3DFloor( px );
    H3DFloat fy = H3DFloor( py );
    H3DFloat fz = H3DFloor( pz );
    
    H3DFloat cx = H3DCeil( px );
    H3DFloat cy = H3DCeil( py );
    H3DFloat cz = H3DCeil( pz );
    
    if( cx >= width() ) cx--;
    if( cy >= height() ) cy--;
    if( cz >= depth() ) cz--;
    
    H3DFloat xd = px - fx;
    H3DFloat yd = py - fy;
    H3DFloat zd = pz - fz;
    
    // interpolate in z
    H3DUtil::RGBA fff = getPixel( (int)fx, (int)fy, (int)fz );
    H3DUtil::RGBA ffc = getPixel( (int)fx, (int)fy, (int)cz );
    H3DUtil::RGBA fcf = getPixel( (int)fx, (int)cy, (int)fz );
    H3DUtil::RGBA fcc = getPixel( (int)fx, (int)cy, (int)cz );
    H3DUtil::RGBA cff = getPixel( (int)cx, (int)fy, (int)fz );
    H3DUtil::RGBA cfc = getPixel( (int)cx, (int)fy, (int)cz );
    H3DUtil::RGBA ccf = getPixel( (int)cx, (int)cy, (int)fz );
    H3DUtil::RGBA ccc = getPixel( (int)cx, (int)cy, (int)cz );
    
    H3DUtil::RGBA i1 = fff * (1-zd) + ffc * zd;
    H3DUtil::RGBA i2 = fcf * (1-zd) + fcc * zd;
    H3DUtil::RGBA j1 = cff * (1-zd) + cfc * zd;
    H3DUtil::RGBA j2 = ccf * (1-zd) + ccc * zd;
    
    H3DUtil::RGBA w1 = i1 * (1-yd) + i2 * yd;
    H3DUtil::RGBA w2 = j1 * (1-yd) + j2 * yd;
    
    H3DUtil::RGBA v = w1 * (1-xd) + w2 * xd;
    
    RGBAToImageValue( v, value );
  }
}


namespace ImageInternals {
  inline H3DFloat getSignedValueAsFloat( void *i, 
                                         unsigned int bytes_to_read ) {
    H3DFloat max_value = (H3DFloat) (H3DPow( 2.0, (int)bytes_to_read* 8 - 1) - 1);
    if( bytes_to_read == 1 ) {
      char v = 0;
      memcpy( &v, i, bytes_to_read );
      return v / max_value;
    } else if( bytes_to_read == 2 ) {
      short v = 0;
      memcpy( &v, i, bytes_to_read );
      return v / max_value;
    } else if( bytes_to_read == 4 ) {
      int v = 0;
      memcpy( &v, i, bytes_to_read );
      return v / max_value;
    } else if( bytes_to_read == 8 ) {
      H3DInt64 v = 0;
      memcpy( &v, i, bytes_to_read );
      return v / max_value;
    } else {
      assert( false );
      return 0;
    }
  }

  inline H3DFloat getUnsignedValueAsFloat( void *i, 
                                           unsigned int bytes_to_read ) {
    // Making sure that bytes_to_read is below 4 bits because otherwise the
    // destination of memory copy is to small. ( the variable v ).
    assert( bytes_to_read <= 4 );
    unsigned long v = 0;
    memcpy( &v,
            i,
            bytes_to_read );
    return v / (H3DFloat) (H3DPow( 2.0, (int)bytes_to_read * 8 ) - 1);
  }

  inline H3DFloat getRationalValueAsFloat( void *i, 
                                           unsigned int bytes_to_read ) {
    assert( bytes_to_read == 4 || bytes_to_read == 8 );
    double v = 0;
    if( bytes_to_read == 4 ) {
      v = *((float *)i);
    } else if( bytes_to_read == 8 ) {
      v = *((double *)i);
    }
    return (H3DFloat) v;
  }

  inline void writeFloatAsSignedValue( H3DFloat r,
                                       void *i, 
                                       unsigned int bytes_to_write ) {
    long v = (long)(r * (H3DPow( 2.0, (int)bytes_to_write * 8 - 1 ) - 1));
    memcpy( i,
            (&v),
            bytes_to_write );
  }

  inline void writeFloatAsUnsignedValue( H3DFloat r,
                                       void *i, 
                                       unsigned int bytes_to_write ) {
    unsigned long v = (unsigned long) (r * (H3DPow( 2.0, (int)bytes_to_write * 8 ) - 1) );
    memcpy( i,
            &v,
            bytes_to_write );
      
  }


  inline void writeFloatAsRationalValue( H3DFloat r,
                                         void *i, 
                                         unsigned int bytes_to_write ) {
    assert( bytes_to_write == 4 || bytes_to_write == 8 );

    if( bytes_to_write == 4 ) {
      *((float *)i) = r;
    } else if( bytes_to_write == 8 ) {
      *((double *)i) = r;
    }
  }

}

H3DUtil::RGBA Image::getPixel( int x, int y, int z ) {
  unsigned int byte_rem = bitsPerPixel() % 8;
  unsigned int bytes_per_pixel = bitsPerPixel() / 8;

  assert( byte_rem == 0 );
  if( bytes_per_pixel <= 8 ) {
    char pixel_data[8]; 
    getElement( pixel_data, x, y, z );
    H3DUtil::RGBA rgba = imageValueToRGBA( pixel_data );
    return rgba;
  } else {
    char *pixel_data = new char[bytes_per_pixel];
    getElement( pixel_data, x, y, z );
    H3DUtil::RGBA rgba = imageValueToRGBA( pixel_data );
    delete [] pixel_data;
    return rgba;
  }
}

void Image::setPixel( const H3DUtil::RGBA &value, int x, int y, int z ) {
  unsigned int byte_rem = bitsPerPixel() % 8;
  unsigned int bytes_per_pixel = bitsPerPixel() / 8;

  assert( byte_rem == 0 );
  if( bytes_per_pixel <= 8 ) {
    char pixel_data[8];
    RGBAToImageValue( value, pixel_data );
    setElement( pixel_data, x, y, z );    
  } else {
    char *pixel_data = new char[bytes_per_pixel];
    RGBAToImageValue( value, pixel_data );
    setElement( pixel_data, x, y, z );
    delete [] pixel_data;
  }
}

H3DUtil::RGBA Image::imageValueToRGBA( void *_pixel_data ) {
  using namespace ImageInternals;

  char *pixel_data = (char *) _pixel_data;

  unsigned int byte_rem = bitsPerPixel() % 8;
  unsigned int bytes_per_pixel = bitsPerPixel() / 8;

  assert( byte_rem == 0 );

  switch( pixelType() ) {
  case Image::LUMINANCE: 
    switch( pixelComponentType() ) { 
    case Image::UNSIGNED: {
      H3DFloat fv = getUnsignedValueAsFloat( pixel_data, 
                                             bytes_per_pixel );
      return H3DUtil::RGBA( fv, fv, fv, 1 );
    }
    case Image::SIGNED: {
      H3DFloat fv = getSignedValueAsFloat( pixel_data, 
                                           bytes_per_pixel );
      return H3DUtil::RGBA( fv, fv, fv, 1 );
    }
    case Image::RATIONAL: {
      H3DFloat fv = getRationalValueAsFloat( pixel_data, 
                                             bytes_per_pixel );
      return H3DUtil::RGBA( fv, fv, fv, 1 );
    }
    };
  case Image::LUMINANCE_ALPHA: {
    unsigned int bytes_per_component = bytes_per_pixel / 2;
    switch( pixelComponentType() ) { 
    case Image::UNSIGNED: {
      H3DFloat fv = getUnsignedValueAsFloat( pixel_data, 
                                             bytes_per_component );
      H3DFloat a = getUnsignedValueAsFloat( pixel_data + bytes_per_component, 
                                            bytes_per_component );
      return H3DUtil::RGBA( fv, fv, fv, a );
    }
    case Image::SIGNED: {
      H3DFloat fv = getSignedValueAsFloat( pixel_data, 
                                           bytes_per_component );
      H3DFloat a = getSignedValueAsFloat( pixel_data + bytes_per_component, 
                                          bytes_per_component );

      return H3DUtil::RGBA( fv, fv, fv, a );
    }
    case Image::RATIONAL: {
      H3DFloat fv = getRationalValueAsFloat( pixel_data, 
                                             bytes_per_component );
      H3DFloat a = getRationalValueAsFloat( pixel_data + bytes_per_component, 
                                             bytes_per_component );
      return H3DUtil::RGBA( fv, fv, fv, a );
    }
    };
  }
  case Image::RGB:
  case Image::VEC3: {
    unsigned int bytes_per_component = bytes_per_pixel / 3;
    switch( pixelComponentType() ) { 
    case Image::UNSIGNED: {
      H3DFloat r = getUnsignedValueAsFloat( pixel_data, 
                                            bytes_per_component );
      H3DFloat g = getUnsignedValueAsFloat( pixel_data + bytes_per_component, 
                                            bytes_per_component );
      H3DFloat b = getUnsignedValueAsFloat( pixel_data + 
                                            2 * bytes_per_component, 
                                            bytes_per_component );
      return H3DUtil::RGBA( r, g, b, 1 );
    }
    case Image::SIGNED: {
      H3DFloat r = getSignedValueAsFloat( pixel_data, 
                                          bytes_per_component );
      H3DFloat g = getSignedValueAsFloat( pixel_data + bytes_per_component, 
                                          bytes_per_component );
      H3DFloat b = getSignedValueAsFloat( pixel_data + 
                                          2 * bytes_per_component, 
                                          bytes_per_component );
      return H3DUtil::RGBA( r, g, b, 1 );
    }
    case Image::RATIONAL: {
      H3DFloat r = getSignedValueAsFloat( pixel_data, 
                                          bytes_per_component );
      H3DFloat g = getSignedValueAsFloat( pixel_data + bytes_per_component, 
                                          bytes_per_component );
      H3DFloat b = getSignedValueAsFloat( pixel_data + 
                                          2 * bytes_per_component, 
                                          bytes_per_component );
      return H3DUtil::RGBA( r, g, b, 1 );
    }
    };
  }
  case Image::BGR:  {
    unsigned int bytes_per_component = bytes_per_pixel / 3;
    switch( pixelComponentType() ) { 
    case Image::UNSIGNED: {
      H3DFloat b = getUnsignedValueAsFloat( pixel_data, 
                                            bytes_per_component );
      H3DFloat g = getUnsignedValueAsFloat( pixel_data + bytes_per_component, 
                                            bytes_per_component );
      H3DFloat r = getUnsignedValueAsFloat( pixel_data + 
                                            2 * bytes_per_component, 
                                            bytes_per_component );
      return H3DUtil::RGBA( r, g, b, 1 );
    }
    case Image::SIGNED: {
      H3DFloat b = getSignedValueAsFloat( pixel_data, 
                                          bytes_per_component );
      H3DFloat g = getSignedValueAsFloat( pixel_data + bytes_per_component, 
                                          bytes_per_component );
      H3DFloat r = getSignedValueAsFloat( pixel_data + 
                                          2 * bytes_per_component, 
                                          bytes_per_component );
      return H3DUtil::RGBA( r, g, b, 1 );
    }
    case Image::RATIONAL: {
      H3DFloat b = getSignedValueAsFloat( pixel_data, 
                                          bytes_per_component );
      H3DFloat g = getSignedValueAsFloat( pixel_data + bytes_per_component, 
                                          bytes_per_component );
      H3DFloat r = getSignedValueAsFloat( pixel_data + 
                                          2 * bytes_per_component, 
                                          bytes_per_component );
      return H3DUtil::RGBA( r, g, b, 1 );
    }
    };
  }
  case Image::RGBA:  {
    unsigned int bytes_per_component = bytes_per_pixel / 4;
    switch( pixelComponentType() ) { 
    case Image::UNSIGNED: {
      H3DFloat r = getUnsignedValueAsFloat( pixel_data, 
                                            bytes_per_component );
      H3DFloat g = getUnsignedValueAsFloat( pixel_data + bytes_per_component, 
                                            bytes_per_component );
      H3DFloat b = getUnsignedValueAsFloat( pixel_data + 
                                            2 * bytes_per_component, 
                                            bytes_per_component );
      H3DFloat a = getUnsignedValueAsFloat( pixel_data + 
                                            3 * bytes_per_component, 
                                            bytes_per_component );
      return H3DUtil::RGBA( r, g, b, a );
    }
    case Image::SIGNED: {
      H3DFloat r = getSignedValueAsFloat( pixel_data, 
                                          bytes_per_component );
      H3DFloat g = getSignedValueAsFloat( pixel_data + bytes_per_component, 
                                          bytes_per_component );
      H3DFloat b = getSignedValueAsFloat( pixel_data + 
                                          2 * bytes_per_component, 
                                          bytes_per_component );
      H3DFloat a = getSignedValueAsFloat( pixel_data + 
                                          3 * bytes_per_component, 
                                          bytes_per_component );
      return H3DUtil::RGBA( r, g, b, a );
    }
    case Image::RATIONAL: {
      H3DFloat r = getSignedValueAsFloat( pixel_data, 
                                          bytes_per_component );
      H3DFloat g = getSignedValueAsFloat( pixel_data + bytes_per_component, 
                                          bytes_per_component );
      H3DFloat b = getSignedValueAsFloat( pixel_data + 
                                          2 * bytes_per_component, 
                                          bytes_per_component );
      H3DFloat a = getSignedValueAsFloat( pixel_data + 
                                          3 * bytes_per_component, 
                                          bytes_per_component );
      return H3DUtil::RGBA( r, g, b, a );
    }
    };
  }
  case Image::BGRA: {
    unsigned int bytes_per_component = bytes_per_pixel / 4;
    switch( pixelComponentType() ) { 
    case Image::UNSIGNED: {
      H3DFloat b = getUnsignedValueAsFloat( pixel_data, 
                                            bytes_per_component );
      H3DFloat g = getUnsignedValueAsFloat( pixel_data + bytes_per_component, 
                                            bytes_per_component );
      H3DFloat r = getUnsignedValueAsFloat( pixel_data + 
                                            2 * bytes_per_component, 
                                            bytes_per_component );
      H3DFloat a = getUnsignedValueAsFloat( pixel_data + 
                                            3 * bytes_per_component, 
                                            bytes_per_component );
      return H3DUtil::RGBA( r, g, b, a );
    }
    case Image::SIGNED: {
      H3DFloat b = getSignedValueAsFloat( pixel_data, 
                                          bytes_per_component );
      H3DFloat g = getSignedValueAsFloat( pixel_data + bytes_per_component, 
                                          bytes_per_component );
      H3DFloat r = getSignedValueAsFloat( pixel_data + 
                                          2 * bytes_per_component, 
                                          bytes_per_component );
      H3DFloat a = getSignedValueAsFloat( pixel_data + 
                                          3 * bytes_per_component, 
                                          bytes_per_component );
      return H3DUtil::RGBA( r, g, b, a );
    }
    case Image::RATIONAL: {
      H3DFloat b = getSignedValueAsFloat( pixel_data, 
                                          bytes_per_component );
      H3DFloat g = getSignedValueAsFloat( pixel_data + bytes_per_component, 
                                          bytes_per_component );
      H3DFloat r = getSignedValueAsFloat( pixel_data + 
                                          2 * bytes_per_component, 
                                          bytes_per_component );
      H3DFloat a = getSignedValueAsFloat( pixel_data + 
                                          3 * bytes_per_component, 
                                          bytes_per_component );
      return H3DUtil::RGBA( r, g, b, a );
    }
    };
  }
  };
  return H3DUtil::RGBA();
}

void Image::RGBAToImageValue( const H3DUtil::RGBA &rgba, void *_pixel_data ) {
  using namespace ImageInternals;

  char *pixel_data = (char *) _pixel_data;

  unsigned int byte_rem = bitsPerPixel() % 8;
  unsigned int bytes_per_pixel = bitsPerPixel() / 8;

  assert( byte_rem == 0 );

  switch( pixelType() ) {
  case Image::LUMINANCE: 
    switch( pixelComponentType() ) { 
    case Image::UNSIGNED: {
      writeFloatAsUnsignedValue( rgba.r,
                                 pixel_data, 
                                 bytes_per_pixel );
      return;
    }
    case Image::SIGNED: {
      writeFloatAsSignedValue( rgba.r,
                               pixel_data, 
                               bytes_per_pixel );
      return;
    }
    case Image::RATIONAL: {
      writeFloatAsRationalValue( rgba.r,
                                 pixel_data, 
                                 bytes_per_pixel );
      return;
    }
    };
  case Image::LUMINANCE_ALPHA: {
    unsigned int bytes_per_component = bytes_per_pixel / 2;
    switch( pixelComponentType() ) { 
    case Image::UNSIGNED: {
      writeFloatAsUnsignedValue( rgba.r,
                                 pixel_data, 
                                 bytes_per_component );
      writeFloatAsUnsignedValue( rgba.a,
                                 pixel_data + bytes_per_component, 
                                 bytes_per_component );
      return;
    }
    case Image::SIGNED: {
      writeFloatAsUnsignedValue( rgba.r,
                                 pixel_data, 
                                 bytes_per_component );
      writeFloatAsUnsignedValue( rgba.a,
                                 pixel_data + bytes_per_component, 
                                 bytes_per_component );
      return;
    }
    case Image::RATIONAL: {
      writeFloatAsRationalValue( rgba.r,
                                 pixel_data, 
                                 bytes_per_component );
      writeFloatAsRationalValue( rgba.a,
                                 pixel_data + bytes_per_component, 
                                 bytes_per_component );
      return;
    }
    };
  }
  case Image::RGB:
  case Image::VEC3: {
    unsigned int bytes_per_component = bytes_per_pixel / 3;
    switch( pixelComponentType() ) { 
    case Image::UNSIGNED: {
      writeFloatAsUnsignedValue( rgba.r,
                                 pixel_data, 
                                 bytes_per_component );
      writeFloatAsUnsignedValue( rgba.g,
                                 pixel_data + bytes_per_component, 
                                 bytes_per_component );
      writeFloatAsUnsignedValue( rgba.b,
                                 pixel_data + 2 * bytes_per_component, 
                                 bytes_per_component );
      return;
    }
    case Image::SIGNED: {
      writeFloatAsSignedValue( rgba.r,
                               pixel_data, 
                               bytes_per_component );
      writeFloatAsSignedValue( rgba.g,
                               pixel_data + bytes_per_component, 
                               bytes_per_component );
      writeFloatAsSignedValue( rgba.b,
                               pixel_data + 2 * bytes_per_component, 
                               bytes_per_component );
      return;
    }
    case Image::RATIONAL: {
      writeFloatAsRationalValue( rgba.r,
                               pixel_data, 
                               bytes_per_component );
      writeFloatAsRationalValue( rgba.g,
                               pixel_data + bytes_per_component, 
                               bytes_per_component );
      writeFloatAsRationalValue( rgba.b,
                               pixel_data + 2 * bytes_per_component, 
                               bytes_per_component );
      return;
    }
    };
  }
  case Image::BGR:  {
    unsigned int bytes_per_component = bytes_per_pixel / 3;
    switch( pixelComponentType() ) { 
    case Image::UNSIGNED: {
      writeFloatAsUnsignedValue( rgba.b,
                                 pixel_data, 
                                 bytes_per_component );
      writeFloatAsUnsignedValue( rgba.g,
                                 pixel_data + bytes_per_component, 
                                 bytes_per_component );
      writeFloatAsUnsignedValue( rgba.r,
                                 pixel_data + 2 * bytes_per_component, 
                                 bytes_per_component );
      return;
    }
    case Image::SIGNED: {
      writeFloatAsSignedValue( rgba.b,
                               pixel_data, 
                               bytes_per_component );
      writeFloatAsSignedValue( rgba.g,
                               pixel_data + bytes_per_component, 
                               bytes_per_component );
      writeFloatAsSignedValue( rgba.r,
                               pixel_data + 2 * bytes_per_component, 
                               bytes_per_component );
      return;
    }
    case Image::RATIONAL: {
      writeFloatAsRationalValue( rgba.b,
                                 pixel_data, 
                                 bytes_per_component );
      writeFloatAsRationalValue( rgba.g,
                                 pixel_data + bytes_per_component, 
                                 bytes_per_component );
      writeFloatAsRationalValue( rgba.r,
                                 pixel_data + 2 * bytes_per_component, 
                                 bytes_per_component );
      return;
    }
    };
  }
  case Image::RGBA:  {
    unsigned int bytes_per_component = bytes_per_pixel / 4;
    switch( pixelComponentType() ) { 
    case Image::UNSIGNED: {
      writeFloatAsUnsignedValue( rgba.r,
                                 pixel_data, 
                                 bytes_per_component );
      writeFloatAsUnsignedValue( rgba.g,
                                 pixel_data + bytes_per_component, 
                                 bytes_per_component );
      writeFloatAsUnsignedValue( rgba.b,
                                 pixel_data + 2 * bytes_per_component, 
                                 bytes_per_component );
      writeFloatAsUnsignedValue( rgba.a,
                                 pixel_data + 3 * bytes_per_component, 
                                 bytes_per_component );
      return;
    }
    case Image::SIGNED: {
      writeFloatAsSignedValue( rgba.r,
                               pixel_data, 
                               bytes_per_component );
      writeFloatAsSignedValue( rgba.g,
                               pixel_data + bytes_per_component, 
                               bytes_per_component );
      writeFloatAsSignedValue( rgba.b,
                               pixel_data + 2 * bytes_per_component, 
                               bytes_per_component );
      writeFloatAsSignedValue( rgba.a,
                                 pixel_data + 3 * bytes_per_component, 
                                 bytes_per_component );
      return;
    }
    case Image::RATIONAL: {
      writeFloatAsRationalValue( rgba.r,
                                 pixel_data, 
                                 bytes_per_component );
      writeFloatAsRationalValue( rgba.g,
                                 pixel_data + bytes_per_component, 
                                 bytes_per_component );
      writeFloatAsRationalValue( rgba.b,
                                 pixel_data + 2 * bytes_per_component, 
                                 bytes_per_component );
      writeFloatAsRationalValue( rgba.a,
                                 pixel_data + 3 * bytes_per_component, 
                                 bytes_per_component );
      return;
    }

    };
  }
  case Image::BGRA: {
   unsigned int bytes_per_component = bytes_per_pixel / 4;
    switch( pixelComponentType() ) { 
    case Image::UNSIGNED: {
      writeFloatAsUnsignedValue( rgba.b,
                                 pixel_data, 
                                 bytes_per_component );
      writeFloatAsUnsignedValue( rgba.g,
                                 pixel_data + bytes_per_component, 
                                 bytes_per_component );
      writeFloatAsUnsignedValue( rgba.r,
                                 pixel_data + 2 * bytes_per_component, 
                                 bytes_per_component );
      writeFloatAsUnsignedValue( rgba.a,
                                 pixel_data + 3 * bytes_per_component, 
                                 bytes_per_component );
      return;
    }
    case Image::SIGNED: {
      writeFloatAsSignedValue( rgba.b,
                               pixel_data, 
                               bytes_per_component );
      writeFloatAsSignedValue( rgba.g,
                               pixel_data + bytes_per_component, 
                               bytes_per_component );
      writeFloatAsSignedValue( rgba.r,
                               pixel_data + 2 * bytes_per_component, 
                               bytes_per_component );
      writeFloatAsSignedValue( rgba.a,
                                 pixel_data + 3 * bytes_per_component, 
                                 bytes_per_component );
      return;
    }
    case Image::RATIONAL: {
      writeFloatAsRationalValue( rgba.b,
                                 pixel_data, 
                                 bytes_per_component );
      writeFloatAsRationalValue( rgba.g,
                                 pixel_data + bytes_per_component, 
                                 bytes_per_component );
      writeFloatAsRationalValue( rgba.r,
                                 pixel_data + 2 * bytes_per_component, 
                                 bytes_per_component );
      writeFloatAsRationalValue( rgba.a,
                                 pixel_data + 3 * bytes_per_component, 
                                 bytes_per_component );
      return;
    }

    };
  }
  };
}

unsigned int Image::nrPixelComponents() {
  PixelType pixel_type = pixelType();
  if( pixel_type == LUMINANCE ) return 1;
  if( pixel_type == LUMINANCE_ALPHA ) return 2;
  if( pixel_type == RGB ||
      pixel_type == BGR ||
      pixel_type == VEC3 ) return 3;
  if( pixel_type == RGBA ||
      pixel_type == BGRA ) return 4;
  return 0;
}

namespace ImageInternals {
  template< class A, class FloatType >
  void buildNormalizedData( FloatType *normalized_data, 
			    void *orig_data,
			    unsigned int nr_elements,
			    float scale,
			    float bias ) {
    A *d = (A*) orig_data;
    
    for (unsigned int i = 0; i < nr_elements; i++) {
      normalized_data[i] = 
	(d[ i ] / FloatType( numeric_limits<A >::max() ) ) * scale + bias;
      if( normalized_data[i] < 0 ) normalized_data[i] = 0;
    }
  }
  
  template< class FloatType >
  FloatType *convertToNormalizedData( Image *image ) {
    int width = image->width();
    int height = image->height();
    int depth = image->depth();
    int nr_voxels = width*height*depth;
    
    int scale = 1;
    int bias = 0;
    Image::PixelComponentType pixel_component_type = image->pixelComponentType();
    unsigned int bits_per_pixel = image->bitsPerPixel();

    unsigned int nr_components = image->nrPixelComponents();
    
    // allocate memory for normalized data.
    FloatType *normalized_data = NULL;
    try {
      normalized_data = new FloatType[nr_voxels];
    } catch (bad_alloc& ba) {
      Console(4) << ba.what() << endl;
      return NULL;
    }
    
    if( pixel_component_type ==Image::UNSIGNED ) {
      if( bits_per_pixel == 8*nr_components ) {
	  buildNormalizedData< unsigned char >( normalized_data, image->getImageData(), 
						nr_voxels * nr_components, (float)scale, (float)bias ); 
      } else if( bits_per_pixel == 16*nr_components ) {
	buildNormalizedData< unsigned short >( normalized_data, image->getImageData(), 
					       nr_voxels * nr_components, (float)scale, (float)bias );
      } else if( bits_per_pixel == 32*nr_components ) {
	  buildNormalizedData< unsigned int >( normalized_data, image->getImageData(), 
					       nr_voxels * nr_components, (float)scale, (float)bias ); 
      } else {
	return NULL;
      }
    } else if( pixel_component_type == Image::SIGNED ) { 
      if( bits_per_pixel == 8*nr_components ) {
	buildNormalizedData< char >( normalized_data, image->getImageData(), 
				     nr_voxels * nr_components, (float)scale, (float)bias ); 
      } else if( bits_per_pixel == 16*nr_components ) {
	buildNormalizedData< short >( normalized_data, image->getImageData(), 
				      nr_voxels * nr_components, (float)scale, (float)bias );
      } else if( bits_per_pixel == 32*nr_components ) {
	buildNormalizedData< int >( normalized_data, image->getImageData(), 
				    nr_voxels * nr_components, (float)scale, (float)bias ); 
      } else { 
	return NULL;
      }
    } else {
      return NULL;
    }
    return normalized_data;
  }
}
  
float *Image::convertToNormalizedFloatData() {
  return ImageInternals::convertToNormalizedData< float >( this );
}

double *Image::convertToNormalizedDoubleData() {
  return ImageInternals::convertToNormalizedData< double >( this );
}
