if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DBUILD_TYPE_DEBUG)
endif()

cmake_minimum_required(VERSION 2.8)

PROJECT(rk_stream)

add_compile_options(-DUSE_RAWSTREAM_LIB)

include_directories(
    include
    ../xcore
    ../xcore/base
    ../aiq_core
    ../common
    ../common/linux
    ../include
    ../include/common
    ../include/common/mediactl
    ../include/iq_parser
    ../include/iq_parser_v2
    ../include/xcore
    ../include/xcore/base
    ../include/algos
    )

set(XCORE_SRC
    ../xcore/xcam_common.cpp
    ../xcore/xcam_log.cpp
    ../xcore/xcam_thread.cpp
    ../xcore/xcam_buffer.cpp
    ../xcore/video_buffer.cpp
    ../xcore/v4l2_buffer_proxy.cpp
    ../xcore/v4l2_device.cpp
    ../xcore/fake_v4l2_device.cpp
    ../xcore/buffer_pool.cpp
    ../xcore/poll_thread.cpp
    ../xcore/smart_buffer_priv.cpp
    )

set(STREAMCFG_SRC
    stream_cfg/rk_vi_user_api_stream_cfg.cpp
    stream_cfg/RawStreamCapUnit.cpp
    stream_cfg/RawStreamProcUnit.cpp
    stream_cfg/MediaInfo.cpp
    stream_cfg/Stream.cpp
    ../common/mediactl/mediactl.c
    )

set(RK_VI_STREAM_CFG_LIB rkrawstream)
set(RK_AIQ_LIB rkaiq)

add_library(${RK_VI_STREAM_CFG_LIB} SHARED ${STREAMCFG_SRC} ${XCORE_SRC} )
target_link_libraries(${RK_VI_STREAM_CFG_LIB} PRIVATE)
install(TARGETS ${RK_VI_STREAM_CFG_LIB} LIBRARY DESTINATION lib)


#set (VI_DEMO_SRCS
#    ../rkvi_demo/rkvi_demo.cpp
#    )

cmake_policy(SET CMP0015 NEW)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/all_lib/${CMAKE_BUILD_TYPE})
link_directories("${LIBRARY_OUTPUT_PATH}" "../build/linux/output/aarch64/all_lib/RelWithDebInfo")
#add_executable(rkvi_demo ${VI_DEMO_SRCS})
#target_link_libraries(rkvi_demo ${RK_VI_STREAM_CFG_LIB} ${RK_AIQ_LIB} pthread)