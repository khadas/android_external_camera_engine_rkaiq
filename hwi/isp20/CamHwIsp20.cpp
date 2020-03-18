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

#include "CamHwIsp20.h"
#include "Isp20PollThread.h"
#include "SensorHw.h"
#include "rk_isp20_hw.h"

namespace RkCam {

CamHwIsp20::CamHwIsp20()
    : _is_exit(false)
    , _first(true)
{}

CamHwIsp20::~CamHwIsp20()
{}


XCamReturn
CamHwIsp20::init(const char* sns_ent_name)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    SmartPtr<Isp20PollThread> isp20Pollthread;
    SmartPtr<PollThread> isp20LumaPollthread;
    SmartPtr<SensorHw> sensorHw;
    SmartPtr<V4l2Device> mipi_tx_devs[3];
    SmartPtr<V4l2Device> mipi_rx_devs[3];

    ENTER_CAMHW_FUNCTION();
    sensorHw = new SensorHw("/dev/v4l-subdev4");
    mSensorDev = sensorHw;
    mSensorDev->open();

    mIspCoreDev = new V4l2SubDevice("/dev/v4l-subdev1");
    mIspCoreDev->open();
    mIspCoreDev->subscribe_event(V4L2_EVENT_FRAME_SYNC);

    mIspLumaDev = new V4l2Device("/dev/video12");
    mIspLumaDev->open();

    mIspStatsDev = new V4l2Device ("/dev/video10");
    mIspStatsDev->open();
    mIspParamsDev = new V4l2Device ("/dev/video11");
    mIspParamsDev->open();
    mIspPostParamsDev = new V4l2Device ("/dev/video13");
    mIspPostParamsDev->open();

    //short frame
    mipi_tx_devs[0] = new V4l2Device ("/dev/video4");//rkisp_rawwr2
    mipi_tx_devs[0]->open();
    mipi_rx_devs[0] = new V4l2Device ("/dev/video9");//rkisp_rawrd2_s
    mipi_rx_devs[0]->open();
    mipi_rx_devs[0]->set_mem_type(V4L2_MEMORY_DMABUF);
    //mid frame
    mipi_tx_devs[1] = new V4l2Device ("/dev/video2");//rkisp_rawwr0
    mipi_tx_devs[1]->open();
    mipi_rx_devs[1] = new V4l2Device ("/dev/video7");//rkisp_rawrd0_m
    mipi_rx_devs[1]->open();
    mipi_rx_devs[1]->set_mem_type(V4L2_MEMORY_DMABUF);
    //long frame
    mipi_tx_devs[2] = new V4l2Device ("/dev/video3");//rkisp_rawwr1
    mipi_tx_devs[2]->open();
    mipi_rx_devs[2] = new V4l2Device ("/dev/video8");//rkisp_rawrd1_l
    mipi_rx_devs[2]->open();
    mipi_rx_devs[2]->set_mem_type(V4L2_MEMORY_DMABUF);
    for (int i = 0; i < 3; i++) {
        mipi_tx_devs[i]->set_buffer_count(8);
        mipi_rx_devs[i]->set_buffer_count(8);
    }

    isp20Pollthread = new Isp20PollThread();
    isp20Pollthread->set_event_handle_dev(sensorHw);
    isp20Pollthread->set_mipi_devs(mipi_tx_devs, mipi_rx_devs, mIspCoreDev);
    mPollthread = isp20Pollthread;
    mPollthread->set_event_device(mIspCoreDev);
    mPollthread->set_isp_stats_device(mIspStatsDev);
    mPollthread->set_isp_params_devices(mIspParamsDev, mIspPostParamsDev);
    mPollthread->set_poll_callback (this);

    isp20LumaPollthread = new PollThread();
    mPollLumathread = isp20LumaPollthread;
    mPollLumathread->set_isp_luma_device(mIspLumaDev);
    mPollLumathread->set_poll_callback (this);

    EXIT_CAMHW_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
CamHwIsp20::deInit()
{
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
CamHwIsp20::prepare(uint32_t width, uint32_t height, int mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    SmartPtr<Isp20PollThread> isp20Pollthread;
    SmartPtr<SensorHw> sensorHw;

    ENTER_CAMHW_FUNCTION();

    // TODO, set working mode to sensor

    isp20Pollthread = mPollthread.dynamic_cast_ptr<Isp20PollThread>();
    isp20Pollthread->set_working_mode(mode);
    sensorHw = mSensorDev.dynamic_cast_ptr<SensorHw>();
    ret = isp20Pollthread->hdr_mipi_start(sensorHw);
    if (ret < 0) {
        LOGE("hdr mipi start err: %d\n", ret);
    }

    ret = mIspLumaDev->start();
    if (ret < 0) {
        LOGE("start isp luma dev err: %d\n", ret);
    }

    ret = mIspCoreDev->start();
    if (ret < 0) {
        LOGE("start isp core dev err: %d\n", ret);
    }

    mIspStatsDev->start();
    if (ret < 0) {
        LOGE("start isp stats dev err: %d\n", ret);
    }

    mIspParamsDev->start();
    if (ret < 0) {
        LOGE("start isp params dev err: %d\n", ret);
    }

    EXIT_CAMHW_FUNCTION();
    return ret;
}

XCamReturn
CamHwIsp20::start()
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    ENTER_CAMHW_FUNCTION();

    mPollthread->start();
    mPollLumathread->start();
    if (ret < 0) {
        LOGE("start isp Poll thread err: %d\n", ret);
    }

    _is_exit = false;

    EXIT_CAMHW_FUNCTION();
    return ret;
}

XCamReturn CamHwIsp20::stop()
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    SmartPtr<Isp20PollThread> isp20Pollthread;

    ENTER_CAMHW_FUNCTION();

    isp20Pollthread = mPollthread.dynamic_cast_ptr<Isp20PollThread>();
    ret = isp20Pollthread->hdr_mipi_stop();
    if (ret < 0) {
        LOGE("hdr mipi stop err: %d\n", ret);
    }

    EXIT_CAMHW_FUNCTION();
    return ret;
}

