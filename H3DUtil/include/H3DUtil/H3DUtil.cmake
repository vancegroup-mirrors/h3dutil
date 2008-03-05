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
//
/// \file H3DUtil.h
/// \brief Base header file that handles all configuration related settings
///
//////////////////////////////////////////////////////////////////////////////

/// \mainpage
/// Copyright 2004, <a href="http://www.sensegraphics.com">SenseGraphics AB</a>

#ifndef __H3DUTIL_H__
#define __H3DUTIL_H__

/// Undef if you do not have zlib(http://www.zlib.net/) installed. 
/// Required for support for parsing zipped files.
#cmakedefine HAVE_ZLIB

/// Undef if you do not have FreeImage(freeimage.sourceforge.net) installed.
/// Image files will not be possible to read (see ImageLoaderFunctions.h).
#cmakedefine HAVE_FREEIMAGE

// Define this if you want to link static link as many external
// libraries as possible.
//#define H3DUTIL_LINK_STATIC_EXTERNALS

#ifdef H3DUTIL_LINK_STATIC_EXTERNALS
// Define this if you are linking Freeimage as a static library
#define FREEIMAGE_LIB
#endif

// The following ifdef block is the standard way of creating macros
// which make exporting from a DLL simpler. All files within this DLL
// are compiled with the H3DUTIL_EXPORTS symbol defined on the command
// line. this symbol should not be defined on any project that uses
// this DLL. This way any other project whose source files include
// this file see H3DUTIL_API functions as being imported from a DLL,
// whereas this DLL sees symbols defined with this macro as being
// exported.
#ifdef WIN32
#include <windows.h>

#ifdef H3DUTIL_LIB
#define H3DUTIL_API
#else

#ifdef H3DUTIL_EXPORTS
#define H3DUTIL_API __declspec(dllexport)
#else
#define H3DUTIL_API __declspec(dllimport)
//#if defined(_MSC_VER) || defined(__BORLANDC__)
//#pragma comment( lib, "H3DUtil.lib" )
//#endif
#endif
#ifdef _MSC_VER
// disable dll-interface warnings for stl-exports 
#pragma warning( disable: 4251 )
#endif

#endif

#endif

#if defined(__APPLE__) && defined(__MACH__)
#define MACOSX
#define H3DUTIL_API
#define HAVE_SYS_TIME_H
#endif

#if defined(__linux)
#define LINUX
#define H3DUTIL_API 
#define HAVE_SYS_TIME_H
#endif

#define H3DUTIL_MAJOR_VERSION 1
#define H3DUTIL_MINOR_VERSION 0

namespace H3DUtil {
  /// Initialize H3DUtil(only needed if using H3DUtil as a static library)?
  void initializeH3DUtil();

  /// Deinitialize H3DUtil(only needed if using H3DUtil as a static library)?
  void deinitializeH3DUtil();

  /// Will return the version of H3DUtil as a double on the form
  /// H3DUTIL_MAJOR_VERSION.H3DUTIL_MINOR_VERSION
  double H3DUTIL_API getH3DUtilVersion();
}

#endif



