# $Id: CMakeLists.txt 1325 2014-10-29 06:08:31Z justin $

if(LFL_STOCKFISH)
  if(LFL64)
    set(BUILD_ARCH x86-64)
  else()
    set(BUILD_ARCH x86-32)
  endif()
  if(LFL_OSX OR LFL_IOS)
    set(BUILD_ARGS ARCH=${BUILD_ARCH} COMP=clang)
  else()
    set(BUILD_ARGS ARCH=${BUILD_ARCH} COMP=gcc)
  endif()
  ExternalProject_Add(Stockfish LOG_BUILD ON BUILD_IN_SOURCE ON CONFIGURE_COMMAND "" INSTALL_COMMAND ""
                      PREFIX ${CMAKE_CURRENT_BINARY_DIR}/Stockfish
                      DOWNLOAD_COMMAND rm -rf Stockfish
                      COMMAND cp -R ${CMAKE_CURRENT_SOURCE_DIR}/Stockfish/src Stockfish
                      BUILD_COMMAND make build ${BUILD_ARGS})
  ExternalProject_Get_Property(Stockfish SOURCE_DIR)
  set(STOCKFISH_BIN ${SOURCE_DIR}/stockfish PARENT_SCOPE)
endif()
