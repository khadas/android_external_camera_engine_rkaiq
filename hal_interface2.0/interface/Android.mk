LOCAL_PATH:= $(call my-dir)
MY_LOCAL_PATH =${LOCAL_PATH}
include $(CLEAR_VARS)

LOCAL_SRC_FILES +=\
	rkisp_control_loop_impl.cpp \
	rkcamera_vendor_tags.cpp \
	rkaiq.cpp \
	AiqCameraHalAdapter.cpp \
	CameraWindow.cpp \
	settings_processor.cpp \
	Metadata2Str.cpp

#states
LOCAL_SRC_FILES += \
	ae_state_machine.cpp \
	af_state_machine.cpp \
	awb_state_machine.cpp

#xcore

#LOCAL_CFLAGS += -DLINUX  -D_FILE_OFFSET_BITS=64 -DHAS_STDINT_H -DENABLE_ASSERT
LOCAL_CPPFLAGS += -std=c++11 -Wno-error -frtti
LOCAL_CPPFLAGS += -DLINUX
LOCAL_CPPFLAGS += $(PRJ_CPPFLAGS)

ifeq (1,$(strip $(shell expr $(PLATFORM_SDK_VERSION) \>= 29)))
LOCAL_SHARED_LIBRARIES += libutils libcutils liblog
LOCAL_SHARED_LIBRARIES += \
	libcamera_metadata

ifeq ($(IS_HAVE_DRM),true)
LOCAL_SHARED_LIBRARIES += \
	libdrm
endif

LOCAL_SHARED_LIBRARIES += \
	librkaiq

#external/camera_engine_rkaiq
LOCAL_C_INCLUDES += \
	external/camera_engine_rkaiq \
	external/camera_engine_rkaiq/include/uAPI \
	external/camera_engine_rkaiq/include/uAPI \
	external/camera_engine_rkaiq/xcore \
	external/camera_engine_rkaiq/xcore/base \
	external/camera_engine_rkaiq/aiq_core \
	external/camera_engine_rkaiq/algos \
	external/camera_engine_rkaiq/hwi \
	external/camera_engine_rkaiq/iq_parser \
	external/camera_engine_rkaiq/uAPI \
	external/camera_engine_rkaiq/common \
	external/camera_engine_rkaiq/common/linux \
	external/camera_engine_rkaiq/include \
	external/camera_engine_rkaiq/include/iq_parser \
	external/camera_engine_rkaiq/include/uAPI \
	external/camera_engine_rkaiq/include/xcore \
	external/camera_engine_rkaiq/include/common \
	external/camera_engine_rkaiq/include/common/mediactl \
	external/camera_engine_rkaiq/include/xcore/base \
	external/camera_engine_rkaiq/include/algos \

#system and frameworks
LOCAL_C_INCLUDES += \
	system/media/camera/include \
	system/media/private/camera/include \
	system/core/libutils/include \
	system/core/include \
	frameworks/native/libs/binder/include \
	frameworks/av/include

#local
LOCAL_C_INCLUDES += \
	$(MY_LOCAL_PATH)/include/ \
	$(MY_LOCAL_PATH)/xcore/

LOCAL_PROPRIETARY_MODULE := true
LOCAL_STATIC_LIBRARIES += android.hardware.camera.common@1.0-helper

LOCAL_CFLAGS += -DANDROID_VERSION_ABOVE_8_X
LOCAL_CFLAGS += -DANDROID_PLATEFORM

LOCAL_HEADER_LIBRARIES += \
	libhardware_headers \
	libbinder_headers \
	gl_headers \
	libutils_headers
endif

LOCAL_MODULE:= librkisp

include $(BUILD_SHARED_LIBRARY)