void
CamHwIsp20::gen_full_isp_params(const struct isp2x_isp_params_cfg* update_params,
                                struct isp2x_isp_params_cfg* full_params)
{
    XCAM_ASSERT (update_params);
    XCAM_ASSERT (full_params);
    int i = 0;

    ENTER_CAMHW_FUNCTION();
    for (; i <= RK_ISP2X_3DLUT_ID; i++)
        if (update_params->module_en_update & (1 << i)) {
            full_params->module_en_update |= 1 << i;
            // clear old bit value
            full_params->module_ens &= ~(1 << i);
            // set new bit value
            full_params->module_ens |= update_params->module_ens & (1 << i);
        }

    for (i = 0; i <= RK_ISP2X_3DLUT_ID; i++) {
        if (update_params->module_cfg_update & (1 << i)) {
            full_params->module_cfg_update |= 1 << i;
            switch (i) {
            case RK_ISP2X_RAWAE_BIG1_ID:
                full_params->meas.rawaebig1 = update_params->meas.rawaebig1;
                break;
            case RK_ISP2X_RAWAE_BIG2_ID:
                full_params->meas.rawaebig2 = update_params->meas.rawaebig2;
                break;
            case RK_ISP2X_RAWAE_BIG3_ID:
                full_params->meas.rawaebig3 = update_params->meas.rawaebig3;
                break;
            case RK_ISP2X_RAWAE_LITE_ID:
                full_params->meas.rawaelite = update_params->meas.rawaelite;
                break;
            case RK_ISP2X_RAWHIST_BIG1_ID:
                full_params->meas.rawhstbig1 = update_params->meas.rawhstbig1;
                break;
            case RK_ISP2X_RAWHIST_BIG2_ID:
                full_params->meas.rawhstbig2 = update_params->meas.rawhstbig2;
                break;
            case RK_ISP2X_RAWHIST_BIG3_ID:
                full_params->meas.rawhstbig3 = update_params->meas.rawhstbig3;
                break;
            case RK_ISP2X_RAWHIST_LITE_ID:
                full_params->meas.rawhstlite = update_params->meas.rawhstlite;
                break;
            case RK_ISP2X_SIAWB_ID:
                full_params->meas.siawb = update_params->meas.siawb;
                break;
            case RK_ISP2X_RAWAWB_ID:
                full_params->meas.rawawb = update_params->meas.rawawb;
                break;
            case RK_ISP2X_AWB_GAIN_ID:
                full_params->others.awb_gain_cfg = update_params->others.awb_gain_cfg;
                break;
            case RK_ISP2X_RAWAF_ID:
                full_params->meas.rawaf = update_params->meas.rawaf;
                break;
            case RK_ISP2X_HDRMGE_ID:
                full_params->others.hdrmge_cfg = update_params->others.hdrmge_cfg;
                break;
            case RK_ISP2X_HDRTMO_ID:
                full_params->others.hdrtmo_cfg = update_params->others.hdrtmo_cfg;
                break;
            default:
                break;
            }
        }
    }
    EXIT_CAMHW_FUNCTION();
}

void CamHwIsp20::dump_isp_config(struct isp2x_isp_params_cfg* isp_params,
                                 SmartPtr<RkAiqIspParamsProxy> aiq_results)
{
    LOGD("isp_params mask: 0x%llx-0x%llx-0x%llx\n",
		    isp_params->module_en_update,
		    isp_params->module_ens,
		    isp_params->module_cfg_update);

    LOGD("aiq_results: ae-lite.winnum=%d; ae-big2.winnum=%d, sub[1].size: [%dx%d]\n",
	   aiq_results->data()->aec_meas.rawaelite.wnd_num,
	   aiq_results->data()->aec_meas.rawaebig2.wnd_num,
	   aiq_results->data()->aec_meas.rawaebig2.subwin[1].h_size,
	   aiq_results->data()->aec_meas.rawaebig2.subwin[1].v_size);

    LOGD("isp_params: ae-lite.winnum=%d; ae-big2.winnum=%d, sub[1].size: [%dx%d]\n",
	   isp_params->meas.rawaelite.wnd_num,
	   isp_params->meas.rawaebig2.wnd_num,
	   isp_params->meas.rawaebig2.subwin[1].h_size,
	   isp_params->meas.rawaebig2.subwin[1].v_size);

    LOGD("isp_params: win size: [%dx%d]-[%dx%d]-[%dx%d]-[%dx%d]\n",
	   isp_params->meas.rawaelite.win.h_size,
	   isp_params->meas.rawaelite.win.v_size,
	   isp_params->meas.rawaebig1.win.h_size,
	   isp_params->meas.rawaebig1.win.v_size,
	   isp_params->meas.rawaebig2.win.h_size,
	   isp_params->meas.rawaebig2.win.v_size,
	   isp_params->meas.rawaebig3.win.h_size,
	   isp_params->meas.rawaebig3.win.v_size);
	LOGD("isp_params: debayer: \n"
	    "clip_en:%d, dist_scale:%d, filter_c_en:%d, filter_g_en:%d\n"
	    "gain_offset:%d,hf_offset:%d,max_ratio:%d,offset:%d,order_max:%d\n"
	    "order_min:%d, shift_num:%d, thed0:%d, thed1:%d\n"
	    "filter1:[%d, %d, %d, %d, %d]\n",
	    "filter2:[%d, %d, %d, %d, %d]\n",
	    isp_params->others.debayer_cfg.clip_en,
	    isp_params->others.debayer_cfg.dist_scale,
	    isp_params->others.debayer_cfg.filter_c_en,
	    isp_params->others.debayer_cfg.filter_g_en,
	    isp_params->others.debayer_cfg.gain_offset,
	    isp_params->others.debayer_cfg.hf_offset,
	    isp_params->others.debayer_cfg.max_ratio,
	    isp_params->others.debayer_cfg.offset,
	    isp_params->others.debayer_cfg.order_max,
	    isp_params->others.debayer_cfg.order_min,
	    isp_params->others.debayer_cfg.shift_num,
	    isp_params->others.debayer_cfg.thed0,
	    isp_params->others.debayer_cfg.thed1,
	    isp_params->others.debayer_cfg.filter1_coe1,
	    isp_params->others.debayer_cfg.filter1_coe2,
	    isp_params->others.debayer_cfg.filter1_coe3,
	    isp_params->others.debayer_cfg.filter1_coe4,
	    isp_params->others.debayer_cfg.filter1_coe5,
	    isp_params->others.debayer_cfg.filter2_coe1,
	    isp_params->others.debayer_cfg.filter2_coe2,
	    isp_params->others.debayer_cfg.filter2_coe3,
	    isp_params->others.debayer_cfg.filter2_coe4,
	    isp_params->others.debayer_cfg.filter2_coe5);
}

