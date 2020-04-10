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

#ifndef __RK_AIQ_UAPI_AF_H__
#define __RK_AIQ_UAPI_AF_H__
#include "base/xcam_common.h"
#include "rk_aiq.h"

typedef enum afAlgType_s {
    AF_ALG_TYPE_INVAILD =0,
    AF_ALG_TYPE_FULLRANGE =1,
    AF_ALG_TYPE_HILLCLIMBING =2,
} afAlgType_t;

typedef struct afAttr_s {
    afAlgType_t type;
} afAttr_t;

typedef struct mfAttr_s {

} mfAttr_t;

typedef struct focusAttr_s {
    bool      bEn;
	opMode_t  opMode;
	afAttr_t  stAuto;
	mfAttr_t stManual;
} focusAttr_t;

XCamReturn rk_aiq_uapi_setFocusAttr(const rk_aiq_sys_ctx_t* ctx, focusAttr_t *attr);
XCamReturn rk_aiq_uapi_getFocusAttr(const rk_aiq_sys_ctx_t* ctx, focusAttr_t *attr);

#endif