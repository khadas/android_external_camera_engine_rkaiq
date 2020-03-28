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

#include "Isp20PollThread.h"
#include "Isp20StatsBuffer.h"
#include "rkisp2-config.h"
#include "SensorHw.h"
#include <fcntl.h>

namespace RkCam {

const char*
Isp20PollThread::mipi_poll_type_to_str[ISP_POLL_MIPI_MAX] =
{
    "mipi_tx_poll",
    "mipi_rx_poll",
};

class MipiPollThread
    : public Thread
{
public:
    MipiPollThread (Isp20PollThread*poll, int type, int dev_index)
        : Thread (Isp20PollThread::mipi_poll_type_to_str[type])
        , _poll (poll)
        , _type (type)
        , _dev_index (dev_index)
    {}

protected:
    virtual bool loop () {
        XCamReturn ret = _poll->mipi_poll_buffer_loop (_type, _dev_index);

        if (ret == XCAM_RETURN_NO_ERROR || ret == XCAM_RETURN_ERROR_TIMEOUT ||
                XCAM_RETURN_BYPASS)
            return true;
        return false;
    }

private:
    Isp20PollThread *_poll;
    int _type;
    int _dev_index;
};

SmartPtr<VideoBuffer>
Isp20PollThread::new_video_buffer(SmartPtr<V4l2Buffer> buf,
                                  SmartPtr<V4l2Device> dev,
                                  int type)
{
    ENTER_CAMHW_FUNCTION();
    SmartPtr<VideoBuffer> video_buf = nullptr;

    if (type == ISP_POLL_3A_STATS) {
        SmartPtr<RkAiqIspParamsProxy> ispParams = nullptr;
        SmartPtr<RkAiqExpParamsProxy> expParams = nullptr;

        _event_handle_dev->getEffectiveParams(expParams, buf->get_buf().sequence);
        // TODO: get ispParams from isp dev;

        video_buf = new Isp20StatsBuffer(buf, dev, ispParams, expParams);
    } else
        return PollThread::new_video_buffer(buf, dev, type);
    EXIT_CAMHW_FUNCTION();

    return video_buf;
}

XCamReturn
Isp20PollThread::notify_sof (int64_t time, int frameid)
{
    ENTER_CAMHW_FUNCTION();
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    XCAM_ASSERT(_event_handle_dev.ptr());
    ret = _event_handle_dev->handle_sof(time, frameid);
    EXIT_CAMHW_FUNCTION();

    return ret;
}

bool
Isp20PollThread::set_event_handle_dev(SmartPtr<SensorHw> &dev)
{
    ENTER_CAMHW_FUNCTION();
    XCAM_ASSERT (dev.ptr());
    _event_handle_dev = dev;
    EXIT_CAMHW_FUNCTION();
    return true;
}

Isp20PollThread::Isp20PollThread()
    : PollThread()
    , _first_trigger(true)
{
    for (int i = 0; i < 3; i++) {
        SmartPtr<MipiPollThread> mipi_poll = new MipiPollThread(this, ISP_POLL_MIPI_TX, i);
        XCAM_ASSERT (mipi_poll.ptr ());
        _isp_mipi_tx_infos[i].loop = mipi_poll;

        mipi_poll = new MipiPollThread(this, ISP_POLL_MIPI_RX, i);
        XCAM_ASSERT (mipi_poll.ptr ());
        _isp_mipi_rx_infos[i].loop = mipi_poll;

        _isp_mipi_tx_infos[i].stop_fds[0] = -1;
        _isp_mipi_tx_infos[i].stop_fds[1] = -1;
        _isp_mipi_rx_infos[i].stop_fds[0] = -1;
        _isp_mipi_rx_infos[i].stop_fds[1] = -1;

    }

    XCAM_LOG_DEBUG ("Isp20PollThread constructed");
}

Isp20PollThread::~Isp20PollThread()
{
    stop();

    XCAM_LOG_DEBUG ("~Isp20PollThread destructed");
}


XCamReturn
Isp20PollThread::start ()
{
    if (create_stop_fds_mipi()) {
        XCAM_LOG_ERROR("create mipi stop fds failed !");
        return XCAM_RETURN_ERROR_UNKNOWN;
    }

    for (int i = 0; i < _mipi_dev_max; i++) {
        _isp_mipi_tx_infos[i].loop->start ();
        _isp_mipi_rx_infos[i].loop->start ();
    }
    return PollThread::start ();
}

XCamReturn
Isp20PollThread::stop ()
{
    for (int i = 0; i < _mipi_dev_max; i++) {
        if (_isp_mipi_tx_infos[i].dev.ptr ()) {
            if (_isp_mipi_tx_infos[i].stop_fds[1] != -1) {
                char buf = 0xf;  // random value to write to flush fd.
                unsigned int size = write(_isp_mipi_tx_infos[i].stop_fds[1], &buf, sizeof(char));
                if (size != sizeof(char))
                    XCAM_LOG_WARNING("Flush write not completed");
            }
            _isp_mipi_tx_infos[i].loop->stop ();
            _isp_mipi_tx_infos[i].buf_list.clear ();
            _isp_mipi_tx_infos[i].cache_list.clear ();
        }

        if (_isp_mipi_rx_infos[i].dev.ptr ()) {
            if (_isp_mipi_rx_infos[i].stop_fds[1] != -1) {
                char buf = 0xf;  // random value to write to flush fd.
                unsigned int size = write(_isp_mipi_rx_infos[i].stop_fds[1], &buf, sizeof(char));
                if (size != sizeof(char))
                    XCAM_LOG_WARNING("Flush write not completed");
            }
            _isp_mipi_rx_infos[i].loop->stop ();
            _isp_mipi_rx_infos[i].buf_list.clear ();
            _isp_mipi_rx_infos[i].cache_list.clear ();
        }
    }

    _isp_hdr_fid2times_map.clear();
    _isp_hdr_fid2ready_map.clear();
    destroy_stop_fds_mipi ();

    return PollThread::stop ();
}

void
Isp20PollThread::set_working_mode(int mode)
{
   _working_mode = mode;

    switch (_working_mode) {
        case RK_AIQ_ISP_HDR_MODE_3_FRAME_HDR:
        case RK_AIQ_ISP_HDR_MODE_3_LINE_HDR:
            _mipi_dev_max = 3;
            break;
        case RK_AIQ_ISP_HDR_MODE_2_FRAME_HDR:
        case RK_AIQ_ISP_HDR_MODE_2_LINE_HDR:
            _mipi_dev_max = 2;
            break;
        default:
            _mipi_dev_max = 0;
    }
}

void
Isp20PollThread::set_mipi_devs(SmartPtr<V4l2Device> mipi_tx_devs[3],
                               SmartPtr<V4l2Device> mipi_rx_devs[3],
                               SmartPtr<V4l2SubDevice> isp_dev)
{
    _isp_core_dev = isp_dev;
    for (int i = 0; i < 3; i++) {
        _isp_mipi_tx_infos[i].dev = mipi_tx_devs[i];
        _isp_mipi_rx_infos[i].dev = mipi_rx_devs[i];
    }
}

XCamReturn
Isp20PollThread::hdr_mipi_start(SmartPtr<SensorHw> sensor)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    uint32_t width, height, pixelformat;

