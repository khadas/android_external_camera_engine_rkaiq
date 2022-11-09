#!/bin/bash
# Run this from within a bash shell
# x86_64 is for simulation do not enable RK platform
export AIQ_BUILD_HOST_DIR=/home/camera/camera/rk356x_linux/buildroot/output/rockchip_rk3566/host
#export AIQ_BUILD_HOST_DIR=/home/camera/camera_data/project_codes/rk3588_ipc_sdk/buildroot/output/rockchip_rk3588_ipc/host
TOOLCHAIN_FILE=$(pwd)/../../cmake/toolchains/aarch64_linux_buildroot.cmake
SOURCE_PATH=$(pwd)/../../
OUTPUT=$(pwd)/output/aarch64

mkdir -p $OUTPUT
pushd $OUTPUT
#if make aiq with raw stream lib, modify -DUSE_RAWSTREAM_LIB value to ON, default value OFF 
cmake -G "Ninja" \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo\
    -DARCH="aarch64" \
    -DRKAIQ_TARGET_SOC=${RKAIQ_TARGET_SOC} \
    -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE \
    -DCMAKE_SKIP_RPATH=TRUE \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=YES \
	-DISP_HW_VERSION=${ISP_HW_VERSION} \
    -DCMAKE_INSTALL_PREFIX="installed" \
    -DRKAIQ_USE_RAWSTREAM_LIB=OFF \
    $SOURCE_PATH \
&& ninja -j$(nproc) \
&& ninja install
popd