XCamReturn
CamHwIsp20::setIspParamsSync()
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    ENTER_CAMHW_FUNCTION();
    while (_effecting_ispparm_map.size() > 10)
        _effecting_ispparm_map.erase(_effecting_ispparm_map.begin());

    // merge all pending params
    if (!_pending_ispparams_queue.empty()) {
        struct isp2x_isp_params_cfg update_params;
        SmartPtr<RkAiqIspParamsProxy> aiq_results;


        xcam_mem_clear (update_params);
        xcam_mem_clear (_full_active_isp_params);
	LOGD("merge isp params num %d\n", _pending_ispparams_queue.size());
        aiq_results = _pending_ispparams_queue.front();
        _pending_ispparams_queue.pop_front();

        ret = convertAiqResultsToIsp20Params(update_params, aiq_results, _last_aiq_results);
        if (ret != XCAM_RETURN_NO_ERROR) {
            LOGE("rkisp1_convert_results error\n");
        }

#if 1 // for test
	/* ae/hist update */
	update_params.module_en_update |= 1 << RK_ISP2X_RAWAE_LITE_ID;
	update_params.module_ens |= 1 << RK_ISP2X_RAWAE_LITE_ID;
	update_params.module_cfg_update |= 1 << RK_ISP2X_RAWAE_LITE_ID;

	/*
	 * update_params.module_en_update |= 1 << RK_ISP2X_RAWAE_BIG1_ID;
	 * update_params.module_ens |= 1 << RK_ISP2X_RAWAE_BIG1_ID;
	 * update_params.module_cfg_update |= 1 << RK_ISP2X_RAWAE_BIG1_ID;
	 */

	update_params.module_en_update |= 1 << RK_ISP2X_RAWAE_BIG2_ID;
	update_params.module_ens |= 1 << RK_ISP2X_RAWAE_BIG2_ID;
	update_params.module_cfg_update |= 1 << RK_ISP2X_RAWAE_BIG2_ID;

        update_params.module_en_update |= 1 << RK_ISP2X_RAWAE_BIG3_ID;
        update_params.module_ens |= 1 << RK_ISP2X_RAWAE_BIG3_ID;
        update_params.module_cfg_update |= 1 << RK_ISP2X_RAWAE_BIG3_ID;


	update_params.module_en_update |= 1 << RK_ISP2X_RAWHIST_LITE_ID;
	update_params.module_ens |= 1 << RK_ISP2X_RAWHIST_LITE_ID;
	update_params.module_cfg_update |= 1 << RK_ISP2X_RAWHIST_LITE_ID;

	/*
	 * update_params.module_en_update |= 1 << RK_ISP2X_RAWHIST_BIG1_ID;
	 * update_params.module_ens |= 1 << RK_ISP2X_RAWHIST_BIG1_ID;
	 * update_params.module_cfg_update |= 1 << RK_ISP2X_RAWHIST_BIG1_ID;
	 */

	update_params.module_en_update |= 1 << RK_ISP2X_RAWHIST_BIG2_ID;
	update_params.module_ens |= 1 << RK_ISP2X_RAWHIST_BIG2_ID;
	update_params.module_cfg_update |= 1 << RK_ISP2X_RAWHIST_BIG2_ID;

        update_params.module_en_update |= 1 << RK_ISP2X_RAWHIST_BIG3_ID;
        update_params.module_ens |= 1 << RK_ISP2X_RAWHIST_BIG3_ID;
        update_params.module_cfg_update |= 1 << RK_ISP2X_RAWHIST_BIG3_ID;

	/* awb update */
/*
 *         update_params.module_en_update |= 1 << RK_ISP2X_AWB_GAIN_ID;
 *         update_params.module_ens |= 1 << RK_ISP2X_AWB_GAIN_ID;
 *         update_params.module_cfg_update |= 1 << RK_ISP2X_AWB_GAIN_ID;
 *
 *         update_params.module_en_update |= 1 << RK_ISP2X_RAWAWB_ID;
 *         update_params.module_ens |= 1 << RK_ISP2X_RAWAWB_ID;
 *         update_params.module_cfg_update |= 1 << RK_ISP2X_RAWAWB_ID;
 */
#endif

        update_params.module_en_update |= 1 << RK_ISP2X_HDRMGE_ID;
        update_params.module_ens |= 1 << RK_ISP2X_HDRMGE_ID;
        update_params.module_cfg_update |= 1 << RK_ISP2X_HDRMGE_ID;

        update_params.module_en_update |= 1 << RK_ISP2X_HDRTMO_ID;
        update_params.module_ens |= 1 << RK_ISP2X_HDRTMO_ID;
        update_params.module_cfg_update |= 1 << RK_ISP2X_HDRTMO_ID;
        gen_full_isp_params(&update_params, &_full_active_isp_params);

        dump_isp_config(&_full_active_isp_params, aiq_results);
    }

    if (mIspParamsDev.ptr()) {
        struct isp2x_isp_params_cfg* isp_params;
        SmartPtr<V4l2Buffer> v4l2buf;

        ret = mIspParamsDev->get_buffer(v4l2buf);
        if (!ret) {
            int buf_index = v4l2buf->get_buf().index;

            isp_params = (struct isp2x_isp_params_cfg*)v4l2buf->get_buf().m.userptr;
            *isp_params = _full_active_isp_params;

            if (mIspParamsDev->queue_buffer (v4l2buf) != 0) {
                XCAM_LOG_ERROR ("RKISP1: failed to ioctl VIDIOC_QBUF for index %d, %d %s.\n",
                                buf_index, errno, strerror(errno));
                return ret;
            }

            XCAM_LOG_DEBUG ("device(%s) queue buffer index %d, queue cnt %d, check exit status again[exit: %d]",
                            XCAM_STR (mIspParamsDev->get_device_name()),
                            buf_index, mIspParamsDev->get_queued_bufcnt(), _is_exit);
        }

        if (_is_exit)
            return XCAM_RETURN_BYPASS;
    }

    EXIT_CAMHW_FUNCTION();
    return ret;
}

