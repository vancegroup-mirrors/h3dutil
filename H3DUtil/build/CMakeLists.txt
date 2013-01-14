IF(WIN32)
  cmake_minimum_required(VERSION 2.6.0)
ENDIF(WIN32)
PROJECT(H3DUtil)


# Make sure the full path is used for installed libraries
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")


# set the install directory to the H3D directory on Windows
IF( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND NOT H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET )
  SET( CMAKE_INSTALL_PREFIX ${H3DUtil_SOURCE_DIR}/../.. CACHE PATH "Install path prefix, prepended onto install directories." FORCE )
  SET( H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET TRUE )
ENDIF( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND NOT H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET )

# Add a cache variable GENERATE_CPACK_PROJECT to have the choice of generating a project
# for packaging H3DAPI. Default is NO since most people will not use this.
IF( NOT DEFINED GENERATE_CPACK_PROJECT )
  SET( GENERATE_CPACK_PROJECT "NO" CACHE BOOL "Decides if a cpack project should be generated. The project in the first loaded CMakeLists will configure CPack." )
  MARK_AS_ADVANCED(GENERATE_CPACK_PROJECT)
ENDIF( NOT DEFINED GENERATE_CPACK_PROJECT )

# Add a cache variable PREFER_STATIC_LIBRARIES to have the choice of generating a project
# linking against static libraries if they exist. Default is NO since most people will not use this.
IF( NOT DEFINED PREFER_STATIC_LIBRARIES )
  SET( PREFER_STATIC_LIBRARIES "NO" CACHE BOOL "Decides if CMake should prefer static libraries to dynamic libraries when both exist." )
  MARK_AS_ADVANCED(PREFER_STATIC_LIBRARIES)
ENDIF( NOT DEFINED PREFER_STATIC_LIBRARIES )

IF( PREFER_STATIC_LIBRARIES )
  SET( CMAKE_FIND_LIBRARY_SUFFIXES .a;${CMAKE_FIND_LIBRARY_SUFFIXES} )  
ENDIF( PREFER_STATIC_LIBRARIES )

SET( H3DUTIL_MAJOR_VERSION 1 )
SET( H3DUTIL_MINOR_VERSION 1 )
SET( H3DUTIL_BUILD_VERSION 1 )

SET( H3DUTIL_FULL_VERSION
${H3DUTIL_MAJOR_VERSION}.${H3DUTIL_MINOR_VERSION}.${H3DUTIL_BUILD_VERSION} ) 

# Add all sources, they are added to a variable called H3DUTIL_SRCS defined
# in the included file. All header files are added to a variable called
# H3DUTIL_HEADERS.
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

# H3DUtil shared library definition.
ADD_LIBRARY(H3DUtil SHARED ${H3DUTIL_SRCS} ${H3DUTIL_HEADERS})

# make sure that the H3DUtil.rc contains the correct svn-version
IF( MSVC )
  SET( H3DUTIL_SVN_VERSION "0" )
  # Find SubWCRev.exe
  FIND_FILE( SubWCRev
             NAMES "SubWCRev.exe"
             DOC   "Set to SubWCRev.exe that comes with TortoiseSVN. Used to find svn revision number." )

  IF( SubWCRev )
    SET( H3DUTIL_SVN_VERSION "$WCREV$" )
  ENDIF( SubWCRev )

  # autogenerate H3DUtil.rc depending on the version
  CONFIGURE_FILE( H3DUtil.rc.cmake ${CMAKE_CURRENT_BINARY_DIR}/H3DUtil.rc )
  
  ADD_CUSTOM_COMMAND( TARGET H3DUtil
                      PRE_BUILD
                      COMMAND ${H3DUtil_SOURCE_DIR}/UpdateResourceFile 
                      ARGS H3DUTIL ${CMAKE_CURRENT_BINARY_DIR}/H3DUtil.rc ${H3DUtil_SOURCE_DIR}/H3DUtil.rc.cmake
                      ${H3DUTIL_MAJOR_VERSION} ${H3DUTIL_MINOR_VERSION}
                      ${H3DUTIL_BUILD_VERSION} "${H3DUTIL_SVN_VERSION}"
                      "H3DUTIL_Output_Name"
                      "$(TargetFileName)" )

  IF( SubWCRev )
    # Update SVN revision in file.
    EXECUTE_PROCESS( COMMAND ${SubWCRev} ${H3DUtil_SOURCE_DIR}/../ ${CMAKE_CURRENT_BINARY_DIR}/H3DUtil.rc ${CMAKE_CURRENT_BINARY_DIR}/H3DUtil.rc )

    ADD_CUSTOM_COMMAND( TARGET H3DUtil 
                        PRE_BUILD 
                        COMMAND ${SubWCRev} 
                        ARGS ${H3DUtil_SOURCE_DIR}/../ ${CMAKE_CURRENT_BINARY_DIR}/H3DUtil.rc ${CMAKE_CURRENT_BINARY_DIR}/H3DUtil.rc )
  ENDIF( SubWCRev )
