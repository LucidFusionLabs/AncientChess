# $Id: CMakeLists.txt 1320 2014-10-26 08:12:52Z justin $

lfl_set_os_toolkit(CHESS)
lfl_project(chess)
lfl_add_package(chess SOURCES chess.cpp chess.rc
                LINK_LIBRARIES ${LFL_APP_LIB} ${CHESS_FRAMEWORK} ${CHESS_GRAPHICS}
                ${LFL_APP_AUDIO} app_null_camera app_null_matrix app_null_fft
                app_simple_resampler app_simple_loader ${LFL_APP_CONVERT}
                app_libpng_png app_null_jpeg app_null_gif app_null_ogg app_null_css ${LFL_APP_FONT}
                app_null_ssl app_null_js app_null_tu ${CHESS_TOOLKIT} ${LFL_APP_OS})
set(LFL_APP_ASSET_FILES ${LFL_APP_ASSET_FILES} ${LFL_SOURCE_DIR}/core/app/assets/Nobile.ttf,32,*)
set(LFL_APP_LIB_FILES ${LFL_APP_LIB_FILES} ${PORTAUDIO_LIB_FILES})
set(CHESS_DEP)
if(LFL_STOCKFISH)
  set(CHESS_DEP ${CHESS_DEP} Stockfish)
  set(LFL_APP_ASSET_FILES ${LFL_APP_ASSET_FILES} ${STOCKFISH_BIN})
else()
  set(CHESS_DEP)
endif()
lfl_post_build_start(chess chess LChess)
add_dependencies(chess ${CHESS_DEP})

if(NOT LFL_MOBILE)
  lfl_add_target(chess_tests EXECUTABLE SOURCES chess_tests.cpp
                 INCLUDE_DIRECTORIES ${GTEST_INCLUDE}
                 LINK_LIBRARIES ${LFL_APP_LIB} ${GTEST_LIB} app_null_framework app_null_graphics
                 app_null_audio app_null_camera app_null_matrix app_null_fft
                 app_simple_resampler app_simple_loader ${LFL_APP_CONVERT}
                 app_null_png app_null_jpeg app_null_gif app_null_ogg app_null_css ${LFL_APP_FONT}
                 app_null_ssl app_null_js app_null_tu app_null_toolkit ${LFL_APP_OS})

  lfl_add_target(magicgen EXECUTABLE SOURCES magicgen.cpp
                 LINK_LIBRARIES ${LFL_APP_LIB} app_null_framework app_null_graphics
                 app_null_audio app_null_camera app_null_matrix app_null_fft
                 app_simple_resampler app_simple_loader ${LFL_APP_CONVERT}
                 app_null_png app_null_jpeg app_null_gif app_null_ogg app_null_css ${LFL_APP_FONT}
                 app_null_ssl app_null_js app_null_tu app_null_toolkit ${LFL_APP_OS})
endif()