XCamReturn
CamHwIsp20::setIspParams(SmartPtr<RkAiqIspParamsProxy>& ispParams)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    SmartLock locker (_mutex);

    ENTER_CAMHW_FUNCTION();
    if (_is_exit) {
        XCAM_LOG_DEBUG ("set 3a config bypass since ia engine has stop");
        return XCAM_RETURN_BYPASS;
    }

    if (_pending_ispparams_queue.size() > 8) {
        XCAM_LOG_DEBUG ("too many pending isp params:%d !", _pending_ispparams_queue.size());
        _pending_ispparams_queue.erase(_pending_ispparams_queue.begin());
    }
    _pending_ispparams_queue.push_back(ispParams);

    setIspParamsSync();

    EXIT_CAMHW_FUNCTION();
    return ret;
}

XCamReturn
CamHwIsp20::getSensorModeData(const char* sns_ent_name,
                              rk_aiq_exposure_sensor_descriptor& sns_des)
{
    const SmartPtr<SensorHw> mSensorSubdev = mSensorDev.dynamic_cast_ptr<SensorHw>();

    return mSensorSubdev->getSensorModeData(sns_ent_name, sns_des);
}

XCamReturn
CamHwIsp20::setExposureParams(SmartPtr<RkAiqExpParamsProxy>& expPar)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    ENTER_CAMHW_FUNCTION();
    SmartPtr<SensorHw> mSensorSubdev = mSensorDev.dynamic_cast_ptr<SensorHw>();

    ret = mSensorSubdev->setExposureParams(expPar);
    EXIT_CAMHW_FUNCTION();
    return ret;
}

XCamReturn
CamHwIsp20::setFocusParams(SmartPtr<RkAiqFocusParamsProxy>& focus_params)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    ENTER_CAMHW_FUNCTION();

    EXIT_CAMHW_FUNCTION();
    return ret;
}

XCamReturn
CamHwIsp20::setHdrProcessCount(int frame_id, int count)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    SmartPtr<Isp20PollThread> isp20Pollthread;

    ENTER_CAMHW_FUNCTION();

    isp20Pollthread = mPollthread.dynamic_cast_ptr<Isp20PollThread>();
    isp20Pollthread->set_hdr_frame_readback_infos(frame_id, count);

    EXIT_CAMHW_FUNCTION();
    return ret;
}


void CamHwIsp20::dumpRawnrFixValue(struct isp2x_rawnr_cfg * pRawnrCfg )
{
	printf("%s:(%d)  enter \n", __FUNCTION__, __LINE__);
	int i = 0;

	//(0x0004)
	printf("(0x0004) gauss_en:%d log_bypass:%d \n", 
		pRawnrCfg->gauss_en,
		pRawnrCfg->log_bypass);

	//(0x0008 - 0x0010)
	printf("(0x0008 - 0x0010) filtpar0-2:%d %d %d \n", 
		pRawnrCfg->filtpar0,
		pRawnrCfg->filtpar1,
		pRawnrCfg->filtpar2);

	//(0x0014 - 0x001c)
	printf("(0x0014 - 0x001c) dgain0-2:%d %d %d \n", 
		pRawnrCfg->dgain0,
		pRawnrCfg->dgain1,
		pRawnrCfg->dgain2);

	//(0x0020 - 0x002c)
	for(int i=0; i<ISP2X_RAWNR_LUMA_RATION_NUM; i++){
		printf("(0x0020 - 0x002c) luration[%d]:%d \n", 
		i, pRawnrCfg->luration[i]);
	}

	//(0x0030 - 0x003c)
	for(int i=0; i<ISP2X_RAWNR_LUMA_RATION_NUM; i++){
		printf("(0x0030 - 0x003c) lulevel[%d]:%d \n", 
		i, pRawnrCfg->lulevel[i]);
	}

	//(0x0040)
	printf("(0x0040) gauss:%d \n", 
		pRawnrCfg->gauss);

	//(0x0044)
	printf("(0x0044) sigma:%d \n", 
		pRawnrCfg->sigma);

	//(0x0048)
	printf("(0x0048) pix_diff:%d \n", 
		pRawnrCfg->pix_diff);

	//(0x004c)
	printf("(0x004c) thld_diff:%d \n", 
		pRawnrCfg->thld_diff);

	//(0x0050)
	printf("(0x0050) gas_weig_scl1:%d  gas_weig_scl2:%d  thld_chanelw:%d \n", 
		pRawnrCfg->gas_weig_scl1,
		pRawnrCfg->gas_weig_scl2,
		pRawnrCfg->thld_chanelw);

	//(0x0054)
	printf("(0x0054) lamda:%d \n", 
		pRawnrCfg->lamda);

	//(0x0058 - 0x005c)
	printf("(0x0058 - 0x005c) fixw0-3:%d %d %d %d\n", 
		pRawnrCfg->fixw0,
		pRawnrCfg->fixw1,
		pRawnrCfg->fixw2,
		pRawnrCfg->fixw3);

	//(0x0060 - 0x0068)
	printf("(0x0060 - 0x0068) wlamda0-2:%d %d %d\n", 
		pRawnrCfg->wlamda0,
		pRawnrCfg->wlamda1,
		pRawnrCfg->wlamda2);

	
	//(0x006c)
	printf("(0x006c) rgain_filp-2:%d bgain_filp:%d\n", 
		pRawnrCfg->rgain_filp,
		pRawnrCfg->bgain_filp);

	
	printf("%s:(%d)  exit \n", __FUNCTION__, __LINE__);
}



