/*
 *  Copyright (c) 2019 Rockchip Corporation
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
 *
 */

#ifndef _RK_AIQ_CALIB_VERSION_H_
/*!
 * ==================== AIQ CALIB VERSION HISTORY ====================
 *
 * v1.0.0
 *  - initial version
 * v1.0.1
 * v1.1.0
 *  - add xml tag check
 * v1.1.1
 *  - magic code:996625
 *  - awb
 *  - difference tolerance and run interval in difference luma value
 * v1.1.2
 *  - magic code:998276
 *  - add COLOR_AS_GREY tag
 * v1.1.3
 *  - magic code:1003001
 *  - Aec: add sensor-AEC-LinearAECtrl-RawStatsEn, ae stats can use rawae/yuvae
 *  - Aec: add system-dcg_setting divide into 2 parts -normal /Hdr
 *  - Aec: add system-Gainrange-Linear/NonLinear, support db_mode
 */

#define RK_AIQ_CALIB_VERSION_REAL_V          "v1.1.3"
#define RK_AIQ_CALIB_VERSION_MAGIC_V         "1003001"

/******* DO NOT EDIT THE FOLLOWINGS ***********/

#define RK_AIQ_CALIB_VERSION_HEAD            "Calib "
#define RK_AIQ_CALIB_VERSION_MAGIC_JOINT     ","
#define RK_AIQ_CALIB_VERSION_MAGIC_CODE_HEAD "magicCode:"
#define RK_AIQ_CALIB_VERSION \
    RK_AIQ_CALIB_VERSION_HEAD\
    RK_AIQ_CALIB_VERSION_REAL_V\
    RK_AIQ_CALIB_VERSION_MAGIC_JOINT\
    RK_AIQ_CALIB_VERSION_MAGIC_CODE_HEAD\
    RK_AIQ_CALIB_VERSION_MAGIC_V

#endif