    if (!sensor.ptr()) {
        //TODO no sensor case
        return XCAM_RETURN_ERROR_SENSOR;
    } else {
        rk_aiq_exposure_sensor_descriptor sns_des;
        sensor->get_format(&sns_des);
        width = sns_des.sensor_output_width;
        height = sns_des.sensor_output_height;
        pixelformat = sns_des.sensor_pixelformat;
    }

    // mipi rx/tx format should match to sensor.
    for (int i = 0; i < _mipi_dev_max; i++) {
        _isp_mipi_tx_infos[i].dev->set_format(width, height, pixelformat,
                                              V4L2_FIELD_NONE, 0);
        ret = _isp_mipi_tx_infos[i].dev->start();
        if (ret < 0)
            LOGE("mipi tx:%d start err: %d\n", ret);

        _isp_mipi_rx_infos[i].dev->set_format(width, height, pixelformat,
                                              V4L2_FIELD_NONE, 0);
        ret = _isp_mipi_rx_infos[i].dev->start();
        if (ret < 0)
            LOGE("mipi rx:%d start err: %d\n", ret);
    }

    return ret;
}

XCamReturn
Isp20PollThread::hdr_mipi_stop()
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    for (int i = 0; i < _mipi_dev_max; i++) {
        ret = _isp_mipi_tx_infos[i].dev->stop();
        if (ret < 0)
            LOGE("mipi tx:%d stop err: %d\n", ret);
        ret = _isp_mipi_rx_infos[i].dev->stop();
        if (ret < 0)
            LOGE("mipi rx:%d stop err: %d\n", ret);
    }

    return ret;
}