void CamHwIsp20::dumpTnrFixValue(struct rkispp_tnr_config  * pTnrCfg)
{
	int i = 0;
	printf("%s:(%d) enter \n", __FUNCTION__, __LINE__);
	//0x0080
	printf("(0x0080) opty_en:%d optc_en:%d gain_en:%d\n", 
		pTnrCfg->opty_en,
		pTnrCfg->optc_en,
		pTnrCfg->gain_en);

	//0x0088
	printf("(0x0088) pk0_y:%d pk1_y:%d pk0_c:%d pk1_c:%d \n", 
		pTnrCfg->pk0_y,
		pTnrCfg->pk1_y,
		pTnrCfg->pk0_c,
		pTnrCfg->pk1_c);
		
	//0x008c
	printf("(0x008c) glb_gain_cur:%d glb_gain_nxt:%d \n", 
		pTnrCfg->glb_gain_cur,
		pTnrCfg->glb_gain_nxt);

	//0x0090
	printf("(0x0090) glb_gain_cur_div:%d gain_glb_filt_sqrt:%d \n", 
		pTnrCfg->glb_gain_cur_div,
		pTnrCfg->glb_gain_cur_sqrt);
	
	//0x0094 - 0x0098
	for(i=0; i<TNR_SIGMA_CURVE_SIZE - 1; i++){
		printf("(0x0094 - 0x0098) sigma_x[%d]:%d \n", 
			i, pTnrCfg->sigma_x[i]);
	}

	//0x009c - 0x00bc
	for(i=0; i<TNR_SIGMA_CURVE_SIZE; i++){
		printf("(0x009c - 0x00bc) sigma_y[%d]:%d \n", 
			i, pTnrCfg->sigma_y[i]);
	}

	//0x00c4 - 0x00cc
	//dir_idx = 0;
	for(i=0; i<TNR_LUMA_CURVE_SIZE; i++){
		printf("(0x00c4 - 0x00cc) luma_curve[%d]:%d \n", 
			i, pTnrCfg->luma_curve[i]);
	}

	//0x00d0
	printf("(0x00d0) txt_th0_y:%d txt_th1_y:%d \n", 
		pTnrCfg->txt_th0_y,
		pTnrCfg->txt_th1_y);
	
	//0x00d4
	printf("(0x00d0) txt_th0_c:%d txt_th1_c:%d \n", 
		pTnrCfg->txt_th0_c,
		pTnrCfg->txt_th1_c);

	//0x00d8
	printf("(0x00d8) txt_thy_dlt:%d txt_thc_dlt:%d \n", 
		pTnrCfg->txt_thy_dlt,
		pTnrCfg->txt_thc_dlt);

	//0x00dc - 0x00ec
	for(i=0; i<TNR_GFCOEF6_SIZE; i++){
		printf("(0x00dc - 0x00ec) gfcoef_y0[%d]:%d \n", 
			i, pTnrCfg->gfcoef_y0[i]);
	}
	for(i=0; i<TNR_GFCOEF3_SIZE; i++){
		printf("(0x00dc - 0x00ec) gfcoef_y1[%d]:%d \n", 
			i, pTnrCfg->gfcoef_y1[i]);
	}
	for(i=0; i<TNR_GFCOEF3_SIZE; i++){
		printf("(0x00dc - 0x00ec) gfcoef_y2[%d]:%d \n", 
			i, pTnrCfg->gfcoef_y2[i]);
	}
	for(i=0; i<TNR_GFCOEF3_SIZE; i++){
		printf("(0x00dc - 0x00ec) gfcoef_y3[%d]:%d \n", 
			i, pTnrCfg->gfcoef_y3[i]);
	}

	//0x00f0 - 0x0100
	for(i=0; i<TNR_GFCOEF6_SIZE; i++){
		printf("(0x00f0 - 0x0100) gfcoef_yg0[%d]:%d \n", 
			i, pTnrCfg->gfcoef_yg0[i]);
	}
	for(i=0; i<TNR_GFCOEF3_SIZE; i++){
		printf("(0x00f0 - 0x0100) gfcoef_yg1[%d]:%d \n", 
			i, pTnrCfg->gfcoef_yg1[i]);
	}
	for(i=0; i<TNR_GFCOEF3_SIZE; i++){
		printf("(0x00f0 - 0x0100) gfcoef_yg2[%d]:%d \n", 
			i, pTnrCfg->gfcoef_yg2[i]);
	}
	for(i=0; i<TNR_GFCOEF3_SIZE; i++){
		printf("(0x00f0 - 0x0100) gfcoef_yg3[%d]:%d \n", 
			i, pTnrCfg->gfcoef_yg3[i]);
	}


	//0x0104 - 0x0110
	for(i=0; i<TNR_GFCOEF6_SIZE; i++){
		printf("(0x0104 - 0x0110) gfcoef_yl0[%d]:%d \n", 
			i, pTnrCfg->gfcoef_yl0[i]);
	}
	for(i=0; i<TNR_GFCOEF3_SIZE; i++){
		printf("(0x0104 - 0x0110) gfcoef_yl1[%d]:%d \n", 
			i, pTnrCfg->gfcoef_yl1[i]);
	}
	for(i=0; i<TNR_GFCOEF3_SIZE; i++){
		printf("(0x0104 - 0x0110) gfcoef_yl2[%d]:%d \n", 
			i, pTnrCfg->gfcoef_yl2[i]);
	}

	//0x0114 - 0x0120
	for(i=0; i<TNR_GFCOEF6_SIZE; i++){
		printf("(0x0114 - 0x0120) gfcoef_cg0[%d]:%d \n", 
			i, pTnrCfg->gfcoef_cg0[i]);
	}
	for(i=0; i<TNR_GFCOEF3_SIZE; i++){
		printf("(0x0114 - 0x0120) gfcoef_cg1[%d]:%d \n", 
			i, pTnrCfg->gfcoef_cg1[i]);
	}
	for(i=0; i<TNR_GFCOEF3_SIZE; i++){
		printf("(0x0114 - 0x0120) gfcoef_cg2[%d]:%d \n", 
			i, pTnrCfg->gfcoef_cg2[i]);
	}


	//0x0124 - 0x012c
	for(i=0; i<TNR_GFCOEF6_SIZE; i++){
		printf("(0x0124 - 0x012c) gfcoef_cl0[%d]:%d \n", 
			i, pTnrCfg->gfcoef_cl0[i]);
	}
	for(i=0; i<TNR_GFCOEF3_SIZE; i++){
		printf("(0x0124 - 0x012c) gfcoef_cl1[%d]:%d \n", 
			i, pTnrCfg->gfcoef_cl1[i]);
	}


	//0x0130 - 0x0134
	//dir_idx = 0;  i = lvl;
	for(i=0; i<TNR_SCALE_YG_SIZE; i++){
		printf("(0x0130 - 0x0134) scale_yg[%d]:%d \n", 
			i, pTnrCfg->scale_yg[i]);
	}

	//0x0138 - 0x013c
	//dir_idx = 1;  i = lvl;
	for(i=0; i<TNR_SCALE_YL_SIZE; i++){
		printf("(0x0138 - 0x013c) scale_yl[%d]:%d \n", 
			i, pTnrCfg->scale_yl[i]);
	}

	//0x0140 - 0x0148
	//dir_idx = 0;  i = lvl;
	for(i=0; i<TNR_SCALE_CG_SIZE; i++){
		printf("(0x0140 - 0x0148) scale_cg[%d]:%d \n", 
			i, pTnrCfg->scale_cg[i]);
		printf("(0x0140 - 0x0148) scale_y2cg[%d]:%d \n", 
			i, pTnrCfg->scale_y2cg[i]);	
	}

	//0x014c - 0x0154
	//dir_idx = 1;  i = lvl;
	for(i=0; i<TNR_SCALE_CL_SIZE; i++){
		printf("(0x014c - 0x0154) scale_cl[%d]:%d \n", 
			i, pTnrCfg->scale_cl[i]);
	}
	for(i=0; i<TNR_SCALE_Y2CL_SIZE; i++){
		printf("(0x014c - 0x0154) scale_y2cl[%d]:%d \n", 
			i, pTnrCfg->scale_y2cl[i]);
	}

	//0x0158 
	for(i=0; i<TNR_WEIGHT_Y_SIZE; i++){
		printf("(0x0158) weight_y[%d]:%d \n", 
			i, pTnrCfg->weight_y[i]);
	}

	printf("%s:(%d) exit \n", __FUNCTION__, __LINE__);
}


