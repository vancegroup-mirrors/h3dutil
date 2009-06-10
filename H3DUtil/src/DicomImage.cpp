//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004-2008, SenseGraphics AB
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
/// \file DicomImage.cpp
/// \brief Image class for an image created from a Dicom file.
///
//
//////////////////////////////////////////////////////////////////////////////

#include <H3DUtil/DicomImage.h>

#ifdef HAVE_DCMTK 

//#define USE_INTERNAL_DICOM_DATA

// DCMTK includes
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcmetinf.h>
#include <dcmtk/dcmdata/dcuid.h>
#include <dcmtk/dcmdata/dcsequen.h>
#include <dcmtk/dcmimage/diregist.h> // required to support color images  
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmjpeg/djdecode.h>

#include <H3DUtil/LinAlgTypes.h>

using namespace H3DUtil;

H3DUtil::DicomImage::DicomImage( const string &url ):
  PixelImage( 0,0,0,0,RGB, UNSIGNED, NULL ) {

  dir_file_info.clear();
  dicom_file_info.loadFile( url.c_str() );

  OFString media_storage;
  DcmMetaInfo *meta_info = dicom_file_info.getMetaInfo();
  DcmDataset *dataset = dicom_file_info.getDataset();
  meta_info->findAndGetOFString( DCM_MediaStorageSOPClassUID, 
                                 media_storage );
  /// Check if the file is a DIRFILE specifiying the slices, i.e.
  /// the volume data is not in the file but each slice is in its own
  /// separate file specified by this DIRFILE.
  if( media_storage == UID_MediaStorageDirectoryStorage ) {
    string::size_type slash_pos = url.rfind("/");
    string::size_type backslash_pos = url.rfind("\\");
    string::size_type pos;
    if( slash_pos == string::npos ) {
      if( backslash_pos == string::npos )
        pos = string::npos;
      else 
        pos = backslash_pos;
    } else {
      if( backslash_pos == string::npos )
        pos = slash_pos;
      else
        pos = slash_pos < backslash_pos ? backslash_pos : slash_pos;
    }
    string path = url.substr( 0, pos + 1 );;
    DcmSequenceOfItems *dir_records;
    dataset->findAndGetSequence( DCM_DirectoryRecordSequence,
                                 dir_records );
    vector< string > urls( dir_records->card() );
    dir_file_info.resize( dir_records->card() );
    for( unsigned int i = 0; i < dir_records->card(); i++ ) {
      DcmItem *item = dir_records->getItem( i );
      OFString filename;
      item->findAndGetOFString( DCM_ReferencedFileID, filename, 2 );
      urls[i] = path + filename.c_str();
      dir_file_info[i].loadFile( urls[i].c_str() );
    }
    loadImage( urls );
  } else {
    loadImage( url );
  }
}

void H3DUtil::DicomImage::loadImage( const string &url ) {
  DJDecoderRegistration::registerCodecs();

  ::DicomImage *image = new ::DicomImage( url.c_str() );
  if (image->getStatus() != EIS_Normal)
    throw CouldNotLoadDicomImage( url,
                                  ::DicomImage::getString(image->getStatus()),
                                  H3D_FULL_LOCATION );
  w = image->getWidth();
  h = image->getHeight();
  d = image->getFrameCount();

  DcmDataset *dataset = dicom_file_info.getDataset();
  double size_x, size_y, size_z;
  if( dataset->findAndGetFloat64( DCM_PixelSpacing, size_x, 0 ).bad() )
    size_x = 1;
  if( dataset->findAndGetFloat64( DCM_PixelSpacing, size_y, 1 ).bad() )
    size_y = 1;
  if( dataset->findAndGetFloat64( DCM_SliceThickness, size_z ).bad() )
    size_z = 1;
  pixel_size = Vec3f( (H3DFloat) size_x,
                      (H3DFloat) size_y,
                      (H3DFloat) size_z ) * 0.001;

  // use the internal representation since that contains the original
  // pixel data. The getOutputData function provides some other representation
  // "after rendering" that contains overlays and such things. Have not 
  // figured out the format for this though. In a 16 bit signed dataset 0
  // will be mapped to 32767
  pixel_component_type = UNSIGNED;
#ifndef USE_INTERNAL_DICOM_DATA
  bits_per_pixel = image->getDepth();
  if( !isPowerOfTwo( bits_per_pixel ) ) {
    bits_per_pixel = nextPowerOfTwo( bits_per_pixel );
  }
  unsigned int frame_size = image->getOutputDataSize();
  image_data = new unsigned char[ frame_size * d ];
  for( unsigned int i = 0; i < d; i++ ) {
    
    
    image->getOutputData( &image_data[ i * frame_size ], 
                          frame_size, 
                          bits_per_pixel, i );
    
    
  }
#else
  // geOutputDataSize cannot be used since it produces different values
  // for bitsPerSample if rescaleSlope and rescaleIntercept is defined
  // in the dicom file.
  switch (image->getInterData()->getRepresentation()) {
  case EPR_Uint8:
    bits_per_pixel = 8;
    pixel_component_type = UNSIGNED; 
    break;
  case EPR_Uint16:
    bits_per_pixel = 16;
    pixel_component_type = UNSIGNED; 
    break;
  case EPR_Uint32: 
    bits_per_pixel = 32;
    pixel_component_type = UNSIGNED; 
    break;
  case EPR_Sint8: 
    bits_per_pixel = 8;
    pixel_component_type = SIGNED; 
    break;
  case EPR_Sint16:
    bits_per_pixel = 16;
    pixel_component_type = SIGNED; 
    break;
  case EPR_Sint32:
    bits_per_pixel = 32;
    pixel_component_type = SIGNED; 
    break;
  }
  unsigned int frame_size = w * h * d * bits_per_pixel / 8;
  image_data = new unsigned char[ frame_size * d ];
  
  memcpy(image_data,
         image->getInterData()->getData(),
         frame_size * d );
#endif
 
  if( image->isMonochrome() )
    pixel_type = LUMINANCE;
  else {
    bits_per_pixel = bits_per_pixel * 3; // RGB image
    pixel_type = RGB; 
  }

  delete image;
  DJDecoderRegistration::cleanup();
}

