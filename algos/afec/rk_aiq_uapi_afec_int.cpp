/*
 * rk_aiq_uapi_afec_int.cpp
 *
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

#include "rk_aiq_uapi_afec_int.h"
#include "afec/rk_aiq_types_afec_algo_prvt.h"

XCamReturn
rk_aiq_uapi_afec_enable(RkAiqAlgoContext *ctx)
{
    FECHandle_t fec_contex = (FECHandle_t)ctx->hFEC;;
    if (fec_contex->fec_en)
        fec_contex->user_en = true;
    else
	return XCAM_RETURN_ERROR_FAILED;

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_uapi_afec_disable(RkAiqAlgoContext *ctx)
{
    FECHandle_t fec_contex = (FECHandle_t)ctx->hFEC;;
    if (fec_contex->fec_en)
	fec_contex->user_en = false;
    else
	return XCAM_RETURN_ERROR_FAILED;

    return XCAM_RETURN_NO_ERROR;
}
