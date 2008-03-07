PROJECT(H3DUtil)

SET( H3DUTIL_MAJOR_VERSION 1 )
SET( H3DUTIL_MINOR_VERSION 0 )
SET( H3DUTIL_BUILD_VERSION 0 )

# add all sources
AUX_SOURCE_DIRECTORY(../src H3DUTIL_SRCS)

# add thre H3DUtil.rc resource file if Visual Studio
IF(MSVC)
  SET( H3DUTIL_SRCS ${H3DUTIL_SRCS} H3DUtil.rc )
ENDIF(MSVC)

# H3DUtil shared library definition.
ADD_LIBRARY(H3DUtil SHARED ${H3DUTIL_SRCS})

IF( MSVC )
  EXECUTE_PROCESS( COMMAND FindSVNVersion ${H3DUtil_SOURCE_DIR}/../src/.svn/entries 
                   OUTPUT_VARIABLE H3DUTIL_SVN_VERSION )
  # autogenerate H3DUtil.rc depending on the version
  CONFIGURE_FILE( H3DUtil.rc.cmake H3DUtil.rc )
  
  ADD_CUSTOM_COMMAND( TARGET H3DUtil 
                      PRE_BUILD 
                      COMMAND UpdateResourceFile 
                      ARGS H3DUTIL H3DUtil.rc H3DUtil.rc.cmake
                      ${H3DUTIL_MAJOR_VERSION} ${H3DUTIL_MINOR_VERSION}
                      ${H3DUTIL_BUILD_VERSION}
                      ${H3DUtil_SOURCE_DIR}/../src/.svn/entries )
ENDIF(MSVC)

SET( H3DUTIL_FULL_VERSION
${H3DUTIL_MAJOR_VERSION}.${H3DUTIL_MINOR_VERSION}.${H3DUTIL_BUILD_VERSION} ) 

IF(WIN32) 
 SET( CMAKE_INSTALL_PREFIX ${H3DUtil_SOURCE_DIR}/../..)
ENDIF(WIN32)


# add all optional libraries to this variable
SET(optionalLibs)

# add all required libraries to this variable
SET(requiredLibs)

INCLUDE_DIRECTORIES( ../include )

# check all available libraries

IF(WIN32)
  SET(requiredLibs ${requiredLibs} winmm.lib )
ENDIF(WIN32)

SET(CMAKE_MODULE_PATH ${H3DUtil_SOURCE_DIR}/modules )

FIND_PACKAGE(FreeImage)
IF(FREEIMAGE_FOUND)
  SET(HAVE_FREEIMAGE 1)
  INCLUDE_DIRECTORIES( ${FREEIMAGE_INCLUDE_DIR} ) 
  SET(optionalLibs ${optionalLibs} ${FREEIMAGE_LIBRARIES} )
ENDIF(FREEIMAGE_FOUND)

FIND_PACKAGE(ZLIB)
IF(ZLIB_FOUND)
  SET(HAVE_ZLIB 1)
  INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_DIR} ) 
  SET(optionalLibs ${optionalLibs} ${ZLIB_LIBRARIES} )
ENDIF(ZLIB_FOUND)

FIND_PACKAGE(PTHREAD REQUIRED)
IF(PTHREAD_FOUND)
  INCLUDE_DIRECTORIES( ${PTHREAD_INCLUDE_DIR} ) 
  SET(requiredLibs ${requiredLibs} ${PTHREAD_LIBRARIES} )
ENDIF(PTHREAD_FOUND)

# make sure symbols are exported.
SET_TARGET_PROPERTIES( H3DUtil PROPERTIES COMPILE_FLAGS "-DH3DUTIL_EXPORTS" )

# add the libraries needed for linking
TARGET_LINK_LIBRARIES( H3DUtil ${requiredLibs} ${optionalLibs} )

# make the name of debug libraries end in _d.
SET_TARGET_PROPERTIES( H3DUtil PROPERTIES DEBUG_POSTFIX "_d" )
SET_TARGET_PROPERTIES( H3DUtil PROPERTIES VERSION ${H3DUTIL_FULL_VERSION} )


#SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

# change the name depending on compiler to be able to tell them apart
# since they are not compatible with each other. 
IF(MSVC70 OR MSVC71)
  SET_TARGET_PROPERTIES( H3DUtil PROPERTIES OUTPUT_NAME
  H3DUtil_vc7 )
ELSEIF(MSVC80)
  SET_TARGET_PROPERTIES( H3DUtil PROPERTIES OUTPUT_NAME
  H3DUtil_vc8 )
ELSEIF(MSCV90)
  SET_TARGET_PROPERTIES( H3DUtil PROPERTIES OUTPUT_NAME
  H3DUtil_vc9 )
ENDIF(MSVC70 OR MSVC71)

# autogenerate H3DUtil.h depending on the libraries available.
CONFIGURE_FILE( ../include/H3DUtil/H3DUtil.cmake ../include/H3DUtil/H3DUtil.h )

INSTALL( TARGETS H3DUtil 
         LIBRARY DESTINATION lib
         RUNTIME DESTINATION bin )

IF(!WIN32)
  INSTALL( DIRECTORY ../include/H3DUtil 
           DESTINATION /usr/local/include 
           PATTERN .svn EXCLUDE )
ENDIF(!WIN32)

    

