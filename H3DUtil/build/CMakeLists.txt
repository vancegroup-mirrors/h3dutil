IF(WIN32)
  cmake_minimum_required(VERSION 2.6.0)
ENDIF(WIN32)
PROJECT(H3DUtil)

# Add a cache variable GENERATE_CPACK_PROJECT to have the choice of generating a project
# for packaging H3DAPI. Default is NO since most people will not use this.
IF( NOT DEFINED GENERATE_CPACK_PROJECT )
  SET( GENERATE_CPACK_PROJECT "NO" CACHE BOOL "Decides if a cpack project should be generated. The project in the first loaded CMakeLists will configure CPack." )
  MARK_AS_ADVANCED(GENERATE_CPACK_PROJECT)
ENDIF( NOT DEFINED GENERATE_CPACK_PROJECT )


IF( NOT DEFINED PREFER_STATIC_LIBRARIES )
  SET( PREFER_STATIC_LIBRARIES "NO" CACHE BOOL "Decides if CMake should prefer static libraries to dynamic libraries when both exist." )
  MARK_AS_ADVANCED(PREFER_STATIC_LIBRARIES)
ENDIF( NOT DEFINED PREFER_STATIC_LIBRARIES )

IF( PREFER_STATIC_LIBRARIES )
  SET( CMAKE_FIND_LIBRARY_SUFFIXES .a;${CMAKE_FIND_LIBRARY_SUFFIXES} )	
ENDIF( PREFER_STATIC_LIBRARIES )

SET( H3DUTIL_MAJOR_VERSION 1 )
SET( H3DUTIL_MINOR_VERSION 0 )
SET( H3DUTIL_BUILD_VERSION 0 )

SET( H3DUTIL_FULL_VERSION
${H3DUTIL_MAJOR_VERSION}.${H3DUTIL_MINOR_VERSION}.${H3DUTIL_BUILD_VERSION} ) 

# add all sources, they are added to a variable called H3DUTIL_SRCS defined
# in the included file
INCLUDE( ${H3DUtil_SOURCE_DIR}/H3DUtilSourceFiles.txt )

# add the H3DUtil.rc resource file if Visual Studio
IF(MSVC)
  SET( H3DUTIL_SRCS ${H3DUTIL_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/H3DUtil.rc )
ENDIF(MSVC)

# add all optional libraries to this variable
SET(optionalLibs)

# add all required libraries to this variable
SET(requiredLibs)

SET(CMAKE_MODULE_PATH ${H3DUtil_SOURCE_DIR}/modules )

FIND_PACKAGE(FreeImage)
IF(FREEIMAGE_FOUND)
  SET(HAVE_FREEIMAGE 1)
  INCLUDE_DIRECTORIES( ${FREEIMAGE_INCLUDE_DIR} )
  SET(optionalLibs ${optionalLibs} ${FREEIMAGE_LIBRARIES} )
ENDIF(FREEIMAGE_FOUND)

FIND_PACKAGE(H3DZLIB)
IF(ZLIB_FOUND)
  SET(HAVE_ZLIB 1)
  INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_DIR} )
  SET(optionalLibs ${optionalLibs} ${ZLIB_LIBRARIES} )
ENDIF(ZLIB_FOUND)

FIND_PACKAGE(DCMTK)
IF(DCMTK_FOUND)
  SET( HAVE_DCMTK 1 )
  INCLUDE_DIRECTORIES( ${DCMTK_INCLUDE_DIR} ) 
  SET(requiredLibs ${requiredLibs} ${DCMTK_LIBRARIES} )
ENDIF(DCMTK_FOUND)

FIND_PACKAGE(TEEM)
IF(TEEM_FOUND)
  SET( HAVE_TEEM 1 )
  INCLUDE_DIRECTORIES( ${TEEM_INCLUDE_DIR} ) 
  SET(requiredLibs ${requiredLibs} ${TEEM_LIBRARIES} )
