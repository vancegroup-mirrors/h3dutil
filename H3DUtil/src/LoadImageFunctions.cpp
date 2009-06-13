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

#define TEEM_STATIC
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
      case FIC_MINISBLACK: 
      case FIC_MINISWHITE:
#ifdef FREEIMAGE_BIGENDIAN
      case FIC_RGB: return RGB;
      case FIC_RGBALPHA: return RGBA;
#else
      case FIC_RGB:
      case FIC_RGBALPHA: break;
#endif
      default: {
        Console(3) << "Warning: UnsupportedFreeImageColorType " << t
             << ". File " << url << " can not be loaded. "
             << "File name might be the name of a downloaded temporary file. "
             << endl;
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
//    unsigned long int uncompressed_size = expected_size;
    
    int err;
    z_stream strm = {
      data, actual_size, 0,
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
  
  /* read in the nrrd header from file */
  if (nrrdLoad(nin, url.c_str(), NULL)) {
    nrrdNuke( nin );
    return NULL;
  }

  Image::PixelType pixel_type = Image::LUMINANCE;
  Image::PixelComponentType component_type;
  unsigned int bits_per_pixel;

  if( nin->type == nrrdTypeChar ) {
    component_type = Image::SIGNED;
    bits_per_pixel = 8;
  } else  if( nin->type == nrrdTypeUChar ) {
    component_type = Image::UNSIGNED;
    bits_per_pixel = 8;
  } else  if( nin->type == nrrdTypeShort ) {
    component_type = Image::SIGNED;
    bits_per_pixel = 16;
  } else  if( nin->type == nrrdTypeUShort ) {
    component_type = Image::UNSIGNED;
    bits_per_pixel = 16;
  } else  if( nin->type == nrrdTypeInt ) {
    component_type = Image::SIGNED;
    bits_per_pixel = 32;
  } else  if( nin->type == nrrdTypeUInt ) {
    component_type = Image::UNSIGNED;
    bits_per_pixel = 32;
  } else  if( nin->type == nrrdTypeLLong ) {
    component_type = Image::SIGNED;
    bits_per_pixel = 64;
  } else  if( nin->type == nrrdTypeULLong ) {
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

  Vec3f spacing = Vec3f( 0.0003, 0.0003, 0.0003 );

  unsigned w_axis = 0, h_axis = 1, d_axis = 2;
  
  if( nin->dim == 4 ) {
    // if dimension == 4, we assume the first dimension is used for each 
    // voxel value
    w_axis = 1;
    h_axis = 2;
    d_axis = 3;

    int nr_components = nin->axis[0].size;
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
    depth = nin->axis[d_axis].size;
  if(!airIsNaN(nin->axis[d_axis].spacing))
    spacing.z = nin->axis[d_axis].spacing;
  else
    Console(3) << "Warning: NRRD file " << url
         << " lacks spacing information in axis 2. Sets to default 0.0003\n";
  }

  if( nin->dim >= 2 ) {
    height = nin->axis[h_axis].size;
  if(!airIsNaN(nin->axis[h_axis].spacing))
    spacing.y = nin->axis[h_axis].spacing;
  else
    Console(3) << "Warning: NRRD file " << url
         << " lacks spacing information in axis 1. Sets to default 0.0003\n";
  }

  if( nin->dim >= 1 ) {
    width = nin->axis[w_axis].size;
  if(!airIsNaN(nin->axis[w_axis].spacing))
    spacing.x = nin->axis[w_axis].spacing;
  else
    Console(3) << "Warning: NRRD file " << url
         << " lacks spacing information in axis 0. Sets to default 0.0003\n";
  }

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
      return new DicomImage( url );
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
        memcpy( data + width * height * depth * bytes_per_pixel, 
                slice_2d->getImageData(), 
                width * height * bytes_per_pixel );
        depth++;
      }
    }
   
    return new PixelImage( width, height, depth, 
                           bits_per_pixel, pixel_type, component_type,
                           data, false, pixel_size );
  }
}
#endif
