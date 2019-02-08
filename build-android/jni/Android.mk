# Copyleft 2018 pEp foundation
#
# This file is under GNU General Public License 3.0
# see LICENSE.txt
 
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := pEpAdapter


LOCAL_CPP_FEATURES += exceptions
LOCAL_CPPFLAGS += -std=c++17


LIB_PEP_ADAPTER_INCLUDE_FILES := $(wildcard $(LOCAL_PATH)/../../*.h*)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../pEpEngine/build-android/include \
            $(LIB_PEP_ADAPTER_INCLUDE_FILES:%=%)

#$(shell sh $(LOCAL_PATH)/../takeOutHeaderFiles.sh $(LOCAL_PATH)/../../../pEpEngine)

LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)../include

LOCAL_SRC_FILES += $(LOCAL_PATH)/../../Adapter.cc \
        $(LOCAL_PATH)/../../slurp.cc \
        $(LOCAL_PATH)/../../status_to_string.cc


include $(BUILD_STATIC_LIBRARY)