void CamHwIsp20::dumpUvnrFixValue(struct rkispp_nr_config  * pNrCfg)
{
	int i = 0;
	printf("%s:(%d) exit \n", __FUNCTION__, __LINE__);
	//0x0080
	printf("(0x0088) uvnr_step1_en:%d uvnr_step2_en:%d nr_gain_en:%d uvnr_nobig_en:%d uvnr_big_en:%d\n", 
		pNrCfg->uvnr_step1_en,
		pNrCfg->uvnr_step2_en,
		pNrCfg->nr_gain_en ,
		pNrCfg->uvnr_nobig_en,
		pNrCfg->uvnr_big_en);	

	//0x0084
	printf("(0x0084) uvnr_gain_1sigma:%d \n", 
		pNrCfg->uvnr_gain_1sigma);
		
	//0x0088
	printf("(0x0088) uvnr_gain_offset:%d \n", 
		pNrCfg->uvnr_gain_offset);
	
	//0x008c
	printf("(0x008c) uvnr_gain_uvgain:%d uvnr_step2_en:%d uvnr_gain_t2gen:%d uvnr_gain_iso:%d\n", 
		pNrCfg->uvnr_gain_uvgain[0],
		pNrCfg->uvnr_gain_uvgain[1],
		pNrCfg->uvnr_gain_t2gen,
		pNrCfg->uvnr_gain_iso);
		

	//0x0090
	printf("(0x0090) uvnr_t1gen_m3alpha:%d \n", 
		pNrCfg->uvnr_t1gen_m3alpha);

	//0x0094
	printf("(0x0094) uvnr_t1flt_mode:%d \n", 
		pNrCfg->uvnr_t1flt_mode);

	//0x0098
	printf("(0x0098) uvnr_t1flt_msigma:%d \n", 
		pNrCfg->uvnr_t1flt_msigma);

	//0x009c
	printf("(0x009c) uvnr_t1flt_wtp:%d \n", 
		pNrCfg->uvnr_t1flt_wtp);

	//0x00a0-0x00a4
	for(i=0; i<NR_UVNR_T1FLT_WTQ_SIZE; i++){
		printf("(0x00a0-0x00a4) uvnr_t1flt_wtq[%d]:%d \n", 
			i, pNrCfg->uvnr_t1flt_wtq[i]);
	}

	//0x00a8
	printf("(0x00a8) uvnr_t2gen_m3alpha:%d \n", 
		pNrCfg->uvnr_t2gen_m3alpha);

	//0x00ac
	printf("(0x00ac) uvnr_t2gen_msigma:%d \n", 
		pNrCfg->uvnr_t2gen_msigma);

	//0x00b0
	printf("(0x00b0) uvnr_t2gen_wtp:%d \n", 
		pNrCfg->uvnr_t2gen_wtp);

	//0x00b4
	for(i=0; i<NR_UVNR_T2GEN_WTQ_SIZE; i++){
		printf("(0x00b4) uvnr_t2gen_wtq[%d]:%d \n", 
			i, pNrCfg->uvnr_t2gen_wtq[i]);
	}

	//0x00b8
	printf("(0x00b8) uvnr_t2flt_msigma:%d \n", 
		pNrCfg->uvnr_t2flt_msigma);

	//0x00bc
	printf("(0x00bc) uvnr_t2flt_wtp:%d \n", 
		pNrCfg->uvnr_t2flt_wtp);
	for(i=0; i<NR_UVNR_T2FLT_WT_SIZE; i++){
		printf("(0x00bc) uvnr_t2flt_wt[%d]:%d \n", 
			i, pNrCfg->uvnr_t2flt_wt[i]);
	}

		
	printf("%s:(%d) entor \n", __FUNCTION__, __LINE__);
}


