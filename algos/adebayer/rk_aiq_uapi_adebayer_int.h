#ifndef _RK_AIQ_UAPI_ADEBAYER_INT_H_
#define _RK_AIQ_UAPI_ADEBAYER_INT_H_

#include "base/xcam_common.h" 
#include "rk_aiq_algo_des.h" 

typedef struct adebayer_attrib_s {
  unsigned char clip_enable;
  unsigned char main_chan_en;
  unsigned char chroma_chan_en;
  unsigned char thed0;
  unsigned char thed1;
  unsigned char dist_scale;
  unsigned char gian_offset;
  unsigned char offset;
  unsigned short hf_offset[9];
  unsigned char sharp_strength[9];
} adebayer_attrib_t;

// need_sync means the implementation should consider
// the thread synchronization
// if called by RkAiqAdebayerHandleInt, the sync has been done
// in framework. And if called by user app directly, 
// sync should be done in inner. now we just need implement
// the case of need_sync == false; need_sync is for future usage.

XCamReturn
rk_aiq_uapi_adebayer_SetAttrib
(
    RkAiqAlgoContext* ctx,
    adebayer_attrib_t attr,
    bool need_sync = true
);

XCamReturn
rk_aiq_uapi_adebayer_GetAttrib
(
    RkAiqAlgoContext*  ctx,
    adebayer_attrib_t* attr
);

#endif//_RK_AIQ_UAPI_ADEBAYER_INT_H_