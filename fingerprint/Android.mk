LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := android.hardware.biometrics.fingerprint@2.1-service.sony
LOCAL_INIT_RC := android.hardware.biometrics.fingerprint@2.1-service.sony.rc
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_SRC_FILES := \
    $(call all-subdir-cpp-files) \
    QSEEComFunc.c \
    ion_buffer.c \
    common.c

LOCAL_CFLAGS += -DFINGERPRINT_TYPE_EGISTEC
LOCAL_CFLAGS += \
    -DEGIS_QSEE_APP_NAME=\"egisap32\"

# Define dynamic power management for everything but the following platforms:
LOCAL_CFLAGS += -DHAS_DYNAMIC_POWER_MANAGEMENT

# This file heavily depends on fpc_ implementations from the
# above fpc_imp_* files. There is no sensible default file
# on some platforms, so just remove the file altogether:
LOCAL_SRC_FILES -= BiometricsFingerprint.cpp

LOCAL_SHARED_LIBRARIES := \
    android.hardware.biometrics.fingerprint@2.1 \
    libcutils \
    libdl \
    libhardware \
    libhidlbase \
    libion \
    liblog \
    libutils