ENDIF(TEEM_FOUND)

FIND_PACKAGE(PTHREAD REQUIRED)
IF(PTHREAD_FOUND)
  INCLUDE_DIRECTORIES( ${PTHREAD_INCLUDE_DIR} ) 
  SET(requiredLibs ${requiredLibs} ${PTHREAD_LIBRARIES} )
ENDIF(PTHREAD_FOUND)

# To allow other projects that use H3DUtil as a subproject to add extra include directories
# when packaging.
IF( GENERATE_CPACK_PROJECT )
  IF( WIN32 )
    SET( H3DUTIL_CPACK_INSTALLED_DIRECTORIES ""
           CACHE INTERNAL "Installed directories of H3DUtil." )

    IF( FREEIMAGE_FOUND )
      SET( H3DUTIL_CPACK_INSTALLED_DIRECTORIES "${FREEIMAGE_INCLUDE_DIR}/..;External/include/FreeImage"
           CACHE INTERNAL "Installed directories of H3DUtil." )
    ENDIF( FREEIMAGE_FOUND )
    
    IF( ZLIB_FOUND )
      SET( H3DUTIL_CPACK_INSTALLED_DIRECTORIES ${H3DUTIL_CPACK_INSTALLED_DIRECTORIES}
                                               "${ZLIB_INCLUDE_DIR};External/include/zlib"
           CACHE INTERNAL "Installed directories of H3DUtil." )
    ENDIF( ZLIB_FOUND )
  
    IF( PTHREAD_FOUND )
      SET( H3DUTIL_CPACK_INSTALLED_DIRECTORIES ${H3DUTIL_CPACK_INSTALLED_DIRECTORIES}
                                               "${PTHREAD_INCLUDE_DIR};External/include/pthread"
           CACHE INTERNAL "Installed directories of H3DUtil." )
    ENDIF( PTHREAD_FOUND )
  ENDIF( WIN32 ) 
  
  SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "H3DUtil. Help functions and utility functions for H3D API and HAPI.")
  SET(CPACK_PACKAGE_VENDOR "SenseGraphics AB")
  SET(CPACK_PACKAGE_CONTACT "support@sensegraphics.com" )
#  SET(CPACK_PACKAGE_DESCRIPTION_FILE "${H3DUtil_SOURCE_DIR}/../ReadMe.txt")
  SET(CPACK_RESOURCE_FILE_LICENSE "${H3DUtil_SOURCE_DIR}/../LICENSE")
  SET(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_CURRENT_BINARY_DIR};H3DUtil;ALL;/" )
  SET(CPACK_PACKAGE_INSTALL_DIRECTORY "H3DUtil" )
  SET(CPACK_PACKAGE_VERSION_MAJOR ${H3DUTIL_MAJOR_VERSION})
  SET(CPACK_PACKAGE_VERSION_MINOR ${H3DUTIL_MINOR_VERSION})
  SET(CPACK_PACKAGE_VERSION_PATCH ${H3DUTIL_BUILD_VERSION})
  SET(DEBIAN_PACKAGE_DEPENDS "zlib1g-dev, libfreeimage-dev" )	      

  INCLUDE(CPack)
  INCLUDE(UseDebian)
  IF(DEBIAN_FOUND)
    ADD_DEBIAN_TARGETS(H3DUtil)
  ENDIF(DEBIAN_FOUND)
ENDIF( GENERATE_CPACK_PROJECT )

# H3DUtil shared library definition.
ADD_LIBRARY(H3DUtil SHARED ${H3DUTIL_SRCS})

