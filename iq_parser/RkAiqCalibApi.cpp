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
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "RkAiqCalibApi.h"
#include <fstream>
#include <iostream>

/******************************************************************************
 * AddAwbCalibV200Profile2AwbCalibV200List
 *****************************************************************************/
bool AddAwbCalibV200Profile2AwbCalibV200List
(
    list_head *profile_list,
    CalibDb_Awb_Calib_Para_V200_t* pAddAwb
) {
    CalibDb_Awb_Calib_Para_V200_t* pNewAwb = NULL;

    // check if pAddAwb  is valid
    if(pAddAwb ==NULL ){
        LOGE("pNewAwb is invalid");
        return(false);
    }
    // check if scene already exists
    CalibDb_Awb_Calib_Para_V200_t* pAwbFind = NULL;
    GetAwbProfileFromAwbCalibV200ListBySceneName(profile_list,pAddAwb->scene,&pAwbFind);
    if(pAwbFind == NULL){
        pNewAwb = (CalibDb_Awb_Calib_Para_V200_t*)malloc(sizeof(CalibDb_Awb_Calib_Para_V200_t));
        //printf("%p\n", pNewAwb);
        memcpy(pNewAwb, pAddAwb, sizeof(CalibDb_Awb_Calib_Para_V200_t));
        list_prepare_item(pNewAwb);
        list_add_tail((list_head*)pNewAwb , profile_list);
        return (true);
    }else{
        LOGE("awb scene (%s) is repeated",pAwbFind->scene);
        return (false);
    }
}


/******************************************************************************
 * GetAwbProfileFromAwbCalibV200ListByName
 *****************************************************************************/
bool GetAwbProfileFromAwbCalibV200ListBySceneName
(
    const list_head* profile_list,
    const char* scene,
    CalibDb_Awb_Calib_Para_V200_t** pAwbProfile,
    int *scene_index
) {
    list_head* p;
    p = profile_list->next;
    if(scene_index != NULL){
        *scene_index = 0;
    }
    while (p != profile_list)
    {
        CalibDb_Awb_Calib_Para_V200_t* pProfile = container_of(p, CalibDb_Awb_Calib_Para_V200_t, listHead);
        //printf("%s  %p ", pProfile->scene, p);
        if (!strncmp(pProfile->scene, scene, sizeof(pProfile->scene))) {
            *pAwbProfile = pProfile;
            break;
        }
        p = p->next;
        if(scene_index != NULL){
            *scene_index = *scene_index +1;
        }
    }
    return(true);
}


/******************************************************************************
 * GetAwbProfileFromAwbCalibV200ListByIdx
 *****************************************************************************/
bool GetAwbProfileFromAwbCalibV200ListByIdx
(
    const list_head* profile_list,
    int idx,
    const CalibDb_Awb_Calib_Para_V200_t** pAwbProfile
) {
    list_head* p;
    p = profile_list->next;
    int cnt = 0;
    while (p != profile_list)
    {
        if (cnt==idx) {
            CalibDb_Awb_Calib_Para_V200_t* pProfile = container_of(p, CalibDb_Awb_Calib_Para_V200_t, listHead);
            //printf("%p ", pProfile);
            *pAwbProfile = pProfile;
            break;
        }
        cnt++;
        p = p->next;
    }
    return(true);
}

/******************************************************************************
 * AddAwbCalibV200Profile2AwbCalibV201List
 *****************************************************************************/
bool AddAwbCalibV201Profile2AwbCalibV201List
(
    list_head *profile_list,
    CalibDb_Awb_Calib_Para_V201_t* pAddAwb
) {
    CalibDb_Awb_Calib_Para_V201_t* pNewAwb = NULL;

    // check if pAddAwb  is valid
    if(pAddAwb ==NULL ){
        LOGE("pNewAwb is invalid");
        return(false);
    }
    // check if scene already exists
    CalibDb_Awb_Calib_Para_V201_t* pAwbFind = NULL;
    GetAwbProfileFromAwbCalibV201ListBySceneName(profile_list,pAddAwb->scene,&pAwbFind);
    if(pAwbFind == NULL){
        pNewAwb = (CalibDb_Awb_Calib_Para_V201_t*)malloc(sizeof(CalibDb_Awb_Calib_Para_V201_t));
        //printf("%p\n", pNewAwb);
        memcpy(pNewAwb, pAddAwb, sizeof(CalibDb_Awb_Calib_Para_V201_t));
        list_prepare_item(pNewAwb);
        list_add_tail((list_head*)pNewAwb , profile_list);
        return (true);
    }else{
        LOGE("awb scene (%s) is repeated",pAwbFind->scene);
        return (false);
    }
}