void
Isp20PollThread::set_hdr_frame_readback_infos(int frame_id, int times)
{
    _mipi_trigger_mutex.lock();
    _isp_hdr_fid2times_map[frame_id] = times;
    _mipi_trigger_mutex.unlock();
    trigger_readback(frame_id);
}

XCamReturn
Isp20PollThread::create_stop_fds_mipi () {
    int i, status = 0;
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    destroy_stop_fds_mipi();

    for (i = 0; i < _mipi_dev_max; i++) {
        status = pipe(_isp_mipi_tx_infos[i].stop_fds);
        if (status < 0) {
            XCAM_LOG_ERROR ("Failed to create mipi tx:%d poll stop pipe: %s",
                            i, strerror(errno));
            ret = XCAM_RETURN_ERROR_UNKNOWN;
            goto exit_error;
        }
        status = fcntl(_isp_mipi_tx_infos[0].stop_fds[0], F_SETFL, O_NONBLOCK);
        if (status < 0) {
            XCAM_LOG_ERROR ("Fail to set event mipi tx:%d stop pipe flag: %s",
                            i, strerror(errno));
            goto exit_error;
        }

        status = pipe(_isp_mipi_rx_infos[i].stop_fds);
        if (status < 0) {
            XCAM_LOG_ERROR ("Failed to create mipi rx:%d poll stop pipe: %s",
                            i, strerror(errno));
            ret = XCAM_RETURN_ERROR_UNKNOWN;
            goto exit_error;
        }
        status = fcntl(_isp_mipi_rx_infos[0].stop_fds[0], F_SETFL, O_NONBLOCK);
        if (status < 0) {
            XCAM_LOG_ERROR ("Fail to set event mipi rx:%d stop pipe flag: %s",
                            i, strerror(errno));
            goto exit_error;
        }
    }

    return XCAM_RETURN_NO_ERROR;
exit_error:
    destroy_stop_fds_mipi();
    return ret;
}

void Isp20PollThread::destroy_stop_fds_mipi () {
    for (int i = 0; i < 3; i++) {
        if (_isp_mipi_tx_infos[i].stop_fds[0] != -1 ||
            _isp_mipi_tx_infos[i].stop_fds[1] != -1) {
            close(_isp_mipi_tx_infos[i].stop_fds[0]);
            close(_isp_mipi_tx_infos[i].stop_fds[1]);
            _isp_mipi_tx_infos[i].stop_fds[0] = -1;
            _isp_mipi_tx_infos[i].stop_fds[1] = -1;
        }

        if (_isp_mipi_rx_infos[i].stop_fds[0] != -1 ||
            _isp_mipi_rx_infos[i].stop_fds[1] != -1) {
            close(_isp_mipi_rx_infos[i].stop_fds[0]);
            close(_isp_mipi_rx_infos[i].stop_fds[1]);
            _isp_mipi_rx_infos[i].stop_fds[0] = -1;
            _isp_mipi_rx_infos[i].stop_fds[1] = -1;
        }
    }
}

