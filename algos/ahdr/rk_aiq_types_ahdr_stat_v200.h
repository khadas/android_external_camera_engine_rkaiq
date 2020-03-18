/******************************************************************************
 *
 * Copyright 2019, Fuzhou Rockchip Electronics Co.Ltd . All rights reserved.
 * No part of this work may be reproduced, modified, distributed, transmitted,
 * transcribed, or translated into any language or computer format, in any form
 * or by any means without written permission of:
 * Fuzhou Rockchip Electronics Co.Ltd .
 *
 *
 *****************************************************************************/
#ifndef __RK_AIQ_TYPES_AHDR_ALGO_V200_H__
#define __RK_AIQ_TYPES_AHDR_ALGO_V200_H__

typedef struct rkisp_ahdr_stats_s {
    unsigned short ro_hdrtmo_lgmin;
    unsigned short ro_hdrtmo_lglow;
    unsigned short ro_hdrtmo_lgmax;
    unsigned short ro_hdrtmo_lghigh;
    unsigned short ro_hdrtmo_lgmean;
    unsigned short ro_hdrtmo_weightkey;
    unsigned short ro_hdrtmo_lgrange0;
    unsigned short ro_hdrtmo_lgrange1;
    unsigned short ro_hdrtmo_lgavgmax;
    unsigned short ro_hdrtmo_palpha;
    unsigned short ro_hdrtmo_linecnt;
    unsigned int ro_array_min_max[32];
} rkisp_ahdr_stats_t;


#endif