ENDIF(MSVC)

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


IF(APPLE)
  SET_TARGET_PROPERTIES (H3DUtil
	  	         PROPERTIES 
#			 BUILD_WITH_INSTALL_RPATH 1
                         INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF(APPLE)

IF( MSVC )
  SET( H3D_MSVC_VERSION 6 )
  SET( TEMP_MSVC_VERSION 1299 )
  WHILE( ${MSVC_VERSION} GREATER ${TEMP_MSVC_VERSION} )
    MATH( EXPR H3D_MSVC_VERSION "${H3D_MSVC_VERSION} + 1" )
    MATH( EXPR TEMP_MSVC_VERSION "${TEMP_MSVC_VERSION} + 100" )
  ENDWHILE( ${MSVC_VERSION} GREATER ${TEMP_MSVC_VERSION} )

  # change the name depending on compiler to be able to tell them apart
  # since they are not compatible with each other. 
  SET_TARGET_PROPERTIES( H3DUtil PROPERTIES OUTPUT_NAME ${PROJECT_NAME}_vc${H3D_MSVC_VERSION}
                                            LINK_FLAGS_DEBUG " /NODEFAULTLIB:msvcrt" )

  # Treat wchar_t as built in type for all visual studio versions.
  # This is default for every version above 7 ( so far ) but we still set it for all.
  SET( H3DUTIL_COMPILE_FLAGS "${H3DUTIL_COMPILE_FLAGS} /Zc:wchar_t")

  IF( MSVC80 )
    # This might be useful for visual studio 2005 users that often recompile the api.
    IF( NOT DEFINED USE_VC8_MP_FLAG )
      SET( USE_VC8_MP_FLAG "NO" CACHE BOOL "In visual studio 8 the MP flag exists but is not documented. Maybe it is unsafe to use. If you want to use it then set this flag to yes." )
    ENDIF( NOT DEFINED USE_VC8_MP_FLAG )

    IF( USE_VC8_MP_FLAG )
      SET( H3DUTIL_COMPILE_FLAGS "${H3DUTIL_COMPILE_FLAGS} /MP" )
    ENDIF( USE_VC8_MP_FLAG )
  ENDIF( MSVC80 )

  IF( ${MSVC_VERSION} GREATER 1399 )
    # Remove compiler warnings about deprecation for visual studio versions 8 and above.
    SET( H3DUTIL_COMPILE_FLAGS "${H3DUTIL_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE" )
  ENDIF( ${MSVC_VERSION} GREATER 1399 )

  IF( ${MSVC_VERSION} GREATER 1499 )
    # Build using several threads for visual studio versions 9 and above.
    SET( H3DUTIL_COMPILE_FLAGS "${H3DUTIL_COMPILE_FLAGS} /MP" )
  ENDIF( ${MSVC_VERSION} GREATER 1499 )
ELSEIF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  #used to link in dynamic library load functions. Needed by some examples in HAPI
  SET_TARGET_PROPERTIES( H3DUtil PROPERTIES LINK_FLAGS "-ldl" )
ENDIF( MSVC )

# set compile flags.
SET_TARGET_PROPERTIES( H3DUtil PROPERTIES COMPILE_FLAGS "${H3DUTIL_COMPILE_FLAGS}"  )

# autogenerate H3DUtil.h depending on the libraries available.
CONFIGURE_FILE( ${H3DUtil_SOURCE_DIR}/../include/H3DUtil/H3DUtil.cmake ${H3DUtil_SOURCE_DIR}/../include/H3DUtil/H3DUtil.h )

INSTALL( TARGETS H3DUtil 
         LIBRARY DESTINATION lib COMPONENT H3DUtil_cpack_runtime
         RUNTIME DESTINATION bin COMPONENT H3DUtil_cpack_runtime
         ARCHIVE DESTINATION lib COMPONENT H3DUtil_cpack_libraries )

IF( NOT ( WIN32 OR GENERATE_CPACK_PROJECT ) )
  # Install header files on non-windows system (e.g. Unix).
  INSTALL( FILES ${H3DUTIL_HEADERS}
           DESTINATION include/H3DUtil )
ENDIF( NOT ( WIN32 OR GENERATE_CPACK_PROJECT ) )

IF( NOT HAPI_SOURCE_DIR )
  include( ${H3DUtil_SOURCE_DIR}/H3DUtilCPack.cmake )
ENDIF( NOT HAPI_SOURCE_DIR )