XCamReturn
Isp20PollThread::mipi_poll_buffer_loop (int type, int dev_index)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    int poll_ret = 0;
    SmartPtr<V4l2Buffer> buf;
    SmartPtr<V4l2Device> dev;
    int stop_fd = -1;

    if (type == ISP_POLL_MIPI_TX) {
        dev = _isp_mipi_tx_infos[dev_index].dev;
        stop_fd = _isp_mipi_tx_infos[dev_index].stop_fds[0];
    } else if (type == ISP_POLL_MIPI_RX) {
        dev = _isp_mipi_rx_infos[dev_index].dev;
        stop_fd = _isp_mipi_rx_infos[dev_index].stop_fds[0];
    } else
        return XCAM_RETURN_ERROR_UNKNOWN;

    poll_ret = dev->poll_event (PollThread::default_poll_timeout,
                                stop_fd);

    if (poll_ret == POLL_STOP_RET) {
        XCAM_LOG_DEBUG ("poll %s buffer stop success !", mipi_poll_type_to_str[type]);
        // stop success, return error to stop the poll thread
        return XCAM_RETURN_ERROR_UNKNOWN;
    }

    if (poll_ret <= 0) {
        XCAM_LOG_ERROR ("mipi_dev_index %d poll %s buffer event got error(0x%x) but continue\n",
                        dev_index, mipi_poll_type_to_str[type], poll_ret);
        ::usleep (10000); // 10ms
        return XCAM_RETURN_ERROR_TIMEOUT;
    }

    ret = dev->dequeue_buffer (buf);
    if (ret != XCAM_RETURN_NO_ERROR) {
        XCAM_LOG_WARNING ("dequeue %s buffer failed", mipi_poll_type_to_str[type]);
        return ret;
    }

    SmartPtr<V4l2BufferProxy> buf_proxy = new V4l2BufferProxy(buf, dev);
    if (type == ISP_POLL_MIPI_TX) {
        handle_tx_buf(buf_proxy, dev_index);
    } else if (type == ISP_POLL_MIPI_RX) {
        handle_rx_buf(buf_proxy, dev_index);
    }

    return ret;
}

void
Isp20PollThread::handle_rx_buf(SmartPtr<V4l2BufferProxy> &rx_buf, int dev_index)
{
    SmartPtr<V4l2BufferProxy> buf = _isp_mipi_rx_infos[dev_index].buf_list.pop(-1);
    XCAM_LOG_DEBUG ("%s dev_index:%d index:%d fd:%d\n",
        __func__, dev_index, buf->get_v4l2_buf_index(), buf->get_expbuf_fd());
}

