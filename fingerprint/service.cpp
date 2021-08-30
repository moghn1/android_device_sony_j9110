/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "android.hardware.biometrics.fingerprint@2.1-service"

#include <hidl/HidlSupport.h>
#include <hidl/HidlTransportSupport.h>
#include "BiometricsFingerprint.h"
#include "egistec/current/BiometricsFingerprint.h"
#include "egistec/legacy/BiometricsFingerprint.h"
#include "egistec/legacy/EGISAPTrustlet.h"

using android::NO_ERROR;
using android::sp;
using android::status_t;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::hardware::biometrics::fingerprint::V2_1::IBiometricsFingerprint;

using FPCHAL = ::fpc::BiometricsFingerprint;
using LegacyEgistecHAL = ::egistec::legacy::BiometricsFingerprint;
using CurrentEgistecHAL = ::egistec::current::BiometricsFingerprint;

int main() {
    android::sp<IBiometricsFingerprint> bio;

#if defined(FINGERPRINT_TYPE_EGISTEC)
    ::egistec::EgisFpDevice dev;
#endif

#if defined(HAS_LEGACY_EGISTEC)
    auto type = dev.GetHwId();
    bool is_old_hal;

    switch (type) {
        case egistec::FpHwId::Egistec:
            ALOGI("Egistec sensor installed");

            {
                ::egistec::legacy::EGISAPTrustlet trustlet;
                is_old_hal = trustlet.MatchFirmware();
                // Scope closes trustlet. While this could be reused,
                // opt for starting fresh in case the command introduces
                // unexpected state changes.
            }
            if (is_old_hal) {
                ALOGI("Using legacy Egistec (Nile) HAL");
                bio = new LegacyEgistecHAL(std::move(dev));
            } else {
                ALOGI("Using new Egistec (Ganges+) HAL on Nile");
                bio = new CurrentEgistecHAL(std::move(dev));
            }
            break;
        case egistec::FpHwId::Fpc:
            ALOGI("FPC sensor installed");
            bio = new FPCHAL();
            break;
        default:
            ALOGE("No HAL instance defined for hardware type %d", type);
            return 1;
    }
#elif defined(FINGERPRINT_TYPE_EGISTEC)
    bio = new CurrentEgistecHAL(std::move(dev));
#else
    bio = new FPCHAL();
#endif

    configureRpcThreadpool(1, true /*callerWillJoin*/);

    if (bio != nullptr) {
        status_t status = bio->registerAsService();
        if (status != NO_ERROR) {
            ALOGE("Cannot start fingerprint service: %d", status);
            return 1;
        }
    } else {
        ALOGE("Can't create instance of BiometricsFingerprint, nullptr");
        return 1;
    }

    joinRpcThreadpool();

    return 0;  // should never get here
}
