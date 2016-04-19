# $Id: CMakeLists.txt 1320 2014-10-26 08:12:52Z justin $

lfl_project(chess)
lfl_add_package(chess SOURCES chess.cpp
                LINK_LIBRARIES ${LFL_APP_LIB} ${LFL_APP_FRAMEWORK} ${LFL_APP_GRAPHICS}
                ${LFL_APP_AUDIO} app_null_camera app_null_matrix app_null_fft
                app_simple_resampler app_simple_loader ${LFL_APP_CONVERT}
                app_libpng_png app_null_jpeg app_null_gif app_null_ogg app_null_css app_null_ttf
                app_null_ssl app_null_js app_null_tu ${LFL_APP_OS})
set(LFL_APP_ASSET_FILES ${LFL_APP_ASSET_FILES} ${LFL_SOURCE_DIR}/core/app/assets/Nobile.ttf,32,*)
set(LFL_APP_LIB_FILES ${LFL_APP_LIB_FILES} ${PORTAUDIO_LIB_FILES})
lfl_post_build_start(chess chess LChess)

lfl_add_target(chess_tests EXECUTABLE SOURCES chess_tests.cpp
               INCLUDE_DIRECTORIES ${GTEST_INCLUDE}
               LINK_LIBRARIES ${LFL_APP_LIB} ${GTEST_LIB} app_null_framework app_null_graphics
               app_null_audio app_null_camera app_null_matrix app_null_fft
               app_simple_resampler app_simple_loader ${LFL_APP_CONVERT}
               app_null_png app_null_jpeg app_null_gif app_null_ogg app_null_css app_null_ttf
               app_null_ssl app_null_js app_null_tu ${LFL_APP_OS})
