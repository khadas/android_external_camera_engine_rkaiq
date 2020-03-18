/*
* alut3d.cpp

* for rockchip v2.0.0
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
/* for rockchip v2.0.0*/

#include "a3dlut/rk_aiq_a3dlut_algo.h"
#include "xcam_log.h"
RKAIQ_BEGIN_DECLARE




/******************************************************************************
 * Alut3dConfig()
 *****************************************************************************/
XCamReturn Alut3dConfig
(
    alut3d_handle_t hAlut3d
) {

    LOGI_A3DLUT("%s: (enter)\n", __FUNCTION__);

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    if (hAlut3d == NULL) {
        return XCAM_RETURN_ERROR_PARAM;
    }
    if(hAlut3d->calib_lut3d->enable == true) {
        hAlut3d->lut3d_hw_conf.enable = true;
        hAlut3d->lut3d_hw_conf.bypass_en = false;
    } else {
        hAlut3d->lut3d_hw_conf.enable = false;
        hAlut3d->lut3d_hw_conf.bypass_en = true;
    }
    hAlut3d->lut3d_hw_conf.lut3d_lut_wsize = 0x2d9;
    memcpy(hAlut3d->lut3d_hw_conf.look_up_table_r, hAlut3d->calib_lut3d->look_up_table_r,
           sizeof(hAlut3d->calib_lut3d->look_up_table_r));
    memcpy(hAlut3d->lut3d_hw_conf.look_up_table_g, hAlut3d->calib_lut3d->look_up_table_g,
           sizeof(hAlut3d->calib_lut3d->look_up_table_g));
    memcpy(hAlut3d->lut3d_hw_conf.look_up_table_b, hAlut3d->calib_lut3d->look_up_table_b,
           sizeof(hAlut3d->calib_lut3d->look_up_table_b));

    LOGI_A3DLUT("%s: (exit)\n", __FUNCTION__);

    return (ret);
}


XCamReturn Alut3dInit(alut3d_handle_t *hAlut3d)
{
    LOGI_A3DLUT("%s: (enter)\n", __FUNCTION__);

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    *hAlut3d = (alut3d_context_t*)malloc(sizeof(alut3d_context_t));

    LOGI_A3DLUT("%s: (exit)\n", __FUNCTION__);
    return(ret);


}

XCamReturn Alut3dRelease(alut3d_handle_t hAlut3d)
{
    LOGI_A3DLUT("%s: (enter)\n", __FUNCTION__);

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    free(hAlut3d);

    LOGI_A3DLUT("%s: (exit)\n", __FUNCTION__);
    return(ret);

}

XCamReturn Alut3dPrepare(const CalibDb_Lut3d_t *calib_lut3d, alut3d_handle_t hAlut3d)
{
    LOGI_A3DLUT("%s: (enter)\n", __FUNCTION__);

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    hAlut3d->calib_lut3d = calib_lut3d;

    LOGI_A3DLUT("%s: (exit)\n", __FUNCTION__);
    return ret;
}
XCamReturn Alut3dPreProc(alut3d_handle_t hAlut3d)
{

    LOGI_A3DLUT("%s: (enter)\n", __FUNCTION__);

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    LOGI_A3DLUT("%s: (exit)\n", __FUNCTION__);
    return(ret);

}
XCamReturn Alut3dProcessing(alut3d_handle_t hAlut3d)
{
    LOGI_A3DLUT("%s: (enter)\n", __FUNCTION__);

    XCamReturn ret = XCAM_RETURN_NO_ERROR;


    LOGI_A3DLUT("%s: (exit)\n", __FUNCTION__);
    return(ret);
}




RKAIQ_END_DECLARE


