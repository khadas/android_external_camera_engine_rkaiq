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

#ifndef _RK_AIQ_VERSION_H_
/*!
 * ==================== AIQ VERSION HISTORY ====================
 *
 * v0.0.9
 *  - initial version, support HDR
 *  - optimized tnr,nr in low light condition
 *  - optimized imx347 iq
 *  - FEC,ORB,LDCH not enabled
 *  - set compile optimized level to Os
 * v0.1.0
 *  - optimize nr quality under low light
 *  - optimize hdr image quality
 *  - fix circular flicker of image luma
 *  - add fec and ldch, default disabled
 * v0.1.1
 *  - fix some bugs of bayer nr, ynr, uvnr 
 *  - optimize imx347 hdr mode image quality
 *  - workaround for imx347 ae flicker when the
 *    luma changed dramatically
 * v0.1.2
 *  - hdr: fix clipRatio error,and redefine tmo params
 *  - fix some bugs in ae smoot
 *  - fix high iso blc problem and uvnr / 3dnr parameters
 *  - fix mmap memory leak
 *  - fix gic bugs
 *  - add color saturation level and color inhibition level setting and getting in accm module
 *  - update imx347 and os04a10 xml
 *  - add gc4c33 xml
 * v0.1.3
 *  - IMX347: split iqfile to hdr and normal, enable fec default
 *  - add dcg setting in aiq
 *  - ablc: iq xml support diff iso diff blc value
 *  - use different iq file for mode hdr and normal
 *  - implement uapi and test
 *  - add Antiflicker-Mode
 *  - add the switch whether to enable HDR module
 *  - using mipi read back mode for normal mode
 *  - enable adebayer module
 *  - update dpcc setting in GC4C33 xml
 */

#define RK_AIQ_VERSION "v0.1.3"
#define RK_AIQ_RELEASE_DATE "2020-05-21"
#endif
