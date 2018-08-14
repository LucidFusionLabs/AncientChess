# $Id: CMakeLists.txt 1320 2014-10-26 08:12:52Z justin $

lfl_set_os_toolkit(CHESS)
lfl_project(OldChess)
add_subdirectory(imports)

lfl_add_package(OldChess SOURCES chess.cpp OldChess-windows/chess.rc
                LINK_LIBRARIES ${LFL_APP_LIB} ${CHESS_FRAMEWORK} ${CHESS_GRAPHICS}
                ${LFL_APP_AUDIO} app_null_camera app_null_matrix app_null_fft
                app_simple_resampler app_simple_loader ${LFL_APP_CONVERT}
                app_libpng_png app_null_jpeg app_null_gif app_null_ogg app_null_css ${LFL_APP_FONT}
                app_null_ssl app_null_js app_null_tu app_null_billing app_null_advertising
                app_null_crashreporting ${CHESS_TOOLKIT} ${LFL_APP_OS}
                ASSET_FILES ${LFL_APP_ASSET_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/assets/*
                ${LFL_SOURCE_DIR}/core/app/assets/Nobile.ttf,32,* ${STOCKFISH_BIN}
                LIB_FILES ${LFL_APP_LIB_FILES} ${PORTAUDIO_LIB_FILES})
if(LFL_STOCKFISH)
  add_dependencies(OldChess Stockfish)
endif()
lfl_post_build_start(OldChess)

if(NOT LFL_MOBILE)
  lfl_add_target(lengine EXECUTABLE SOURCES engine.cpp
                 LINK_LIBRARIES ${LFL_APP_LIB} app_null_framework app_null_graphics
                 app_null_audio app_null_camera app_null_matrix app_null_fft
                 app_simple_resampler app_simple_loader ${LFL_APP_CONVERT}
                 app_null_png app_null_jpeg app_null_gif app_null_ogg app_null_css ${LFL_APP_FONT}
                 app_null_ssl app_null_js app_null_tu app_null_crashreporting
                 app_null_toolkit ${LFL_APP_OS})
  add_dependencies(OldChess lengine)
  lfl_post_build_copy_asset_bin(OldChess lengine)

  lfl_add_target(chess_tests EXECUTABLE SOURCES chess_tests.cpp
                 INCLUDE_DIRECTORIES ${GTEST_INCLUDE}
                 LINK_LIBRARIES ${LFL_APP_LIB} ${GTEST_LIB} app_null_framework app_null_graphics
                 app_null_audio app_null_camera app_null_matrix app_null_fft
                 app_simple_resampler app_simple_loader ${LFL_APP_CONVERT}
                 app_null_png app_null_jpeg app_null_gif app_null_ogg app_null_css ${LFL_APP_FONT}
                 app_null_ssl app_null_js app_null_tu app_null_crashreporting
                 app_null_toolkit ${LFL_APP_OS})
  lfl_post_build_copy_bin(OldChess chess_tests)

  if(CHESS_MAGICGEN)
    lfl_add_target(magicgen EXECUTABLE SOURCES magicgen.cpp
                   LINK_LIBRARIES ${LFL_APP_LIB} app_null_framework app_null_graphics
                   app_null_audio app_null_camera app_null_matrix app_null_fft
                   app_simple_resampler app_simple_loader ${LFL_APP_CONVERT}
                   app_null_png app_null_jpeg app_null_gif app_null_ogg app_null_css ${LFL_APP_FONT}
                   app_null_ssl app_null_js app_null_tu app_null_toolkit ${LFL_APP_OS})
  endif()
endif()
