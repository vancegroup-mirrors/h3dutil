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
/// \file LoadImageFunctions.cpp
/// \brief .cpp file for functions to load images
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
#endif // HAVE_FREEIMAGE

#ifdef HAVE_TEEM
#include <H3DUtil/PixelImage.h>

//#define TEEM_STATIC
#include <teem/nrrd.h>
#include <teem/air.h>
#endif // HAVE_TEEM

#ifdef HAVE_DCMTK
#ifndef WIN32 
// for Unix platforms this has to be defined before including anything
// from dcmtk
#define HAVE_CONFIG_H
#endif
#include <dcmtk/dcmdata/dcmetinf.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/dcmdata/dcdeftag.h>

#ifndef WIN32 
#include <dirent.h>
#endif
#include <algorithm>

#endif // HAVE_DCMTK

#include <H3DUtil/PixelImage.h>
#include <H3DUtil/DicomImage.h>
#include <fstream>
#include <memory>

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

      FREE_IMAGE_COLOR_TYPE t = FreeImage_GetColorType( bm );
      switch( t ) {
      case FIC_PALETTE: {
        // We have a palatted image. Convert to RGB or RGBA.
        RGBQUAD *palette = FreeImage_GetPalette( bm );
        BYTE *transparency_table = FreeImage_GetTransparencyTable( bm );
        bool is_transparent = FreeImage_GetTransparencyCount( bm ) > 0;

        Image::PixelType pixel_type = is_transparent ? Image::RGBA : Image::RGB;
        Image::PixelComponentType pixel_component_type = Image::UNSIGNED;
        unsigned int width =  FreeImage_GetWidth( bm );
        unsigned int height =  FreeImage_GetHeight( bm );
        unsigned int depth =  1;
        unsigned int bytes_per_pixel = is_transparent ? 4 : 3;
        unsigned int size = width * height * depth * bytes_per_pixel;

        // build the new pixel data
        unsigned char *data = new unsigned char[ size ];
        for( unsigned int y = 0; y < height; y++ ) {
          for( unsigned int x = 0; x < width; x++ ) {
            unsigned int i = (x + y * width) * bytes_per_pixel;
            BYTE index;
            FreeImage_GetPixelIndex( bm, x, y, &index );
            data[ i ] = palette[index].rgbRed;
            data[ i + 1 ] = palette[index].rgbGreen;
            data[ i + 2 ] = palette[index].rgbBlue;
            if( is_transparent ) {
              data[ i + 3 ] = transparency_table[index];
            }
          }
        }

        return new PixelImage( width,
                               height,
                               depth,
                               bytes_per_pixel * 8,
                               pixel_type,
                               pixel_component_type,
                               data,
                               false,
                               Vec3f( 1, 1, 1 ) );

      }
      case FIC_MINISBLACK: 
      case FIC_MINISWHITE:
      case FIC_RGB:
      case FIC_RGBALPHA: break;
      default: {
        Console(3) << "Warning: UnsupportedFreeImageColorType " << t
             << ". File " << url << " can not be loaded. "
             << "File name might be the name of a downloaded temporary file. "
             << endl;

        FreeImage_Unload( bm );
        return NULL;
      }
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
    ( raw_image_info.bits_per_pixel / 8 );

  unsigned char * data = new unsigned char[expected_size];
  
  ifstream is( url.c_str(), ios::in | ios::binary );
  if( !is.good() ) {
    return NULL;
  }
  
  is.read( (char *)data, expected_size );
  unsigned int actual_size = (unsigned int) is.gcount();
  is.close();
  
#ifdef HAVE_ZLIB
  
  if( actual_size < expected_size ){
    
    unsigned char * data2 = new unsigned char[expected_size];
//    unsigned long int uncompressed_size = expected_size;
    
    int err;
    z_stream strm = {
      data, (uInt)actual_size, 0,
      data2, expected_size, 0
    };
    
    err = inflateInit2(&strm,47);
    
    if( err == Z_MEM_ERROR ){
      Console(3) << "Warning: zlib memory error." << endl;
      delete[] data;
      delete[] data2;
      return NULL;
    }
    if( err == Z_VERSION_ERROR ){
      Console(3) << "Warning: zlib version error." << endl;
      delete[] data;
      delete[] data2;
      return NULL;
    }
    
    err = inflate(&strm,Z_FINISH);
    
    if( err == Z_DATA_ERROR ){
      Console(3) << "Warning: zlib unrecognizable data error." << endl;
      delete[] data;
      delete[] data2;
      return NULL;
    }
    if( err == Z_STREAM_ERROR ){
      Console(3) << "Warning: zlib stream error." << endl;
      delete[] data;
      delete[] data2;
      return NULL;
    }
    if( err == Z_BUF_ERROR ){
      Console(3) << "Warning: zlib out of memory error." << endl;
      delete[] data;
      delete[] data2;
      return NULL;
    }
    
    err = inflateEnd(&strm);
    
    if( err == Z_STREAM_ERROR ){
      Console(3) << "Warning: zlib stream error." << endl;
      delete[] data;
      delete[] data2;
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

#ifdef HAVE_TEEM
Image *H3DUtil::loadNrrdFile( const string &url ) {
  Nrrd *nin;
  
  /* create a new nrrd */
  nin = nrrdNew();
  
  /* tell nrrdLoad to only read the header, not the data */
  NrrdIoState *nio = nrrdIoStateNew();
  nrrdIoStateSet(nio, nrrdIoStateSkipData, AIR_TRUE);

  // read in the nrrd header from file , not the data.
  // The reason for this approach is that H3DUtil will allocate the data later
  // to make sure that there are no problems with allocating data over
  // boundaries for shared libraries in Windows.
  if( nrrdLoad( nin, url.c_str(), nio ) ) {
    nio = nrrdIoStateNix(nio);
    nrrdNuke(nin);
    return NULL;
  }
  
  /* we're done with the nrrdIoState, this sets it to NULL */
  nio = nrrdIoStateNix(nio);

  Image::PixelType pixel_type = Image::LUMINANCE;
  Image::PixelComponentType component_type;
  unsigned int bits_per_pixel;

  if( nin->type == nrrdTypeChar ) {
    component_type = Image::SIGNED;
    bits_per_pixel = 8;
  } else if( nin->type == nrrdTypeUChar ) {
    component_type = Image::UNSIGNED;
    bits_per_pixel = 8;
  } else if( nin->type == nrrdTypeShort ) {
    component_type = Image::SIGNED;
    bits_per_pixel = 16;
  } else if( nin->type == nrrdTypeUShort ) {
    component_type = Image::UNSIGNED;
    bits_per_pixel = 16;
  } else if( nin->type == nrrdTypeInt ) {
    component_type = Image::SIGNED;
    bits_per_pixel = 32;
  } else if( nin->type == nrrdTypeUInt ) {
    component_type = Image::UNSIGNED;
    bits_per_pixel = 32;
  } else if( nin->type == nrrdTypeLLong ) {
    component_type = Image::SIGNED;
    bits_per_pixel = 64;
  } else if( nin->type == nrrdTypeULLong ) {
    component_type = Image::UNSIGNED;
    bits_per_pixel = 64;
  } else if( nin->type == nrrdTypeFloat ) {
    component_type = Image::RATIONAL;
    bits_per_pixel = 32;
  } else  if( nin->type == nrrdTypeDouble ) {
    component_type = Image::RATIONAL;
    bits_per_pixel = 64;
  } else {
    nrrdNuke(nin);
    return NULL;
  }
  
  unsigned int width = 1, height = 1, depth = 1;

  Vec3f spacing = Vec3f( 0.0003f, 0.0003f, 0.0003f );

  unsigned w_axis = 0, h_axis = 1, d_axis = 2;
  
  if( nin->dim == 4 ) {
    // if dimension == 4, we assume the first dimension is used for each 
    // voxel value
    w_axis = 1;
    h_axis = 2;
    d_axis = 3;

    size_t nr_components = nin->axis[0].size;
    if( nr_components == 2 ) {
      pixel_type = Image::LUMINANCE_ALPHA;
      bits_per_pixel *= 2;
    } else if( nr_components == 3 ) {
      pixel_type = Image::RGB;
      bits_per_pixel *= 3;
    } else if( nr_components == 4 ) {
      pixel_type = Image::RGBA;
      bits_per_pixel *= 4;
    }
  }
   
  if( nin->dim >= 3 ) {
    depth = (unsigned int) nin->axis[d_axis].size;
  if(!airIsNaN(nin->axis[d_axis].spacing))
    spacing.z = (H3DFloat)( nin->axis[d_axis].spacing );
  else
    Console(3) << "Warning: NRRD file " << url
         << " lacks spacing information in axis 2. Sets to default 0.0003\n";
  }

  if( nin->dim >= 2 ) {
    height = (unsigned int) nin->axis[h_axis].size;
  if(!airIsNaN(nin->axis[h_axis].spacing))
    spacing.y = (H3DFloat)( nin->axis[h_axis].spacing );
  else
    Console(3) << "Warning: NRRD file " << url
         << " lacks spacing information in axis 1. Sets to default 0.0003\n";
  }

  if( nin->dim >= 1 ) {
    width = (unsigned int) nin->axis[w_axis].size;
  if(!airIsNaN(nin->axis[w_axis].spacing))
    spacing.x = (H3DFloat)( nin->axis[w_axis].spacing );
  else
    Console(3) << "Warning: NRRD file " << url
         << " lacks spacing information in axis 0. Sets to default 0.0003\n";
  }

  // Allocate the data.
  // The reason for allocating the memory for the data ourself instead of
  // letting the teem library take care of that is to make sure that there
  // are no problems with deleting data from H3DUtil that is allocated
  // in another shared library on a Windows system.
  unsigned int size = (width * height * depth * bits_per_pixel)/8;
  unsigned char * data = new unsigned char[ size ];
  nin->data = data;
  if( nrrdLoad( nin, url.c_str(), NULL ) ) {
    // free nrrd struct memory but not data.
    nrrdNix(nin);
    delete [] data;
    return NULL;
  }
  // We assume this will work well on other systems, note that this is not
  // yet tested properly.
  Image *image =  new PixelImage( width, height, depth, bits_per_pixel,
                                  pixel_type, component_type,
                                  (unsigned char *)nin->data,
                                  false, spacing );
  // free nrrd struct memory but not data.
  nrrdNix(nin);
  return image;
}

int H3DUtil::saveImageAsNrrdFile( const string &filename,
                                  Image *image ) {
    Nrrd *nin;
  
  /* create a new nrrd */
  nin = nrrdNew();

  unsigned int bits_per_pixel = image->bitsPerPixel();
 
  Image::PixelComponentType component_type = image->pixelComponentType();
  Image::PixelType pixel_type = image->pixelType();
  const Vec3f &voxel_size = image->pixelSize(); 

  // dimensions
  unsigned int nr_components = 1;
  if( pixel_type == Image::LUMINANCE ) {
    nr_components = 1;
  } else if( pixel_type == Image::LUMINANCE_ALPHA ) {
    nr_components = 2;
  } else if( pixel_type == Image::BGR || pixel_type == Image::RGB || 
             pixel_type == Image::VEC3) {
    nr_components = 3;
  } else if( pixel_type == Image::RGBA || pixel_type == Image::BGRA ) {
    nr_components = 4;
  } else {
    return -1;
  }

  int bytes_per_component =  bits_per_pixel / (nr_components * 8);
  if( bits_per_pixel % (nr_components * 8) > 0 ) bytes_per_component++;

  // save the type
  if( component_type == Image::SIGNED ) {
    if( bytes_per_component == 1 ) nin->type = nrrdTypeChar;
    else if( bytes_per_component == 2 ) nin->type = nrrdTypeShort;
    else if( bytes_per_component == 4 ) nin->type = nrrdTypeInt;
    else if( bytes_per_component == 8 ) nin->type = nrrdTypeLLong;
    else return -1; 
  } else if( component_type == Image::UNSIGNED ) {
    if( bytes_per_component == 1 ) nin->type = nrrdTypeUChar;
    else if( bytes_per_component == 2 ) nin->type = nrrdTypeUShort;
    else if( bytes_per_component == 4 ) nin->type = nrrdTypeUInt;
    else if( bytes_per_component == 8 ) nin->type = nrrdTypeULLong;
    else return -1; 
  } else  if( component_type == Image::RATIONAL ) {
    if(  bytes_per_component == 4 ) nin->type = nrrdTypeFloat;
    else if( bytes_per_component == 8 ) nin->type = nrrdTypeDouble;
    else return -1; 
  } else {
    return -1; 
  }



  nin->dim = 4;
  nin->axis[0].size = nr_components;
  nin->axis[1].size = image->width();
  nin->axis[2].size = image->height();
  nin->axis[3].size = image->depth();

  nin->axis[1].spacing = voxel_size.x;
  nin->axis[2].spacing = voxel_size.y;
  nin->axis[3].spacing = voxel_size.z;

  nin->data = image->getImageData();

  int res = nrrdSave( filename.c_str(), nin, NULL );
  char *err = biffGetDone( NRRD );

  // free nrrd struct memory but not data.
  nrrdNix( nin );

  return res;
}
#endif

#ifdef HAVE_DCMTK
H3DUTIL_API Image *H3DUtil::loadDicomFile( const string &url,
                                           bool load_single_file ) {
  if( load_single_file ) {
   try {
     // dicom data is specified from topleft corner. we have to convert it so
     // it is specified from the bottomleft corner
     auto_ptr< DicomImage > slice_2d( new DicomImage( url ) );

     // get properties from dicom image
     unsigned int width  = slice_2d->width();
     unsigned int height = slice_2d->height();
     // depth will be incremented below depending on the number of slices
     // used
     unsigned int depth = 1;
     Vec3f pixel_size = slice_2d->pixelSize();
     Image::PixelType pixel_type = slice_2d->pixelType();
     unsigned int bits_per_pixel = slice_2d->bitsPerPixel();
     Image::PixelComponentType component_type = 
       slice_2d->pixelComponentType();    
     
     unsigned bytes_per_pixel = 
       bits_per_pixel % 8 == 0 ? 
       bits_per_pixel / 8 : bits_per_pixel / 8 + 1;
     
     // allocate and copy data into correct order.
     unsigned char *data = 
       new unsigned char[ width * height * bytes_per_pixel ];
     
     unsigned char * slice_data = (unsigned char *)slice_2d->getImageData();
     for( unsigned int row = 0; row < height; row++ ) {
       memcpy( data + row*width* bytes_per_pixel, 
               slice_data + (height - row - 1 ) * width * bytes_per_pixel,
               width * bytes_per_pixel );
     }

     // return new image with the correct row order.
     return new PixelImage( width, height, depth, 
                            bits_per_pixel, pixel_type, component_type,
                            data, false, pixel_size );

    } catch( const DicomImage::CouldNotLoadDicomImage &e ) {
      cerr << e << endl;
      return NULL;
    }
  } else {
    // the filenames for all files to compose.
    vector< string > filenames;

    // divide url into path and filename
    size_t found = url.find_last_of("/\\");
    string path, filename;

    if( found != string::npos ) {
      path = url.substr(0,found);
      filename = url.substr(found+1);
    } else {
      path = "";
      filename = url;
    }

    // find files in the same directory as the original file that starts
    // with the same characters.
#ifdef WIN32
    LPWIN32_FIND_DATA find_data = new WIN32_FIND_DATA;
    HANDLE handle = FindFirstFile( 
      (path + "/" + filename.substr( 0,3 ) +"*" ).c_str(), find_data );
    if( handle != INVALID_HANDLE_VALUE ) {
      string name = path + "\\" + find_data->cFileName;
      filenames.push_back( name );
      //Console(3) << name << endl;
      while( FindNextFile(handle, find_data) ) {
        //Console(3) << find_data->cFileName << endl;
        filenames.push_back( string( path + "\\" + find_data->cFileName ) );
      }
    }
    delete find_data;
#else
    string prefix = filename.substr( 0,3 );
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(path.c_str())) != NULL) {
      while ((dirp = readdir(dp)) != NULL) {
        string name = string(dirp->d_name);
        if( prefix == name.substr( 0,3 ) ) {
          filenames.push_back( string( path.c_str() ) + "/" + name );
        }
      }
      closedir(dp);
    }
#endif

    // sort them in alphabetical order
    std::sort( filenames.begin(), filenames.end() );

    if( filenames.empty() ) return NULL;

    auto_ptr< DicomImage > slice_2d;
    
    // read the original slice in order to get image information
    try {
      slice_2d.reset( new DicomImage( url ) );
    } catch( const DicomImage::CouldNotLoadDicomImage &e ) {
      Console(3) << e << endl;
      return NULL;
    }

    unsigned int width  = slice_2d->width();
    unsigned int height = slice_2d->height();
    // depth will be incremented below depending on the number of slices
    // used
    unsigned int depth = 0;
    Vec3f pixel_size = slice_2d->pixelSize();
    Image::PixelType pixel_type = slice_2d->pixelType();
    unsigned int bits_per_pixel = slice_2d->bitsPerPixel();
    Image::PixelComponentType component_type = 
      slice_2d->pixelComponentType();    

    unsigned bytes_per_pixel = 
      bits_per_pixel % 8 == 0 ? 
      bits_per_pixel / 8 : bits_per_pixel / 8 + 1;

    unsigned char *data = 
      new unsigned char[ width * height * filenames.size() * bytes_per_pixel ];

    DcmDataset *orig_data_set = slice_2d->getDicomFileInfo().getDataset();

    // get the series unique id for the original file
    OFString orig_series_instance_UID;
    OFCondition res = orig_data_set->findAndGetOFString( DCM_SeriesInstanceUID, 
                                                         orig_series_instance_UID );
   

    bool use_all_files = (res != EC_Normal || orig_series_instance_UID == "" );

    // Checking the z value of the ImagePositionPatient to know in which
    // order the dicom files should be read.
    // In the future these might be changed to Vec3f and we check
    // the entire vector to know which order to read the images.
    H3DFloat patient_pos1;
    H3DFloat patient_pos2;
    H3DFloat patient_orn[6];
    bool first_patient_pos_set = false;
    // Iterate through filename and get the two first valid ones and try
    // to read information from them. If information is read from two, then
    // break. This could perhaps be coupled with the other for loop.
    for( unsigned int i = 0; i < filenames.size(); i++ ) {
      DcmFileFormat fileformat;
      if( fileformat.loadFile(filenames[i].c_str()).good() ) {
        OFString string_value;
        DcmDataset * dataset = fileformat.getDataset();
        OFCondition res = dataset->findAndGetOFString( DCM_SeriesInstanceUID,
                                                       string_value );
        // only use the files that match the series instance of the original 
        // file.
        if( use_all_files ||
            string_value == orig_series_instance_UID ) {
          OFCondition res = dataset->findAndGetOFString(
            DCM_ImagePositionPatient, string_value, 2 );
          if( res == EC_Normal ) {
            if( first_patient_pos_set ) {
              patient_pos2 = (H3DFloat)( atof( string_value.c_str() ) );
              OFString string_value2;
              OFCondition res2 = dataset->findAndGetOFStringArray(
                DCM_ImageOrientationPatient, string_value2 );
              if( res2 == EC_Normal ) {
                size_t start_pos = 0;
                for( unsigned int j = 0; j < 6; j++ ) {
                  size_t pos = string_value2.find( "\\", start_pos, 2 );
                  patient_orn[j] = (H3DFloat)( atof( string_value2.substr(
                    start_pos, pos - start_pos ).c_str() ) );
                  start_pos = pos + 1;
                }
                if( patient_pos2 > patient_pos1 ) {
                  // Sort in reverse alphabetical order.
                  vector< string > tmp_filenames;
                  tmp_filenames.reserve( filenames.size() );
                  for( int j = (int)filenames.size() - 1; j >= 0; j-- )
                    tmp_filenames.push_back( filenames[j] );
                  filenames = tmp_filenames;
                }

                if( H3DAbs( patient_orn[0] - 1 ) > Constants::f_epsilon ||
                    H3DAbs( patient_orn[1] - 0 ) > Constants::f_epsilon ||
                    H3DAbs( patient_orn[2] - 0 ) > Constants::f_epsilon ||
                    H3DAbs( patient_orn[3] - 0 ) > Constants::f_epsilon ||
                    H3DAbs( patient_orn[4] - 1 ) > Constants::f_epsilon ||
                    H3DAbs( patient_orn[5] - 0 ) > Constants::f_epsilon ) {
                  Console(3) << "Warning: ImageOrientationPatient is not "
                             << "the assumed default. Dicom image might not "
                             << "be read correctly." << endl;
                }
                break;
              }
            } else {
              // Set patient_pos1 and break.
              patient_pos1 = (H3DFloat)( atof( string_value.c_str() ) );
              first_patient_pos_set = true;
            }
          }
        }
      }
    }

    // read all files and compose them into one image.
    for( unsigned int i = 0; i < filenames.size(); ++i ) {
      DcmFileFormat fileformat;
      OFString series_instance_UID;

      if (fileformat.loadFile(filenames[i].c_str()).good()) {
        DcmDataset *dataset = fileformat.getDataset();
        OFCondition res = dataset->findAndGetOFString( DCM_SeriesInstanceUID,
                                                       series_instance_UID );
      }
     
      // only use the files that match the seris instance of the original 
      // file.
      if( use_all_files || series_instance_UID == orig_series_instance_UID ) {
        try {
          slice_2d.reset( new DicomImage( filenames[i] ) );
        } catch( const DicomImage::CouldNotLoadDicomImage &e ) {
          Console(3) << e << endl;
          delete [] data;
          return NULL;
        }

        // dicom data is specified from topleft corner. we have to convert it
        // so it is specified from the bottomleft corner
        unsigned char * slice_data = (unsigned char *)slice_2d->getImageData();
        for( unsigned int row = 0; row < height; row++ ) {
          memcpy( data + (width * height * depth  + row*width)*
                         bytes_per_pixel,
                  slice_data + (height - row - 1 ) * width * bytes_per_pixel,
                  width * bytes_per_pixel );
        }
        depth++;
      }
    }
   
    return new PixelImage( width, height, depth, 
                           bits_per_pixel, pixel_type, component_type,
                           data, false, pixel_size );
  }
}
#endif