/******************************************************************************
 * GetAwbProfileFromAwbCalibV201ListByName
 *****************************************************************************/
bool GetAwbProfileFromAwbCalibV201ListBySceneName
(
    const list_head* profile_list,
    const char* scene,
    CalibDb_Awb_Calib_Para_V201_t** pAwbProfile,
    int *scene_index
) {
    list_head* p;
    p = profile_list->next;
    if(scene_index != NULL){
        *scene_index = 0;
    }
    while (p != profile_list)
    {
        CalibDb_Awb_Calib_Para_V201_t* pProfile = container_of(p, CalibDb_Awb_Calib_Para_V201_t, listHead);
        //printf("%s  %p ", pProfile->scene, p);
        if (!strncmp(pProfile->scene, scene, sizeof(pProfile->scene))) {
            *pAwbProfile = pProfile;
            break;
        }
        p = p->next;
        if(scene_index != NULL){
            *scene_index = *scene_index +1;
        }
    }
    return(true);
}


/******************************************************************************
 * GetAwbProfileFromAwbCalibV201ListByIdx
 *****************************************************************************/
bool GetAwbProfileFromAwbCalibV201ListByIdx
(
    const list_head* profile_list,
    int idx,
    const CalibDb_Awb_Calib_Para_V201_t** pAwbProfile
) {
    list_head* p;
    p = profile_list->next;
    int cnt = 0;
    while (p != profile_list)
    {
        if (cnt==idx) {
            CalibDb_Awb_Calib_Para_V201_t* pProfile = container_of(p, CalibDb_Awb_Calib_Para_V201_t, listHead);
            //printf("%p ", pProfile);
            *pAwbProfile = pProfile;
            break;
        }
        cnt++;
        p = p->next;
    }
    return(true);
}

/******************************************************************************
 * AddAwbAdjustProfile2AwbAdjustList
 *****************************************************************************/
bool AddAwbAdjustProfile2AwbAdjustList
(
    list_head *profile_list,
    CalibDb_Awb_Adjust_Para_t* pAddAwb
) {
    CalibDb_Awb_Adjust_Para_t* pNewAwb = NULL;

    // check if pAddAwb  is valid
    if(pAddAwb ==NULL ){
        LOGE("pNewAwb is invalid");
        return(false);
    }
    // check if scene already exists
    CalibDb_Awb_Adjust_Para_t* pAwbFind = NULL;
    GetAwbProfileFromAwbAdjustListBySceneName(profile_list,pAddAwb->scene,&pAwbFind);
    if(pAwbFind == NULL){
        pNewAwb = (CalibDb_Awb_Adjust_Para_t*)malloc(sizeof(CalibDb_Awb_Adjust_Para_t));
        //printf("%p\n", pNewAwb);
        memcpy(pNewAwb, pAddAwb, sizeof(CalibDb_Awb_Adjust_Para_t));
        list_prepare_item(pNewAwb);
        list_add_tail((list_head*)pNewAwb , profile_list);
        return (true);
    }else{
        LOGE("awb scene (%s) is repeated",pAwbFind->scene);
        return (false);
    }
}


/******************************************************************************
 * GetAwbProfileFromAwbAdjustListByName
 *****************************************************************************/
bool GetAwbProfileFromAwbAdjustListBySceneName
(
    const list_head* profile_list,
    const char* scene,
    CalibDb_Awb_Adjust_Para_t** pAwbProfile
) {
    list_head* p;
    p = profile_list->next;
    while (p != profile_list)
    {
        CalibDb_Awb_Adjust_Para_t* pProfile = container_of(p, CalibDb_Awb_Adjust_Para_t, listHead);
        //printf("%s  %p ", pProfile->scene, p);
        if (!strncmp(pProfile->scene, scene, sizeof(pProfile->scene))) {
            *pAwbProfile = pProfile;
            break;
        }
        p = p->next;
    }
    return(true);
}


/******************************************************************************
 * GetAwbProfileFromAwbAdjustListByIdx
 *****************************************************************************/
bool GetAwbProfileFromAwbAdjustListByIdx
(
    const list_head* profile_list,
    int idx,
    const CalibDb_Awb_Adjust_Para_t** pAwbProfile
) {
    list_head* p;
    p = profile_list->next;
    int cnt = 0;
    while (p != profile_list)
    {
        if (cnt==idx) {
            CalibDb_Awb_Adjust_Para_t* pProfile = container_of(p, CalibDb_Awb_Adjust_Para_t, listHead);
            //printf("%p ", pProfile);
            *pAwbProfile = pProfile;
            break;
        }
        cnt++;
        p = p->next;
    }
    return(true);
}


