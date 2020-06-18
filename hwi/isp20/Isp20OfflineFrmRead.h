/*
 * Isp20OfflineFrmRead.h - isp20 offline frame reading thread for event and buffer
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
#ifndef _ISP20_OFFLINE_FRM_READ_H_
#define _ISP20_OFFLINE_FRM_READ_H_

#include "xcam_thread.h"
#include "poll_thread.h"
#include "SensorHw.h"
#include "LensHw.h"
#include "CamHwIsp20.h"

using namespace XCam;

namespace RkCam {

#define ISP2X_FASTBOOT_DELAY_MS         100
#define ISP2X_FASTBOOT_VIDEO_BUF_NUM    30

#define RKISP_CMD_GET_SHARED_BUF \
    _IOR('V', BASE_VIDIOC_PRIVATE + 2, struct rkisp_fastboot_resmem)

struct rkisp_fastboot_video_buf {
    u32 index;
    u32 frame_id;
    u32 timestamp;
    u32 time_reg;
    u32 gain_reg;
    u8 *bufaddr;
    u32 bufsize;
} __attribute__ ((packed));

/**
 * struct rkisp_fastboot_resmem_head
 */
struct rkisp_fastboot_resmem_head {
    u16 enable;
    u16 complete;
    u16 frm_total;
    u16 hdr_mode;
    u16 width;
    u16 height;
    u32 bus_fmt;

    struct rkisp_fastboot_video_buf l_buf[ISP2X_FASTBOOT_VIDEO_BUF_NUM];
    struct rkisp_fastboot_video_buf m_buf[ISP2X_FASTBOOT_VIDEO_BUF_NUM];
    struct rkisp_fastboot_video_buf s_buf[ISP2X_FASTBOOT_VIDEO_BUF_NUM];
} __attribute__ ((packed));

/**
 * struct rkisp_fastboot_resmem - shared buffer for fastboot with risc-v side
 */
struct rkisp_fastboot_resmem {
    u32 resmem_padr;
    u32 resmem_size;
} __attribute__ ((packed));

class OfflineFrmRdThread
    : public Thread
{
public:
    explicit OfflineFrmRdThread (Isp20PollThread *poll)
        : Thread("offlineFrmRead"),
          _poll (poll) {
        _queued_index = 0;
        _mfd = -1;
        _mem_vir_addr = NULL;
        _rx_dev_max = 0;
        _buffer_size = 0;
        _vbuf_count = 0;
        _initialized = false;
    }
    bool initialize();
    void set_mipi_devs(SmartPtr<V4l2Device> mipi_rx_devs[3], SmartPtr<V4l2SubDevice> isp_dev);
protected:
    virtual bool loop ();
    void uninitialize();
    void queue_all_buffer();
    void dequeue_all_buffer ();
    unsigned long get_buffer_addr(int index);
    void triger_readback();
private:
    Isp20PollThread *_poll;
    int _mfd;
    uint8_t *_mem_vir_addr;
    struct rkisp_fastboot_resmem _resmem;
    struct rkisp_fastboot_resmem_head _resmem_head;
    SmartPtr<V4l2SubDevice> _isp_dev;
    SmartPtr<V4l2Device>  _rx_dev[3];
    int _rx_dev_max;
    int _queued_index;
    int _buffer_size;
    int _vbuf_count;
    bool _initialized;
};

}
#endif
