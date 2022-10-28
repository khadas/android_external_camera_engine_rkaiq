#include "rk_aiq_comm.h"
#include "rkispp-config.h"
#include "rkisp2-config.h"
#include "PdafStreamProcUnit.h"
#include "CamHwIsp20.h"
#include "code_to_pixel_format.h"
#include "rkcif-config.h"

namespace RkCam {

PdafStreamProcUnit::PdafStreamProcUnit (int type)
{
    mPdafDev = NULL;
    mPdafStream = NULL;
    mStartFlag = false;
    mBufType = type;
}

PdafStreamProcUnit::~PdafStreamProcUnit ()
{
}

void PdafStreamProcUnit::set_devices(CamHwIsp20* camHw)
{
    mCamHw = camHw;
}

XCamReturn
PdafStreamProcUnit::prepare(rk_sensor_pdaf_info_t *pdaf_inf)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    uint32_t pixelformat;

    stop();
    mPdafDev = new V4l2Device(pdaf_inf->pdaf_vdev);
    mPdafDev->open();
    mPdafStream = new RKPdafStream(mPdafDev, ISP_POLL_PDAF_STATS);
    mPdafStream->setPollCallback (this);

    mPdafMeas.pdLRInDiffLine = pdaf_inf->pdaf_lrdiffline;
    mPdafMeas.pdWidth = pdaf_inf->pdaf_width;
    mPdafMeas.pdHeight = pdaf_inf->pdaf_height;
    ret = mPdafDev->set_format(pdaf_inf->pdaf_width, pdaf_inf->pdaf_height,
            pdaf_inf->pdaf_pixelformat, V4L2_FIELD_NONE, 0);

    return ret;
}

void PdafStreamProcUnit::start()
{
    int mem_mode;

    if (mPdafStream.ptr() && !mStartFlag) {
        mPdafDev->io_control (RKCIF_CMD_GET_CSI_MEMORY_MODE, &mem_mode);
        if (mem_mode != CSI_LVDS_MEM_WORD_LOW_ALIGN) {
            mem_mode = CSI_LVDS_MEM_WORD_LOW_ALIGN;
            mPdafDev->io_control (RKCIF_CMD_SET_CSI_MEMORY_MODE, &mem_mode);
            LOGI_AF("memory mode of pdaf video need low align, mem_mode %d", mem_mode);
        }
        mPdafStream->start();
        mStartFlag = true;
    }
}

void PdafStreamProcUnit::stop()
{
    if (mPdafStream.ptr() && mStartFlag) {
        mPdafStream->stop();
        mStartFlag = false;
    }
}

XCamReturn
PdafStreamProcUnit::poll_buffer_ready (SmartPtr<V4l2BufferProxy> &buf, int dev_index)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    if (mCamHw->mHwResLintener) {
        SmartPtr<VideoBuffer> video_buf = buf.dynamic_cast_ptr<VideoBuffer>();
        SmartPtr<PdafBufferProxy> pdaf_buf = buf.dynamic_cast_ptr<PdafBufferProxy>();

        pdaf_buf->pdaf_meas = mPdafMeas;
        mCamHw->mHwResLintener->hwResCb(video_buf);
    }

    return ret;
}

}