# make sure that the H3DUtil.rc contains the correct svn-version
IF( MSVC )
  EXECUTE_PROCESS( COMMAND ${H3DUtil_SOURCE_DIR}/FindSVNVersion ${H3DUtil_SOURCE_DIR}/../src/.svn/entries 
                   OUTPUT_VARIABLE H3DUTIL_SVN_VERSION )
  # autogenerate H3DUtil.rc depending on the version
  CONFIGURE_FILE( H3DUtil.rc.cmake ${CMAKE_CURRENT_BINARY_DIR}/H3DUtil.rc )
  ADD_CUSTOM_COMMAND( TARGET H3DUtil 
                      PRE_BUILD 
                      COMMAND ${H3DUtil_SOURCE_DIR}/UpdateResourceFile 
                      ARGS H3DUTIL ${CMAKE_CURRENT_BINARY_DIR}/H3DUtil.rc H3DUtil.rc.cmake
                      ${H3DUTIL_MAJOR_VERSION} ${H3DUTIL_MINOR_VERSION}
                      ${H3DUTIL_BUILD_VERSION}
                      ${H3DUtil_SOURCE_DIR}/../src/.svn/entries )
ENDIF(MSVC)

# set the install directory to the H3D directory on Windows
IF(WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
  SET( CMAKE_INSTALL_PREFIX ${H3DUtil_SOURCE_DIR}/../.. CACHE PATH "Install path prefix, prepended onto install directories." FORCE )
ENDIF(WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )

INCLUDE_DIRECTORIES( ${H3DUtil_SOURCE_DIR}/../include )

# check all available libraries

IF(WIN32)
  SET(requiredLibs ${requiredLibs} winmm.lib )
ENDIF(WIN32)

# make sure symbols are exported.
SET( H3DUTIL_COMPILE_FLAGS "-DH3DUTIL_EXPORTS" )

# add the libraries needed for linking
TARGET_LINK_LIBRARIES( H3DUtil ${requiredLibs} ${optionalLibs} )

# make the name of debug libraries end in _d.
SET_TARGET_PROPERTIES( H3DUtil PROPERTIES DEBUG_POSTFIX "_d" )

# set the version of the library
SET_TARGET_PROPERTIES( H3DUtil PROPERTIES VERSION ${H3DUTIL_FULL_VERSION} )

# change the name depending on compiler to be able to tell them apart
# since they are not compatible with each other. 
IF(MSVC70 OR MSVC71)
  SET_TARGET_PROPERTIES( H3DUtil PROPERTIES OUTPUT_NAME H3DUtil_vc7 )
ELSEIF(MSVC80)
  SET_TARGET_PROPERTIES( H3DUtil PROPERTIES OUTPUT_NAME H3DUtil_vc8 )
  SET( H3DUTIL_COMPILE_FLAGS "${H3DUTIL_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE" )
ELSEIF(MSVC90)
  SET_TARGET_PROPERTIES( H3DUtil PROPERTIES OUTPUT_NAME H3DUtil_vc9 )
  SET( H3DUTIL_COMPILE_FLAGS "${H3DUTIL_COMPILE_FLAGS} /MP -D_CRT_SECURE_NO_DEPRECATE" )
ELSEIF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  #used to link in dynamic library load functions. Needed by some examples in HAPI
  SET_TARGET_PROPERTIES( H3DUtil PROPERTIES LINK_FLAGS "-ldl" )
ENDIF(MSVC70 OR MSVC71)

# set compile flags.
SET_TARGET_PROPERTIES( H3DUtil PROPERTIES COMPILE_FLAGS "${H3DUTIL_COMPILE_FLAGS}"  )

# autogenerate H3DUtil.h depending on the libraries available.
CONFIGURE_FILE( ${H3DUtil_SOURCE_DIR}/../include/H3DUtil/H3DUtil.cmake ${H3DUtil_SOURCE_DIR}/../include/H3DUtil/H3DUtil.h )

INSTALL( TARGETS H3DUtil 
         LIBRARY DESTINATION lib
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib )

IF(NOT WIN32)
  INSTALL( DIRECTORY ../include/H3DUtil 
           DESTINATION include 
           PATTERN .svn EXCLUDE )
ENDIF(NOT WIN32)

    

