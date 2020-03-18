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

#ifndef _RK_AIQ_H_
#define _RK_AIQ_H_

#include "base/xcam_common.h"
#include "rk_aiq_types.h"

XCAM_BEGIN_DECLARE

typedef struct rk_aiq_static_metas_s {
} rk_aiq_static_metas_t;

typedef struct rk_aiq_metas_s {
    uint32_t frame_id;
} rk_aiq_metas_t;

typedef enum rk_aiq_err_code {

} rk_aiq_err_code_e;

typedef struct rk_aiq_err_msg_s {
    int err_code;
} rk_aiq_err_msg_t;

typedef XCamReturn (*rk_aiq_error_cb)(rk_aiq_err_msg_t* err_msg);
typedef XCamReturn (*rk_aiq_metas_cb)(rk_aiq_metas_t* metas);

XCAM_END_DECLARE

#endif
