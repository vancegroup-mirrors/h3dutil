cmake_minimum_required(VERSION 2.4.8)
PROJECT(H3DUtil)

IF( WIN32 )
  # Macro used to set include directories on windows.
  MACRO( STRIP_AND_ADD_LIBRARY_DIRECTORIES )
    FOREACH(TEMP_LIST_ITEM ${ARGV})
      STRING(REGEX REPLACE  "[/]([^/])*\\.lib$" "" TEMP_LINK_DIR ${TEMP_LIST_ITEM})
      STRING( COMPARE NOTEQUAL ${TEMP_LIST_ITEM} ${TEMP_LINK_DIR} LINK_STRING_NOT_EQUAL )
      IF( ${LINK_STRING_NOT_EQUAL} )
        LINK_DIRECTORIES( ${TEMP_LINK_DIR} )
      ENDIF( ${LINK_STRING_NOT_EQUAL} )
    ENDFOREACH(TEMP_LIST_ITEM ${ARGV})
  ENDMACRO( STRIP_AND_ADD_LIBRARY_DIRECTORIES )
ENDIF( WIN32 )

SET( H3DUTIL_MAJOR_VERSION 1 )
SET( H3DUTIL_MINOR_VERSION 0 )
SET( H3DUTIL_BUILD_VERSION 0 )

SET( H3DUTIL_FULL_VERSION
${H3DUTIL_MAJOR_VERSION}.${H3DUTIL_MINOR_VERSION}.${H3DUTIL_BUILD_VERSION} ) 

# add all sources
AUX_SOURCE_DIRECTORY(../src H3DUTIL_SRCS)

# add the H3DUtil.rc resource file if Visual Studio
IF(MSVC)
  SET( H3DUTIL_SRCS ${H3DUTIL_SRCS} ${H3DUtil_SOURCE_DIR}/H3DUtil.rc )
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
  IF(WIN32)
    STRIP_AND_ADD_LIBRARY_DIRECTORIES( ${FREEIMAGE_LIBRARIES})
  ELSE(WIN32)
    SET(optionalLibs ${optionalLibs} ${FREEIMAGE_LIBRARIES} )
  ENDIF(WIN32)
ENDIF(FREEIMAGE_FOUND)

FIND_PACKAGE(H3DZLIB)
IF(ZLIB_FOUND)
  SET(HAVE_ZLIB 1)
  INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_DIR} )
  IF(WIN32)
    STRIP_AND_ADD_LIBRARY_DIRECTORIES( ${ZLIB_LIBRARIES})
  ELSE(WIN32)
    SET(optionalLibs ${optionalLibs} ${ZLIB_LIBRARIES} )
  ENDIF(WIN32)
ENDIF(ZLIB_FOUND)

FIND_PACKAGE(PTHREAD REQUIRED)
IF(PTHREAD_FOUND)
  INCLUDE_DIRECTORIES( ${PTHREAD_INCLUDE_DIR} ) 
  IF(WIN32)
    STRIP_AND_ADD_LIBRARY_DIRECTORIES( ${PTHREAD_LIBRARIES})
  ELSE(WIN32)
    SET(requiredLibs ${requiredLibs} ${PTHREAD_LIBRARIES} )
  ENDIF(WIN32)
ENDIF(PTHREAD_FOUND)

# To allow other projects that use H3DUtil as a subproject to add extra include directories
# when packaging.
IF( GENERATE_CPACK_PROJECT )
  IF( WIN32 )
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
ENDIF( GENERATE_CPACK_PROJECT )

# H3DUtil shared library definition.
ADD_LIBRARY(H3DUtil SHARED ${H3DUTIL_SRCS})

# make sure that the H3DUtil.rc contains the correct svn-version
IF( MSVC )
  EXECUTE_PROCESS( COMMAND ${H3DUtil_SOURCE_DIR}/FindSVNVersion ${H3DUtil_SOURCE_DIR}/../src/.svn/entries 
                   OUTPUT_VARIABLE H3DUTIL_SVN_VERSION )
  # autogenerate H3DUtil.rc depending on the version
  CONFIGURE_FILE( H3DUtil.rc.cmake ${H3DUtil_SOURCE_DIR}/../build/H3DUtil.rc )
  ADD_CUSTOM_COMMAND( TARGET H3DUtil 
                      PRE_BUILD 
                      COMMAND ${H3DUtil_SOURCE_DIR}/UpdateResourceFile 
                      ARGS H3DUTIL H3DUtil.rc H3DUtil.rc.cmake
                      ${H3DUTIL_MAJOR_VERSION} ${H3DUTIL_MINOR_VERSION}
                      ${H3DUTIL_BUILD_VERSION}
                      ${H3DUtil_SOURCE_DIR}/../src/.svn/entries )
ENDIF(MSVC)

# set the install directory to the H3D directory on Windows
IF(WIN32) 
 SET( CMAKE_INSTALL_PREFIX ${H3DUtil_SOURCE_DIR}/../..)
ENDIF(WIN32)

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
  SET_TARGET_PROPERTIES( H3DUtil PROPERTIES OUTPUT_NAME
  H3DUtil_vc7 )
ELSEIF(MSVC80)
  SET_TARGET_PROPERTIES( H3DUtil PROPERTIES OUTPUT_NAME
  H3DUtil_vc8 )
ELSEIF(MSVC90)
  SET_TARGET_PROPERTIES( H3DUtil PROPERTIES OUTPUT_NAME H3DUtil_vc9 )
  SET( H3DUTIL_COMPILE_FLAGS "${H3DUTIL_COMPILE_FLAGS} /MP" )
ENDIF(MSVC70 OR MSVC71)

# set compile flags.
SET_TARGET_PROPERTIES( H3DUtil PROPERTIES COMPILE_FLAGS "${H3DUTIL_COMPILE_FLAGS}"  )

# autogenerate H3DUtil.h depending on the libraries available.
CONFIGURE_FILE( ${H3DUtil_SOURCE_DIR}/../include/H3DUtil/H3DUtil.cmake ${H3DUtil_SOURCE_DIR}/../include/H3DUtil/H3DUtil.h )

INSTALL( TARGETS H3DUtil 
         LIBRARY DESTINATION lib
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib )

IF(!WIN32)
  INSTALL( DIRECTORY ../include/H3DUtil 
           DESTINATION /usr/local/include 
           PATTERN .svn EXCLUDE )
ENDIF(!WIN32)

    

