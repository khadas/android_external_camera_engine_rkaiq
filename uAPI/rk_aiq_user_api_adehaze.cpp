#include "rk_aiq_user_api_adehaze.h"
#include "RkAiqHandleInt.h"




XCamReturn  rk_aiq_user_api_adehaze_setSwAttrib(rk_aiq_sys_ctx_t* sys_ctx, adehaze_sw_t attr)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    RkAiqAdhazHandleInt* algo_handle =
        algoHandle<RkAiqAdhazHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_ADHAZ);

    if (algo_handle) {
        return algo_handle->setSwAttrib(attr);
    }

    return (ret);
}

XCamReturn  rk_aiq_user_api_adehaze_getSwAttrib(rk_aiq_sys_ctx_t* sys_ctx, adehaze_sw_t *attr)
{
	XCamReturn ret = XCAM_RETURN_NO_ERROR;
	
	RkAiqAdhazHandleInt* algo_handle =
		algoHandle<RkAiqAdhazHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_ADHAZ);
	
	if (algo_handle) {
		return algo_handle->getSwAttrib(attr);
	}
	
	return (ret);

}




