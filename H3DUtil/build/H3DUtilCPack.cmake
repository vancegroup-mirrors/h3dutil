IF( NOT TARGET H3DUtil )
  MESSAGE( FATAL_ERROR "Include file H3DUtilCPack.cmake require the target H3DUtil to exist. Please add H3DUtil/build/CMakeLists.txt as subdirectory first." )
ENDIF( NOT TARGET H3DUtil )

# Add all sources, they are added to a variable called H3DUTIL_SRCS defined
# in the included file. All header files are added to a variable called
# H3DUTIL_HEADERS.
INCLUDE( ${H3DUtil_SOURCE_DIR}/H3DUtilSourceFiles.txt )

# To allow other projects that use H3DUtil as a subproject to add extra include directories
# when packaging.
IF( GENERATE_CPACK_PROJECT )
  IF( WIN32 )
    # EXTERNAL_INCLUDES and EXTERNAL_INCLUDE_INSTALL_PATHS must be of equal lengths.
    # The reason for defining these variables here is in case we want to add functionality
    # to configure installation in some other way (using FIND-modules for example).
    SET( EXTERNAL_INCLUDES "" )
    SET( EXTERNAL_INCLUDE_INSTALL_PATHS "" )
    SET( EXTERNAL_LIBRARIES "" )
    SET( EXTERNAL_STATIC_LIBRARIES "" )
    SET( EXTERNAL_BINARIES "" )
    
    # Add a cache variable which indicates where the Externals directory used for packaging
    # HAPI is located. If not set then FIND modules will be used instead.
    IF( NOT DEFINED HAPI_CPACK_EXTERNAL_ROOT )
      IF( NOT DEFINED H3DUtil_CPACK_EXTERNAL_ROOT )
        SET( H3DUtil_CPACK_EXTERNAL_ROOT "$ENV{H3D_EXTERNAL_ROOT}" CACHE PATH "Set to the External directory used with H3DUtil, needed to pack properly. If not set FIND_modules will be used instead." )
        MARK_AS_ADVANCED(H3DUtil_CPACK_EXTERNAL_ROOT)
      ENDIF( NOT DEFINED H3DUtil_CPACK_EXTERNAL_ROOT )
    ELSE( NOT DEFINED HAPI_CPACK_EXTERNAL_ROOT )
      SET( H3DUtil_CPACK_EXTERNAL_ROOT ${HAPI_CPACK_EXTERNAL_ROOT} )
    ENDIF( NOT DEFINED HAPI_CPACK_EXTERNAL_ROOT )
    
    SET( EXTERNAL_BIN_PATH "bin32" )
    SET( EXTERNAL_BIN_REPLACE_PATH "bin64" )
    IF( CMAKE_SIZEOF_VOID_P EQUAL 8 ) # check if the system is 64 bit
      SET( EXTERNAL_BIN_PATH "bin64" )
      SET( EXTERNAL_BIN_REPLACE_PATH "bin32" )
    ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    
    IF( EXISTS ${H3DUtil_CPACK_EXTERNAL_ROOT} )
      SET( EXTERNAL_INCLUDES ${H3DUtil_CPACK_EXTERNAL_ROOT}/include/pthread/
                             ${H3DUtil_CPACK_EXTERNAL_ROOT}/include/FreeImage/
                             ${H3DUtil_CPACK_EXTERNAL_ROOT}/include/zlib/
                             ${H3DUtil_CPACK_EXTERNAL_ROOT}/include/dcmtk/
                             ${H3DUtil_CPACK_EXTERNAL_ROOT}/include/teem/ )
      SET( EXTERNAL_INCLUDE_INSTALL_PATHS External/include/pthread
                                          External/include/FreeImage
                                          External/include/zlib
                                          External/include/dcmtk
                                          External/include/teem )
      SET( EXTERNAL_LIBRARIES ${H3DUtil_CPACK_EXTERNAL_ROOT}/lib32/pthreadVC2.lib
                              ${H3DUtil_CPACK_EXTERNAL_ROOT}/lib32/FreeImage.lib
                              ${H3DUtil_CPACK_EXTERNAL_ROOT}/lib32/zlib.lib
                              ${H3DUtil_CPACK_EXTERNAL_ROOT}/lib32/teem.lib )
      SET( DCM_NAME_LIST ofstd dcmjpeg ijg8 ijg12 ijg16 dcmdata dcmimgle dcmimage )
      FOREACH( library_name ${DCM_NAME_LIST} )
        SET( EXTERNAL_STATIC_LIBRARIES ${EXTERNAL_STATIC_LIBRARIES}
                                       #${H3DUtil_CPACK_EXTERNAL_ROOT}/lib32/static/${library_name}_vc7.lib
                                       #${H3DUtil_CPACK_EXTERNAL_ROOT}/lib32/static/${library_name}_vc8.lib
                                       #${H3DUtil_CPACK_EXTERNAL_ROOT}/lib32/static/${library_name}_vc9.lib
                                       ${H3DUtil_CPACK_EXTERNAL_ROOT}/lib32/static/${library_name}_vc10.lib
                                       ${H3DUtil_CPACK_EXTERNAL_ROOT}/lib32/static/${library_name}_vc10_d.lib )
      ENDFOREACH( library_name )
      SET( EXTERNAL_STATIC_LIBRARIES ${EXTERNAL_STATIC_LIBRARIES}
                                     ${H3DUtil_CPACK_EXTERNAL_ROOT}/lib32/static/teem.lib )

      SET( EXTERNAL_BINARIES ${H3DUtil_CPACK_EXTERNAL_ROOT}/${EXTERNAL_BIN_PATH}/pthreadVC2.dll
                             ${H3DUtil_CPACK_EXTERNAL_ROOT}/${EXTERNAL_BIN_PATH}/FreeImage.dll
                             ${H3DUtil_CPACK_EXTERNAL_ROOT}/${EXTERNAL_BIN_PATH}/zlib1.dll
                             ${H3DUtil_CPACK_EXTERNAL_ROOT}/${EXTERNAL_BIN_PATH}/teem.dll )

    ELSEIF( NOT DEFINED HAPI_CPACK_EXTERNAL_ROOT )
      MESSAGE( WARNING "H3DUtil_CPACK_EXTERNAL_ROOT must be set to the External directory used by H3DUtil in order to package properly." )
    ENDIF( EXISTS ${H3DUtil_CPACK_EXTERNAL_ROOT} )
    
    IF( EXTERNAL_INCLUDES )
      list( LENGTH EXTERNAL_INCLUDES EXTERNAL_INCLUDES_LENGTH )
      math( EXPR EXTERNAL_INCLUDES_LENGTH "${EXTERNAL_INCLUDES_LENGTH} - 1" )
      foreach( val RANGE ${EXTERNAL_INCLUDES_LENGTH} )
        list(GET EXTERNAL_INCLUDES ${val} val1)
        list(GET EXTERNAL_INCLUDE_INSTALL_PATHS ${val} val2)
        INSTALL( DIRECTORY ${val1}
                 DESTINATION ${val2}
                 COMPONENT H3DUtil_cpack_external_source
                 REGEX "(/.svn)|(/CVS)" EXCLUDE )
      endforeach( val )
    ENDIF( EXTERNAL_INCLUDES )
    
    foreach( library ${EXTERNAL_LIBRARIES} )
      IF( EXISTS ${library} )
        INSTALL( FILES ${library}
                 DESTINATION External/lib32
                 COMPONENT H3DUtil_cpack_external_source )
      ENDIF( EXISTS ${library} )
      # Add the other library path as well
      STRING( REGEX REPLACE "(/lib32/)" "/lib64/" other_library ${library} )
      IF( EXISTS ${other_library} )
        INSTALL( FILES ${other_library}
                 DESTINATION External/lib64
                 COMPONENT H3DUtil_cpack_external_source )
      ENDIF( EXISTS ${other_library} )
    endforeach( library )
    
    foreach( library ${EXTERNAL_STATIC_LIBRARIES} )
      IF( EXISTS ${library} )
        INSTALL( FILES ${library}
                 DESTINATION External/lib32/static
                 COMPONENT H3DUtil_cpack_external_source )
      ENDIF( EXISTS ${library} )
      # Add the other library path as well
      STRING( REGEX REPLACE "(/lib32/)" "/lib64/" other_library ${library} )
      IF( EXISTS ${other_library} )
        INSTALL( FILES ${other_library}
                 DESTINATION External/lib64/static
                 COMPONENT H3DUtil_cpack_external_source )
      ENDIF( EXISTS ${other_library} )
    endforeach( library )
    
    foreach( binary ${EXTERNAL_BINARIES} )
      IF( EXISTS ${binary} )
        INSTALL( FILES ${binary}
                 DESTINATION External/${EXTERNAL_BIN_PATH}
                 COMPONENT H3DUtil_cpack_external_runtime )
      ENDIF( EXISTS ${binary} )
      # Add the other binary path as external_source since it only needs to be included when
      # a user wants to build H3D or against it.
      STRING( REGEX REPLACE "(/${EXTERNAL_BIN_PATH}/)" "/${EXTERNAL_BIN_REPLACE_PATH}/" other_binary ${binary} )
      IF( EXISTS ${other_binary} )
        INSTALL( FILES ${other_binary}
                 DESTINATION External/${EXTERNAL_BIN_REPLACE_PATH}
                 COMPONENT H3DUtil_cpack_external_source )
      ENDIF( EXISTS ${other_binary} )
    endforeach( binary )
    
    
    # setting names and dependencies between components and also grouping them.
    set(CPACK_COMPONENT_H3DUTIL_CPACK_EXTERNAL_RUNTIME_DISPLAY_NAME "External runtime")
    set(CPACK_COMPONENT_H3DUTIL_CPACK_EXTERNAL_RUNTIME_DESCRIPTION "External runtime binaries needed by H3DUtil.")
    set(CPACK_COMPONENT_H3DUTIL_CPACK_EXTERNAL_RUNTIME_GROUP "H3DUtil_cpack_group")
    set(CPACK_COMPONENT_H3DUTIL_CPACK_EXTERNAL_RUNTIME_INSTALL_TYPES Developer Full)
    
    set(CPACK_COMPONENT_H3DUTIL_CPACK_EXTERNAL_SOURCE_DISPLAY_NAME "External header/libraries")
    set(CPACK_COMPONENT_H3DUTIL_CPACK_EXTERNAL_SOURCE_DESCRIPTION "External headers and libraries needed by H3DUtil.")
    set(CPACK_COMPONENT_H3DUTIL_CPACK_EXTERNAL_SOURCE_DEPENDS H3DUtil_cpack_external_runtime )
    set(CPACK_COMPONENT_H3DUTIL_CPACK_EXTERNAL_SOURCE_GROUP "H3DUtil_cpack_group")
    set(CPACK_COMPONENT_H3DUTIL_CPACK_EXTERNAL_SOURCE_INSTALL_TYPES Developer Full)
  ENDIF( WIN32 )
  
  SET( INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES} H3DUtil )
  SET( INSTALL_RUNTIME_AND_LIBRARIES_ONLY_POST_BUILD ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_POST_BUILD}
                                                     COMMAND ${H3D_cmake_runtime_path} 
                                                     ARGS -DBUILD_TYPE=$(Configuration) -DCOMPONENT=H3DUtil_cpack_runtime -P cmake_install.cmake
                                                     COMMAND ${H3D_cmake_runtime_path} 
                                                     ARGS -DBUILD_TYPE=$(Configuration) -DCOMPONENT=H3DUtil_cpack_libraries -P cmake_install.cmake )
  
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
  
  # Install header files
  INSTALL( FILES ${H3DUTIL_HEADERS}
           DESTINATION H3DUtil/include/H3DUtil
           COMPONENT H3DUtil_cpack_headers )

  # H3DUtil.cmake that goes to headers is not needed unless sources is required.
  INSTALL( FILES ${H3DUtil_SOURCE_DIR}/../include/H3DUtil/H3DUtil.cmake
			DESTINATION H3DUtil/include/H3DUtil
			COMPONENT H3DUtil_cpack_sources )
  
  # Install src files.
  INSTALL( FILES ${H3DUTIL_SRCS}
           DESTINATION H3DUtil/src
           COMPONENT H3DUtil_cpack_sources )

  INSTALL( FILES ${H3DUtil_SOURCE_DIR}/../changelog
                 ${H3DUtil_SOURCE_DIR}/../LICENSE
           DESTINATION H3DUtil
           COMPONENT H3DUtil_cpack_sources )

  INSTALL( FILES ${H3DUtil_SOURCE_DIR}/CMakeLists.txt
                 ${H3DUtil_SOURCE_DIR}/H3DUtil.rc.cmake
                 ${H3DUtil_SOURCE_DIR}/H3DUtilSourceFiles.txt
                 ${H3DUtil_SOURCE_DIR}/UpdateResourceFile.exe
                 ${H3DUtil_SOURCE_DIR}/H3DUtilCPack.cmake
           DESTINATION H3DUtil/build
           COMPONENT H3DUtil_cpack_sources )

  INSTALL( FILES ${H3DUtil_SOURCE_DIR}/modules/FindDCMTK.cmake
                 ${H3DUtil_SOURCE_DIR}/modules/FindFreeImage.cmake
                 ${H3DUtil_SOURCE_DIR}/modules/FindH3DZLIB.cmake
                 ${H3DUtil_SOURCE_DIR}/modules/FindMd5sum.cmake
                 ${H3DUtil_SOURCE_DIR}/modules/FindPTHREAD.cmake
                 ${H3DUtil_SOURCE_DIR}/modules/FindTEEM.cmake
                 ${H3DUtil_SOURCE_DIR}/modules/StripAndAddLibraryDirectories.cmake
                 ${H3DUtil_SOURCE_DIR}/modules/UseDebian.cmake
           DESTINATION H3DUtil/build/modules
           COMPONENT H3DUtil_cpack_sources )

  # setting names and dependencies between components and also grouping them.
  set(CPACK_COMPONENT_H3DUTIL_CPACK_RUNTIME_DISPLAY_NAME "Runtime")
  set(CPACK_COMPONENT_H3DUTIL_CPACK_RUNTIME_DESCRIPTION "The runtime libraries (dlls) for H3DUtil.")
  set(CPACK_COMPONENT_H3DUTIL_CPACK_RUNTIME_DEPENDS H3DUtil_cpack_external_runtime )
  set(CPACK_COMPONENT_H3DUTIL_CPACK_RUNTIME_GROUP "H3DUtil_cpack_group")
  set(CPACK_COMPONENT_H3DUTIL_CPACK_RUNTIME_INSTALL_TYPES Developer Full)
  
  # Apparently circular dependencies are no problem to handle, so libraries depends on headers, and headers depends on libraries.
  set(CPACK_COMPONENT_H3DUTIL_CPACK_LIBRARIES_DISPLAY_NAME "Libraries")
  set(CPACK_COMPONENT_H3DUTIL_CPACK_LIBRARIES_DESCRIPTION "H3DUtil libraries, needed for building against H3DUtil.")
  set(CPACK_COMPONENT_H3DUTIL_CPACK_LIBRARIES_DEPENDS H3DUtil_cpack_external_source H3DUtil_cpack_headers )
  set(CPACK_COMPONENT_H3DUTIL_CPACK_LIBRARIES_GROUP "H3DUtil_cpack_group")
  set(CPACK_COMPONENT_H3DUTIL_CPACK_LIBRARIES_INSTALL_TYPES Developer Full)
  
  set(CPACK_COMPONENT_H3DUTIL_CPACK_HEADERS_DISPLAY_NAME "C++ Headers")
  set(CPACK_COMPONENT_H3DUTIL_CPACK_HEADERS_DESCRIPTION "H3DUtil C++ headers, needed for building against H3DUtil.")
  set(CPACK_COMPONENT_H3DUTIL_CPACK_HEADERS_DEPENDS H3DUtil_cpack_external_source H3DUtil_cpack_libraries )
  set(CPACK_COMPONENT_H3DUTIL_CPACK_HEADERS_GROUP "H3DUtil_cpack_group")
  set(CPACK_COMPONENT_H3DUTIL_CPACK_HEADERS_INSTALL_TYPES Developer Full)
  
  set(CPACK_COMPONENT_H3DUTIL_CPACK_SOURCES_DISPLAY_NAME "C++ Source")
  set(CPACK_COMPONENT_H3DUTIL_CPACK_SOURCES_DESCRIPTION "Everything needed to build H3DUtil.")
  set(CPACK_COMPONENT_H3DUTIL_CPACK_SOURCES_DEPENDS H3DUtil_cpack_headers )
  set(CPACK_COMPONENT_H3DUTIL_CPACK_SOURCES_GROUP "H3DUtil_cpack_group")
  set(CPACK_COMPONENT_H3DUTIL_CPACK_SOURCES_INSTALL_TYPES Full)
  
  set(CPACK_COMPONENT_GROUP_H3DUTIL_CPACK_GROUP_DISPLAY_NAME "H3DUtil")

  IF( NOT H3D_USE_DEPENDENCIES_ONLY )
    INCLUDE(CPack)
    INCLUDE(UseDebian)
    IF(DEBIAN_FOUND)
      ADD_DEBIAN_TARGETS(H3DUtil)
    ENDIF(DEBIAN_FOUND)
  ENDIF( NOT H3D_USE_DEPENDENCIES_ONLY )
  
ENDIF( GENERATE_CPACK_PROJECT )
