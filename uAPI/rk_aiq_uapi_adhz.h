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

#ifndef __RK_AIQ_UAPI_ADHZ_H__
#define __RK_AIQ_UAPI_ADHZ_H__

#include "base/xcam_common.h"
#include "rk_aiq.h"

typedef struct adhzAttr_s{
	int sw_dhaz_en = 1;
	int cfg_alpha = 0;
	int enchance_en = 0;
		
} adhzAttr_t;

typedef struct mdhzAttr_s{
	int sw_dhaz_en = 1;
	int cfg_alpha = 1;
	int enchance_en = 0;
} mdhzAttr_t;


typedef struct dhzAttr_s {
    opMode_t op;
    adhzAttr_t stAuto;
    mdhzAttr_t stManual;
｝dhzAttr_t;

XCamReturn rk_aiq_uapi_setDhzAttr(const rk_aiq_sys_ctx_t* ctx, dhzAttr_t *attr);
XCamReturn rk_aiq_uapi_getDhzAttr(const rk_aiq_sys_ctx_t* ctx, dhzAttr_t *attr);


#endif

