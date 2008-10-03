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
/// \file H3DUtil.cpp
/// \brief .cpp file that contains dll initialization functions.
//
//////////////////////////////////////////////////////////////////////////////

#include <H3DUtil/H3DUtil.h>

#include <iostream>
#include <sstream>
using namespace std;

#ifdef HAVE_FREEIMAGE
#include <FreeImage.h>
#if defined(_MSC_VER) || defined(__BORLANDC__)
#ifdef H3DUTIL_LINK_STATIC_EXTERNALS
#pragma comment( lib, "FreeImage_static.lib" )
#else
#pragma comment( lib, "FreeImage.lib" )
#endif
#endif
#endif

using namespace H3DUtil;

// Initialize H3DUtil(only needed if using H3DUtil as a static library).
void H3DUtil::initializeH3DUtil() {

#ifdef HAVE_FREEIMAGE
    FreeImage_Initialise();
#endif
}

/// Deinitialize H3DUtil(only needed if using H3DUtil as a static library).
void H3DUtil::deinitializeH3DUtil() {
#ifdef HAVE_FREEIMAGE
  FreeImage_DeInitialise();
#endif
}

double H3DUtil::getH3DUtilVersion(){
  std::stringstream stm;
  stm << H3DUTIL_MAJOR_VERSION;
  stm << ".";
  stm << H3DUTIL_MINOR_VERSION;
  return atof( stm.str().c_str() );
}

#ifdef WIN32
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
                       ) {
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH: {
    initializeH3DUtil();
    break;
  }
  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETACH:
    break;
  case DLL_PROCESS_DETACH:
    deinitializeH3DUtil();
    break;
  }
  return TRUE;
}
#else 
#ifdef __cplusplus
extern "C" {
#endif
  void __attribute__((constructor)) initH3DUtil( void ) {
    initializeH3DUtil();
  }
  void __attribute__((destructor)) finiH3DUtil( void ) {
    deinitializeH3DUtil();
  }
#ifdef __cplusplus
}
#endif

#endif