void H3DUtil::DicomImage::loadImage( const vector< string > &urls ) {
  if( urls.size() > 0 ) {
    ::DicomImage *image = new ::DicomImage( urls[0].c_str() );
    if (image->getStatus() != EIS_Normal)
      throw CouldNotLoadDicomImage( urls[0],
                                    ::DicomImage::getString(image->getStatus()),
                                    H3D_FULL_LOCATION );
    w = image->getWidth();
    h = image->getHeight();
    d = urls.size();
    bits_per_pixel = image->getDepth();
    if( !isPowerOfTwo( bits_per_pixel ) ) {
      bits_per_pixel = nextPowerOfTwo( bits_per_pixel );
    }
    
    DcmDataset *dataset = dir_file_info[0].getDataset();
    double size_x, size_y, size_z;
    if( dataset->findAndGetFloat64( DCM_PixelSpacing, size_x, 0 ).bad() )
      size_x = 1;
    if( dataset->findAndGetFloat64( DCM_PixelSpacing, size_y, 1 ).bad() )
    size_y = 1;
    if( dataset->findAndGetFloat64( DCM_SliceThickness, size_z ).bad() )
      size_z = 1;
    pixel_size = Vec3f( (H3DFloat) size_x,
                        (H3DFloat) size_y,
                        (H3DFloat) size_z ) * 0.001;

    unsigned int bits_per_component = bits_per_pixel;
    if( image->isMonochrome() )
      pixel_type = LUMINANCE;
    else {
      bits_per_pixel = bits_per_pixel * 3; // RGB image
      pixel_type = RGB; 
    }
    
    pixel_component_type = UNSIGNED;
    
    unsigned int frame_size = image->getOutputDataSize();
    image_data = new unsigned char[ frame_size * d ];

    image->getOutputData( &image_data[ 0 ], 
                          frame_size, 
                          bits_per_component, 0 ); 
    delete image;
    for( unsigned int i = 1; i < urls.size(); i++ ) {
      image = new ::DicomImage( urls[i].c_str() );
      if (image->getStatus() != EIS_Normal) 
        throw CouldNotLoadDicomImage( urls[i],
                                      ::DicomImage::getString(image->getStatus()),
                                      H3D_FULL_LOCATION );
      assert( w == image->getWidth() && 
              h == image->getHeight() && 
              image->getFrameCount() == 1 );
      image->getOutputData( &image_data[ i * frame_size ], 
                            frame_size, 
                            bits_per_component, 0 ); 
      delete image;
    }
  }
}


/// Convert a pixel value(range 0 to 1) to the corresponing hounsfield value
/// from the dicom file.
H3DFloat H3DUtil::DicomImage::pixelToHounsfieldValue( H3DFloat pixel_value ) {
  DcmDataset *dataset = dicom_file_info.getDataset();
  double slope, intercept;
  if( dataset->findAndGetFloat64( DCM_RescaleSlope, slope ).bad() )
    slope = 1;
  if( dataset->findAndGetFloat64( DCM_RescaleIntercept, intercept ).bad() )
    intercept = 0;

  return (H3DFloat) (pixel_value * slope + intercept);
}

/// Convert a hounsfield value to the corresponding pixel value(range 0 to 1)
/// from the dicom file.
H3DFloat H3DUtil::DicomImage::hounsfieldToPixelValue( H3DFloat hv ) {
  DcmDataset *dataset = dicom_file_info.getDataset();
  double slope, intercept;
  if( dataset->findAndGetFloat64( DCM_RescaleSlope, slope ).bad() )
    slope = 1;
  if( dataset->findAndGetFloat64( DCM_RescaleIntercept, intercept ).bad() )
    intercept = 0;

  return (H3DFloat) ((hv - intercept) / slope);
}


#endif //HAVE_DKMTK
