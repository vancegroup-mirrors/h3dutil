# The name of our project is "HELLO".  CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
project (H3DUtil)

# Create a library called "Hello" which includes the source file "hello.cxx".
# The extension is already found.  Any number of sources could be listed here.

AUX_SOURCE_DIRECTORY(../src H3DUTIL_SRCS)

SET(optionalLibs)

INCLUDE_DIRECTORIES( ../include )

SET(CMAKE_SYSTEM_INCLUDE_PATH ${CMAKE_SYSTEM_INCLUDE_PATH}
"$ENV{H3D_EXTERNAL_ROOT}\\include" 
"$ENV{H3D_EXTERNAL_ROOT}\\include\\pthread" 
../../External/include 
../../External/include/pthread 
)

SET(CMAKE_SYSTEM_LIBRARY_PATH ${CMAKE_SYSTEM_LIBRARY_PATH}
"$ENV{H3D_EXTERNAL_ROOT}\\lib" ../../External/lib )

SET(requiredLibs)

IF(WIN32)
  SET(requiredLibs ${requiredLibs} winmm.lib )
ENDIF(WIN32)

SET(CMAKE_MODULE_PATH "modules/")

FIND_PACKAGE(PTHREAD REQUIRED)

IF(PTHREAD_FOUND)
  INCLUDE_DIRECTORIES( ${PTHREAD_INCLUDE_DIR} ) 
  SET(requiredLibs ${requiredLibs} ${PTHREAD_LIBRARIES} )
ENDIF(PTHREAD_FOUND)

ADD_LIBRARY(H3DUtil SHARED ${H3DUTIL_SRCS})
SET_TARGET_PROPERTIES( H3DUtil PROPERTIES COMPILE_FLAGS "-DH3DUTIL_EXPORTS" )
TARGET_LINK_LIBRARIES( H3DUtil ${requiredLibs} )

INSTALL( TARGETS H3DUtil 
	 LIBRARY DESTINATION lib
	 RUNTIME DESTINATION bin )

INSTALL( DIRECTORY ../include/H3DUtil 
	 DESTINATION /usr/local/include 
	 PATTERN .svn EXCLUDE )


    