void Isp20PollThread::sync_tx_buf()
{
    SmartPtr<V4l2BufferProxy> buf_s, buf_m, buf_l;
    uint32_t sequence_s = -1, sequence_m = -1, sequence_l = -1;

    _mipi_buf_mutex.lock();

    for (int i = 0; i < _mipi_dev_max; i++) {
        if (_isp_mipi_tx_infos[i].buf_list.is_empty()) {
            _mipi_buf_mutex.unlock();
            return;
        }
    }

    buf_l = _isp_mipi_tx_infos[ISP_MIPI_HDR_L].buf_list.front();
    if (buf_l.ptr())
        sequence_l = buf_l->get_sequence();

    buf_m = _isp_mipi_tx_infos[ISP_MIPI_HDR_M].buf_list.front();
    if (buf_m.ptr())
        sequence_m = buf_m->get_sequence();

    buf_s = _isp_mipi_tx_infos[ISP_MIPI_HDR_S].buf_list.front();
    if (buf_s.ptr()) {
        sequence_s = buf_s->get_sequence();
        // drop mid frame buf if mid frame id less than short frame id
        while (buf_m.ptr() && sequence_m < sequence_s) {
            // drop mid frame buf if mid frame id less than mid frame id
            XCAM_LOG_WARNING("%s drop mid frame:%d < short frame:%d\n",
                __func__, sequence_m, sequence_s);
            _isp_mipi_tx_infos[ISP_MIPI_HDR_M].buf_list.erase(buf_m);
            buf_m = _isp_mipi_tx_infos[ISP_MIPI_HDR_M].buf_list.front();
            if (buf_m.ptr())
                sequence_m = buf_m->get_sequence();
        }
        // drop long frame buf if long frame id less than short frame id
        while (buf_l.ptr() && sequence_l < sequence_s) {
            XCAM_LOG_WARNING("%s drop long frame:%d < short frame:%d\n",
                __func__, sequence_l, sequence_s);
            _isp_mipi_tx_infos[ISP_MIPI_HDR_L].buf_list.erase(buf_l);
            buf_l = _isp_mipi_tx_infos[ISP_MIPI_HDR_L].buf_list.front();
            if (buf_l.ptr())
                sequence_l = buf_l->get_sequence();
        }

        if ((_working_mode == RK_AIQ_ISP_HDR_MODE_3_FRAME_HDR ||
             _working_mode == RK_AIQ_ISP_HDR_MODE_3_LINE_HDR) &&
            buf_m.ptr() && buf_l.ptr() && buf_s.ptr() &&
            sequence_l == sequence_s && sequence_m == sequence_s) {
            _isp_mipi_rx_infos[ISP_MIPI_HDR_S].cache_list.push(buf_s);
            _isp_mipi_rx_infos[ISP_MIPI_HDR_M].cache_list.push(buf_m);
            _isp_mipi_rx_infos[ISP_MIPI_HDR_L].cache_list.push(buf_l);

            _isp_mipi_tx_infos[ISP_MIPI_HDR_S].buf_list.erase(buf_s);
            _isp_mipi_tx_infos[ISP_MIPI_HDR_M].buf_list.erase(buf_m);
            _isp_mipi_tx_infos[ISP_MIPI_HDR_L].buf_list.erase(buf_l);
            _mipi_trigger_mutex.lock();
            _isp_hdr_fid2ready_map[sequence_s] = true;
            _mipi_trigger_mutex.unlock();
            trigger_readback(sequence_s);
        } else if ((_working_mode == RK_AIQ_ISP_HDR_MODE_2_FRAME_HDR ||
                    _working_mode == RK_AIQ_ISP_HDR_MODE_2_LINE_HDR) &&
                   buf_m.ptr() && buf_s.ptr() && sequence_m == sequence_s) {
            _isp_mipi_rx_infos[ISP_MIPI_HDR_S].cache_list.push(buf_s);
            _isp_mipi_rx_infos[ISP_MIPI_HDR_M].cache_list.push(buf_m);

            _isp_mipi_tx_infos[ISP_MIPI_HDR_S].buf_list.erase(buf_s);
            _isp_mipi_tx_infos[ISP_MIPI_HDR_M].buf_list.erase(buf_m);
            _mipi_trigger_mutex.lock();
            _isp_hdr_fid2ready_map[sequence_s] = true;
            _mipi_trigger_mutex.unlock();
            trigger_readback(sequence_s);
        } else {
            _isp_mipi_tx_infos[ISP_MIPI_HDR_S].buf_list.erase(buf_s);
        }
    }
    _mipi_buf_mutex.unlock();
}

void
Isp20PollThread::handle_tx_buf(SmartPtr<V4l2BufferProxy> &tx_buf, int dev_index)
{
    XCAM_LOG_DEBUG ("%s dev_index:%d sequence:%d\n",
        __func__, dev_index, tx_buf->get_sequence());
    _isp_mipi_tx_infos[dev_index].buf_list.push(tx_buf);
    sync_tx_buf();
}