void CamHwIsp20::dumpYnrFixValue(struct rkispp_nr_config  * pNrCfg)
{
	printf("%s:(%d) enter \n", __FUNCTION__, __LINE__);
	
	int i = 0;
	
	//0x0104 - 0x0108
	for(i=0; i<NR_YNR_SGM_DX_SIZE; i++){
		printf("(0x0104 - 0x0108) ynr_sgm_dx[%d]:%d \n",
			i, pNrCfg->ynr_sgm_dx[i]);
	}

	//0x010c - 0x012c
	for(i = 0; i < NR_YNR_SGM_Y_SIZE; i++){
		printf("(0x010c - 0x012c) ynr_lsgm_y[%d]:%d \n",
			i, pNrCfg->ynr_lsgm_y[i]);
	}

	//0x0130
	for(i=0; i<NR_YNR_CI_SIZE; i++){
		printf("(0x0130) ynr_lci[%d]:%d \n",
			i, pNrCfg->ynr_lci[i]);
	}

	//0x0134
	for(i=0; i<NR_YNR_LGAIN_MIN_SIZE; i++){
		printf("(0x0134) ynr_lgain_min[%d]:%d \n",
			i, pNrCfg->ynr_lgain_min[i]);
	}

	//0x0138
	printf("(0x0138) ynr_lgain_max:%d \n",
			pNrCfg->ynr_lgain_max);
	
	
	//0x013c
	printf("(0x013c) ynr_lmerge_bound:%d ynr_lmerge_ratio:%d\n",
			pNrCfg->ynr_lmerge_bound,
			pNrCfg->ynr_lmerge_ratio);

	//0x0140
	for(i=0; i<NR_YNR_LWEIT_FLT_SIZE; i++){
		printf("(0x0140) ynr_lweit_flt[%d]:%d \n",
			i, pNrCfg->ynr_lweit_flt[i]);
	}

	//0x0144 - 0x0164
	for(i = 0; i < NR_YNR_SGM_Y_SIZE; i++){
		printf("(0x0144 - 0x0164) ynr_hsgm_y[%d]:%d \n",
			i, pNrCfg->ynr_hsgm_y[i]);
	}

	//0x0168
	for(i=0; i<NR_YNR_CI_SIZE; i++){
		printf("(0x0168) ynr_hlci[%d]:%d \n",
			i, pNrCfg->ynr_hlci[i]);
	}

	//0x016c
	for(i=0; i<NR_YNR_CI_SIZE; i++){
		printf("(0x016c) ynr_lhci[%d]:%d \n",
			i, pNrCfg->ynr_lhci[i]);
	}

	//0x0170
	for(i=0; i<NR_YNR_CI_SIZE; i++){
		printf("(0x0170) ynr_hhci[%d]:%d \n",
			i, pNrCfg->ynr_hhci[i]);
	}

	//0x0174
	for(i=0; i<NR_YNR_HGAIN_SGM_SIZE; i++){
		printf("(0x0174) ynr_hgain_sgm[%d]:%d \n",
			i, pNrCfg->ynr_hgain_sgm[i]);
	}

	//0x0178 - 0x0188
    for(i=0; i<5; i++){
		printf("(0x0178 - 0x0188) ynr_hweit_d[%d - %d]:%d %d %d %d \n",
			i*4 + 0, i*4 + 3,
			pNrCfg->ynr_hweit_d[i*4 + 0],
			pNrCfg->ynr_hweit_d[i*4 + 1],
			pNrCfg->ynr_hweit_d[i*4 + 2],
			pNrCfg->ynr_hweit_d[i*4 + 3]);
	}
		

	//0x018c - 0x01a0
	for(i = 0; i < 6; i++){
		printf("(0x018c - 0x01a0) ynr_hgrad_y[%d - %d]:%d %d %d %d \n",
			i*4 + 0, i*4 + 3,
			pNrCfg->ynr_hgrad_y[i*4 + 0],
			pNrCfg->ynr_hgrad_y[i*4 + 1],
			pNrCfg->ynr_hgrad_y[i*4 + 2],
			pNrCfg->ynr_hgrad_y[i*4 + 3]);
	}

	//0x01a4 -0x01a8
	for(i=0; i<NR_YNR_HWEIT_SIZE; i++){
		printf("(0x01a4 -0x01a8) ynr_hweit[%d]:%d \n",
			i, pNrCfg->ynr_hweit[i]);
	}

	//0x01b0
	printf("(0x01b0) ynr_hmax_adjust:%d \n",
			pNrCfg->ynr_hmax_adjust);

	//0x01b4
	printf("(0x01b4) ynr_hstrength:%d \n",
			pNrCfg->ynr_hstrength);

	//0x01b8
	printf("(0x01b8) ynr_lweit_cmp0-1:%d %d\n",
			pNrCfg->ynr_lweit_cmp[0],
			pNrCfg->ynr_lweit_cmp[1]);

	//0x01bc
	printf("(0x01bc) ynr_lmaxgain_lv4:%d \n",
			pNrCfg->ynr_lmaxgain_lv4);

	//0x01c0 - 0x01e0 
	for(i=0; i<NR_YNR_HSTV_Y_SIZE; i++){
		printf("(0x01c0 - 0x01e0 ) ynr_hstv_y[%d]:%d \n",
			i, pNrCfg->ynr_hstv_y[i]);
	}
	
	//0x01e4  - 0x01e8
	for(i=0; i<NR_YNR_ST_SCALE_SIZE; i++){
		printf("(0x01e4  - 0x01e8 ) ynr_st_scale[%d]:%d \n",
			i, pNrCfg->ynr_st_scale[i]);
	}

	printf("%s:(%d) exit \n", __FUNCTION__, __LINE__);
	
}


