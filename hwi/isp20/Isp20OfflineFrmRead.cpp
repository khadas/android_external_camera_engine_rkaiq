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

#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "Isp20PollThread.h"
#include "Isp20OfflineFrmRead.h"

namespace RkCam {

bool
OfflineFrmRdThread::initialize () {
    _mfd = open("/dev/mem", O_RDWR | O_SYNC);
    if (-1 == _mfd) {
        LOGE_CAMHW("%s: open /dev/mem failed\n", __FUNCTION__);
        return false;
    }

    if (0 > _isp_dev->io_control(RKISP_CMD_GET_SHARED_BUF, &_resmem)) {
        LOGE_CAMHW("%s: get isp shared buf failed\n", __FUNCTION__);
        return false;
    }
    if ((0 == _resmem.resmem_padr) || (0 == _resmem.resmem_size)) {
        LOGE_CAMHW("%s: isp shared buf is incorrect\n", __FUNCTION__);
        return false;
    }
    LOGD_CAMHW("%s: _resmem.resmem_padr 0x%x, 0x%x\n", __FUNCTION__, _resmem.resmem_padr, _resmem.resmem_size);
    _mem_vir_addr = (uint8_t *)mmap(NULL, _resmem.resmem_size, PROT_WRITE, MAP_SHARED, _mfd, _resmem.resmem_padr);
    if (NULL == _mem_vir_addr) {
        LOGE_CAMHW("%s: mmap failed\n", __FUNCTION__);
        return false;
    }
    memcpy(&_resmem_head, _mem_vir_addr, sizeof(_resmem_head));

    _buffer_size = _resmem_head.width * _resmem_head.height * 12 / 8;

    if (_resmem_head.hdr_mode == 0)
        _rx_dev_max = 1;
    else if (_resmem_head.hdr_mode == 5)
        _rx_dev_max = 2;
    else if (_resmem_head.hdr_mode == 6)
        _rx_dev_max = 3;
    else
        LOGE_CAMHW("%s: hdr_mode (%d) in resmem_head is wrong!",  __FUNCTION__, _resmem_head.hdr_mode);

    _vbuf_count = _rx_dev[0]->get_buffer_count();
    LOGD_CAMHW("%s: w: %d, h: %d, hdr: %d, buf_size: %d, dev_max: %d, vbuf_count: %d\n", __FUNCTION__,
               _resmem_head.width, _resmem_head.height, _resmem_head.hdr_mode, _buffer_size, _rx_dev_max, _vbuf_count);
    _initialized = true;
    _poll->set_loop_status(true);
    return true;
}

void
OfflineFrmRdThread::dequeue_all_buffer () {
    struct v4l2_buffer v4l2_buf;
    struct v4l2_plane planes[FMT_NUM_PLANES];

    memset(planes, 0, sizeof(struct v4l2_plane) * FMT_NUM_PLANES);
    memset(&v4l2_buf, 0, sizeof(v4l2_buf));
    for (int i = 0; i < _vbuf_count; i++) {
        v4l2_buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
        v4l2_buf.memory = V4L2_MEMORY_USERPTR;
        v4l2_buf.m.planes = planes;
        v4l2_buf.length = FMT_NUM_PLANES;
        for (int j = 0; j < _rx_dev_max; j++) {
            if (-1 == _rx_dev[j]->io_control(VIDIOC_DQBUF, &v4l2_buf))
                LOGE_CAMHW("%s: VIDIOC_DQBUF failed, device name: %s\n", __FUNCTION__, _rx_dev[j]->get_device_name());
        }
    }
}

void
OfflineFrmRdThread::queue_all_buffer() {
    struct v4l2_buffer v4l2_buf;
    struct v4l2_plane planes[FMT_NUM_PLANES];

    for (int i = 0; i < _vbuf_count; i++, _queued_index++) {
        memset(&v4l2_buf, 0, sizeof(v4l2_buf));
        v4l2_buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
        v4l2_buf.memory = V4L2_MEMORY_USERPTR;
        v4l2_buf.index = i;
        memset(planes, 0, sizeof(struct v4l2_plane) * FMT_NUM_PLANES);
        v4l2_buf.m.planes = planes;
        v4l2_buf.m.planes[0].length = _buffer_size;
        v4l2_buf.length = FMT_NUM_PLANES;
        v4l2_buf.m.planes[0].bytesused = v4l2_buf.m.planes[0].length;
        for(int j = 0; j < _rx_dev_max; j++) {
            v4l2_buf.m.planes[0].m.userptr = get_buffer_addr(j);
            if (-1 == _rx_dev[j]->io_control(VIDIOC_QBUF, &v4l2_buf))
                LOGE_CAMHW("%s: VIDIOC_QBUF failed, device name: %s\n", __FUNCTION__, _rx_dev[j]->get_device_name());
        }
        triger_readback();
        usleep(ISP2X_FASTBOOT_DELAY_MS * 1000);
    }
}

unsigned long
OfflineFrmRdThread::get_buffer_addr (int index) {
    unsigned long addr = 0;

    if (index == 0)
        addr = (unsigned long)(_mem_vir_addr + ((u32)_resmem_head.s_buf[_queued_index].bufaddr - _resmem.resmem_padr));
    else if (index == 1)
        addr = (unsigned long)(_mem_vir_addr + ((u32)_resmem_head.m_buf[_queued_index].bufaddr - _resmem.resmem_padr));
    else if (index == 2)
        addr = (unsigned long)(_mem_vir_addr + ((u32)_resmem_head.l_buf[_queued_index].bufaddr - _resmem.resmem_padr));
    else
        LOGE_CAMHW("index (%d )is wrong!\n", index);

    return addr;
}

void
OfflineFrmRdThread::triger_readback()
{
    struct isp2x_csi_trigger tg;

    tg.frame_timestamp = _resmem_head.s_buf[_queued_index].timestamp;
    tg.frame_id = _resmem_head.s_buf[_queued_index].frame_id;
    tg.times = _rx_dev_max;
    LOGD_CAMHW("%s: id: %d, timestamp: %lld, times: %d\n", __FUNCTION__, tg.frame_id, tg.frame_timestamp, tg.times);
    _isp_dev->io_control(RKISP_CMD_TRIGGER_READ_BACK, &tg);
}

bool
OfflineFrmRdThread::loop () {
    if (_initialized) {
        struct v4l2_buffer v4l2_buf;
        struct v4l2_plane planes[FMT_NUM_PLANES];

        queue_all_buffer();

        while(_queued_index < _resmem_head.frm_total) {
            memset(&v4l2_buf, 0, sizeof(v4l2_buf));
            v4l2_buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
            v4l2_buf.memory = V4L2_MEMORY_USERPTR;
            memset(planes, 0, sizeof(struct v4l2_plane) * FMT_NUM_PLANES);
            v4l2_buf.m.planes = planes;
            v4l2_buf.length = FMT_NUM_PLANES;
            v4l2_buf.m.planes[0].length = _buffer_size;
            v4l2_buf.m.planes[0].bytesused = v4l2_buf.m.planes[0].length;

            for(int i = 0; i < _rx_dev_max; i++) {
                if (-1 == _rx_dev[i]->io_control(VIDIOC_DQBUF, &v4l2_buf))
                    LOGE_CAMHW("%s: VIDIOC_DQBUF failed, device name: %s\n", __FUNCTION__, _rx_dev[i]->get_device_name());

                v4l2_buf.m.planes[0].m.userptr = get_buffer_addr(i);

                if (-1 == _rx_dev[i]->io_control(VIDIOC_QBUF, &v4l2_buf))
                    LOGE_CAMHW("%s: VIDIOC_QBUF failed, device name: %s\n", __FUNCTION__, _rx_dev[i]->get_device_name());
            }
            triger_readback();
            _queued_index++;
            usleep(ISP2X_FASTBOOT_DELAY_MS * 1000);
        }

        dequeue_all_buffer();
        uninitialize();
        _poll->set_loop_status(false);
    }
    return false;
}

void
OfflineFrmRdThread::set_mipi_devs(SmartPtr<V4l2Device> mipi_rx_devs[3], SmartPtr<V4l2SubDevice> isp_dev)
{
    _isp_dev = isp_dev;

    for (int i = 0; i < 3; i++) {
        _rx_dev[i] = mipi_rx_devs[i];
    }
}

void
OfflineFrmRdThread::uninitialize(void)
{
    if (-1 == munmap(_mem_vir_addr, _resmem.resmem_size))
        LOGE_CAMHW("%s: munmap failed\n", __FUNCTION__);
    if (-1 != _mfd)
        close(_mfd);
}

}; //namspace RkCam
