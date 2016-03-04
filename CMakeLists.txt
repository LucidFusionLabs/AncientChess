# $Id: CMakeLists.txt 1320 2014-10-26 08:12:52Z justin $

lfl_project(chess)
lfl_add_target(chess chess.cpp)
target_compile_definitions(chess PUBLIC ${LFL_APP_DEF})
target_include_directories(chess PUBLIC ${LFL_APP_INCLUDE})
target_link_libraries(chess ${LFL_APP_LIB} ${LFL_APP_OS} ${LFL_APP_FRAMEWORK} ${LFL_APP_GRAPHICS}
                      ${LFL_APP_AUDIO} app_null_camera app_null_matrix app_null_fft
                      app_simple_resampler app_simple_loader ${LFL_APP_CONVERT}
                      app_libpng_png app_null_jpeg app_null_gif app_null_css app_null_ttf app_null_ssl
                      app_null_js app_null_tu)
lfl_post_build_start(chess chess LChess)

add_executable(chess_tests chess_tests.cpp)
target_include_directories(chess_tests PUBLIC ${GTEST_INCLUDE})
target_link_libraries(chess_tests ${LFL_APP_LIB} ${GTEST_LIB} app_null_os app_null_framework app_null_graphics
                      app_null_audio app_null_camera app_null_matrix app_null_fft
                      app_simple_resampler app_simple_loader ${LFL_APP_CONVERT}
                      app_null_png app_null_jpeg app_null_gif app_null_css app_null_ttf app_null_ssl
                      app_null_js app_null_tu)
