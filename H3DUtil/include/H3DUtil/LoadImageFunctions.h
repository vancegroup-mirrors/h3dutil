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
/// \file LoadImageFunctions.h
/// \brief Header file which contains functions for loading image files.
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
  /// \param url The url of the image to load.
  /// \returns A pointer to and Image class containing the data
  /// of the loaded url.
  H3DUTIL_API Image *loadFreeImage( const string &url );
#endif

#ifdef HAVE_TEEM
  /// \ingroup ImageLoaderFunctions
  /// Loads a file in the Nrrd file format as an image.
  /// \param url The url of the image to load.
  /// \returns A pointer to and Image class containing the data
  /// of the loaded url. NULL if unsuccessful.
  H3DUTIL_API Image *loadNrrdFile( const string &url );

  /// \ingroup ImageLoaderFunctions
  /// Saves an image in the Nrrd file format.
  /// \param url The filename to save to.
  /// \param image The image to save.
  /// \returns 0 on success.
  H3DUTIL_API int saveImageAsNrrdFile( const string &url,
                                       Image *image );
#endif

#ifdef HAVE_DCMTK
  /// \ingroup ImageLoaderFunctions
  /// Loads a file in the DICOM file format as an image.
  /// The DICOM standard (section C.7.6.2.1.1) says the following:
  ///  "The %Image Position (0020,0032) specifies the x, y, and z coordinates of
  /// the upper left hand corner of the image; it is the center of the first 
  /// voxel transmitted. %Image Orientation (0020,0037) specifies the direction
  /// cosines of the first row and the first column with respect to the
  /// patient.
  ///
  /// These Attributes shall be provide as a pair. Row value for the x, y, and
  /// z axes respectively followed by the Column value for the x, y, and z axes
  /// respectively. The direction of the axes is defined fully by the patient's
  /// orientation. The x-axis is increasing to the left hand side of the
  /// patient. The y-axis is increasing to the posterior side of the patient.
  /// The z-axis is increasing toward the head of the patient.
  ///
  /// The patient based coordinate system is a right handed system"
  /// Basically this means that the ImagePositionPatient and
  /// ImageOrientationPatient data entries in dicom file should be used when
  /// positioning the data in world space. Since this function loads the file
  /// as an Image with three dimension the ImageOrientationPatient is assumed
  /// to be the default value of "1/0/0/0/1/0". If this is not the case a
  /// warning is printed to the Console. Because the image class specifies
  /// origo in the bottom left corner whereas the DICOM class specifies origo
  /// in the top left corner the y-coordinate will be flipped. This means that
  /// if the top left corner of the DICOM image is desired the texture
  /// coordinate (0,1) have to be used.
  ///
  /// When composing a voxel data set using several dicom files it is assumed
  /// that the numbering of the files corresponds to decreasing or increasing
  /// z-values in the ImagePositionPatient data entry. If this is true the
  /// slices will be stacked such that the first slice is the slice with the
  /// highest z-value of ImagePositionPatient. It might be worth noting that
  /// the default X3D Texture coordinate generation is from +Z to -Z for the
  /// r texture coordinate (3rd value) which means that when a DICOM voxel data
  /// set is displayed using that setup it will look as if the first slice in
  /// the data set is in the back. Flipping the data set in the z direction
  /// ( or generate other texture coordinates ) will fix this issue.
  /// See image below for a graphical description of the above information:
  /// \image html DICOM_coord_system.png
  /// \param url The url of the image to load.
  /// \param load_single_file If true the loader will only load the
  /// file url specifies. If false, it will look in the same directory
  /// for files of the same image set, and compose them to a 3d image
  /// if they are found.
  /// \returns A pointer to and Image class containing the data
  /// of the loaded url. NULL if unsuccessful.
  H3DUTIL_API Image *loadDicomFile( const string &url, 
                                    bool load_single_file = true );
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