void
Isp20PollThread::trigger_readback(uint32_t sequence)
{
    std::map<uint32_t, int>::iterator it_times;
    std::map<uint32_t, bool>::iterator it_ready;
    SmartPtr<V4l2Buffer> v4l2buf;
    SmartPtr<V4l2BufferProxy> buf_proxy;
    bool buf_ready = false, trigger = false;
    int i, times = -1;

    _mipi_trigger_mutex.lock();
    it_ready = _isp_hdr_fid2ready_map.find(sequence);
    if (it_ready != _isp_hdr_fid2ready_map.end())
        buf_ready = _isp_hdr_fid2ready_map[sequence];

    it_times = _isp_hdr_fid2times_map.find(sequence);
    if (it_times != _isp_hdr_fid2times_map.end())
        times = _isp_hdr_fid2times_map[sequence];

    if (times > 0 && buf_ready) {
        trigger = true;
        _isp_hdr_fid2ready_map.erase(it_ready);
        _isp_hdr_fid2times_map.erase(it_times);
    } else if (buf_ready && times == -1 &&
               _isp_hdr_fid2times_map.begin() != _isp_hdr_fid2times_map.end() &&
               _isp_hdr_fid2times_map.begin()->first > sequence) {
        // fid2times loss of frame, the latest sequence > image sequence,
        // the image no times match, just readback 2 time
        times = 2;
        trigger = true;
        XCAM_LOG_WARNING("%s fid2times loss, the lates id:%d. the image id:%d\n",
            __func__, _isp_hdr_fid2times_map.begin()->first, sequence);
    } else if (times >= 0 && !buf_ready &&
               _isp_hdr_fid2ready_map.begin() != _isp_hdr_fid2ready_map.end() &&
               _isp_hdr_fid2ready_map.begin()->first > sequence) {
        // image loss of frame, the latest sequence > fid2times sequence,
        // drop fid2times sequence
        it_ready = _isp_hdr_fid2ready_map.begin();
        sequence = it_ready->first;
        XCAM_LOG_WARNING("%s image loss, the lates id:%d.\n",
            __func__, sequence);
        _isp_hdr_fid2times_map.erase(it_times);
        for (it_times = _isp_hdr_fid2times_map.begin();
             it_times != _isp_hdr_fid2times_map.end(); it_times++) {
             if (it_times->first < sequence) {
                 _isp_hdr_fid2times_map.erase(it_times);
             } else if (it_times->first == sequence) {
                 times = it_times->second;
                 trigger = true;
                 _isp_hdr_fid2times_map.erase(it_times);
                 _isp_hdr_fid2ready_map.erase(it_ready);
                 break;
             }
        }
    }
    _mipi_trigger_mutex.unlock();
    if (trigger) {
        struct isp2x_csi_trigger tg = {
            .frame_id = sequence,
            .times = times - 1,
        };

	if (_first_trigger) {
	    tg.times = 1;
	    _first_trigger = false;
	}

        XCAM_LOG_DEBUG ("%s frame id:%d times:%d\n",
            __func__, sequence, tg.times);
        for (i = 0; i < _mipi_dev_max; i++) {
                buf_proxy = _isp_mipi_rx_infos[i].cache_list.pop(-1);
                _isp_mipi_rx_infos[i].buf_list.push(buf_proxy);

                _isp_mipi_rx_infos[i].dev->get_buffer(v4l2buf);
                v4l2buf->set_expbuf_fd(buf_proxy->get_expbuf_fd());
                _isp_mipi_rx_infos[i].dev->queue_buffer(v4l2buf);
        }
        _isp_core_dev->io_control(RKISP_CMD_TRIGGER_READ_BACK, &tg);
    }
}
}; //namspace RkCam