void CamHwIsp20::dumpSharpFixValue(struct rkispp_sharp_config  * pSharpCfg)
{
	printf("%s:(%d) enter \n", __FUNCTION__, __LINE__);
	int i = 0;
	
	//0x0080
	printf("(0x0080) alpha_adp_en:%d yin_flt_en:%d edge_avg_en:%d\n",
			pSharpCfg->alpha_adp_en,
			pSharpCfg->yin_flt_en,
			pSharpCfg->edge_avg_en);
	
	
	//0x0084
	printf("(0x0084) hbf_ratio:%d ehf_th:%d pbf_ratio:%d\n",
			pSharpCfg->hbf_ratio,
			pSharpCfg->ehf_th,
			pSharpCfg->pbf_ratio);

	//0x0088
	printf("(0x0088) edge_thed:%d dir_min:%d smoth_th4:%d\n",
			pSharpCfg->edge_thed,
			pSharpCfg->dir_min,
			pSharpCfg->smoth_th4);

	//0x008c
	printf("(0x008c) l_alpha:%d g_alpha:%d \n",
			pSharpCfg->l_alpha,
			pSharpCfg->g_alpha);
	

	//0x0090
	for(i = 0; i < 3; i++){
		printf("(0x0090) pbf_k[%d]:%d  \n",
			i, pSharpCfg->pbf_k[i]);
	}

	

	//0x0094 - 0x0098
	for(i = 0; i < 6; i++){
		printf("(0x0094 - 0x0098) mrf_k[%d]:%d  \n",
			i, pSharpCfg->mrf_k[i]);
	}
	

	//0x009c -0x00a4
	for(i = 0; i < 12; i++){
		printf("(0x009c -0x00a4) mbf_k[%d]:%d  \n",
			i, pSharpCfg->mbf_k[i]);
	}
	
	
	//0x00a8 -0x00ac
	for(i = 0; i < 6; i++){
		printf("(0x00a8 -0x00ac) hrf_k[%d]:%d  \n",
			i, pSharpCfg->hrf_k[i]);
	}
	

	//0x00b0
	for(i = 0; i < 3; i++){
		printf("(0x00b0) hbf_k[%d]:%d  \n",
			i, pSharpCfg->hbf_k[i]);
	}

	
	//0x00b4
	for(i = 0; i < 3; i++){
		printf("(0x00b4) eg_coef[%d]:%d  \n",
			i, pSharpCfg->eg_coef[i]);
	}

	//0x00b8
	for(i = 0; i < 3; i++){
		printf("(0x00b8) eg_smoth[%d]:%d  \n",
			i, pSharpCfg->eg_smoth[i]);
	}


	//0x00bc - 0x00c0
	for(i = 0; i < 6; i++){
		printf("(0x00bc - 0x00c0) eg_gaus[%d]:%d  \n",
			i, pSharpCfg->eg_gaus[i]);
	}
	

	//0x00c4 - 0x00c8
	for(i = 0; i < 6; i++){
		printf("(0x00c4 - 0x00c8) dog_k[%d]:%d  \n",
			i, pSharpCfg->dog_k[i]);
	}


	//0x00cc - 0x00d0
	for(i=0; i<SHP_LUM_POINT_SIZE; i++){
		printf("(0x00cc - 0x00d0) lum_point[%d]:%d  \n",
			i, pSharpCfg->lum_point[i]);
	}
	
	//0x00d4
	printf("(0x00d4) pbf_shf_bits:%d  mbf_shf_bits:%d hbf_shf_bits:%d\n",
		pSharpCfg->pbf_shf_bits,
		pSharpCfg->mbf_shf_bits,
		pSharpCfg->hbf_shf_bits);
	

	//0x00d8 - 0x00dc
	for(i=0; i<SHP_SIGMA_SIZE; i++){
		printf("(0x00d8 - 0x00dc) pbf_sigma[%d]:%d  \n",
			i, pSharpCfg->pbf_sigma[i]);
	}

	//0x00e0 - 0x00e4
	for(i=0; i<SHP_LUM_CLP_SIZE; i++){
		printf("(0x00e0 - 0x00e4) lum_clp_m[%d]:%d  \n",
			i, pSharpCfg->lum_clp_m[i]);
	}

	//0x00e8 - 0x00ec
	for(i=0; i<SHP_LUM_MIN_SIZE; i++){
		printf("(0x00e8 - 0x00ec) lum_min_m[%d]:%d  \n",
			i, pSharpCfg->lum_min_m[i]);	
	}

	//0x00f0 - 0x00f4
	for(i=0; i<SHP_SIGMA_SIZE; i++){
		printf("(0x00f0 - 0x00f4) mbf_sigma[%d]:%d  \n",
			i, pSharpCfg->mbf_sigma[i]);	
	}

	//0x00f8 - 0x00fc
	for(i=0; i<SHP_LUM_CLP_SIZE; i++){
		printf("(0x00f8 - 0x00fc) lum_clp_h[%d]:%d  \n",
			i, pSharpCfg->lum_clp_h[i]);	
	}

	//0x0100 - 0x0104
	for(i=0; i<SHP_SIGMA_SIZE; i++){
		printf("(0x0100 - 0x0104) hbf_sigma[%d]:%d  \n",
			i, pSharpCfg->hbf_sigma[i]);
	}

	//0x0108 - 0x010c
	for(i=0; i<SHP_EDGE_LUM_THED_SIZE; i++){
		printf("(0x0108 - 0x010c) edge_lum_thed[%d]:%d  \n",
			i, pSharpCfg->edge_lum_thed[i]);
	}

	//0x0110 - 0x0114
	for(i=0; i<SHP_CLAMP_SIZE; i++){
		printf("(0x0110 - 0x0114) clamp_pos[%d]:%d  \n",
			i, pSharpCfg->clamp_pos[i]);
	}

	//0x0118 - 0x011c
	for(i=0; i<SHP_CLAMP_SIZE; i++){
		printf("(0x0118 - 0x011c) clamp_neg[%d]:%d  \n",
			i, pSharpCfg->clamp_neg[i]);
	}

	//0x0120 - 0x0124
	for(i=0; i<SHP_DETAIL_ALPHA_SIZE; i++){
		printf("(0x0120 - 0x0124) detail_alpha[%d]:%d  \n",
			i, pSharpCfg->detail_alpha[i]);
	}

	//0x0128
	printf("(0x0128) rfl_ratio:%d  rfh_ratio:%d\n",
			pSharpCfg->rfl_ratio, pSharpCfg->rfh_ratio);
	
	// mf/hf ratio

	//0x012C
	printf("(0x012C) m_ratio:%d  h_ratio:%d\n",
			pSharpCfg->m_ratio, pSharpCfg->h_ratio);
	
	printf("%s:(%d) exit \n", __FUNCTION__, __LINE__);
}

}; //namspace RkCam
