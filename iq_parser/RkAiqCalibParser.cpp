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

#include "RkAiqCalibParser.h"
#include "calibtags.h"
#include "xmltags.h"
#include <fstream>
#include <iostream>


using namespace std;
namespace RkCam {

#define MEMSET( TARGET, C, LEN) memset(TARGET,C,LEN)
#define MEMCPY( DST, SRC, LEN)  memcpy(DST,SRC,LEN)

//#define DEBUG_LOG
//#define INFO_LOG

#ifdef INFO_LOG
#define INFO_PRINT(TAG_NAME) LOG1("[Line:%d, Tag: %s]\n",__LINE__,TAG_NAME.c_str())
#else
#define INFO_PRINT(TAG_NAME)
#endif



#define redirectOut std::cout

/******************************************************************************
* Toupper
*****************************************************************************/
char* Toupper(const char* s) {
    int i = 0;
    char* p = (char*)s;
    char tmp;
    int len = 0;

    if (p) {
        len = strlen(p);
        for (i = 0; i < len; i++) {
            tmp = p[i];

            if (tmp >= 'a' && tmp <= 'z')
                tmp = tmp - 32;

            p[i] = tmp;
        }
    }
    return p;
}

int RkAiqCalibParser::ParseCharToHex
(
    XmlTag*  tag,          /**< trimmed c string */
    uint32_t*    reg_value
) {
    bool ok;

    *reg_value = tag->ValueToUInt(&ok);
    if (!ok) {
        LOGE( "%s(%d): parse error: invalid register value:\n", __FUNCTION__, __LINE__, tag->Value());
        return (false);
    } else {
#ifdef DEBUG_LOG
        LOGD( "%s(%d): parse reg valevalue:\n", __FUNCTION__, __LINE__, *reg_value);
#endif
    }

    return 0;
}

void RkAiqCalibParser::parseReadWriteCtrl(bool ctrl)
{
    xmlParseReadWrite = ctrl;
}

/******************************************************************************
* ParseFloatArray
*****************************************************************************/
int RkAiqCalibParser::ParseFloatArray
(
    const XMLNode*  pNode,          /**< trimmed c string */
    float*       values,            /**< pointer to memory */
    const int   num,                /**< number of expected float values */
    int  printAccuracy              /**< accuracy of XML write */
) {

    if (!xmlParseReadWrite) // read
    {
        const char* c_string = XmlTag(pNode->ToElement()).Value();
        float* value = values;
        char* str = (char*)c_string;
        int last = strlen(str);
        /* calc. end adress of string */
        char* str_last = str + (last - 1);

        std::string s_string(str);
        size_t find_start = s_string.find("[", 0);
        size_t find_end = s_string.find("]", 0);

        if ((find_start == std::string::npos) || (find_end == std::string::npos)) {
#ifdef DEBUG_LOG
            LOGD( "%s(%d): start=%d,end=%d\n", __FUNCTION__, __LINE__, find_start, find_end);
#endif
            return -1;
        }

        str = (char*)c_string + find_start;
        str_last = (char*)c_string + find_end;

        /* skipped left parenthesis */
        str++;

        /* skip spaces */
        while (*str == 0x20 || *str == 0x09 || (*str == 0x0a) || (*str == 0x0d)) {
            str++;
        }

        int cnt = 0;
        int scanned;
        float f;

        /* parse the c-string */
        while ((str != str_last) && (cnt < num)) {
            scanned = sscanf(str, "%f", &f);
            if (scanned != 1) {
                LOGE( "%s(%d): %f err\n", __FUNCTION__, __LINE__, f);
                goto err1;
            }
            else {
                value[cnt] = f;
                cnt++;
            }

            /* remove detected float */
            while ((*str != 0x20) && (*str != 0x09) && (*str != ',') && (*str != ']')) {
                str++;
            }

            /* skip spaces and comma */
            while ((*str == 0x20) || (*str == ',') || (*str == 0x09) || (*str == 0x0a) || (*str == 0x0d)) {
                str++;
            }
        }

#ifdef DEBUG_LOG
        for (int i = 0; i < cnt; i++) {
            LOGD( "%f,\n", value[i]);
        }
#endif

        return (cnt);
    }
    else if (xmlParseReadWrite == XML_PARSER_WRITE) // write
    {
        char cmd[16] = {"%s%.0f "};
        if (printAccuracy > 9)
            printAccuracy = 9;
        cmd[4] = '0' + printAccuracy;
        autoTabForward();
        autoTabForward();
        int cols = XmlTag(pNode->ToElement()).SizeCol();
        int rows = XmlTag(pNode->ToElement()).SizeRow();
        char str[8192];
        char* strp = str;
        snprintf(str, sizeof(str), "\n%s[", autoTabStr);
        for (int i = 0; i < rows; i++)
        {
            for (int k = 0; k < cols; k++)
                snprintf(str, sizeof(str), cmd, strp, values[i * cols + k]);
            if (i < rows - 1) {
                strcat(str, "\n");
                strcat(str, autoTabStr);
            }
        }
        autoTabBackward();
        strcat(str, "]\n");
        strcat(str, autoTabStr);

        XMLNode* pComment = (XMLNode*)pNode->ToElement()->FirstChild();
        pComment->SetValue((const char *)str);

        autoTabBackward();
        return (num);
    }



err1:
    MEMSET(values, 0, (sizeof(float)* num));

    return (0);

}

/******************************************************************************
* ParseDoubleArray
*****************************************************************************/
int RkAiqCalibParser::ParseDoubleArray
(
    const XMLNode *pNode,          /**< trimmed c string */
    double*       values,            /**< pointer to memory */
    const int    num                 /**< number of expected float values */
) {
    if (xmlParseReadWrite == XML_PARSER_READ) // read
    {
        const char* c_string = XmlTag(pNode->ToElement()).Value();
        double* value = values;
        char* str = (char*)c_string;
        int last = strlen(str);
        /* calc. end adress of string */
        char* str_last = str + (last - 1);

        std::string s_string(str);
        size_t find_start = s_string.find("[", 0);
        size_t find_end = s_string.find("]", 0);

        if ((find_start == std::string::npos) || (find_end == std::string::npos)) {
#ifdef DEBUG_LOG
            LOGD( "%s(%d): start=%d,end=%d\n", __FUNCTION__, __LINE__, find_start, find_end);
#endif
            return -1;
        }

        str = (char*)c_string + find_start;
        str_last = (char*)c_string + find_end;

        /* skipped left parenthesis */
        str++;

        /* skip spaces */
        while (*str == 0x20 || *str == 0x09 || (*str == 0x0a) || (*str == 0x0d)) {
            str++;
        }

        int cnt = 0;
        int scanned;
        double d;

        /* parse the c-string */
        while ((str != str_last) && (cnt < num)) {
            scanned = sscanf(str, "%lf", &d);
            if (scanned != 1) {
                LOGE( "%s(%d): %f err\n", __FUNCTION__, __LINE__, d);
                goto err2;
            }
            else {
                value[cnt] = d;
                cnt++;
            }

            /* remove detected float */
            while ((*str != 0x20) && (*str != 0x09) && (*str != ',') && (*str != ']')) {
                str++;
            }

            /* skip spaces and comma */
            while ((*str == 0x20) || (*str == ',') || (*str == 0x09) || (*str == 0x0a) || (*str == 0x0d)) {
                str++;
            }
        }

#ifdef DEBUG_LOG
        for (int i = 0; i < cnt; i++) {
            LOGD( "%f,\n", value[i]);
        }
#endif

        return (cnt);
    }
    else if (xmlParseReadWrite == XML_PARSER_WRITE) // write
    {
        autoTabForward();
        autoTabForward();
        int cols = XmlTag(pNode->ToElement()).SizeCol();
        int rows = XmlTag(pNode->ToElement()).SizeRow();
        char str[8192];
        snprintf(str, sizeof(str), "\n%s[", autoTabStr);
        char tmp_val_str[25];
        for (int i = 0; i < rows; i++)
        {
            for (int k = 0; k < cols; k++) {
                snprintf(tmp_val_str, sizeof(tmp_val_str), "%.14e ", values[i * cols + k]);
                strcat(str, tmp_val_str);
            }
            if (i < rows - 1) {
                strcat(str, "\n");
                strcat(str, autoTabStr);
            }
        }
        autoTabBackward();
        strcat(str, "]\n");
        strcat(str, autoTabStr);

        XMLNode* pComment = (XMLNode*)pNode->ToElement()->FirstChild();
        pComment->SetValue((const char *)str);
        autoTabBackward();

        return (num);
    }


err2:
    MEMSET(values, 0, (sizeof(float)* num));

    return (0);

}

/******************************************************************************
* ParseUintArray
*****************************************************************************/
int RkAiqCalibParser::ParseUintArray
(
    const XMLNode *pNode,          /**< trimmed c string */
    uint32_t*      values,            /**< pointer to memory */
    const int   num                 /**< number of expected float values */
) {
    if (!xmlParseReadWrite) // read
    {
        uint32_t* value = values;
        const char* c_string = XmlTag(pNode->ToElement()).Value();
        char* str = (char*)c_string;

        int last = strlen(str);
        char* str_last = str + (last - 1);

        std::string s_string(str);
        size_t find_start = s_string.find("[", 0);
        size_t find_end = s_string.find("]", 0);

        if ((find_start == std::string::npos) || (find_end == std::string::npos)) {
#ifdef DEBUG_LOG
            LOGE( "%s(%d): start=%d,end=%d\n", __FUNCTION__, __LINE__, find_start, find_end);
#endif
            return -1;
        }

        str = (char*)c_string + find_start;
        str_last = (char*)c_string + find_end;

        /* calc. end adress of string */
        // char *str_last = str + (last-1);

        /* skipped left parenthesis */
        str++;

        /* skip spaces */
        while (*str == 0x20 || *str == 0x09 || (*str == 0x0a) || (*str == 0x0d)) {
            str++;
        }

        int cnt = 0;
        int scanned;
        uint32_t f;

        /* parse the c-string */
        while ((str != str_last) && (cnt < num)) {
            scanned = sscanf(str, "%u", &f);
            if (scanned != 1) {
                LOGE( "%s(%d): f:%f error\n", __FUNCTION__, __LINE__, f);
                goto err1;
            }
            else {
                value[cnt] = f;
                cnt++;
            }

            /* remove detected float */
            while ((*str != 0x20) && (*str != 0x09) && (*str != ',') && (*str != ']')) {
                str++;
            }

            /* skip spaces and comma */
            while ((*str == 0x20) || (*str == ',') || (*str == 0x09) || (*str == 0x0a) || (*str == 0x0d)) {
                str++;
            }
        }

#ifdef DEBUG_LOG
        for (int i = 0; i < cnt; i++) {
            LOGD( "%u,\n", value[i]);
        }
#endif
        return (cnt);
    }
    else if (xmlParseReadWrite == XML_PARSER_WRITE) // write
    {
        autoTabForward();
        autoTabForward();
        int cols = XmlTag(pNode->ToElement()).SizeCol();
        int rows = XmlTag(pNode->ToElement()).SizeRow();
        char str[8192];
        snprintf(str, sizeof(str), "\n%s[", autoTabStr);
        char tmp_val_str[25];
        for (int i = 0; i < rows; i++)
        {
            for (int k = 0; k < cols; k++) {
                snprintf(tmp_val_str, sizeof(tmp_val_str), "%d ", values[i * cols + k]);
                strcat(str, tmp_val_str);
            }
            if (i < rows - 1) {
                strcat(str, "\n");
                strcat(str, autoTabStr);
            }
        }
        autoTabBackward();
        strcat(str, "]\n");
        strcat(str, autoTabStr);

        XMLNode* pComment = (XMLNode*)pNode->ToElement()->FirstChild();
        pComment->SetValue((const char *)str);
        autoTabBackward();

        return num;
    }


err1:
    MEMSET(values, 0, (sizeof(uint32_t)* num));

    return (0);
}

/******************************************************************************
* ParseIntArray
*****************************************************************************/
int RkAiqCalibParser::ParseIntArray
(
    const XMLNode *pNode,          /**< trimmed c string */
    int32_t*      values,            /**< pointer to memory */
    const int   num                 /**< number of expected float values */
) {

    if (!xmlParseReadWrite) // read
    {
        int32_t* value = values;
        const char* c_string = XmlTag(pNode->ToElement()).Value();
        char* str = (char*)c_string;

        int last = strlen(str);
        char* str_last = str + (last - 1);

        std::string s_string(str);
        size_t find_start = s_string.find("[", 0);
        size_t find_end = s_string.find("]", 0);

        if ((find_start == std::string::npos) || (find_end == std::string::npos)) {

            return -1;
        }

        str = (char*)c_string + find_start;
        str_last = (char*)c_string + find_end;

        /* calc. end adress of string */
        // char *str_last = str + (last-1);

        /* skipped left parenthesis */
        str++;

        /* skip spaces */
        while (*str == 0x20 || *str == 0x09 || (*str == 0x0a) || (*str == 0x0d)) {
            str++;
        }

        int cnt = 0;
        int scanned;
        int32_t f;

        /* parse the c-string */
        while ((str != str_last) && (cnt < num)) {
            scanned = sscanf(str, "%d", &f);
            if (scanned != 1) {
                LOGE( "%s(%d): f:%f error\n", __FUNCTION__, __LINE__, f);
                goto err1;
            }
            else {
                value[cnt] = f;
                cnt++;
            }

            /* remove detected float */
            while ((*str != 0x20) && (*str != 0x09) && (*str != ',') && (*str != ']')) {
                str++;
            }

            /* skip spaces and comma */
            while ((*str == 0x20) || (*str == ',') || (*str == 0x09) || (*str == 0x0a) || (*str == 0x0d)) {
                str++;
            }
        }

#ifdef DEBUG_LOG
        for (int i = 0; i < cnt; i++) {
            LOGD( "%d,\n", value[i]);
        }
#endif
        return (cnt);
    }
    else if (xmlParseReadWrite == XML_PARSER_WRITE) // write
    {
        autoTabForward();
        autoTabForward();
        int cols = XmlTag(pNode->ToElement()).SizeCol();
        int rows = XmlTag(pNode->ToElement()).SizeRow();
        char str[8192];
        snprintf(str, sizeof(str), "\n%s[", autoTabStr);
        char tmp_val_str[25];
        for (int i = 0; i < rows; i++)
        {
            for (int k = 0; k < cols; k++) {
                snprintf(tmp_val_str, sizeof(tmp_val_str), "%d", values[i * cols + k]);
                strcat(str, tmp_val_str);
            }

            if (i < rows - 1) {
                strcat(str, "\n");
                strcat(str, autoTabStr);
            }
        }
        autoTabBackward();
        strcat(str, "]\n");
        strcat(str, autoTabStr);

        XMLNode* pComment = (XMLNode*)pNode->ToElement()->FirstChild();
        pComment->SetValue((const char *)str);
        autoTabBackward();

        return num;
    }


err1:
    MEMSET(values, 0, (sizeof(int32_t)* num));

    return (0);
}

/******************************************************************************
* ParseUchartArray//cxf
*****************************************************************************/
int RkAiqCalibParser::ParseUcharArray
(
    const XMLNode *pNode,          /**< trimmed c string */
    uint8_t*     values,            /**< pointer to memory */
    const int   num                 /**< number of expected float values */
) {

    if (!xmlParseReadWrite) // read
    {
        uint8_t* value = values;
        const char* c_string = XmlTag(pNode->ToElement()).Value();
        char* str = (char*)c_string;

        int last = strlen(str);
        char* str_last = str + (last - 1);

        std::string s_string(str);
        size_t find_start = s_string.find("[", 0);
        size_t find_end = s_string.find("]", 0);

        if ((find_start == std::string::npos) || (find_end == std::string::npos)) {

            return -1;
        }

        str = (char*)c_string + find_start;
        str_last = (char*)c_string + find_end;


        /* calc. end adress of string */
        // char *str_last = str + (last-1);

        /* skipped left parenthesis */
        str++;

        /* skip spaces */
        while (*str == 0x20 || *str == 0x09 || (*str == 0x0a) || (*str == 0x0d)) {
            str++;
        }

        int cnt = 0;
        int scanned;
        uint16_t f;  // uint8_t f;

        /* parse the c-string */
        while ((str != str_last) && (cnt < num)) {
            scanned = sscanf(str, "%hu", &f);  // %hhu
            if (scanned != 1) {
                LOGD( "%s(%d):f:%f\n", __FUNCTION__, __LINE__, f);
                goto err1;
            }
            else {
                value[cnt] = f;
                cnt++;
            }

            /* remove detected float */
            while ((*str != 0x20) && (*str != 0x09) && (*str != ',') && (*str != ']')) {
                str++;
            }

            /* skip spaces and comma */
            while ((*str == 0x20) || (*str == ',') || (*str == 0x09) || (*str == 0x0a) || (*str == 0x0d)) {
                str++;
            }

        }

#ifdef DEBUG_LOG
        for (int i = 0; i < cnt; i++) {
            LOGD( "%u,\n", value[i]);
        }
#endif

        return (cnt);
    }
    else if (xmlParseReadWrite == XML_PARSER_WRITE) // write
    {
        autoTabForward();
        autoTabForward();
        int cols = XmlTag(pNode->ToElement()).SizeCol();
        int rows = XmlTag(pNode->ToElement()).SizeRow();
        char str[8192];
        snprintf(str, sizeof(str), "\n%s[", autoTabStr);
        char tmp_val_str[25];
        for (int i = 0; i < rows; i++)
        {
            for (int k = 0; k < cols; k++) {
                snprintf(tmp_val_str, sizeof(tmp_val_str), "%d ", values[i * cols + k]);
                strcat(str, tmp_val_str);
            }

            if (i < rows - 1) {
                strcat(str, "\n");
                strcat(str, autoTabStr);
            }
        }
        autoTabBackward();
        strcat(str, "]\n");
        strcat(str, autoTabStr);

        XMLNode* pComment = (XMLNode*)pNode->ToElement()->FirstChild();
        pComment->SetValue((const char *)str);
        autoTabBackward();

        return num;
    }

err1:
    MEMSET(values, 0, (sizeof(uint8_t)* num));

    return (0);
}
/******************************************************************************
* ParseUchartArray
*****************************************************************************/
int RkAiqCalibParser::ParseCharArray
(
    const XMLNode *pNode,          /**< trimmed c string */
    int8_t*      values,            /**< pointer to memory */
    const int   num                 /**< number of expected float values */
) {

    XMLNode* pComment = (XMLNode*)pNode->ToElement()->FirstChild();

    if (!xmlParseReadWrite) // read
    {
        int8_t*  value = values;
        const char* c_string = XmlTag(pNode->ToElement()).Value();
        char* str = (char*)c_string;

        int last = strlen(str);
        char* str_last = str + (last - 1);

        std::string s_string(str);
        size_t find_start = s_string.find("[", 0);
        size_t find_end = s_string.find("]", 0);

        if ((find_start == std::string::npos) || (find_end == std::string::npos)) {
#ifdef DEBUG_LOG
            LOGE( "%s(%d): start=%d,end=%d\n", __FUNCTION__, __LINE__, find_start, find_end);
#endif
            return -1;
        }

        str = (char*)c_string + find_start;
        str_last = (char*)c_string + find_end;

        /* calc. end adress of string */
        // char *str_last = str + (last-1);

        /* skipped left parenthesis */
        str++;

        /* skip spaces */
        while (*str == 0x20 || *str == 0x09 || (*str == 0x0a) || (*str == 0x0d)) {
            str++;
        }

        int cnt = 0;
        int scanned;
        int16_t f;

        /* parse the c-string */
        while ((str != str_last) && (cnt < num)) {
            scanned = sscanf(str, "%hd", &f);
            if (scanned != 1) {
                LOGE("%s(%d): error", __FUNCTION__, __LINE__);
                goto err1;
            }
            else {
                value[cnt] = f;
                cnt++;
            }

            /* remove detected float */
            while ((*str != 0x20) && (*str != 0x09) && (*str != ',') && (*str != ']')) {
                str++;
            }

            /* skip spaces and comma */
            while ((*str == 0x20) || (*str == ',') || (*str == 0x09) || (*str == 0x0a) || (*str == 0x0d)) {
                str++;
            }
        }

#ifdef DEBUG_LOG
        for (int i = 0; i < cnt; i++) {
            LOGD( "%d,\n", value[i]);
        }
#endif
        return (cnt);
    }
    else if (xmlParseReadWrite == XML_PARSER_WRITE) // write
    {
        autoTabForward();
        autoTabForward();
        int cols = XmlTag(pNode->ToElement()).SizeCol();
        int rows = XmlTag(pNode->ToElement()).SizeRow();

        char str[8192];
        snprintf(str, sizeof(str), "\n%s[", autoTabStr);
        char tmp_val_str[25];
        for (int i = 0; i < rows; i++)
        {
            for (int k = 0; k < cols; k++) {
                snprintf(tmp_val_str, sizeof(tmp_val_str), "%d ", values[i * cols + k]);
                strcat(str, tmp_val_str);
            }

            if (i < rows - 1) {
                strcat(str, "\n");
                strcat(str, autoTabStr);
            }
        }
        autoTabBackward();
        strcat(str, "]\n");
        strcat(str, autoTabStr);

        XMLNode* pComment = (XMLNode*)pNode->ToElement()->FirstChild();
        pComment->SetValue((const char *)str);
        autoTabBackward();

        return num;
    }

err1:
    MEMSET(values, 0, (sizeof(uint8_t)* num));

    return (0);
}


/******************************************************************************
* ParseUshortArray
*****************************************************************************/
int RkAiqCalibParser::ParseUshortArray
(
    const XMLNode *pNode,          /**< trimmed c string */
    uint16_t*    values,            /**< pointer to memory */
    const int   num                 /**< number of expected float values */
) {

    XMLNode* pComment = (XMLNode*)pNode->ToElement()->FirstChild();

    if (!xmlParseReadWrite) // read
    {
        uint16_t* value = values;
        const char* c_string = XmlTag(pNode->ToElement()).Value();
        char* str = (char*)c_string;
        int last = strlen(str);
        char* str_last = str + (last - 1);

        std::string s_string(str);
        size_t find_start = s_string.find("[", 0);
        size_t find_end = s_string.find("]", 0);

        if ((find_start == std::string::npos) || (find_end == std::string::npos)) {
#ifdef DEBUG_LOG
            LOGE( "%s(%d): parse error! start:%s end:%s\n",
                  __FUNCTION__, __LINE__, find_start, find_end);
#endif
            return -1;
        }

        str = (char*)c_string + find_start;
        str_last = (char*)c_string + find_end;


        /* calc. end adress of string */
        //char *str_last = str + (last-1);

        /* skipped left parenthesis */
        str++;

        /* skip spaces */
        while (*str == 0x20 || *str == 0x09 || (*str == 0x0a) || (*str == 0x0d)) {
            str++;
        }

        int cnt = 0;
        int scanned;
        uint16_t f;

        /* parse the c-string */
        while ((str != str_last) && (cnt < num)) {
            scanned = sscanf(str, "%hu", &f);
            if (scanned != 1) {
                LOGD( "%s(%d): parse error!\n", __FUNCTION__, __LINE__);
                goto err1;
            }
            else {
                value[cnt] = f;
                cnt++;
            }

            /* remove detected float */
            while ((*str != 0x20) && (*str != 0x09) && (*str != ',') && (*str != ']')) {
                str++;
            }

            /* skip spaces and comma */
            while ((*str == 0x20) || (*str == ',') || (*str == 0x09) || (*str == 0x0a) || (*str == 0x0d)) {
                str++;
            }
        }

#ifdef DEBUG_LOG
        for (int i = 0; i < cnt; i++) {
            LOGD( "%u,\n", value[i]);
        }
#endif
        return (cnt);
    }
    else if (xmlParseReadWrite == XML_PARSER_WRITE) // write
    {
        autoTabForward();
        autoTabForward();
        int cols = XmlTag(pNode->ToElement()).SizeCol();
        int rows = XmlTag(pNode->ToElement()).SizeRow();
        char str[8192];
        snprintf(str, sizeof(str), "\n%s[", autoTabStr);
        char tmp_val_str[25];
        for (int i = 0; i < rows; i++)
        {
            for (int k = 0; k < cols; k++) {
                snprintf(tmp_val_str, sizeof(tmp_val_str), "%d ", values[i * cols + k]);
                strcat(str, tmp_val_str);
            }

            if (i < rows - 1) {
                strcat(str, "\n");
                strcat(str, autoTabStr);
            }
        }
        autoTabBackward();
        strcat(str, "]\n");
        strcat(str, autoTabStr);

        XMLNode* pComment = (XMLNode*)pNode->ToElement()->FirstChild();
        pComment->SetValue((const char *)str);
        autoTabBackward();

        return num;
    }


err1:
    MEMSET(values, 0, (sizeof(uint16_t)* num));

    return (0);

}


/******************************************************************************
* ParseShortArray
*****************************************************************************/
int RkAiqCalibParser::ParseShortArray
(
    const XMLNode *pNode,          /**< trimmed c string */
    int16_t*     values,            /**< pointer to memory */
    const int   num                 /**< number of expected float values */
) {
    XMLNode* pComment = (XMLNode*)pNode->ToElement()->FirstChild();

    if (!xmlParseReadWrite) // read
    {
        int16_t* value = values;
        const char* c_string = XmlTag(pNode->ToElement()).Value();
        char* str = (char*)c_string;

        int last = strlen(str);
        char* str_last = str + (last - 1);

        std::string s_string(str);
        size_t find_start = s_string.find("[", 0);
        size_t find_end = s_string.find("]", 0);

        if ((find_start == std::string::npos) || (find_end == std::string::npos)) {
#ifdef DEBUG_LOG
            LOGE( "%s(%d): parse error!\n", __FUNCTION__, __LINE__);
#endif
            return -1;
        }

        str = (char*)c_string + find_start;
        str_last = (char*)c_string + find_end;

        /* calc. end adress of string */
        // char *str_last = str + (last-1);

        /* skipped left parenthesis */
        str++;

        /* skip spaces */
        while (*str == 0x20 || *str == 0x09 || (*str == 0x0a) || (*str == 0x0d)) {
            str++;
        }

        int cnt = 0;
        int scanned;
        int16_t f;

        /* parse the c-string */
        while ((str != str_last) && (cnt < num)) {
            scanned = sscanf(str, "%hd", &f);
            if (scanned != 1) {
                LOGE( "%s(%d): parse error!\n", __FUNCTION__, __LINE__);
                goto err1;
            }
            else {
                value[cnt] = f;
                cnt++;
            }

            /* remove detected float */
            while ((*str != 0x20) && (*str != 0x09) && (*str != ',') && (*str != ']')) {
                str++;
            }

            /* skip spaces and comma */
            while ((*str == 0x20) || (*str == ',') || (*str == 0x09) || (*str == 0x0a) || (*str == 0x0d)) {
                str++;
            }
        }

#ifdef DEBUG_LOG
        for (int i = 0; i < cnt; i++) {
            LOGD( "%d,\n", value[i]);
        }
#endif
        return (cnt);
    }
    else if (xmlParseReadWrite == XML_PARSER_WRITE) // write
    {
        autoTabForward();
        autoTabForward();
        int cols = XmlTag(pNode->ToElement()).SizeCol();
        int rows = XmlTag(pNode->ToElement()).SizeRow();
        char str[8192];
        snprintf(str, sizeof(str), "\n%s[", autoTabStr);
        char tmp_val_str[25];
        for (int i = 0; i < rows; i++)
        {
            for (int k = 0; k < cols; k++) {
                snprintf(tmp_val_str, sizeof(tmp_val_str), "%d ", values[i * cols + k]);
                strcat(str, tmp_val_str);
            }

            if (i < rows - 1) {
                strcat(str, "\n");
                strcat(str, autoTabStr);
            }
        }
        autoTabBackward();
        strcat(str, "]\n");
        strcat(str, autoTabStr);

        XMLNode* pComment = (XMLNode*)pNode->ToElement()->FirstChild();
        pComment->SetValue((const char *)str);
        autoTabBackward();

        return num;
    }


err1:
    MEMSET(values, 0, (sizeof(uint16_t)* num));

    return (0);

}


/******************************************************************************
* ParseByteArray
*****************************************************************************/
int RkAiqCalibParser::ParseByteArray
(
    const XMLNode *pNode,          /**< trimmed c string */
    uint8_t*     values,            /**< pointer to memory */
    const int   num                 /**< number of expected float values */
) {

    XMLNode* pComment = (XMLNode*)pNode->ToElement()->FirstChild();

    if (!xmlParseReadWrite) // read
    {
        uint8_t* value = values;
        const char* c_string = XmlTag(pNode->ToElement()).Value();

        char* str = (char*)c_string;
        int last = strlen(str);
        char* str_last = str + (last - 1);

        std::string s_string(str);
        size_t find_start = s_string.find("[", 0);
        size_t find_end = s_string.find("]", 0);

        if ((find_start == std::string::npos) || (find_end == std::string::npos)) {
#ifdef DEBUG_LOG
            LOGE( "%s(%d): parse error!\n", __FUNCTION__, __LINE__);
#endif
            return -1;
        }

        str = (char*)c_string + find_start;
        str_last = (char*)c_string + find_end;

        /* calc. end adress of string */
        //char *str_last = str + (last-1);

        /* skipped left parenthesis */
        str++;

        /* skip spaces */
        while (*str == 0x20 || *str == 0x09 || (*str == 0x0a) || (*str == 0x0d)) {
            str++;
        }

        int cnt = 0;
        int scanned;
        uint16_t f;

        /* parse the c-string */
        while ((str != str_last) && (cnt < num)) {
            scanned = sscanf(str, "%hu", &f);
            if (scanned != 1) {
                LOGE( "%s(%d): parse error!\n", __FUNCTION__, __LINE__);
                goto err1;
            }
            else {
                value[cnt] = (uint8_t)f;
                cnt++;
            }

            /* remove detected float */
            while ((*str != 0x20) && (*str != 0x09) && (*str != ',') && (*str != ']')) {
                str++;
            }

            /* skip spaces and comma */
            while ((*str == 0x20) || (*str == ',') || (*str == 0x09) || (*str == 0x0a) || (*str == 0x0d)) {
                str++;
            }
        }

#ifdef DEBUG_LOG
        for (int i = 0; i < cnt; i++) {
            LOGD( "%u,\n", value[i]);
        }
#endif
        return (cnt);

    }
    else if (xmlParseReadWrite == XML_PARSER_WRITE) // write
    {
        autoTabForward();
        autoTabForward();
        int cols = XmlTag(pNode->ToElement()).SizeCol();
        int rows = XmlTag(pNode->ToElement()).SizeRow();
        char str[8192];
        snprintf(str, sizeof(str), "\n%s[", autoTabStr);
        char tmp_val_str[25];
        for (int i = 0; i < rows; i++)
        {
            for (int k = 0; k < cols; k++) {
                snprintf(tmp_val_str, sizeof(tmp_val_str), "%d ", values[i * cols + k]);
                strcat(str, tmp_val_str);
            }

            if (i < rows - 1) {
                strcat(str, "\n");
                strcat(str, autoTabStr);
            }
        }
        autoTabBackward();
        strcat(str, "]\n");
        strcat(str, autoTabStr);

        XMLNode* pComment = (XMLNode*)pNode->ToElement()->FirstChild();
        pComment->SetValue((const char *)str);
        autoTabBackward();
        return num;
    }

err1:
    MEMSET(values, 0, (sizeof(uint8_t)* num));

    return (0);

}


/******************************************************************************
* ParseString
*****************************************************************************/
int RkAiqCalibParser::ParseString
(
    const XMLNode *pNode,          /**< trimmed c string */
    char*        values,            /**< pointer to memory */
    const int    size                 /**< size of memory */
) {


    if (xmlParseReadWrite == XML_PARSER_READ) // read
    {
        const char* c_string = XmlTag(pNode->ToElement()).Value();
        char* str = (char*)c_string;
        int last = strlen(str);
        char* str_last = str + (last - 1);

        if (*str == '[')
            str += 1;
        if (*str_last == ']')
            *str_last = '\0';

        memset(values, 0, size);
        strncpy(values, str, size - 1);
        //values[size] = '';

        return 0;
    }
    else if (xmlParseReadWrite == XML_PARSER_WRITE)
    {
        char str[8192];
        //sprintf(str, "[%s]", values);
        autoTabForward();
        autoTabForward();
        snprintf(str, sizeof(str), "\n%s%s", autoTabStr, values);
        autoTabBackward();
        strcat(str, "\n");
        strcat(str, autoTabStr);
        autoTabBackward();
        XMLNode* pComment = (XMLNode*)pNode->ToElement()->FirstChild();
        pComment->SetValue((const char *)str);
        snprintf(str, sizeof(str), "[1 %zu]", strlen(values));
        XMLElement* pElement = (((XMLNode*)pNode)->ToElement());
        pElement->SetAttribute(CALIB_ATTRIBUTE_SIZE, str);
        return 0;
    }
    return 0;
}


/******************************************************************************
* ParseLscProfileArray
*****************************************************************************/
int RkAiqCalibParser::ParseLscProfileArray
(
    const XMLNode *pNode,          /**< trimmed c string */
    CalibDb_Lsc_ProfileName_t values[],           /**< pointer to memory */
    const int           num                 /**< number of expected float values */
) {

    if (!xmlParseReadWrite) // read
    {
        for (int i = 0; i < LSC_PROFILES_NUM_MAX; i++)
            memset(values[i], 0, LSC_PROFILE_NAME);

        const char *c_string = XmlTag(pNode->ToElement()).Value();

        Toupper(c_string);
        char *str = (char *)c_string;

        int last = strlen(str);

        /* calc. end adress of string */
        char *str_last = str + (last - 1);

        /* skip beginning spaces */
        while (*str == 0x20 || *str == 0x09)
        {
            str++;
        }

        /* skip ending spaces */
        while (*str_last == 0x20 || *str_last == 0x09)
        {
            str_last--;
        }

        int cnt = 0;
        int scanned;
        CalibDb_Lsc_ProfileName_t f;
        memset(f, 0, sizeof(f));

        /* parse the c-string */
        while ((str != str_last) && (cnt < num))
        {
            scanned = sscanf(str, "%s", f);
            if (scanned != 1)
            {
                LOGE("%s(%d): parse error!\n", __FUNCTION__, __LINE__);
                goto err1;
            }
            else
            {
                strncpy(values[cnt], f, strlen(f));
                values[cnt][strlen(f)] = '\0';
                cnt++;
            }

            /* remove detected string */
            while ((*str != 0x20) && (*str != ',') && (*str != ']') && (str != str_last))
            {
                str++;
            }

            if (str != str_last)
            {
                /* skip spaces and comma */
                while ((*str == 0x20) || (*str == ','))
                {
                    str++;
                }
            }

            memset(f, 0, sizeof(f));
        }

        return (cnt);
    }
    else if (xmlParseReadWrite == XML_PARSER_WRITE) // write
    {
        autoTabForward();
        autoTabForward();
        char str[8192];
        snprintf(str, sizeof(str), "\n%s", autoTabStr);
        int i = 0;
        size_t str_len = 0;
        while (values[i][0] != 0)
        {
            str_len += strlen(values[i]);
            strcat(str, values[i++]);
            str_len++;
        }
        autoTabBackward();
        strcat(str, "\n");
        strcat(str, autoTabStr);
        XMLNode* pComment = (XMLNode*)pNode->ToElement()->FirstChild();
        pComment->SetValue((const char *)str);

        snprintf(str, sizeof(str), "[1 %zu]", str_len - 1);
        pComment = (XMLNode *)pNode;
        pComment->ToElement()->SetAttribute(CALIB_ATTRIBUTE_SIZE, str);
        autoTabBackward();

        return num;
    }



err1:
    memset(values, 0, (sizeof(uint16_t)* num));

    return (0);
}

void RkAiqCalibParser::autoTabForward()
{
    if (autoTabIdx == 124)
        return;
    autoTabStr[autoTabIdx] = ' ';
    autoTabIdx += 4;
    autoTabStr[autoTabIdx] = '\0';
}

void RkAiqCalibParser::autoTabBackward()
{
    if (autoTabIdx == 0)
        return;
    autoTabStr[autoTabIdx] = ' ';
    autoTabIdx -= 4;
    autoTabStr[autoTabIdx] = '\0';
}

RkAiqCalibParser::RkAiqCalibParser(CamCalibDbContext_t *pCalibDb)
{
    mCalibDb = pCalibDb;
    xmlParseReadWrite = XML_PARSER_READ;
    memset(autoTabStr, ' ', sizeof(autoTabStr));
    autoTabIdx = 0;
    autoTabStr[0] = '\0';
}

RkAiqCalibParser::~RkAiqCalibParser()
{

}

bool RkAiqCalibParser::doParse
(
    const char* device
)
{
    int errorID;
    XMLDocument doc;

    bool res = true;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    errorID = doc.LoadFile(device);

    LOGD("%s(%d): doc.LoadFile filename:%s  errorID:%d\n",
         __FUNCTION__, __LINE__, device, errorID);

    if (doc.Error()) {
        LOGE("%s(%d): Error: Parse error errorID %d\n",
             __FUNCTION__, __LINE__, errorID);
        return (false);
    }
    XMLElement* proot = doc.RootElement();
    std::string tagname(proot->Name());
    if (tagname != CALIB_FILESTART_TAG) {
        LOGE("Error: Not a calibration data file");

        return (false);
    }
    // parse Read Mode
    parseReadWriteCtrl(XML_PARSER_READ);

    // parse header section
    XMLElement* pheader = proot->FirstChildElement(CALIB_HEADER_TAG);
    if (pheader) {
        res = parseEntryHeader(pheader->ToElement(), NULL);
        if (!res) {
            return (res);
        }
    }

    // parse sensor section
    XMLElement* psensor = proot->FirstChildElement(CALIB_SENSOR_TAG);
    if (psensor) {
        res = parseEntrySensor(psensor->ToElement(), NULL);
        if (!res) {
            return (res);
        }
    }

    // parse system section
    XMLElement* psystem = proot->FirstChildElement(CALIB_SYSTEM_TAG);
    if (psystem) {
        res = parseEntrySystem(psystem->ToElement(), NULL);
        if (!res) {
            return (res);
        }
    }

    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);

    return (res);
}

bool RkAiqCalibParser::doGenerate
(
    const char* deviceRef,
    const char* deviceOutput
)
{
    int errorID;
    XMLDocument doc;

    bool res = true;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    errorID = doc.LoadFile(deviceRef);

    LOGD("%s(%d): doc.LoadFile filename:%s  errorID:%d\n",
         __FUNCTION__, __LINE__, deviceRef, errorID);

    if (doc.Error()) {
        LOGE("%s(%d): Error: Parse error errorID %d\n",
             __FUNCTION__, __LINE__, errorID);
        return (false);
    }
    XMLElement* proot = doc.RootElement();
    std::string tagname(proot->Name());
    if (tagname != CALIB_FILESTART_TAG) {
        LOGE("Error: Not a calibration data file");

        return (false);
    }
    // parse Read Mode
    parseReadWriteCtrl(XML_PARSER_WRITE);

    // parse header section
    XMLElement* pheader = proot->FirstChildElement(CALIB_HEADER_TAG);
    if (pheader) {
        res = parseEntryHeader(pheader->ToElement(), NULL);
        if (!res) {
            return (res);
        }
    }

    // parse sensor section
    XMLElement* psensor = proot->FirstChildElement(CALIB_SENSOR_TAG);
    if (psensor) {
        res = parseEntrySensor(psensor->ToElement(), NULL);
        if (!res) {
            return (res);
        }
    }

    // parse system section
    XMLElement* psystem = proot->FirstChildElement(CALIB_SYSTEM_TAG);
    if (psystem) {
        res = parseEntrySystem(psystem->ToElement(), NULL);
        if (!res) {
            return (res);
        }
    }

    errorID = doc.SaveFile(deviceOutput);

    LOGD("%s(%d): doc.SaveFile filename:%s  errorID:%d\n",
         __FUNCTION__, __LINE__, deviceRef, errorID);

    if (doc.Error()) {
        LOGE("%s(%d): Error: Generate error ID %d\n",
             __FUNCTION__, __LINE__, errorID);
        return (false);
    }

    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);

    return (res);
}


bool RkAiqCalibParser::parseEntryCell
(
    const XMLElement*   pelement,
    int                 noElements,
    parseCellContent    func,
    void*                param
) {
    int cnt = 0;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild && (cnt < noElements)) {
        XmlCellTag tag = XmlCellTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if (tagname == CALIB_CELL_TAG) {
            autoTabForward();
            bool result = (this->*func)(pchild->ToElement(), /*param*/(void*)&cnt);
            autoTabBackward();
            if (!result) {
                return (result);
            }
        }
        else {
            LOG1("unknown cell tag: %s", tagname.c_str());

            return (false);
        }

        pchild = pchild->NextSibling();
        cnt++;
    }

    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();

    return (true);
}

bool RkAiqCalibParser::parseEntryCell2
(
    const XMLElement*   pelement,
    int                 noElements,
    parseCellContent2    func,
    void*                param
) {
    int cnt = 0;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild && (cnt < noElements)) {
        XmlCellTag tag = XmlCellTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if (tagname == CALIB_CELL_TAG) {
            autoTabForward();
            bool result = (this->*func)(pchild->ToElement(), param, cnt);
            autoTabBackward();
            if (!result) {
                return (result);
            }
        }
        else {
            LOG1("unknown cell tag: %s", tagname.c_str());

            return (false);
        }

        pchild = pchild->NextSibling();
        cnt++;
    }

    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();

    return (true);
}


bool RkAiqCalibParser::parseEntryHeader
(
    const XMLElement*   pelement,
    void*                param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        const char* value = tag.Value();
        std::string tagname(pchild->ToElement()->Name());

#ifdef DEBUG_LOG
        LOG1("tag: %s", tagname.c_str());
#endif
        if ((tagname == CALIB_HEADER_CODE_XML_PARSE_VERSION)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->header.parse_version,
                        sizeof(mCalibDb->header.parse_version));
        }
        else if ((tagname == CALIB_HEADER_CREATION_DATE_TAG)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->header.date,
                        sizeof(mCalibDb->header.date));
        }
        else if ((tagname == CALIB_HEADER_CREATOR_TAG)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->header.author,
                        sizeof(mCalibDb->header.author));
        }
        else if ((tagname == CALIB_HEADER_GENERATOR_VERSION_TAG)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->header.gen_verion,
                        sizeof(mCalibDb->header.gen_verion));
        }
        else if ((tagname == CALIB_HEADER_SENSOR_NAME_TAG)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->header.sensor_name,
                        sizeof(mCalibDb->header.sensor_name));
        }
        else if ((tagname == CALIB_HEADER_SAMPLE_NAME_TAG)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->header.sample_name,
                        sizeof(mCalibDb->header.sample_name));
        }
        else {

            LOGE("parse error in header section (unknow tag: %s)", tagname.c_str());
            return (false);
        }

        pchild = pchild->NextSibling();
    }

    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();

    return (true);

}

bool RkAiqCalibParser::parseEntrySensor
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    (void)param;

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if (tagname == CALIB_SENSOR_AWB_TAG) {
            if (!parseEntrySensorAwb(pchild->ToElement())) {
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_AEC) {
            if (!parseEntrySensorAec(pchild->ToElement())) {
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_AHDR) {
            if (!parseEntrySensorAhdr(pchild->ToElement())) {
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_BLC) {
            if (!parseEntrySensorBlc(pchild->ToElement())) {
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_LUT3D) {
            if (!parseEntrySensorLut3d(pchild->ToElement())) {
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_DPCC) {
            if (!parseEntrySensorDpcc(pchild->ToElement())) {
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_BAYERNR) {
            if (!parseEntrySensorBayerNr(pchild->ToElement())) {
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_LSC) {
            if (!parseEntrySensorLsc(pchild->ToElement())) {
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_RKDM) {
            if (!parseEntrySensorRKDM(pchild->ToElement())) {
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_CCM) {
            if (!parseEntrySensorCCM(pchild->ToElement())) {
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_UVNR) {
            if (!parseEntrySensorUVNR(pchild->ToElement())) {
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_GAMMA) {
            if (!parseEntrySensorGamma(pchild->ToElement())) {
                return (false);
            }
        } else if (tagname == CALIB_SENSOR_YNR) {
            if (!parseEntrySensorYnr(pchild->ToElement()))
            {
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_GIC) {
            if (!parseEntrySensorGic(pchild->ToElement())) {
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_MFNR) {
            if (!parseEntrySensorMFNR(pchild->ToElement())) {
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_SHARP) {
            if (!parseEntrySensorSharp(pchild->ToElement())) {
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_EDGEFILTER) {
            if (!parseEntrySensorEdgeFilter(pchild->ToElement())) {
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_DEHAZE) {
            if (!parseEntrySensorDehaze(pchild->ToElement())) {
                return (false);
            }
        } else if (tagname == CALIB_SENSOR_AF) {
            if (!parseEntrySensorAf(pchild->ToElement())) {
                return (false);
            }
        } else if (tagname == CALIB_SENSOR_LDCH) {
            if (!parseEntrySensorLdch(pchild->ToElement())) {
                return (false);
            }
        } else if (tagname == CALIB_SENSOR_FEC) {
            if (!parseEntrySensorFec(pchild->ToElement())) {
                return (false);
            }
        } else if (tagname == CALIB_SENSOR_LUMA_DETECT) {
            if (!parseEntrySensorLumaDetect(pchild->ToElement())) {
                return (false);
            }
        } else if (tagname == CALIB_SENSOR_ORB) {
            if (!parseEntrySensorOrb(pchild->ToElement())) {
                return (false);
            }
        } else if (tagname == CALIB_SENSOR_SENSORINFO) {
            if (!parseEntrySensorInfo(pchild->ToElement())) {
                return (false);
            }
        } else if (tagname == CALIB_SENSOR_CPSL) {
            if (!parseEntrySensorCpsl(pchild->ToElement())) {
                return (false);
            }
        }
        pchild = pchild->NextSibling();
    }

    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

/******************************************************************************
* RkAiqCalibParser::parseEntryAwb
*****************************************************************************/
bool RkAiqCalibParser::parseEntrySensorAwb
(
    const XMLElement*   pelement,
    void*                param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();
    if (xmlParseReadWrite == XML_PARSER_READ) {
        memset(&mCalibDb->awb, 0, sizeof(mCalibDb->awb));
    }
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_AWB_AWB_ENABLE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            unsigned char tempVal = mCalibDb->awb.awbEnable;
            int no = ParseUcharArray(pchild, &tempVal, 1);
            mCalibDb->awb.awbEnable = (tempVal == 0 ? false : true);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_WB_BYPASS)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            unsigned char tempVal = mCalibDb->awb.wbBypass;
            int no = ParseUcharArray(pchild, &tempVal, 1);
            mCalibDb->awb.wbBypass = (tempVal == 0 ? false : true);
            DCT_ASSERT((no == tag.Size()));
        }
        else if (tagname == CALIB_SENSOR_AWB_V200_TAG) {
            if (!parseEntrySensorAwbMeasureParaV200(pchild->ToElement())) {
                LOGE("parse error in AWB (%s)", tagname);
                return (false);
            }
        } else if (tagname == CALIB_SENSOR_AWB_V201_TAG) {
            if (!parseEntrySensorAwbMeasureParaV201(pchild->ToElement())) {
                LOGE("parse error in AWB (%s)", tagname);
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_AWB_STATEGYPARA_TAG) {
            if (!parseEntrySensorAwbStategyPara(pchild->ToElement())) {
                LOGE("parse error in AWB (%s)", tagname);
                return (false);
            }
        } else if (tagname == CALIB_SENSOR_AWB_REMOSAICPARA ) {
            if (!parseEntrySensorAwbRemosaicPara(pchild->ToElement())) {
                LOGE("parse error in AWB (%s)", tagname);
                return (false);
            }
        }
        else {
            LOGE("parse error in AWB section (unknow tag:%s)", tagname.c_str());
            return (false);
        }

        pchild = pchild->NextSibling();
    }

    DCT_ASSERT((mCalibDb->awb.stategy_cfg.lightNum == mCalibDb->awb.measure_para_v200.lightNum));
    DCT_ASSERT((mCalibDb->awb.stategy_cfg.lightNum == mCalibDb->awb.measure_para_v201.lightNum));
    //v201 to do
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAwbMeasureParaV200
(
    const XMLElement*   pelement,
    void*                param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if (tagname == CALIB_SENSOR_AWB_GLOBALS_TAG) {
            if (!parseEntrySensorAwbMeasureGlobalsV200(pchild->ToElement())) {
                LOGE("parse error in AWB globals (%s)", tagname);
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_AWB_LIGHTSOURCES) {
            mCalibDb->awb.measure_para_v200.lightNum = 0;
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorAwbMeasureLightSourcesV200)) {
                LOGE("parse error in AWB light sources (%s)", tagname);
                return (false);
            }
        }
        else {
            LOGE("parse error in AWB section (unknow tag:%s)", tagname.c_str());
            return (false);
        }

        pchild = pchild->NextSibling();
    }

    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();

    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAwbStategyPara
(
    const XMLElement*   pelement,
    void*                param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if (tagname == CALIB_SENSOR_AWB_GLOBALS_TAG) {
            if (!parseEntrySensorAwbStategyGlobals(pchild->ToElement())) {
                LOGE("parse error in AWB globals (%s)", tagname);
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_AWB_LIGHTSOURCES) {
            mCalibDb->awb.stategy_cfg.lightNum = 0;
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorAwbStategyLightSources)) {
                LOGE("parse error in AWB light sources (%s)", tagname);
                return (false);
            }
        }
        else {
            LOGE("parse error in AWB section (unknow tag:%s)", tagname.c_str());
            return (false);
        }

        pchild = pchild->NextSibling();
    }

    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();

    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAwbMeasureGlobalsV200
(
    const XMLElement*   pelement,
    void*                param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_AWB_HDRFRAMECHOOSE)) {
            if (!parseEntrySensorAwbFrameChoose(pchild->ToElement())) {
                LOGE("parse error in AWB (%s)", tagname);
                return (false);
            }
        }
        else if ((tagname == CALIB_SENSOR_AWB_LSCBYPASSENABLE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            unsigned char tempVal = mCalibDb->awb.measure_para_v200.lscBypEnable;
            int no = ParseUcharArray(pchild, &tempVal, 1);
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->awb.measure_para_v200.lscBypEnable = (tempVal == 0 ? false : true);
        }
        else if ((tagname == CALIB_SENSOR_AWB_UVDETECTIONENABLE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            unsigned char tempVal = mCalibDb->awb.measure_para_v200.uvDetectionEnable;
            int no = ParseUcharArray(pchild, &tempVal, 1);
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->awb.measure_para_v200.uvDetectionEnable = (tempVal == 0 ? false : true);
        }
        else if ((tagname == CALIB_SENSOR_AWB_XYDETECTIONENABLE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            unsigned char tempVal = mCalibDb->awb.measure_para_v200.xyDetectionEnable;
            int no = ParseUcharArray(pchild, &tempVal, 1);
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->awb.measure_para_v200.xyDetectionEnable = (tempVal == 0 ? false : true);
        }
        else if ((tagname == CALIB_SENSOR_AWB_YUVDETECTIONENABLE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            unsigned char tempVal = mCalibDb->awb.measure_para_v200.yuvDetectionEnable;
            int no = ParseUcharArray(pchild, &tempVal, 1);
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->awb.measure_para_v200.yuvDetectionEnable = (tempVal == 0 ? false : true);
        }
        else if ((tagname == CALIB_SENSOR_AWB_LSUSEDFORYUVDET)) {
            mCalibDb->awb.measure_para_v200.lsUsedForYuvDetNum = 0;
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorAwbLsForYuvDet)) {
                LOGE("parse error in AWB  (%s)", tagname);
                return (false);
            }
        }
        else if ((tagname == CALIB_SENSOR_AWB_DOWNSCALEMODE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->awb.measure_para_v200.dsMode, 1);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_BLCKMEASUREMODE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->awb.measure_para_v200.blkMeasureMode, 1);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_MEASUREWINDOW)) {
            if (!parseEntrySensorAwbMeasureWindow(pchild->ToElement())) {
                LOGE("parse error in AWB  (%s)", tagname);
                return (false);
            }
        }
        else if ((tagname == CALIB_SENSOR_AWB_MULTIWINDOWENABLE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            unsigned char tempVal = mCalibDb->awb.measure_para_v200.multiwindow_en;
            int no = ParseUcharArray(pchild, &tempVal, 1);
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->awb.measure_para_v200.multiwindow_en = (tempVal == 0 ? false : true);
        }
        else if (tagname == CALIB_SENSOR_AWB_LIMITRANGE) {
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());
                if ((subTagname == CALIB_SENSOR_AWB_LIMITRANGE_Y)
                        && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (subTag.Size() > 0)) {
                    unsigned short tmpValue[2] = { mCalibDb->awb.measure_para_v200.minY, mCalibDb->awb.measure_para_v200.maxY };
                    int no = ParseUshortArray(psubchild, tmpValue, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                    mCalibDb->awb.measure_para_v200.minY = tmpValue[0];
                    mCalibDb->awb.measure_para_v200.maxY = tmpValue[1];
                }
                else if ((subTagname == CALIB_SENSOR_AWB_LIMITRANGE_R)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    unsigned short tmpValue[2] = { mCalibDb->awb.measure_para_v200.minR, mCalibDb->awb.measure_para_v200.maxR };
                    int no = ParseUshortArray(psubchild, tmpValue, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                    mCalibDb->awb.measure_para_v200.minR = tmpValue[0];
                    mCalibDb->awb.measure_para_v200.maxR = tmpValue[1];
                }
                else if ((subTagname == CALIB_SENSOR_AWB_LIMITRANGE_G)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    unsigned short tmpValue[2] = { mCalibDb->awb.measure_para_v200.minG, mCalibDb->awb.measure_para_v200.maxG };
                    int no = ParseUshortArray(psubchild, tmpValue, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                    mCalibDb->awb.measure_para_v200.minG = tmpValue[0];
                    mCalibDb->awb.measure_para_v200.maxG = tmpValue[1];
                }
                else if ((subTagname == CALIB_SENSOR_AWB_LIMITRANGE_B)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    unsigned short tmpValue[2] = { mCalibDb->awb.measure_para_v200.minB, mCalibDb->awb.measure_para_v200.maxB };
                    int no = ParseUshortArray(psubchild, tmpValue, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                    mCalibDb->awb.measure_para_v200.minB = tmpValue[0];
                    mCalibDb->awb.measure_para_v200.maxB = tmpValue[1];
                }
                else {
                    LOGE("unknown subTag %s", subTagname.c_str());
                }
                psubchild = psubchild->NextSibling();
            }
        }
        else if ((tagname == CALIB_SENSOR_AWB_PSEUDOLUMWEIGHT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.measure_para_v200.rgb2tcs_param.pseudoLuminanceWeight, tag.Size(), 6);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_ROTATIONMAT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.measure_para_v200.rgb2tcs_param.rotationMat, tag.Size(), 6);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_MULTIWINDOW)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            //unsigned short tempVal[32];
            int no = ParseUshortArray(pchild, mCalibDb->awb.measure_para_v200.multiwindow[0], tag.Size());
            DCT_ASSERT((no == tag.Size()));
            DCT_ASSERT((no <= CALD_AWB_WINDOW_NUM_MAX * 4));
            //MEMCPY(mCalibDb->awb.measure_para_v200.multiwindow, tempVal, sizeof(short)*no);
        }
        else if (tagname == CALIB_SENSOR_AWB_EXCLUDERANGE) {
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorAwbGlobalsExclude)) {
                LOGE("parse error in AWB globals excluderange (%s)", tagname.c_str());
                return (false);
            }

        }
        else {
            LOGE("parse error in AWB section (unknow tag:%s)", tagname.c_str());
            //return ( false );
        }

        pchild = pchild->NextSibling();
    }

    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();


    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAwbStategyGlobals
(
    const XMLElement*   pelement,
    void*                param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {

        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());
        if ((tagname == CALIB_SENSOR_AWB_WBGAINADJUSTENABLE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            unsigned char tempVal = mCalibDb->awb.stategy_cfg.wbGainAdjustEn;
            int no = ParseUcharArray(pchild, &tempVal, 1);
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->awb.stategy_cfg.wbGainAdjustEn = (tempVal == 0 ? false : true);

        }
        else if ((tagname == CALIB_SENSOR_AWB_WBGAINDAYLIGHTCLIPENABLE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            unsigned char tempVal = mCalibDb->awb.stategy_cfg.wbGainDaylightClipEn;
            int no = ParseUcharArray(pchild, &tempVal, 1);
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->awb.stategy_cfg.wbGainDaylightClipEn = (tempVal == 0 ? false : true);
        }
        else if ((tagname == CALIB_SENSOR_AWB_WBGAINCLIPEANBLE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            unsigned char tempVal = mCalibDb->awb.stategy_cfg.wbGainClipEn;
            int no = ParseUcharArray(pchild, &tempVal, 1);
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->awb.stategy_cfg.wbGainClipEn = (tempVal == 0 ? false : true);
        }
        else if ((tagname == CALIB_SENSOR_AWB_LSFORFIRSTFRAME)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->awb.stategy_cfg.lsForFirstFrame,
                        sizeof(mCalibDb->awb.stategy_cfg.lsForFirstFrame));
        }
        else if ((tagname == CALIB_SENSOR_AWB_UVRANGESMALLENABLE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            unsigned char tempVal = mCalibDb->awb.stategy_cfg.uvRange_small_enable;
            int no = ParseUcharArray(pchild, &tempVal, 1);
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->awb.stategy_cfg.uvRange_small_enable = (tempVal == 0 ? false : true);
        }
        else if ((tagname == CALIB_SENSOR_AWB_CA_ENABLE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            unsigned char tempVal = mCalibDb->awb.stategy_cfg.ca_enable;
            int no = ParseUcharArray(pchild, &tempVal, 1);
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->awb.stategy_cfg.ca_enable = (tempVal == 0 ? false : true);
        }
        else if ((tagname == CALIB_SENSOR_AWB_TOLERANCE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->awb.stategy_cfg.tolerance, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_RUNINTERNAL)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUintArray(pchild, &mCalibDb->awb.stategy_cfg.runInterval, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_MULTIWINDOWMODE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->awb.stategy_cfg.multiwindowMode, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if (tagname == CALIB_SENSOR_AWB_DAMPFACTOR) {
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            autoTabForward();
            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());

                if (subTagname == CALIB_SENSOR_AWB_DFSTEP) {
                    int no = ParseFloatArray(psubchild, &mCalibDb->awb.stategy_cfg.dFStep, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if (subTagname == CALIB_SENSOR_AWB_DFMIN) {
                    int no = ParseFloatArray(psubchild, &mCalibDb->awb.stategy_cfg.dFMin, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if (subTagname == CALIB_SENSOR_AWB_DFMAX) {
                    int no = ParseFloatArray(psubchild, &mCalibDb->awb.stategy_cfg.dFMax, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if (subTagname == CALIB_SENSOR_AWB_LVIIRSIZE) {
                    int no = ParseIntArray(psubchild, &mCalibDb->awb.stategy_cfg.LvIIRsize, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if (subTagname == CALIB_SENSOR_AWB_LVVARTH) {
                    int no = ParseFloatArray(psubchild, &mCalibDb->awb.stategy_cfg.LvVarTh, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                psubchild = psubchild->NextSibling();
            }
            autoTabBackward();
        }
        else if ((tagname == CALIB_SENSOR_AWB_LVMATRIX)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUintArray(pchild, mCalibDb->awb.stategy_cfg.LVMatrix, tag.Size());
            DCT_ASSERT((no <= CALD_AWB_LV_NUM_MAX));
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->awb.stategy_cfg.LV_NUM = no;
        }
        else if ((tagname == CALIB_SENSOR_AWB_LV_THL)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUintArray(pchild, &mCalibDb->awb.stategy_cfg.LV_THL, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_LV_THL2)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUintArray(pchild, &mCalibDb->awb.stategy_cfg.LV_THL2, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_LV_THH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUintArray(pchild, &mCalibDb->awb.stategy_cfg.LV_THH, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_LV_THH2)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUintArray(pchild, &mCalibDb->awb.stategy_cfg.LV_THH2, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_WP_THL)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUintArray(pchild, &mCalibDb->awb.stategy_cfg.WP_THL, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_WP_THH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUintArray(pchild, &mCalibDb->awb.stategy_cfg.WP_THH, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_PRODIS_THL)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->awb.stategy_cfg.proDis_THL, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_PRODIS_THH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->awb.stategy_cfg.proDis_THH, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_PROLV_INDOOR_THL)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUintArray(pchild, &mCalibDb->awb.stategy_cfg.proLV_Indoor_THL, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_PROLV_INDOOR_THH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUintArray(pchild, &mCalibDb->awb.stategy_cfg.proLV_Indoor_THH, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_PROLV_OUTDOOR_THL)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUintArray(pchild, &mCalibDb->awb.stategy_cfg.proLV_Outdoor_THL, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_PROLV_OUTDOOR_THH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUintArray(pchild, &mCalibDb->awb.stategy_cfg.proLV_Outdoor_THH, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_SPATIALGAIN_L)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.stategy_cfg.spatialGain_L, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            DCT_ASSERT((no == 4));
        }
        else if ((tagname == CALIB_SENSOR_AWB_SPATIALGAIN_H)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.stategy_cfg.spatialGain_H, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            DCT_ASSERT((no == 4));
        }
        else if ((tagname == CALIB_SENSOR_AWB_TEMPORALDEFAULTGAIN)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.stategy_cfg.temporalDefaultGain, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            DCT_ASSERT((no == 4));
        }
        else if ((tagname == CALIB_SENSOR_AWB_TEMPORALCALGAINSETSIZE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->awb.stategy_cfg.temporalCalGainSetSize, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            DCT_ASSERT((no <= CALD_AWB_TEMPORAL_GAIN_SIZE_MAX));
        }
        else if ((tagname == CALIB_SENSOR_AWB_TEMPORALGAINSETWEIGHT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, mCalibDb->awb.stategy_cfg.temporalGainSetWeight, tag.Size());
            DCT_ASSERT((no <= CALD_AWB_TEMPORAL_GAIN_SIZE_MAX));
            DCT_ASSERT((no == tag.Size()));
            DCT_ASSERT((no == mCalibDb->awb.stategy_cfg.temporalCalGainSetSize));//check
        }
        else if ((tagname == CALIB_SENSOR_AWB_WPNUMPERCTH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->awb.stategy_cfg.wpNumPercTh, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_TEMPWEIGTH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, mCalibDb->awb.stategy_cfg.tempWeight, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            DCT_ASSERT((no <= CALD_AWB_LV_NUM_MAX));
        }
        else if ((tagname == CALIB_SENSOR_AWB_CA_LACALCFACTOR)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->awb.stategy_cfg.ca_LACalcFactor, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_CA_TARGETGAIN)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.stategy_cfg.ca_targetGain, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if (tagname == CALIB_SENSOR_AWB_SINGLECOLORPROCESS) {
            if (!parseEntrySensorAwbSingleColor(pchild->ToElement())) {
                LOGE("parse error in AWB (%s)", tagname);
                return (false);
            }
        }
        else if ((tagname == CALIB_SENSOR_AWB_CONVERGEDVARTH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->awb.stategy_cfg.convergedVarTh, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_LINERGBG)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.stategy_cfg.lineRgBg, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_LINERGBGPROJCCT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.stategy_cfg.lineRgProjCCT, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if (tagname == CALIB_SENSOR_AWB_XYREGIONSTABLESELECTION) {
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            autoTabForward();
            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());

                if (subTagname == CALIB_SENSOR_AWB_XYREGIONSIZE) {
                    int no = ParseIntArray(psubchild, &mCalibDb->awb.stategy_cfg.xyTypeListSize, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if (subTagname == CALIB_SENSOR_AWB_LVVARTH) {
                    int no = ParseFloatArray(psubchild, &mCalibDb->awb.stategy_cfg.varianceLumaTh, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                psubchild = psubchild->NextSibling();
            }
            autoTabBackward();
        }
        else if (tagname == CALIB_SENSOR_AWB_WBGAINADJUST) {
            if (!parseEntrySensorAwbwbGainAdjust(pchild->ToElement())) {
                LOGE("parse error in AWB (%s)", tagname);
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_AWB_WBGAINDAYLIGHTCLIP) {
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            autoTabForward();
            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());

                if (subTagname == CALIB_SENSOR_AWB_OUTDOORCCTMIN) {
                    int no = ParseFloatArray(psubchild, &mCalibDb->awb.stategy_cfg.cct_clip_cfg.outdoor_cct_min, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                psubchild = psubchild->NextSibling();
            }
            autoTabBackward();
        }
        else if (tagname == CALIB_SENSOR_AWB_WBGAINCLIP) {
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            autoTabForward();
            int cct_num = 0;
            int cri_bound_up_num = 0;
            int cri_bound_low_num = 0;
            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());

                if (subTagname == CALIB_SENSOR_AWB_CCT) {
                    int no = ParseFloatArray(psubchild, mCalibDb->awb.stategy_cfg.cct_clip_cfg.cct, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                    cct_num = no;
                }
                else if (subTagname == CALIB_SENSOR_AWB_CRIBOUNDUP) {
                    int no = ParseFloatArray(psubchild, mCalibDb->awb.stategy_cfg.cct_clip_cfg.cri_bound_up, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                    cri_bound_up_num = no;
                }
                else if (subTagname == CALIB_SENSOR_AWB_CRIBOUNDLOW) {
                    int no = ParseFloatArray(psubchild, mCalibDb->awb.stategy_cfg.cct_clip_cfg.cri_bound_low, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                    cri_bound_low_num = no;
                }
                psubchild = psubchild->NextSibling();
            }
            if((cct_num != cri_bound_up_num) || (cct_num != cri_bound_low_num)) {
                LOGE("para error in AWB section tag:%s  number of cct should be equal to cri_bound_up and cri_bound_low",
                     tagname.c_str());
                return ( false );
            }
            mCalibDb->awb.stategy_cfg.cct_clip_cfg.grid_num = cct_num;
            autoTabBackward();
        }
        else {
            LOGE("parse error in AWB section (unknow tag:%s)", tagname.c_str());
            //return ( false );
        }

        pchild = pchild->NextSibling();
    }

    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();

    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAwbLsForYuvDet
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        const char* value = tag.Value();
        std::string tagname(pchild->ToElement()->Name());
        if ((tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_NAME)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->awb.measure_para_v200.lsUsedForYuvDet[index],
                        sizeof(mCalibDb->awb.measure_para_v200.lsUsedForYuvDet[index]));//check
        }
        pchild = pchild->NextSibling();
    }
    DCT_ASSERT((index <= CALD_AWB_LS_NUM_MAX));
    mCalibDb->awb.measure_para_v200.lsUsedForYuvDetNum++;
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAwbFrameChoose
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());
        if ((tagname == CALIB_SENSOR_AWB_MODE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->awb.measure_para_v200.hdrFrameChooseMode, 1);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_FRAMECHOOSE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->awb.measure_para_v200.hdrFrameChoose, 1);
            DCT_ASSERT((no == tag.Size()));
        }

        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}
bool RkAiqCalibParser::parseEntrySensorAwbFrameChooseV201
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());
        if ((tagname == CALIB_SENSOR_AWB_MODE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->awb.measure_para_v201.hdrFrameChooseMode, 1);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_FRAMECHOOSE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->awb.measure_para_v201.hdrFrameChoose, 1);
            DCT_ASSERT((no == tag.Size()));
        }

        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAwbMeasureWindow
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());
        if ((tagname == CALIB_SENSOR_AWB_MODE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseIntArray(pchild, &mCalibDb->awb.measure_para_v200.measeureWindow.mode, 1);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_RESALL)) {
            mCalibDb->awb.measure_para_v200.measeureWindow.resNum = 0;
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorAwbWindow)) {
                LOGE("parse error in AWB light sources (%s)", tagname);
                return (false);
            }
        }

        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}


bool RkAiqCalibParser::parseEntrySensorAwbWindow
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        const char* value = tag.Value();
        std::string tagname(pchild->ToElement()->Name());
        if ((tagname == CALIB_SENSOR_AWB_RESOLUTION)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->awb.measure_para_v200.measeureWindow.resName[index],
                        sizeof(mCalibDb->awb.measure_para_v200.measeureWindow.resName[index]));
        }
        else if ((tagname == CALIB_SENSOR_AWB_MEASUREWINDOWSIZE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUshortArray(pchild, mCalibDb->awb.measure_para_v200.measeureWindow.window[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
            DCT_ASSERT((no <= 4));
        }
        pchild = pchild->NextSibling();
    }
    DCT_ASSERT((index <= CALD_AWB_RRES_NUM_MAX));
    mCalibDb->awb.measure_para_v200.measeureWindow.resNum++;
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}
bool RkAiqCalibParser::parseEntrySensorAwbwbGainAdjust
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());
        if ((tagname == CALIB_SENSOR_AWB_CTGRIDNUM)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseIntArray(pchild, &mCalibDb->awb.stategy_cfg.cct_lut_cfg[0].ct_grid_num, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_AWB_CTINRANGE)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.stategy_cfg.cct_lut_cfg[0].ct_range, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            DCT_ASSERT((no == 2));
        } else if ((tagname == CALIB_SENSOR_AWB_CRIGRIDNUM)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseIntArray(pchild, &mCalibDb->awb.stategy_cfg.cct_lut_cfg[0].cri_grid_num, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_AWB_CRIINRANGE)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.stategy_cfg.cct_lut_cfg[0].cri_range, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            DCT_ASSERT((no == 2));
        }
        else if ((tagname == CALIB_SENSOR_AWB_LUTALL)) {

            mCalibDb->awb.stategy_cfg.cct_lut_cfg_num = tag.Size();
            DCT_ASSERT((mCalibDb->awb.stategy_cfg.cct_lut_cfg_num <= CALD_AWB_CT_LV_NUM_MAX));
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorAwbCctLutAll)) {
                LOGE("parse error in AWB wbGainAdjust (%s)", tagname);
                return (false);
            }
        }
        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}
bool RkAiqCalibParser::parseEntrySensorAwbCctLutAll
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        const char* value = tag.Value();
        std::string tagname(pchild->ToElement()->Name());
        if ((tagname == CALIB_SENSOR_AWB_LVVALUE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->awb.stategy_cfg.cct_lut_cfg[index].lv, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_CTOUT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.stategy_cfg.cct_lut_cfg[index].ct_lut_out, tag.Size(), 6);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_CRIOUT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.stategy_cfg.cct_lut_cfg[index].cri_lut_out, tag.Size(), 6);
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAwbSingleColor
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());
        if ((tagname == CALIB_SENSOR_AWB_COLORBLOCK)) {
            mCalibDb->awb.stategy_cfg.sSelColorNUM = 0;
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorAwbColBlk)) {
                LOGE("parse error in AWB light sources (%s)", tagname);
                return (false);
            }
        }
        else if ((tagname == CALIB_SENSOR_AWB_LIGHTSOURCEUSEDFORESTIMIATION)) {
            mCalibDb->awb.stategy_cfg.sIllEstNum = 0;
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorAwbLsForEstimation)) {
                LOGE("parse error in AWB light sources (%s)", tagname);
                return (false);
            }
        }
        else if ((tagname == CALIB_SENSOR_AWB_ALPHA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->awb.stategy_cfg.sAlpha, 1);
            DCT_ASSERT((no == tag.Size()));
        }

        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}
bool RkAiqCalibParser::parseEntrySensorAwbColBlk
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        const char* value = tag.Value();
        std::string tagname(pchild->ToElement()->Name());
        if ((tagname == CALIB_SENSOR_AWB_INDEX)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseUshortArray(pchild, &mCalibDb->awb.stategy_cfg.sIndSelColor[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_MEANC)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->awb.stategy_cfg.sMeanCh[0][index], tag.Size(), 6);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_MEANH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->awb.stategy_cfg.sMeanCh[1][index], tag.Size(), 6);
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }
    DCT_ASSERT((index <= CALD_AWB_SGC_NUM_MAX));
    mCalibDb->awb.stategy_cfg.sSelColorNUM++;
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}
bool RkAiqCalibParser::parseEntrySensorAwbLsForEstimation
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        const char* value = tag.Value();
        std::string tagname(pchild->ToElement()->Name());
        if ((tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_NAME)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->awb.stategy_cfg.sNameIllEst[index],
                        sizeof(mCalibDb->awb.stategy_cfg.sNameIllEst[index]));
        }
        else if ((tagname == CALIB_SENSOR_AWB_RGAIN)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->awb.stategy_cfg.srGain[index], tag.Size(), 6);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_BGAIN)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->awb.stategy_cfg.sbGain[index], tag.Size(), 6);
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }
    DCT_ASSERT((index <= CALD_AWB_LS_NUM_MAX));
    mCalibDb->awb.stategy_cfg.sIllEstNum++;
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAwbGlobalsExclude
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string Tagname(pchild->ToElement()->Name());

        if ((Tagname == CALIB_SENSOR_AWB_EXCLUDERANGE_DOMAIN)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->awb.measure_para_v200.excludeWpRange[index].domain, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AWB_EXCLUDE_MODE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->awb.measure_para_v200.excludeWpRange[index].mode, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AWB_EXCLUDE_WINDOW)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int xuyv[4] = { mCalibDb->awb.measure_para_v200.excludeWpRange[index].xu[0],
                            mCalibDb->awb.measure_para_v200.excludeWpRange[index].xu[1],
                            mCalibDb->awb.measure_para_v200.excludeWpRange[index].yv[0],
                            mCalibDb->awb.measure_para_v200.excludeWpRange[index].yv[1]
                          };
            int no = ParseIntArray(pchild, xuyv, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->awb.measure_para_v200.excludeWpRange[index].xu[0] = xuyv[0];
            mCalibDb->awb.measure_para_v200.excludeWpRange[index].xu[1] = xuyv[1];
            mCalibDb->awb.measure_para_v200.excludeWpRange[index].yv[0] = xuyv[2];
            mCalibDb->awb.measure_para_v200.excludeWpRange[index].yv[1] = xuyv[3];

        }
        pchild = pchild->NextSibling();
    }
    DCT_ASSERT((index <= CALD_AWB_EXCRANGE_NUM_MAX));
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAwbMeasureLightSourcesV200
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        const char* value = tag.Value();
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_NAME)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->awb.measure_para_v200.lightName[index],
                        sizeof(mCalibDb->awb.measure_para_v200.lightName[index]));
        }
        else if ((tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_LIGHTUREGION)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.measure_para_v200.uvRange_param[index].pu_region, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_LIGHTVREGION)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.measure_para_v200.uvRange_param[index].pv_region, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_SMALLUREGION)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.measure_para_v200.uvRange_param_small[index].pu_region, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_SMALLVREGION)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.measure_para_v200.uvRange_param_small[index].pv_region, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if (tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_LIGHTXYREGION) {
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            autoTabForward();
            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());

                if ((subTagname == CALIB_SENSOR_AWB_LIGHTSOURCES_NORMAL)
                        && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (subTag.Size() > 0)) {
                    float tempVal[4] = {mCalibDb->awb.measure_para_v200.xyRangeLight[index].NorrangeX[0],
                                        mCalibDb->awb.measure_para_v200.xyRangeLight[index].NorrangeX[1],
                                        mCalibDb->awb.measure_para_v200.xyRangeLight[index].NorrangeY[0],
                                        mCalibDb->awb.measure_para_v200.xyRangeLight[index].NorrangeY[1]
                                       };
                    int no = ParseFloatArray(psubchild, tempVal, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                    mCalibDb->awb.measure_para_v200.xyRangeLight[index].NorrangeX[0] = tempVal[0];
                    mCalibDb->awb.measure_para_v200.xyRangeLight[index].NorrangeX[1] = tempVal[1];
                    mCalibDb->awb.measure_para_v200.xyRangeLight[index].NorrangeY[0] = tempVal[2];
                    mCalibDb->awb.measure_para_v200.xyRangeLight[index].NorrangeY[1] = tempVal[3];
                }
                else if ((subTagname == CALIB_SENSOR_AWB_LIGHTSOURCES_BIG)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    float tempVal[4] = { mCalibDb->awb.measure_para_v200.xyRangeLight[index].SperangeX[0],
                                         mCalibDb->awb.measure_para_v200.xyRangeLight[index].SperangeX[1],
                                         mCalibDb->awb.measure_para_v200.xyRangeLight[index].SperangeY[0],
                                         mCalibDb->awb.measure_para_v200.xyRangeLight[index].SperangeY[1]
                                       };
                    int no = ParseFloatArray(psubchild, tempVal, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                    mCalibDb->awb.measure_para_v200.xyRangeLight[index].SperangeX[0] = tempVal[0];
                    mCalibDb->awb.measure_para_v200.xyRangeLight[index].SperangeX[1] = tempVal[1];
                    mCalibDb->awb.measure_para_v200.xyRangeLight[index].SperangeY[0] = tempVal[2];
                    mCalibDb->awb.measure_para_v200.xyRangeLight[index].SperangeY[1] = tempVal[3];

                }
                else if ((subTagname == CALIB_SENSOR_AWB_LIGHTSOURCES_SMALL)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    float tempVal[4] = { mCalibDb->awb.measure_para_v200.xyRangeLight[index].SmalrangeX[0],
                                         mCalibDb->awb.measure_para_v200.xyRangeLight[index].SmalrangeX[1],
                                         mCalibDb->awb.measure_para_v200.xyRangeLight[index].SmalrangeY[0],
                                         mCalibDb->awb.measure_para_v200.xyRangeLight[index].SmalrangeY[1]
                                       };
                    int no = ParseFloatArray(psubchild, tempVal, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                    mCalibDb->awb.measure_para_v200.xyRangeLight[index].SmalrangeX[0] = tempVal[0];
                    mCalibDb->awb.measure_para_v200.xyRangeLight[index].SmalrangeX[1] = tempVal[1];
                    mCalibDb->awb.measure_para_v200.xyRangeLight[index].SmalrangeY[0] = tempVal[2];
                    mCalibDb->awb.measure_para_v200.xyRangeLight[index].SmalrangeY[1] = tempVal[3];
                }
                psubchild = psubchild->NextSibling();
            }
            autoTabBackward();
        }
        else if (tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_YUVREGION) {
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            autoTabForward();
            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());

                if ((subTagname == CALIB_SENSOR_AWB_LIGHTSOURCES_K2SET)
                        && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (subTag.Size() > 0)) {
                    int no = ParseIntArray(psubchild, &mCalibDb->awb.measure_para_v200.yuv3DRange_param[index].slope_inv_neg_uv, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AWB_LIGHTSOURCES_B0SET)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseIntArray(psubchild, &mCalibDb->awb.measure_para_v200.yuv3DRange_param[index].b_uv, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AWB_LIGHTSOURCES_K3SET)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseIntArray(psubchild, &mCalibDb->awb.measure_para_v200.yuv3DRange_param[index].slope_factor_uv, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AWB_LIGHTSOURCES_K_YDISSET)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseIntArray(psubchild, &mCalibDb->awb.measure_para_v200.yuv3DRange_param[index].slope_ydis, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AWB_LIGHTSOURCES_B_YDISSET)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseIntArray(psubchild, &mCalibDb->awb.measure_para_v200.yuv3DRange_param[index].b_ydis, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AWB_LIGHTSOURCES_UREFSET)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseUcharArray(psubchild, &mCalibDb->awb.measure_para_v200.yuv3DRange_param[index].ref_u, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AWB_LIGHTSOURCES_VREFSET)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseUcharArray(psubchild, &mCalibDb->awb.measure_para_v200.yuv3DRange_param[index].ref_v, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AWB_LIGHTSOURCES_DISSET)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseUshortArray(psubchild, mCalibDb->awb.measure_para_v200.yuv3DRange_param[index].dis, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AWB_LIGHTSOURCES_THSET)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseUcharArray(psubchild, mCalibDb->awb.measure_para_v200.yuv3DRange_param[index].th, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));

                }
                psubchild = psubchild->NextSibling();
            }
            autoTabBackward();
        }
        else {
            LOGE("UNKNOWN tag: %s", tagname.c_str());
        }

        pchild = pchild->NextSibling();
    }
    DCT_ASSERT((index <= CALD_AWB_LS_NUM_MAX));
    mCalibDb->awb.measure_para_v200.lightNum++;
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}
bool RkAiqCalibParser::parseEntrySensorAwbStategyLightSources
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        const char* value = tag.Value();
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_NAME)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->awb.stategy_cfg.awb_light_info[index].light_name,
                        sizeof(mCalibDb->awb.stategy_cfg.awb_light_info[index].light_name));
        }
        else if ((tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_DOORTYPE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->awb.stategy_cfg.awb_light_info[index].doorType, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_STANDARDGAINVALUE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.stategy_cfg.awb_light_info[index].standardGainValue, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_STAWEIGTHSET)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, mCalibDb->awb.stategy_cfg.awb_light_info[index].staWeight, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_SPATIALGAIN_LV_THSET)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            unsigned int tempVal[2] = { mCalibDb->awb.stategy_cfg.awb_light_info[index].spatialGain_LV_THL, mCalibDb->awb.stategy_cfg.awb_light_info[index].spatialGain_LV_THH};
            int no = ParseUintArray(pchild, tempVal, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->awb.stategy_cfg.awb_light_info[index].spatialGain_LV_THL = tempVal[0];
            mCalibDb->awb.stategy_cfg.awb_light_info[index].spatialGain_LV_THH = tempVal[1];
        }
        else if ((tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_XYTYPE2ENABLE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->awb.stategy_cfg.awb_light_info[index].xyType2Enable, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_WEIGHTCURVE_RATIO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.stategy_cfg.awb_light_info[index].weightCurve_ratio, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_WEIGHTCURVE_WEIGHT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.stategy_cfg.awb_light_info[index].weightCurve_weight, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else {
            LOGE("UNKNOWN tag: %s", tagname.c_str());
        }

        pchild = pchild->NextSibling();
    }
    DCT_ASSERT((index <= CALD_AWB_LS_NUM_MAX));
    mCalibDb->awb.stategy_cfg.lightNum++;
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAwbMeasureParaV201
(
    const XMLElement*   pelement,
    void*                param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if (tagname == CALIB_SENSOR_AWB_GLOBALS_TAG) {
            if (!parseEntrySensorAwbMeasureGlobalsV201(pchild->ToElement())) {
                LOGE("parse error in AWB globals (%s)", tagname);
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_AWB_LIGHTSOURCES) {
            mCalibDb->awb.measure_para_v201.lightNum = 0;
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorAwbMeasureLightSourcesV201)) {
                LOGE("parse error in AWB light sources (%s)", tagname);
                return (false);
            }
        }
        else {
            LOGE("parse error in AWB section (unknow tag:%s)", tagname.c_str());
            return (false);
        }

        pchild = pchild->NextSibling();
    }

    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();

    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAwbMeasureGlobalsV201
(
    const XMLElement*   pelement,
    void*                param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_AWB_HDRFRAMECHOOSE)) {
            if (!parseEntrySensorAwbFrameChooseV201(pchild->ToElement())) {
                LOGE("parse error in AWB (%s)", tagname);
                return (false);
            }
        }
        else if ((tagname == CALIB_SENSOR_AWB_LSCBYPASSENABLE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            unsigned char tempVal = mCalibDb->awb.measure_para_v201.lscBypEnable;
            int no = ParseUcharArray(pchild, &tempVal, 1);
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->awb.measure_para_v201.lscBypEnable = (tempVal == 0 ? false : true);
        }
        else if ((tagname == CALIB_SENSOR_AWB_UVDETECTIONENABLE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            unsigned char tempVal = mCalibDb->awb.measure_para_v201.uvDetectionEnable;
            int no = ParseUcharArray(pchild, &tempVal, 1);
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->awb.measure_para_v201.uvDetectionEnable = (tempVal == 0 ? false : true);
        }
        else if ((tagname == CALIB_SENSOR_AWB_XYDETECTIONENABLE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            unsigned char tempVal = mCalibDb->awb.measure_para_v201.xyDetectionEnable;
            int no = ParseUcharArray(pchild, &tempVal, 1);
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->awb.measure_para_v201.xyDetectionEnable = (tempVal == 0 ? false : true);
        }
        else if ((tagname == CALIB_SENSOR_AWB_YUVDETECTIONENABLE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            unsigned char tempVal = mCalibDb->awb.measure_para_v201.yuvDetectionEnable;
            int no = ParseUcharArray(pchild, &tempVal, 1);
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->awb.measure_para_v201.yuvDetectionEnable = (tempVal == 0 ? false : true);
        }
        else if ((tagname == CALIB_SENSOR_AWB_LSUSEDFORYUVDET)) {
            mCalibDb->awb.measure_para_v201.lsUsedForYuvDetNum = 0;
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorAwbLsForYuvDetV201)) {
                LOGE("parse error in AWB  (%s)", tagname);
                return (false);
            }
        }
        else if ((tagname == CALIB_SENSOR_AWB_WPDIFFWEIENABLE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            unsigned char tempVal = mCalibDb->awb.measure_para_v201.wpDiffWeiEnable;
            int no = ParseUcharArray(pchild, &tempVal, 1);
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->awb.measure_para_v201.wpDiffWeiEnable = (tempVal == 0 ? false : true);
        }
        else if ((tagname == CALIB_SENSOR_AWB_WPDIFFBLKWEIENABLE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            unsigned char tempVal = mCalibDb->awb.measure_para_v201.blkWeightEnable;
            int no = ParseUcharArray(pchild, &tempVal, 1);
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->awb.measure_para_v201.blkWeightEnable = (tempVal == 0 ? false : true);
        }
        else if ((tagname == CALIB_SENSOR_AWB_BLKSTATISTICSENABLE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            unsigned char tempVal = mCalibDb->awb.measure_para_v201.blkStatisticsEnable;
            int no = ParseUcharArray(pchild, &tempVal, 1);
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->awb.measure_para_v201.blkStatisticsEnable = (tempVal == 0 ? false : true);
        }
        else if ((tagname == CALIB_SENSOR_AWB_DOWNSCALEMODE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->awb.measure_para_v201.dsMode, 1);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_BLCKMEASUREMODE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->awb.measure_para_v201.blkMeasureMode, 1);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_MULTIWINDOWENABLE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            unsigned char tempVal = mCalibDb->awb.measure_para_v201.multiwindow_en;
            int no = ParseUcharArray(pchild, &tempVal, 1);
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->awb.measure_para_v201.multiwindow_en = (tempVal == 0 ? false : true);
        }
        else if ((tagname == CALIB_SENSOR_AWB_MEASUREWINDOW)) {
            if (!parseEntrySensorAwbMeasureWindowV201(pchild->ToElement())) {
                LOGE("parse error in AWB  (%s)", tagname);
                return (false);
            }
        }

        else if (tagname == CALIB_SENSOR_AWB_LIMITRANGE) {
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());
                if ((subTagname == CALIB_SENSOR_AWB_LIMITRANGE_Y)
                        && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (subTag.Size() > 0)) {
                    unsigned short tmpValue[2] = { mCalibDb->awb.measure_para_v201.minY, mCalibDb->awb.measure_para_v201.maxY };
                    int no = ParseUshortArray(psubchild, tmpValue, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                    mCalibDb->awb.measure_para_v201.minY = tmpValue[0];
                    mCalibDb->awb.measure_para_v201.maxY = tmpValue[1];
                }
                else if ((subTagname == CALIB_SENSOR_AWB_LIMITRANGE_R)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    unsigned short tmpValue[2] = { mCalibDb->awb.measure_para_v201.minR, mCalibDb->awb.measure_para_v201.maxR };
                    int no = ParseUshortArray(psubchild, tmpValue, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                    mCalibDb->awb.measure_para_v201.minR = tmpValue[0];
                    mCalibDb->awb.measure_para_v201.maxR = tmpValue[1];
                }
                else if ((subTagname == CALIB_SENSOR_AWB_LIMITRANGE_G)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    unsigned short tmpValue[2] = { mCalibDb->awb.measure_para_v201.minG, mCalibDb->awb.measure_para_v201.maxG };
                    int no = ParseUshortArray(psubchild, tmpValue, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                    mCalibDb->awb.measure_para_v201.minG = tmpValue[0];
                    mCalibDb->awb.measure_para_v201.maxG = tmpValue[1];
                }
                else if ((subTagname == CALIB_SENSOR_AWB_LIMITRANGE_B)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    unsigned short tmpValue[2] = { mCalibDb->awb.measure_para_v201.minB, mCalibDb->awb.measure_para_v201.maxB };
                    int no = ParseUshortArray(psubchild, tmpValue, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                    mCalibDb->awb.measure_para_v201.minB = tmpValue[0];
                    mCalibDb->awb.measure_para_v201.maxB = tmpValue[1];
                }
                else {
                    LOGE("unknown subTag %s", subTagname.c_str());
                }
                psubchild = psubchild->NextSibling();
            }
        }
        else if ((tagname == CALIB_SENSOR_AWB_PSEUDOLUMWEIGHT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.measure_para_v201.rgb2tcs_param.pseudoLuminanceWeight, tag.Size(), 6);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_ROTATIONMAT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.measure_para_v201.rgb2tcs_param.rotationMat, tag.Size(), 6);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_RGB2ROTATIONYUVMAT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.measure_para_v201.rgb2RYuv_matrix, tag.Size(), 6);
            DCT_ASSERT((no == tag.Size()));
            DCT_ASSERT((no == 16));
        }
        else if ((tagname == CALIB_SENSOR_AWB_MULTIWINDOW)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            //unsigned short tempVal[32];
            int no = ParseUshortArray(pchild, mCalibDb->awb.measure_para_v201.multiwindow[0], tag.Size());
            DCT_ASSERT((no == tag.Size()));
            DCT_ASSERT((no <= CALD_AWB_WINDOW_NUM_MAX * 4));
            //MEMCPY(mCalibDb->awb.measure_para_v201.multiwindow, tempVal, sizeof(short)*no);
        }
        else if (tagname == CALIB_SENSOR_AWB_EXCLUDERANGE) {
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorAwbGlobalsExcludeV201)) {
                LOGE("parse error in AWB globals excluderange (%s)", tagname.c_str());
                return (false);
            }

        }
        else if (tagname == CALIB_SENSOR_AWB_WPDIFFWEIGHT) {
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());
                if (subTagname == CALIB_SENSOR_AWB_WPDIFFWEIENABLETH) {
                    const XMLNode* psubsubchild = psubchild->ToElement()->FirstChild();
                    while (psubsubchild) {
                        XmlTag subsubTag = XmlTag(psubsubchild->ToElement());
                        std::string subsubTagname(psubsubchild->ToElement()->Name());
                        if (subsubTagname == CALIB_SENSOR_AWB_WPNOTH) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->awb.measure_para_v201.wpDiffNoTh, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if (subsubTagname == CALIB_SENSOR_AWB_LVVALUETH) {
                            int no = ParseUintArray(psubsubchild, &mCalibDb->awb.measure_para_v201.wpDiffLvValueTh, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        psubsubchild = psubsubchild->NextSibling();
                    }
                }
                else if ((subTagname == CALIB_SENSOR_AWB_WPDIFFWEI_Y)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseUcharArray(psubchild, mCalibDb->awb.measure_para_v201.wpDiffwei_y, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AWB_PERFECTBINCONF)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseUcharArray(psubchild, mCalibDb->awb.measure_para_v201.perfectBin, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AWB_WPDIFFWEILVTH)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseFloatArray(psubchild, mCalibDb->awb.measure_para_v201.wpDiffweiSet_w_LvValueTh, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AWB_WPDIFFWEIRATIOTH)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseFloatArray(psubchild, mCalibDb->awb.measure_para_v201.wpDiffWeiRatioTh, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if (subTagname == CALIB_SENSOR_AWB_WPDIFFWEI_W_HIGHLV) {
                    const XMLNode* psubsubchild = psubchild->ToElement()->FirstChild();
                    while (psubsubchild) {
                        XmlTag subsubTag = XmlTag(psubsubchild->ToElement());
                        std::string subsubTagname(psubsubchild->ToElement()->Name());

                        if (subsubTagname == CALIB_SENSOR_AWB_WPRATIO1) {
                            int no = ParseFloatArray(psubsubchild, mCalibDb->awb.measure_para_v201.wpDiffweiSet_w_HigLV[0], subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if (subsubTagname == CALIB_SENSOR_AWB_WPRATIO2) {
                            int no = ParseFloatArray(psubsubchild, mCalibDb->awb.measure_para_v201.wpDiffweiSet_w_HigLV[1], subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if (subsubTagname == CALIB_SENSOR_AWB_WPRATIO3) {
                            int no = ParseFloatArray(psubsubchild, mCalibDb->awb.measure_para_v201.wpDiffweiSet_w_HigLV[2], subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        psubsubchild = psubsubchild->NextSibling();
                    }
                }
                else if (subTagname == CALIB_SENSOR_AWB_WPDIFFWEI_W_LOWLV) {
                    const XMLNode* psubsubchild = psubchild->ToElement()->FirstChild();
                    while (psubsubchild) {
                        XmlTag subsubsubTag = XmlTag(psubsubchild->ToElement());
                        std::string subsubTagname(psubsubchild->ToElement()->Name());

                        if (subsubTagname == CALIB_SENSOR_AWB_WPRATIO1) {
                            int no = ParseFloatArray(psubsubchild, mCalibDb->awb.measure_para_v201.wpDiffweiSet_w_LowLV[0], subsubsubTag.Size());
                            DCT_ASSERT((no == subsubsubTag.Size()));
                        }
                        else if (subsubTagname == CALIB_SENSOR_AWB_WPRATIO2) {
                            int no = ParseFloatArray(psubsubchild, mCalibDb->awb.measure_para_v201.wpDiffweiSet_w_LowLV[1], subsubsubTag.Size());
                            DCT_ASSERT((no == subsubsubTag.Size()));
                        }
                        else if (subsubTagname == CALIB_SENSOR_AWB_WPRATIO3) {
                            int no = ParseFloatArray(psubsubchild, mCalibDb->awb.measure_para_v201.wpDiffweiSet_w_LowLV[2], subsubsubTag.Size());
                            DCT_ASSERT((no == subsubsubTag.Size()));
                        }
                        psubsubchild = psubsubchild->NextSibling();
                    }
                }
                else {
                    LOGE("unknown tag %s", subTagname.c_str());
                }
                psubchild = psubchild->NextSibling();
            }
        }
        else if ((tagname == CALIB_SENSOR_AWB_WPDIFFBLKWEIGHT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUshortArray(pchild, mCalibDb->awb.measure_para_v201.blkWeight, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            DCT_ASSERT((no == CALD_AWB_GRID_NUM_TOTAL));
        }
        else {
            LOGE("parse error in AWB section (unknow tag:%s)", tagname.c_str());
            //return ( false );
        }

        pchild = pchild->NextSibling();
    }

    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();


    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAwbMeasureLightSourcesV201
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        const char* value = tag.Value();
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_NAME)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->awb.measure_para_v201.lightName[index],
                        sizeof(mCalibDb->awb.measure_para_v201.lightName[index]));
        }
        else if ((tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_LIGHTUREGION)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.measure_para_v201.uvRange_param[index].pu_region, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_LIGHTVREGION)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.measure_para_v201.uvRange_param[index].pv_region, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_SMALLUREGION)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.measure_para_v201.uvRange_param_small[index].pu_region, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_SMALLVREGION)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.measure_para_v201.uvRange_param_small[index].pv_region, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if (tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_LIGHTXYREGION) {
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            autoTabForward();
            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());

                if ((subTagname == CALIB_SENSOR_AWB_LIGHTSOURCES_NORMAL)
                        && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (subTag.Size() > 0)) {
                    float tempVal[4] = {mCalibDb->awb.measure_para_v201.xyRangeLight[index].NorrangeX[0],
                                        mCalibDb->awb.measure_para_v201.xyRangeLight[index].NorrangeX[1],
                                        mCalibDb->awb.measure_para_v201.xyRangeLight[index].NorrangeY[0],
                                        mCalibDb->awb.measure_para_v201.xyRangeLight[index].NorrangeY[1]
                                       };
                    int no = ParseFloatArray(psubchild, tempVal, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                    mCalibDb->awb.measure_para_v201.xyRangeLight[index].NorrangeX[0] = tempVal[0];
                    mCalibDb->awb.measure_para_v201.xyRangeLight[index].NorrangeX[1] = tempVal[1];
                    mCalibDb->awb.measure_para_v201.xyRangeLight[index].NorrangeY[0] = tempVal[2];
                    mCalibDb->awb.measure_para_v201.xyRangeLight[index].NorrangeY[1] = tempVal[3];
                }
                else if ((subTagname == CALIB_SENSOR_AWB_LIGHTSOURCES_BIG)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    float tempVal[4] = { mCalibDb->awb.measure_para_v201.xyRangeLight[index].SperangeX[0],
                                         mCalibDb->awb.measure_para_v201.xyRangeLight[index].SperangeX[1],
                                         mCalibDb->awb.measure_para_v201.xyRangeLight[index].SperangeY[0],
                                         mCalibDb->awb.measure_para_v201.xyRangeLight[index].SperangeY[1]
                                       };
                    int no = ParseFloatArray(psubchild, tempVal, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                    mCalibDb->awb.measure_para_v201.xyRangeLight[index].SperangeX[0] = tempVal[0];
                    mCalibDb->awb.measure_para_v201.xyRangeLight[index].SperangeX[1] = tempVal[1];
                    mCalibDb->awb.measure_para_v201.xyRangeLight[index].SperangeY[0] = tempVal[2];
                    mCalibDb->awb.measure_para_v201.xyRangeLight[index].SperangeY[1] = tempVal[3];

                }
                else if ((subTagname == CALIB_SENSOR_AWB_LIGHTSOURCES_SMALL)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    float tempVal[4] = { mCalibDb->awb.measure_para_v201.xyRangeLight[index].SmalrangeX[0],
                                         mCalibDb->awb.measure_para_v201.xyRangeLight[index].SmalrangeX[1],
                                         mCalibDb->awb.measure_para_v201.xyRangeLight[index].SmalrangeY[0],
                                         mCalibDb->awb.measure_para_v201.xyRangeLight[index].SmalrangeY[1]
                                       };
                    int no = ParseFloatArray(psubchild, tempVal, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                    mCalibDb->awb.measure_para_v201.xyRangeLight[index].SmalrangeX[0] = tempVal[0];
                    mCalibDb->awb.measure_para_v201.xyRangeLight[index].SmalrangeX[1] = tempVal[1];
                    mCalibDb->awb.measure_para_v201.xyRangeLight[index].SmalrangeY[0] = tempVal[2];
                    mCalibDb->awb.measure_para_v201.xyRangeLight[index].SmalrangeY[1] = tempVal[3];
                }
                psubchild = psubchild->NextSibling();
            }
            autoTabBackward();
        }
        else if (tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_RTYUVREGION) {
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            autoTabForward();
            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());

                if ((subTagname == CALIB_SENSOR_AWB_LIGHTSOURCES_THCURVE_U_SET)
                        && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (subTag.Size() > 0)) {
                    int no = ParseFloatArray(psubchild, mCalibDb->awb.measure_para_v201.yuv3D2Range_param[index].thcurve_u, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AWB_LIGHTSOURCES_THCURVE_TH_SET)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseFloatArray(psubchild, mCalibDb->awb.measure_para_v201.yuv3D2Range_param[index].thcure_th, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AWB_LIGHTSOURCES_LINEVECTOR)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseFloatArray(psubchild, mCalibDb->awb.measure_para_v201.yuv3D2Range_param[index].line, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                psubchild = psubchild->NextSibling();
            }
            autoTabBackward();
        }
        else {
            LOGE("UNKNOWN tag: %s", tagname.c_str());
        }

        pchild = pchild->NextSibling();
    }
    DCT_ASSERT((index <= CALD_AWB_LS_NUM_MAX));
    mCalibDb->awb.measure_para_v201.lightNum++;
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}
bool RkAiqCalibParser::parseEntrySensorAwbLsForYuvDetV201
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        const char* value = tag.Value();
        std::string tagname(pchild->ToElement()->Name());
        if ((tagname == CALIB_SENSOR_AWB_LIGHTSOURCES_NAME)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->awb.measure_para_v201.lsUsedForYuvDet[index],
                        sizeof(mCalibDb->awb.measure_para_v201.lsUsedForYuvDet[index]));//check
        }
        pchild = pchild->NextSibling();
    }
    DCT_ASSERT((index <= CALD_AWB_LS_NUM_MAX));
    mCalibDb->awb.measure_para_v201.lsUsedForYuvDetNum++;
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAwbMeasureWindowV201
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());
        if ((tagname == CALIB_SENSOR_AWB_MODE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseIntArray(pchild, &mCalibDb->awb.measure_para_v201.measeureWindow.mode, 1);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_AWB_RESALL)) {
            mCalibDb->awb.measure_para_v201.measeureWindow.resNum = 0;
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorAwbWindowV201)) {
                LOGE("parse error in AWB light sources (%s)", tagname);
                return (false);
            }
        }

        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}


bool RkAiqCalibParser::parseEntrySensorAwbWindowV201
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        const char* value = tag.Value();
        std::string tagname(pchild->ToElement()->Name());
        if ((tagname == CALIB_SENSOR_AWB_RESOLUTION)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->awb.measure_para_v201.measeureWindow.resName[index],
                        sizeof(mCalibDb->awb.measure_para_v201.measeureWindow.resName[index]));
        }
        else if ((tagname == CALIB_SENSOR_AWB_MEASUREWINDOWSIZE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUshortArray(pchild, mCalibDb->awb.measure_para_v201.measeureWindow.window[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
            DCT_ASSERT((no <= 4));
        }
        pchild = pchild->NextSibling();
    }
    DCT_ASSERT((index <= CALD_AWB_RRES_NUM_MAX));
    mCalibDb->awb.measure_para_v201.measeureWindow.resNum++;
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAwbGlobalsExcludeV201
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string Tagname(pchild->ToElement()->Name());

        if ((Tagname == CALIB_SENSOR_AWB_EXCLUDERANGE_DOMAIN)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->awb.measure_para_v201.excludeWpRange[index].domain, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AWB_EXCLUDE_MODE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->awb.measure_para_v201.excludeWpRange[index].mode, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AWB_EXCLUDE_WINDOW)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int xuyv[4] = { mCalibDb->awb.measure_para_v201.excludeWpRange[index].xu[0],
                            mCalibDb->awb.measure_para_v201.excludeWpRange[index].xu[1],
                            mCalibDb->awb.measure_para_v201.excludeWpRange[index].yv[0],
                            mCalibDb->awb.measure_para_v201.excludeWpRange[index].yv[1]
                          };
            int no = ParseIntArray(pchild, xuyv, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->awb.measure_para_v201.excludeWpRange[index].xu[0] = xuyv[0];
            mCalibDb->awb.measure_para_v201.excludeWpRange[index].xu[1] = xuyv[1];
            mCalibDb->awb.measure_para_v201.excludeWpRange[index].yv[0] = xuyv[2];
            mCalibDb->awb.measure_para_v201.excludeWpRange[index].yv[1] = xuyv[3];

        }
        pchild = pchild->NextSibling();
    }
    DCT_ASSERT((index <= CALD_AWB_EXCRANGE_NUM_MAX));
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAwbRemosaicPara
(
    const XMLElement*   pelement,
    void*                param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_AWB_REMOSAICPARA_ENABLE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            unsigned char tempVal = mCalibDb->awb.remosaic_cfg.enable;
            int no = ParseUcharArray(pchild, &tempVal, 1);
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->awb.remosaic_cfg.enable = (tempVal == 0 ? false : true);
        }
        else if ((tagname == CALIB_SENSOR_AWB_REMOSAICPARA_WBGAIN)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->awb.remosaic_cfg.sensor_awb_gain, tag.Size(), 6);
            DCT_ASSERT((no == tag.Size()));
        }
        else {
            LOGE("parse error in AWB section (unknow tag:%s)", tagname.c_str());
            //return ( false );
        }

        pchild = pchild->NextSibling();
    }

    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();


    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAec
(
    const XMLElement*   pelement,
    void*                param
) {
    (void)param;

    LOGI("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

#ifdef DEBUG_LOG
        LOGE("%s(%d): tagname: %s\n", __FUNCTION__, __LINE__, tagname);
#endif

        if (tagname == CALIB_SENSOR_AEC_ENABLE
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            ParseUcharArray(pchild, &mCalibDb->aec.CommCtrl.enable, tag.Size());
        }
        else if(tagname == CALIB_SENSOR_AEC_HISTSTATSMODE
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            char* value = Toupper(tag.Value());
            char* value2 = Toupper(tag.Value());
            char* value3 = value2;
            int i = 0;
            while (*value != '\0') {
                if (*value == 'R') {
                    *value2++ = 'R';
                    i++;
                }
                else if (*value == 'G') {
                    *value2++ = 'G';
                    i++;
                }
                else if (*value == 'B') {
                    *value2++ = 'B';
                    i++;
                }
                else if (*value == 'Y') {
                    *value2++ = 'Y';
                    i++;
                }
                value++;
            }
            *value2 = '\0';
            std::string s_value(value3);
#ifdef DEBUG_LOG
            LOGE("%s(%d): value: %s\n", __FUNCTION__, __LINE__, s_value);
#endif
            if (xmlParseReadWrite == XML_PARSER_READ)
            {
                if (s_value == CALIB_SENSOR_AEC_HISTSTATSMODE_R) {
                    mCalibDb->aec.CommCtrl.HistStatsMode = CAM_HIST_MODE_R;
                }
                else if (s_value == CALIB_SENSOR_AEC_HISTSTATSMODE_G) {
                    mCalibDb->aec.CommCtrl.HistStatsMode = CAM_HIST_MODE_G;
                }
                else if (s_value == CALIB_SENSOR_AEC_HISTSTATSMODE_B) {
                    mCalibDb->aec.CommCtrl.HistStatsMode = CAM_HIST_MODE_B;
                }
                else if (s_value == CALIB_SENSOR_AEC_HISTSTATSMODE_RGB) {
                    mCalibDb->aec.CommCtrl.HistStatsMode = CAM_HIST_MODE_RGB_COMBINED;
                }
                else if (s_value == CALIB_SENSOR_AEC_HISTSTATSMODE_Y) {
                    mCalibDb->aec.CommCtrl.HistStatsMode = CAM_HIST_MODE_Y;
                }
                else {
                    mCalibDb->aec.CommCtrl.HistStatsMode = CAM_HIST_MODE_INVALID;
                    LOGE("%s(%d): invalid AEC HistStatsMode = %s end\n", __FUNCTION__, __LINE__, s_value);
                }
            }
            else if (xmlParseReadWrite == XML_PARSER_WRITE)
            {
                XMLNode *pNode = (XMLNode*)pchild;
                switch (mCalibDb->aec.CommCtrl.HistStatsMode)
                {
                case CAM_HIST_MODE_R:
                    pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_HISTSTATSMODE_R);
                    break;
                case CAM_HIST_MODE_G:
                    pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_HISTSTATSMODE_G);
                    break;
                case CAM_HIST_MODE_B:
                    pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_HISTSTATSMODE_B);
                    break;
                case CAM_HIST_MODE_RGB_COMBINED:
                    pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_HISTSTATSMODE_RGB);
                    break;
                case CAM_HIST_MODE_Y:
                    pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_HISTSTATSMODE_Y);
                    break;
                default:
                    pNode->FirstChild()->SetValue("INVALID");
                    LOGE("%s(%d): (XML Write)invalid AEC HistStatsMode = %d end\n", __FUNCTION__, __LINE__, mCalibDb->aec.CommCtrl.HistStatsMode);
                    break;
                }
            }

        }
        else if (tagname == CALIB_SENSOR_AEC_RAWSTATSMODE
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            char* value = Toupper(tag.Value());
            char* value2 = Toupper(tag.Value());
            char* value3 = value2;
            int i = 0;
            while (*value != '\0') {
                if (*value == 'R') {
                    *value2++ = 'R';
                    i++;
                }
                else if (*value == 'G') {
                    *value2++ = 'G';
                    i++;
                }
                else if (*value == 'B') {
                    *value2++ = 'B';
                    i++;
                }
                else if (*value == 'Y') {
                    *value2++ = 'Y';
                    i++;
                }
                value++;
            }
            *value2 = '\0';
            std::string s_value(value3);
#ifdef DEBUG_LOG
            LOGE("%s(%d): value = %s\n", __FUNCTION__, __LINE__, s_value);
#endif
            if (xmlParseReadWrite == XML_PARSER_READ)
            {
                if (s_value == CALIB_SENSOR_AEC_RAWSTATSMODE_R) {
                    mCalibDb->aec.CommCtrl.RawStatsMode = CAM_RAWSTATS_MODE_R;
                }
                else if (s_value == CALIB_SENSOR_AEC_RAWSTATSMODE_G) {
                    mCalibDb->aec.CommCtrl.RawStatsMode = CAM_RAWSTATS_MODE_G;
                }
                else if (s_value == CALIB_SENSOR_AEC_RAWSTATSMODE_B) {
                    mCalibDb->aec.CommCtrl.RawStatsMode = CAM_RAWSTATS_MODE_B;
                }
                else if (s_value == CALIB_SENSOR_AEC_RAWSTATSMODE_Y) {
                    mCalibDb->aec.CommCtrl.RawStatsMode = CAM_RAWSTATS_MODE_Y;
                }
                else {
                    mCalibDb->aec.CommCtrl.RawStatsMode = CAM_RAWSTATS_MODE_INVALID;
                    LOGE("%s(%d): invalid AEC RawStatsMode = %s\n", __FUNCTION__, __LINE__, s_value);
                }
            }
            else if (xmlParseReadWrite == XML_PARSER_WRITE)
            {
                XMLNode *pNode = (XMLNode*)pchild;
                switch (mCalibDb->aec.CommCtrl.RawStatsMode)
                {
                case CAM_HIST_MODE_R:
                    pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_HISTSTATSMODE_R);
                    break;
                case CAM_HIST_MODE_G:
                    pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_HISTSTATSMODE_G);
                    break;
                case CAM_HIST_MODE_B:
                    pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_HISTSTATSMODE_B);
                    break;
                case CAM_HIST_MODE_Y:
                    pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_HISTSTATSMODE_Y);
                    break;
                default:
                    pNode->FirstChild()->SetValue("INVALID");
                    LOGE("%s(%d): (XML Write)invalid AEC HistStatsMode = %d end\n", __FUNCTION__, __LINE__, mCalibDb->aec.CommCtrl.HistStatsMode);
                    break;
                }
            }
        }
        else if (tagname == CALIB_SENSOR_AEC_YRANGEMODE
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            char* value = Toupper(tag.Value());
            std::string s_value(value);
            if (xmlParseReadWrite == XML_PARSER_READ)
            {
                if (s_value == CALIB_SENSOR_AEC_YRANGEMODE_FULL) {
                    mCalibDb->aec.CommCtrl.YRangeMode = CAM_YRANGE_MODE_FULL;
                }
                else if (s_value == CALIB_SENSOR_AEC_YRANGEMODE_LIMITED) {
                    mCalibDb->aec.CommCtrl.YRangeMode = CAM_YRANGE_MODE_LIMITED;
                }
                else {
                    mCalibDb->aec.CommCtrl.YRangeMode = CAM_YRANGE_MODE_INVALID;
                    LOGE("%s(%d): invalid AEC YRangeMode = %s\n", __FUNCTION__, __LINE__, s_value);
                }
            }
            else if (xmlParseReadWrite == XML_PARSER_WRITE)
            {
                XMLNode *pNode = (XMLNode*)pchild;
                if (mCalibDb->aec.CommCtrl.YRangeMode == CAM_YRANGE_MODE_FULL) {
                    pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_YRANGEMODE_FULL);
                }
                else if (mCalibDb->aec.CommCtrl.YRangeMode == CAM_YRANGE_MODE_LIMITED) {
                    pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_YRANGEMODE_LIMITED);
                }
                else {
                    pNode->FirstChild()->SetValue("Invalid");
                    LOGE("%s(%d): (XML Write)invalid AEC YRangeMode = %d\n", __FUNCTION__, __LINE__, mCalibDb->aec.CommCtrl.YRangeMode);
                }
            }

        }
        else if ((tagname == CALIB_SENSOR_AEC_AECRUNINTERVAL)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            ParseUcharArray(pchild, &mCalibDb->aec.CommCtrl.AecRunInterval, tag.Size());
        }
        else if ((tagname == CALIB_SENSOR_AEC_AECOPTYPE)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            char* value = Toupper(tag.Value());
            std::string s_value(value);
            if (xmlParseReadWrite == XML_PARSER_READ)
            {
                if (s_value == CALIB_SENSOR_AEC_AECOPTYPE_AUTO) {
                    mCalibDb->aec.CommCtrl.AecOpType = RK_AIQ_OP_MODE_AUTO;
                }
                else if (s_value == CALIB_SENSOR_AEC_AECOPTYPE_MANUAL) {
                    mCalibDb->aec.CommCtrl.AecOpType = RK_AIQ_OP_MODE_MANUAL;
                }
                else {
                    mCalibDb->aec.CommCtrl.AecOpType = RK_AIQ_OP_MODE_INVALID;
                    LOGE("%s(%d): invalid AEC AecOpType = %s\n", __FUNCTION__, __LINE__, s_value);
                }
            }
            else if (xmlParseReadWrite == XML_PARSER_WRITE)
            {
                XMLNode *pNode = (XMLNode*)pchild;
                if (mCalibDb->aec.CommCtrl.AecOpType == RK_AIQ_OP_MODE_AUTO) {
                    pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_AECOPTYPE_AUTO);
                }
                else if (mCalibDb->aec.CommCtrl.AecOpType == RK_AIQ_OP_MODE_MANUAL) {
                    pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_AECOPTYPE_MANUAL);
                }
                else {
                    pNode->FirstChild()->SetValue("Invalid");
                    LOGE("%s(%d): (XML Write)invalid AEC AecOpType = %d\n", __FUNCTION__, __LINE__, mCalibDb->aec.CommCtrl.AecOpType);
                }

            }
        }
        else if (tagname == CALIB_SENSOR_AEC_AECSPEED) {
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            autoTabForward();
#ifdef DEBUG_LOG
            LOGE("%s(%d): Tagname = %s\n", __FUNCTION__, __LINE__, tagname);
#endif
            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                LOGE("%s(%d): subTagname = %s\n", __FUNCTION__, __LINE__, subTagname);
#endif
                if ((subTagname == CALIB_SENSOR_AEC_DAMPOVER)
                        && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (subTag.Size() > 0)) {
                    int no = ParseFloatArray(psubchild, &mCalibDb->aec.CommCtrl.stAuto.stAeSpeed.DampOver, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AEC_DAMPUNDER)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseFloatArray(psubchild, &mCalibDb->aec.CommCtrl.stAuto.stAeSpeed.DampUnder, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AEC_DAMPDARK2BRIGHT)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseFloatArray(psubchild, &mCalibDb->aec.CommCtrl.stAuto.stAeSpeed.DampDark2Bright, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AEC_DAMPBRIGHT2DARK)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseFloatArray(psubchild, &mCalibDb->aec.CommCtrl.stAuto.stAeSpeed.DampBright2Dark, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else {
                    LOGE("%s(%d): parse error in  AeSpeed (unknow tag: %s )\n", __FUNCTION__, __LINE__, subTagname);
                }
                psubchild = psubchild->NextSibling();
            }
            autoTabBackward();

        }
        else if (tagname == CALIB_SENSOR_AEC_AECDELAYFRMNUM) {
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            autoTabForward();
#ifdef DEBUG_LOG
            LOGE("%s(%d): Tagname = %s\n", __FUNCTION__, __LINE__, tagname);
#endif
            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                LOGE("%s(%d): subTagname = %s\n", __FUNCTION__, __LINE__, subTagname);
#endif
                if ((subTagname == CALIB_SENSOR_AEC_BLACKDELAY)
                        && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (subTag.Size() > 0)) {
                    int no = ParseUcharArray(psubchild, &mCalibDb->aec.CommCtrl.stAuto.BlackDelayFrame, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AEC_WHITEDELAY)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseUcharArray(psubchild, &mCalibDb->aec.CommCtrl.stAuto.WhiteDelayFrame, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else {
                    LOGE("%s(%d): parse error in  AeDelayFrame (unknow tag: %s )\n", __FUNCTION__, __LINE__, subTagname);
                }
                psubchild = psubchild->NextSibling();
            }
            autoTabBackward();
        }
        else if (tagname == CALIB_SENSOR_AEC_AECDNSWITCH) {
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            autoTabForward();
#ifdef DEBUG_LOG
            LOGE("%s(%d): Tagname = %s\n", __FUNCTION__, __LINE__, tagname);
#endif
            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());

#ifdef DEBUG_LOG
                LOGE("%s(%d): subTagname = %s\n", __FUNCTION__, __LINE__, subTagname);
#endif
                if ((subTagname == CALIB_SENSOR_AEC_AECDNSWITCH_DNTRIGGER)
                        && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (subTag.Size() > 0)) {
                    int no = ParseUcharArray(psubchild, &mCalibDb->aec.CommCtrl.stDNSwitch.DNTrigger, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AEC_AECDNSWITCH_DNMODE)
                         && (subTag.isType(XmlTag::TAG_TYPE_CHAR))
                         && (subTag.Size() > 0)) {
                    char* value = Toupper(subTag.Value());
                    std::string s_value(value);
                    if (xmlParseReadWrite == XML_PARSER_READ)
                    {
                        if (s_value == CALIB_SENSOR_AEC_AECDNSWITCH_DNMODE_DAY) {
                            mCalibDb->aec.CommCtrl.stDNSwitch.DNMode = AEC_DNMODE_DAY;
                        }
                        else if (s_value == CALIB_SENSOR_AEC_AECDNSWITCH_DNMODE_NIGHT) {
                            mCalibDb->aec.CommCtrl.stDNSwitch.DNMode = AEC_DNMODE_NIGHT;
                        }
                        else {
                            mCalibDb->aec.CommCtrl.stDNSwitch.DNMode = AEC_DNMODE_MIN;
                            LOGE("%s(%d): invalid AEC DNSwitch-DNMode = %s\n", __FUNCTION__, __LINE__, s_value);
                        }
                    }
                    else if (xmlParseReadWrite == XML_PARSER_WRITE)
                    {
                        XMLNode *pNode = (XMLNode*)psubchild;
                        if (mCalibDb->aec.CommCtrl.stDNSwitch.DNMode == AEC_DNMODE_DAY) {
                            pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_AECDNSWITCH_DNMODE_DAY);
                        }
                        else if (mCalibDb->aec.CommCtrl.stDNSwitch.DNMode == AEC_DNMODE_NIGHT) {
                            pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_AECDNSWITCH_DNMODE_NIGHT);
                        }
                        else {
                            pNode->FirstChild()->SetValue("Invalid");
                            LOGE("%s(%d): (XML Write)invalid AEC DNSwitch-DNMode = %d\n", __FUNCTION__, __LINE__, mCalibDb->aec.CommCtrl.stDNSwitch.DNMode);
                        }

                    }
                }
                else if ((subTagname == CALIB_SENSOR_AEC_AECDNSWITCH_FILLLIGHTMODE)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseUcharArray(psubchild, &mCalibDb->aec.CommCtrl.stDNSwitch.FillLightMode, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AEC_AECDNSWITCH_D2NFACTH)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseFloatArray(psubchild, &mCalibDb->aec.CommCtrl.stDNSwitch.Day2NightFacTh, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                } else if ((subTagname == CALIB_SENSOR_AEC_AECDNSWITCH_D2NFRMCNT)
                           && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                           && (subTag.Size() > 0)) {
                    int no = ParseUcharArray(psubchild, &mCalibDb->aec.CommCtrl.stDNSwitch.Day2NightFrmCnt, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                } else if ((subTagname == CALIB_SENSOR_AEC_AECDNSWITCH_VBNIGHTMODE)
                           && (subTag.isType(XmlTag::TAG_TYPE_STRUCT))
                           && (subTag.Size() > 0)) {
                    const XMLNode* psubsubchild = psubchild->ToElement()->FirstChild();
                    autoTabForward();
                    while (psubsubchild) {
                        XmlTag subsubTag = XmlTag(psubsubchild->ToElement());
                        std::string subsubTagname(psubsubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                        LOGE("%s(%d): subsubTagname = %s\n", __FUNCTION__, __LINE__, subsubTagname);
#endif
                        if ((subsubTagname == CALIB_SENSOR_AEC_ENABLE)
                                && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                && (subsubTag.Size() > 0)) {
                            int no = ParseUcharArray(psubsubchild, &mCalibDb->aec.CommCtrl.stDNSwitch.stVBNightMode.enable, subTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_AECDNSWITCH_N2DFRMCNT)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            int no = ParseUcharArray(psubsubchild, &mCalibDb->aec.CommCtrl.stDNSwitch.stVBNightMode.Night2DayFrmCnt, subTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_AECDNSWITCH_N2DFACTH)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.CommCtrl.stDNSwitch.stVBNightMode.Night2DayFacTh, subTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else {
                            LOGE("%s(%d): parse error in DNSwitch - VBNightMode(unknow tag: %s )\n", __FUNCTION__, __LINE__, subsubTagname);
                        }
                        psubsubchild = psubsubchild->NextSibling();
                    }
                    autoTabBackward();
                }
                else if ((subTagname == CALIB_SENSOR_AEC_AECDNSWITCH_IRNIGHTMODE)
                         && (subTag.isType(XmlTag::TAG_TYPE_STRUCT))
                         && (subTag.Size() > 0)) {
                    const XMLNode* psubsubchild = psubchild->ToElement()->FirstChild();
                    autoTabForward();
                    while (psubsubchild) {
                        XmlTag subsubTag = XmlTag(psubsubchild->ToElement());
                        std::string subsubTagname(psubsubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                        LOGE("%s(%d): subsubTagname = %s\n", __FUNCTION__, __LINE__, subsubTagname);
#endif
                        if ((subsubTagname == CALIB_SENSOR_AEC_ENABLE)
                                && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                && (subsubTag.Size() > 0)) {
                            int no = ParseUcharArray(psubsubchild, &mCalibDb->aec.CommCtrl.stDNSwitch.stIRNightMode.enable, subTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_AECDNSWITCH_N2DFRMCNT)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            int no = ParseUcharArray(psubsubchild, &mCalibDb->aec.CommCtrl.stDNSwitch.stIRNightMode.Night2DayFrmCnt, subTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_AECDNSWITCH_N2DFACTH)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.CommCtrl.stDNSwitch.stIRNightMode.Night2DayFacTh, subTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_AECDNSWITCH_IR_RG)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.CommCtrl.stDNSwitch.stIRNightMode.IRRgain, subTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_AECDNSWITCH_IR_BG)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.CommCtrl.stDNSwitch.stIRNightMode.IRBgain, subTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_AECDNSWITCH_MAX_DIS)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.CommCtrl.stDNSwitch.stIRNightMode.MaxWbDis, subTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_AECDNSWITCH_VB_PERCENT)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.CommCtrl.stDNSwitch.stIRNightMode.VbPercent, subTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        } else {
                            LOGE("%s(%d): parse error in DNSwitch - IRNightMode(unknow tag: %s )\n", __FUNCTION__, __LINE__, subsubTagname);
                        }
                        psubsubchild = psubsubchild->NextSibling();
                    }
                    autoTabBackward();
                }
                else {
                    LOGE("%s(%d): parse error in DNSwitch (unknow tag: %s )\n", __FUNCTION__, __LINE__, subTagname);
                }
                psubchild = psubchild->NextSibling();
            }
            autoTabBackward();
        }
        else if (tagname == CALIB_SENSOR_AEC_ANTIFLICKER) {
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            autoTabForward();
#ifdef DEBUG_LOG
            LOGE("%s(%d): Tagname = %s\n", __FUNCTION__, __LINE__, tagname);
#endif
            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                LOGE("%s(%d): subTagname = %s\n", __FUNCTION__, __LINE__, subTagname);
#endif
                if ((subTagname == CALIB_SENSOR_AEC_ANTIFLICKER_ENABLE)
                        && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (subTag.Size() > 0)) {
                    uint8_t temp = mCalibDb->aec.CommCtrl.stAntiFlicker.enable;
                    int no = ParseUcharArray(psubchild, &temp, subTag.Size());
                    mCalibDb->aec.CommCtrl.stAntiFlicker.enable = (temp == 0) ? false : true;
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AEC_ANTIFLICKER_FREQ)
                         && (subTag.isType(XmlTag::TAG_TYPE_CHAR))
                         && (subTag.Size() > 0)) {
                    char* value = Toupper(subTag.Value());
                    std::string s_value(value);
                    if (xmlParseReadWrite == XML_PARSER_READ)
                    {
                        if (s_value == CALIB_SENSOR_AEC_ANTIFLICKER_FREQ_50HZ) {
                            mCalibDb->aec.CommCtrl.stAntiFlicker.Frequency = AEC_FLICKER_FREQUENCY_50HZ;
                        }
                        else if (s_value == CALIB_SENSOR_AEC_ANTIFLICKER_FREQ_60HZ) {
                            mCalibDb->aec.CommCtrl.stAntiFlicker.Frequency = AEC_FLICKER_FREQUENCY_60HZ;
                        }
                        else {
                            mCalibDb->aec.CommCtrl.stAntiFlicker.Frequency = AEC_FLICKER_FREQUENCY_OFF;
                            LOGE("%s(%d): invalid stAntiFlicker.Frequency = %s\n", __FUNCTION__, __LINE__, s_value);
                        }
                    }
                    else if (xmlParseReadWrite == XML_PARSER_WRITE)
                    {
                        XMLNode *pNode = (XMLNode*)psubchild;
                        if (mCalibDb->aec.CommCtrl.stAntiFlicker.Frequency == AEC_FLICKER_FREQUENCY_50HZ) {
                            pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_ANTIFLICKER_FREQ_50HZ);
                        }
                        else if (mCalibDb->aec.CommCtrl.stAntiFlicker.Frequency == AEC_FLICKER_FREQUENCY_60HZ) {
                            pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_ANTIFLICKER_FREQ_60HZ);
                        }
                        else {
                            pNode->FirstChild()->SetValue("Invalid");
                            LOGE("%s(%d): (XML Write)invalid AEC stAntiFlicker.Frequency = %d\n", __FUNCTION__, __LINE__, mCalibDb->aec.CommCtrl.stAntiFlicker.Frequency);
                        }

                    }
                }
                else if ((subTagname == CALIB_SENSOR_AEC_ANTIFLICKER_MODE)
                         && (subTag.isType(XmlTag::TAG_TYPE_CHAR))
                         && (subTag.Size() > 0)) {
                    char* value = Toupper(subTag.Value());
                    std::string s_value(value);
                    if (xmlParseReadWrite == XML_PARSER_READ)
                    {
                        if (s_value == CALIB_SENSOR_AEC_ANTIFLICKER_MODE_AUTO) {
                            mCalibDb->aec.CommCtrl.stAntiFlicker.Mode = AEC_ANTIFLICKER_AUTO_MODE;
                        }
                        else if (s_value == CALIB_SENSOR_AEC_ANTIFLICKER_MODE_NORMAL) {
                            mCalibDb->aec.CommCtrl.stAntiFlicker.Mode = AEC_ANTIFLICKER_NORMAL_MODE;
                        }
                        else {
                            mCalibDb->aec.CommCtrl.stAntiFlicker.Mode = AEC_ANTIFLICKER_AUTO_MODE;
                            LOGE("%s(%d): invalid stAntiFlicker.Mode = %s\n", __FUNCTION__, __LINE__, s_value);
                        }
                    }
                    else if (xmlParseReadWrite == XML_PARSER_WRITE)
                    {
                        XMLNode *pNode = (XMLNode*)psubchild;
                        if (mCalibDb->aec.CommCtrl.stAntiFlicker.Mode == AEC_ANTIFLICKER_AUTO_MODE) {
                            pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_ANTIFLICKER_MODE_AUTO);
                        }
                        else if (mCalibDb->aec.CommCtrl.stAntiFlicker.Mode == AEC_ANTIFLICKER_NORMAL_MODE) {
                            pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_ANTIFLICKER_MODE_NORMAL);
                        }
                        else {
                            pNode->FirstChild()->SetValue("Invalid");
                            LOGE("%s(%d): (XML Write)invalid AEC stAntiFlicker.Mode = %d\n", __FUNCTION__, __LINE__, mCalibDb->aec.CommCtrl.stAntiFlicker.Mode);
                        }

                    }
                }
                else {
                    redirectOut << "parse error in  stAntiFlicker (unknow tag: "
                                << subTagname
                                << ")"
                                << std::endl;
                }
                psubchild = psubchild->NextSibling();
            }
            autoTabBackward();
        }
        else if (tagname == CALIB_SENSOR_AEC_AECFRAMERATEMODE) {
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            autoTabForward();
#ifdef DEBUG_LOG
            LOGE("%s(%d): Tagname = %s\n", __FUNCTION__, __LINE__, tagname);
#endif
            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                LOGE("%s(%d): subTagname = %s\n", __FUNCTION__, __LINE__, subTagname);
#endif
                if ((subTagname == CALIB_SENSOR_AEC_ISFPSFIX)
                        && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (subTag.Size() > 0)) {
                    uint8_t temp = mCalibDb->aec.CommCtrl.stAuto.stFrmRate.isFpsFix;
                    int no = ParseUcharArray(psubchild, &temp, subTag.Size());
                    mCalibDb->aec.CommCtrl.stAuto.stFrmRate.isFpsFix = (temp == 0) ? false : true;
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AEC_FPSVALUE)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseUcharArray(psubchild, &mCalibDb->aec.CommCtrl.stAuto.stFrmRate.FpsValue, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else {
                    redirectOut << "parse error in  stFrmRate (unknow tag: "
                                << subTagname
                                << ")"
                                << std::endl;
                }
                psubchild = psubchild->NextSibling();
            }
            autoTabBackward();
        }
        else if (tagname == CALIB_SENSOR_AEC_AECRANGE) {
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            autoTabForward();
#ifdef DEBUG_LOG
            LOGE("%s(%d): Tagname = %s\n", __FUNCTION__, __LINE__, tagname);
#endif

            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                LOGE("%s(%d): subTagname = %s\n", __FUNCTION__, __LINE__, subTagname);
#endif

                if (subTagname == CALIB_SENSOR_AEC_AECRANGE_ENABLE
                        && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (subTag.Size() > 0)) {
                    uint8_t temp = mCalibDb->aec.CommCtrl.stAuto.SetAeRangeEn;
                    int no = ParseUcharArray(psubchild, &temp, subTag.Size());
                    mCalibDb->aec.CommCtrl.stAuto.SetAeRangeEn = (temp == 0) ? false : true;
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if(subTagname == CALIB_SENSOR_AEC_LINEARAE) {
                    const XMLNode* psubsubchild = psubchild->ToElement()->FirstChild();
                    autoTabForward();
                    while (psubsubchild) {
                        XmlTag subsubTag = XmlTag(psubsubchild->ToElement());
                        std::string subsubTagname(psubsubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                        LOGE("%s(%d): subsubTagname = %s\n", __FUNCTION__, __LINE__, subsubTagname);
#endif
                        if ((subsubTagname == CALIB_SENSOR_AEC_TIMERANGE)
                                && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                && (subsubTag.Size() > 0)) {
                            float tempVal[2] = { mCalibDb->aec.CommCtrl.stAuto.stLinAeRange.stExpTimeRange.Min, mCalibDb->aec.CommCtrl.stAuto.stLinAeRange.stExpTimeRange.Max };
                            int no = ParseFloatArray(psubsubchild, tempVal, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                            mCalibDb->aec.CommCtrl.stAuto.stLinAeRange.stExpTimeRange.Min = tempVal[0];
                            mCalibDb->aec.CommCtrl.stAuto.stLinAeRange.stExpTimeRange.Max = tempVal[1];
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_GAINRANGE)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            float tempVal[2] = { mCalibDb->aec.CommCtrl.stAuto.stLinAeRange.stGainRange.Min, mCalibDb->aec.CommCtrl.stAuto.stLinAeRange.stGainRange.Max };
                            int no = ParseFloatArray(psubsubchild, tempVal, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                            mCalibDb->aec.CommCtrl.stAuto.stLinAeRange.stGainRange.Min = tempVal[0];
                            mCalibDb->aec.CommCtrl.stAuto.stLinAeRange.stGainRange.Max = tempVal[1];
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_ISPDGAINRANGE)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            float tempVal[2] = { mCalibDb->aec.CommCtrl.stAuto.stLinAeRange.stIspDGainRange.Min, mCalibDb->aec.CommCtrl.stAuto.stLinAeRange.stIspDGainRange.Max };
                            int no = ParseFloatArray(psubsubchild, tempVal, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                            mCalibDb->aec.CommCtrl.stAuto.stLinAeRange.stIspDGainRange.Min = tempVal[0];
                            mCalibDb->aec.CommCtrl.stAuto.stLinAeRange.stIspDGainRange.Max = tempVal[1];
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_PIRISRANGE)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            float tempVal[2] = { mCalibDb->aec.CommCtrl.stAuto.stLinAeRange.stPIrisRange.Min, mCalibDb->aec.CommCtrl.stAuto.stLinAeRange.stPIrisRange.Max };
                            int no = ParseFloatArray(psubsubchild, tempVal, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                            mCalibDb->aec.CommCtrl.stAuto.stLinAeRange.stPIrisRange.Min = tempVal[0];
                            mCalibDb->aec.CommCtrl.stAuto.stLinAeRange.stPIrisRange.Max = tempVal[1];
                        }
                        else {
                            LOGE("%s(%d): parse error in  stLinAeRange (unknow tag: %s )\n", __FUNCTION__, __LINE__, subsubTagname);
                        }
                        psubsubchild = psubsubchild->NextSibling();
                    }
                    autoTabBackward();
                }
                else if (subTagname == CALIB_SENSOR_AEC_HDRAE) {
                    const XMLNode* psubsubchild = psubchild->ToElement()->FirstChild();
                    autoTabForward();
                    while (psubsubchild) {
                        XmlTag subsubTag = XmlTag(psubsubchild->ToElement());
                        std::string subsubTagname(psubsubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                        LOGE("%s(%d): subsubTagname = %s\n", __FUNCTION__, __LINE__, subsubTagname);
#endif
                        if ((subsubTagname == CALIB_SENSOR_AEC_HDR_TIMERANGE_MIN)
                                && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                && (subsubTag.Size() > 0)) {
                            float tempVal[3];
                            for (int i = 0; i < 3; i++)
                                tempVal[i] = mCalibDb->aec.CommCtrl.stAuto.stHdrAeRange.stExpTimeRange[i].Min;
                            int no = ParseFloatArray(psubsubchild, tempVal, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                            for (int i = 0; i < 3; i++)
                                mCalibDb->aec.CommCtrl.stAuto.stHdrAeRange.stExpTimeRange[i].Min = tempVal[i];
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_HDR_TIMERANGE_MAX)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            float tempVal[3];
                            for (int i = 0; i < 3; i++)
                                tempVal[i] = mCalibDb->aec.CommCtrl.stAuto.stHdrAeRange.stExpTimeRange[i].Max;
                            int no = ParseFloatArray(psubsubchild, tempVal, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                            for (int i = 0; i < 3; i++)
                                mCalibDb->aec.CommCtrl.stAuto.stHdrAeRange.stExpTimeRange[i].Max = tempVal[i];
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_HDR_GAINRANGE_MIN)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            float tempVal[3];
                            for (int i = 0; i < 3; i++)
                                tempVal[i] = mCalibDb->aec.CommCtrl.stAuto.stHdrAeRange.stGainRange[i].Min;
                            int no = ParseFloatArray(psubsubchild, tempVal, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                            for (int i = 0; i < 3; i++)
                                mCalibDb->aec.CommCtrl.stAuto.stHdrAeRange.stGainRange[i].Min = tempVal[i];
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_HDR_GAINRANGE_MAX)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            float tempVal[3];
                            for (int i = 0; i < 3; i++)
                                tempVal[i] = mCalibDb->aec.CommCtrl.stAuto.stHdrAeRange.stGainRange[i].Max;
                            int no = ParseFloatArray(psubsubchild, tempVal, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                            for (int i = 0; i < 3; i++)
                                mCalibDb->aec.CommCtrl.stAuto.stHdrAeRange.stGainRange[i].Max = tempVal[i];
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_HDR_ISPDGAINRANGE_MIN)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            float tempVal[3];
                            for (int i = 0; i < 3; i++)
                                tempVal[i] = mCalibDb->aec.CommCtrl.stAuto.stHdrAeRange.stIspDGainRange[i].Min;
                            int no = ParseFloatArray(psubsubchild, tempVal, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                            for (int i = 0; i < 3; i++)
                                mCalibDb->aec.CommCtrl.stAuto.stHdrAeRange.stIspDGainRange[i].Min = tempVal[i];
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_HDR_ISPDGAINRANGE_MAX)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            float tempVal[3];
                            for (int i = 0; i < 3; i++)
                                tempVal[i] = mCalibDb->aec.CommCtrl.stAuto.stHdrAeRange.stIspDGainRange[i].Max;
                            int no = ParseFloatArray(psubsubchild, tempVal, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                            for (int i = 0; i < 3; i++)
                                mCalibDb->aec.CommCtrl.stAuto.stHdrAeRange.stIspDGainRange[i].Max = tempVal[i];
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_PIRISRANGE)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            float tempVal[2] = { mCalibDb->aec.CommCtrl.stAuto.stHdrAeRange.stPIrisRange.Min, mCalibDb->aec.CommCtrl.stAuto.stHdrAeRange.stPIrisRange.Max };
                            int no = ParseFloatArray(psubsubchild, tempVal, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                            mCalibDb->aec.CommCtrl.stAuto.stHdrAeRange.stPIrisRange.Min = tempVal[0];
                            mCalibDb->aec.CommCtrl.stAuto.stHdrAeRange.stPIrisRange.Max = tempVal[1];
                        }
                        psubsubchild = psubsubchild->NextSibling();
                    }
                    autoTabBackward();
                }
                else {
                    LOGE("%s(%d): parse error in  AeRange (unknow tag: %s )\n", __FUNCTION__, __LINE__, subTagname);
                }
                psubchild = psubchild->NextSibling();
            }
            autoTabBackward();
        }
        else if (tagname == CALIB_SENSOR_AEC_AECINITVALUE) {
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            autoTabForward();
#ifdef DEBUG_LOG
            LOGE("%s(%d): Tagname = %s\n", __FUNCTION__, __LINE__, tagname);
#endif
            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                LOGE("%s(%d): subTagname = %s\n", __FUNCTION__, __LINE__, subTagname);
#endif
                if (subTagname == CALIB_SENSOR_AEC_LINEARAE) {
                    const XMLNode* psubsubchild = psubchild->ToElement()->FirstChild();
                    autoTabForward();
                    while (psubsubchild) {
                        XmlTag subsubTag = XmlTag(psubsubchild->ToElement());
                        std::string subsubTagname(psubsubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                        LOGE("%s(%d): subsubTagname = %s\n", __FUNCTION__, __LINE__, subsubTagname);
#endif
                        if ((subsubTagname == CALIB_SENSOR_AEC_INITTIMEVALUE)
                                && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                && (subsubTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.CommCtrl.stInitExp.stLinExpInitExp.InitTimeValue, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                            mCalibDb->aec.CommCtrl.stInitExp.stLinExpInitExp.array_size = no;
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_INITGAINVALUE)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.CommCtrl.stInitExp.stLinExpInitExp.InitGainValue, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_INITISPDGAINVALUE)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.CommCtrl.stInitExp.stLinExpInitExp.InitIspDGainValue, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_INITPIRISVALUE)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.CommCtrl.stInitExp.stLinExpInitExp.InitPIrisValue, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else {
                            LOGE("%s(%d): parse error in  stLinExpInitExp (unknow tag: %s )\n", __FUNCTION__, __LINE__, subsubTagname);
                        }
                        psubsubchild = psubsubchild->NextSibling();
                    }
                    autoTabBackward();
                }
                else if (subTagname == CALIB_SENSOR_AEC_HDRAE) {
                    const XMLNode* psubsubchild = psubchild->ToElement()->FirstChild();
                    autoTabForward();
                    while (psubsubchild) {
                        XmlTag subsubTag = XmlTag(psubsubchild->ToElement());
                        std::string subsubTagname(psubsubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                        LOGE("%s(%d): subsubTagname = %s\n", __FUNCTION__, __LINE__, subsubTagname);
#endif
                        if ((subsubTagname == CALIB_SENSOR_AEC_INITTIMEVALUE)
                                && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                && (subsubTag.Size() > 0)) {
                            float tempVal[3];
                            for (int i = 0; i < 3; i++)
                                tempVal[i] = mCalibDb->aec.CommCtrl.stInitExp.stHdrExpInitExp.InitTimeValue.fCoeff[i];
                            int no = ParseFloatArray(psubsubchild, tempVal, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                            for (int i = 0; i < 3; i++)
                                mCalibDb->aec.CommCtrl.stInitExp.stHdrExpInitExp.InitTimeValue.fCoeff[i] = tempVal[i];
                            mCalibDb->aec.CommCtrl.stInitExp.stHdrExpInitExp.array_size = no;
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_INITGAINVALUE)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            float tempVal[3];
                            for (int i = 0; i < 3; i++)
                                tempVal[i] = mCalibDb->aec.CommCtrl.stInitExp.stHdrExpInitExp.InitGainValue.fCoeff[i];
                            int no = ParseFloatArray(psubsubchild, tempVal, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                            for (int i = 0; i < 3; i++)
                                mCalibDb->aec.CommCtrl.stInitExp.stHdrExpInitExp.InitGainValue.fCoeff[i] = tempVal[i];
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_INITISPDGAINVALUE)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            float tempVal[3];
                            for (int i = 0; i < 3; i++)
                                tempVal[i] = mCalibDb->aec.CommCtrl.stInitExp.stHdrExpInitExp.InitIspDGainValue.fCoeff[i];
                            int no = ParseFloatArray(psubsubchild, tempVal, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                            for (int i = 0; i < 3; i++)
                                mCalibDb->aec.CommCtrl.stInitExp.stHdrExpInitExp.InitIspDGainValue.fCoeff[i] = tempVal[i];
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_INITPIRISVALUE)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.CommCtrl.stInitExp.stHdrExpInitExp.InitPIrisValue, subsubTag.Size());
                        }
                        else {
                            LOGE("%s(%d): parse error in  stHdrExpInitExp (unknow tag: %s )\n", __FUNCTION__, __LINE__, subsubTagname);
                        }
                        psubsubchild = psubsubchild->NextSibling();
                    }
                    autoTabBackward();
                }
                else {
                    LOGE("%s(%d): parse error in  stInitExp (unknow tag: %s )\n", __FUNCTION__, __LINE__, subTagname);
                }
                psubchild = psubchild->NextSibling();
            }
            autoTabBackward();
        }
        else if (tagname == CALIB_SENSOR_AEC_AECGRIDWEIGHT) {
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            autoTabForward();
#ifdef DEBUG_LOG
            LOGE("%s(%d): Tagname = %s\n", __FUNCTION__, __LINE__, tagname);
#endif

            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                LOGE("%s(%d): subTagname = %s\n", __FUNCTION__, __LINE__, subTagname);
#endif
                if ((subTagname == CALIB_SENSOR_AEC_DAYGRIDWEIGHTS)
                        && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (subTag.Size() > 0)) {
                    int no = ParseUcharArray(psubchild, mCalibDb->aec.CommCtrl.DayGridWeights.uCoeff, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AEC_NIGHTGRIDWEIGHTS)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseUcharArray(psubchild, mCalibDb->aec.CommCtrl.NightGridWeights.uCoeff, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else {
                    redirectOut
                            << "parse error in  Gridweights (unknow tag: "
                            << subTagname
                            << ")"
                            << std::endl;
                }
                psubchild = psubchild->NextSibling();
            }
            autoTabBackward();
        }
        else if (tagname == CALIB_SENSOR_AEC_MANUALCTRL) {
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            autoTabForward();
#ifdef DEBUG_LOG
            LOGE("%s(%d): Tagname = %s\n", __FUNCTION__, __LINE__, tagname);
#endif
            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                LOGE("%s(%d): subTagname = %s\n", __FUNCTION__, __LINE__, subTagname);
#endif
                if (subTagname == CALIB_SENSOR_AEC_LINEARAE) {
                    const XMLNode* psubsubchild = psubchild->ToElement()->FirstChild();
                    autoTabForward();
                    while (psubsubchild) {
                        XmlTag subsubTag = XmlTag(psubsubchild->ToElement());
                        std::string subsubTagname(psubsubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                        LOGE("%s(%d): subsubTagname = %s\n", __FUNCTION__, __LINE__, subsubTagname);
#endif
                        if ((subsubTagname == CALIB_SENSOR_AEC_MANUALCTRL_TIMEEN)
                                && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                && (subsubTag.Size() > 0)) {
                            uint8_t temp = mCalibDb->aec.CommCtrl.stManual.stLinMe.ManualTimeEn;
                            int no = ParseUcharArray(psubsubchild, &temp, subsubTag.Size());
                            mCalibDb->aec.CommCtrl.stManual.stLinMe.ManualTimeEn = (temp == 0) ? false : true;
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_MANUALCTRL_GAINEN)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            uint8_t temp = mCalibDb->aec.CommCtrl.stManual.stLinMe.ManualGainEn;
                            int no = ParseUcharArray(psubsubchild, &temp, subsubTag.Size());
                            mCalibDb->aec.CommCtrl.stManual.stLinMe.ManualGainEn = (temp == 0) ? false : true;
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_MANUALCTRL_ISPDGAINEN)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            uint8_t temp = mCalibDb->aec.CommCtrl.stManual.stLinMe.ManualIspDgainEn;
                            int no = ParseUcharArray(psubsubchild, &temp, subsubTag.Size());
                            mCalibDb->aec.CommCtrl.stManual.stLinMe.ManualIspDgainEn = (temp == 0) ? false : true;
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_MANUALCTRL_PIRISEN)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            uint8_t temp = mCalibDb->aec.CommCtrl.stManual.stLinMe.ManualPIrisEn;
                            int no = ParseUcharArray(psubsubchild, &temp, subsubTag.Size());
                            mCalibDb->aec.CommCtrl.stManual.stLinMe.ManualPIrisEn = (temp == 0) ? false : true;
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_MANUALCTRL_TIMEVALUE)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.CommCtrl.stManual.stLinMe.TimeValue, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_MANUALCTRL_GAINVALUE)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.CommCtrl.stManual.stLinMe.GainValue, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_MANUALCTRL_ISPDGAINVALUE)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.CommCtrl.stManual.stLinMe.IspDGainValue, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        } else if ((subsubTagname == CALIB_SENSOR_AEC_MANUALCTRL_PIRISVALUE)
                                   && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                   && (subsubTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.CommCtrl.stManual.stLinMe.PIrisValue, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else {
                            LOGE("%s(%d): parse error in  stManual.stLinMe (unknow tag: %s )\n", __FUNCTION__, __LINE__, subsubTagname);
                        }
                        psubsubchild = psubsubchild->NextSibling();
                    }
                    autoTabBackward();
                }
                else if (subTagname == CALIB_SENSOR_AEC_HDRAE) {
                    const XMLNode* psubsubchild = psubchild->ToElement()->FirstChild();
                    autoTabForward();
                    while (psubsubchild) {
                        XmlTag subsubTag = XmlTag(psubsubchild->ToElement());
                        std::string subsubTagname(psubsubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                        LOGE("%s(%d): subsubTagname = %s\n", __FUNCTION__, __LINE__, subsubTagname);
#endif
                        if ((subsubTagname == CALIB_SENSOR_AEC_MANUALCTRL_TIMEEN)
                                && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                && (subsubTag.Size() > 0)) {
                            uint8_t temp = mCalibDb->aec.CommCtrl.stManual.stHdrMe.ManualTimeEn;
                            int no = ParseUcharArray(psubsubchild, &temp, subsubTag.Size());
                            mCalibDb->aec.CommCtrl.stManual.stHdrMe.ManualTimeEn = (temp == 0) ? false : true;
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_MANUALCTRL_GAINEN)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            uint8_t temp = mCalibDb->aec.CommCtrl.stManual.stHdrMe.ManualGainEn;
                            int no = ParseUcharArray(psubsubchild, &temp, subsubTag.Size());
                            mCalibDb->aec.CommCtrl.stManual.stHdrMe.ManualGainEn = (temp == 0) ? false : true;
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_MANUALCTRL_ISPDGAINEN)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            uint8_t temp = mCalibDb->aec.CommCtrl.stManual.stHdrMe.ManualIspDgainEn;
                            int no = ParseUcharArray(psubsubchild, &temp, subsubTag.Size());
                            mCalibDb->aec.CommCtrl.stManual.stHdrMe.ManualIspDgainEn = (temp == 0) ? false : true;
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_MANUALCTRL_PIRISEN)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            uint8_t temp = mCalibDb->aec.CommCtrl.stManual.stHdrMe.ManualPIrisEn;
                            int no = ParseUcharArray(psubsubchild, &temp, subsubTag.Size());
                            mCalibDb->aec.CommCtrl.stManual.stHdrMe.ManualPIrisEn = (temp == 0) ? false : true;
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_MANUALCTRL_TIMEVALUE)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            float tempVal[3];
                            for (int i = 0; i < 3; i++)
                                tempVal[i] = mCalibDb->aec.CommCtrl.stManual.stHdrMe.TimeValue.fCoeff[i];
                            int no = ParseFloatArray(psubsubchild, tempVal, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                            for (int i = 0; i < 3; i++)
                                mCalibDb->aec.CommCtrl.stManual.stHdrMe.TimeValue.fCoeff[i] = tempVal[i];
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_MANUALCTRL_GAINVALUE)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            float tempVal[3];
                            for (int i = 0; i < 3; i++)
                                tempVal[i] = mCalibDb->aec.CommCtrl.stManual.stHdrMe.GainValue.fCoeff[i];
                            int no = ParseFloatArray(psubsubchild, tempVal, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                            for (int i = 0; i < 3; i++)
                                mCalibDb->aec.CommCtrl.stManual.stHdrMe.GainValue.fCoeff[i] = tempVal[i];
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_MANUALCTRL_ISPDGAINVALUE)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            float tempVal[3];
                            for (int i = 0; i < 3; i++)
                                tempVal[i] = mCalibDb->aec.CommCtrl.stManual.stHdrMe.IspDGainValue.fCoeff[i];
                            int no = ParseFloatArray(psubsubchild, tempVal, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                            for (int i = 0; i < 3; i++)
                                mCalibDb->aec.CommCtrl.stManual.stHdrMe.IspDGainValue.fCoeff[i] = tempVal[i];
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_MANUALCTRL_PIRISVALUE)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.CommCtrl.stManual.stHdrMe.PIrisValue, subsubTag.Size());
                        }
                        else {
                            LOGE("%s(%d): parse error in  stManual.stHdrMe (unknow tag: %s )\n", __FUNCTION__, __LINE__, subsubTagname);
                        }
                        psubsubchild = psubsubchild->NextSibling();
                    }
                    autoTabBackward();
                }
                else {
                    LOGE("%s(%d): parse error in  stManual (unknow tag: %s )\n", __FUNCTION__, __LINE__, subTagname);
                }
                psubchild = psubchild->NextSibling();
            }
            autoTabBackward();
        }
        else if (tagname == CALIB_SENSOR_AEC_AECROUTE) {
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            autoTabForward();
#ifdef DEBUG_LOG
            LOGE("%s(%d): Tagname = %s\n", __FUNCTION__, __LINE__, tagname);
#endif
            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                LOGE("%s(%d): subTagname = %s\n", __FUNCTION__, __LINE__, subTagname);
#endif
                if (subTagname == CALIB_SENSOR_AEC_LINEARAE) {
                    if (!parseEntryCell(psubchild->ToElement(), subTag.Size(), &RkAiqCalibParser::parseEntrySensorAecLinearAeRoute)) {
                        LOGE("parse error in AEC LinAeRoute(%s)", tagname);
                        return (false);
                    }
                }
                else if (subTagname == CALIB_SENSOR_AEC_HDRAE) {
                    if (!parseEntryCell(psubchild->ToElement(), subTag.Size(), &RkAiqCalibParser::parseEntrySensorAecHdrAeRoute)) {
                        LOGE("parse error in AEC HdrAeRoute (%s)", tagname);
                        return (false);
                    }
                }
                else {
                    LOGE("%s(%d): parse error in  AecRoute (unknow tag: %s )\n", __FUNCTION__, __LINE__, subTagname);
                }
                psubchild = psubchild->NextSibling();
            }
            autoTabBackward();
        }
        else if (tagname == CALIB_SENSOR_AEC_LINEARAE_CTRL) {
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            autoTabForward();
#ifdef DEBUG_LOG
            LOGE("%s(%d): Tagname = %s\n", __FUNCTION__, __LINE__, tagname);
#endif
            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                LOGE("%s(%d): subTagname = %s\n", __FUNCTION__, __LINE__, subTagname.c_str());
#endif
                if ((subTagname == CALIB_SENSOR_AEC_SETPOINT)
                        && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (subTag.Size() > 0)) {
                    int no = ParseFloatArray(psubchild, &mCalibDb->aec.LinearAeCtrl.SetPoint, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AEC_NIGHTSETPOINT)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseFloatArray(psubchild, &mCalibDb->aec.LinearAeCtrl.NightSetPoint, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AEC_DYSETPOINTEN)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    uint8_t temp = mCalibDb->aec.LinearAeCtrl.DySetPointEn;
                    ParseUcharArray(psubchild, &temp, subTag.Size());
                    mCalibDb->aec.LinearAeCtrl.DySetPointEn = (temp == 0) ? false : true;
                }
                else if ((subTagname == CALIB_SENSOR_AEC_EVBIAS)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseFloatArray(psubchild, &mCalibDb->aec.LinearAeCtrl.Evbias, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if (subTagname == CALIB_SENSOR_AEC_DYNAMICSETPOINT) {
                    if (!parseEntryCell(psubchild->ToElement(), subTag.Size(), &RkAiqCalibParser::parseEntrySensorAecLinearAeDynamicPoint)) {
                        LOGE("parse error in AEC linear dynamic setpoint (%s)", tagname);
                        return (false);
                    }
                }
                else if ((subTagname == CALIB_SENSOR_AEC_TOLERANCE)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseFloatArray(psubchild, &mCalibDb->aec.LinearAeCtrl.Tolerance, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AEC_STRATEGYMODE)
                         && (subTag.isType(XmlTag::TAG_TYPE_CHAR))
                         && (subTag.Size() > 0)) {
                    char* value = Toupper(subTag.Value());
                    std::string s_value(value);
#ifdef DEBUG_LOG
                    LOGE("%s(%d): s_value = %s\n", __FUNCTION__, __LINE__, s_value);
#endif
                    if (xmlParseReadWrite == XML_PARSER_READ)
                    {
                        if (s_value == CALIB_SENSOR_AEC_STRATEGYMODE_LOWLIGHT) {
                            mCalibDb->aec.LinearAeCtrl.StrategyMode = RKAIQ_AEC_STRATEGY_MODE_LOWLIGHT_PRIOR;
                        }
                        else if (s_value == CALIB_SENSOR_AEC_STRATEGYMODE_HIGHLIGHT) {
                            mCalibDb->aec.LinearAeCtrl.StrategyMode = RKAIQ_AEC_STRATEGY_MODE_HIGHLIGHT_PRIOR;
                        }
                        else {
                            mCalibDb->aec.LinearAeCtrl.StrategyMode = RKAIQ_AEC_STRATEGY_MODE_AUTO;
                            redirectOut << "invalid AEC LinAe StrategyMode (" << s_value << ")" << std::endl;
                        }
                    }
                    else if (xmlParseReadWrite == XML_PARSER_WRITE)
                    {
                        XMLNode *pNode = (XMLNode *)psubchild;
                        if (mCalibDb->aec.LinearAeCtrl.StrategyMode == RKAIQ_AEC_STRATEGY_MODE_LOWLIGHT_PRIOR)
                            pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_STRATEGYMODE_LOWLIGHT);
                        else if (mCalibDb->aec.LinearAeCtrl.StrategyMode == RKAIQ_AEC_STRATEGY_MODE_HIGHLIGHT_PRIOR)
                            pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_STRATEGYMODE_HIGHLIGHT);
                        else {
                            pNode->FirstChild()->SetValue("AUTO");
                            redirectOut << "(XML Wrtie)invalid AEC LinAe StrategyMode (" << mCalibDb->aec.LinearAeCtrl.StrategyMode << ")" << std::endl;
                        }
                    }

                }
                else if ((subTagname == CALIB_SENSOR_AEC_AOE)
                         && (subTag.isType(XmlTag::TAG_TYPE_STRUCT))
                         && (subTag.Size() > 0)) {

                    const XMLNode* psubsubchild = psubchild->ToElement()->FirstChild();
                    autoTabForward();
                    while (psubsubchild) {
                        XmlTag subsubTag = XmlTag(psubsubchild->ToElement());
                        std::string subsubTagname(psubsubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                        LOGE("%s(%d): subsubTagname = %s\n", __FUNCTION__, __LINE__, subsubTagname);
#endif
                        if ((subsubTagname == CALIB_SENSOR_AEC_AOE_ENABLE)
                                && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                && (subsubTag.Size() > 0)) {
                            uint8_t temp = mCalibDb->aec.LinearAeCtrl.AOE_Enable;
                            int no = ParseUcharArray(psubsubchild, &temp, subsubTag.Size());
                            mCalibDb->aec.LinearAeCtrl.AOE_Enable = (temp == 0) ? false : true;
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_AOE_MAX_POINT)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.LinearAeCtrl.AOE_Max_point, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_AOE_MIN_POINT)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.LinearAeCtrl.AOE_Min_point, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_AOE_Y_MAX_TH)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.LinearAeCtrl.AOE_Y_Max_th, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_AOE_Y_MIN_TH)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.LinearAeCtrl.AOE_Y_Min_th, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_AOE_STEP_INC)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.LinearAeCtrl.AOE_Step_Inc, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_AOE_STEP_DEC)
                                 && (subsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subsubTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.LinearAeCtrl.AOE_Step_Dec, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else {
                            LOGE("%s(%d): parse error in AEC-LiearAE AOE section (unknow subsubTagname: %s )\n", __FUNCTION__, __LINE__, subsubTagname);
                        }
                        psubsubchild = psubsubchild->NextSibling();
                    }
                    autoTabBackward();
                }
                else if ((subTagname == CALIB_SENSOR_LINAECTRL_BACKLIGHT_CONFIG_TAG)
                         && (subTag.isType(XmlTag::TAG_TYPE_STRUCT))
                         && (subTag.Size() > 0)) {
                    const XMLNode* psecsubchild = psubchild->ToElement()->FirstChild();
                    autoTabForward();
                    while (psecsubchild) {
                        XmlTag secsubtag = XmlTag(psecsubchild->ToElement());
                        std::string secsubTagname(psecsubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                        redirectOut << "secsubTagname: " << secsubTagname << std::endl;
#endif
                        if (secsubTagname == CALIB_SENSOR_LINAECTRL_BACKLIGHT_ENABLE
                                && (secsubtag.Size() > 0)) {
                            int no = ParseUcharArray(psecsubchild, &mCalibDb->aec.LinearAeCtrl.BackLightConf.enable, secsubtag.Size());
                            DCT_ASSERT((no == secsubtag.Size()));
                        }
                        else  if (secsubTagname == CALIB_SENSOR_LINAECTRL_BACKLIGHT_MODE
                                  && (secsubtag.isType(XmlTag::TAG_TYPE_CHAR))
                                  && (secsubtag.Size() > 0)) {
                            char* value = Toupper(secsubtag.Value());
                            std::string s_value(value);
#ifdef DEBUG_LOG
                            redirectOut << "value:" << value << std::endl;
                            redirectOut << s_value << std::endl;
#endif
                            if (xmlParseReadWrite == XML_PARSER_READ)
                            {
                                if (s_value == CALIB_SENSOR_LINAECTRL_BACKLIGHT_MODE_WEIGHT) {
                                    mCalibDb->aec.LinearAeCtrl.BackLightConf.Mode = AEC_BACKLIT_MODE_WEIGHT_METHOD;
                                }
                                else if (s_value == CALIB_SENSOR_LINAECTRL_BACKLIGHT_MODE_DARKROI) {
                                    mCalibDb->aec.LinearAeCtrl.BackLightConf.Mode = AEC_BACKLIT_MODE_DARKROI_METHOD;
                                }
                                else {
                                    mCalibDb->aec.LinearAeCtrl.BackLightConf.Mode = AEC_BACKLIT_MODE_INVALID;
                                    redirectOut << "invalid LINAECTRL BackLitMode (" << s_value << ")" << std::endl;
                                }
                            }
                            else if (xmlParseReadWrite == XML_PARSER_WRITE)
                            {
                                XMLNode* pNode = (XMLNode *)psecsubchild;
                                if (mCalibDb->aec.LinearAeCtrl.BackLightConf.Mode == AEC_BACKLIT_MODE_WEIGHT_METHOD)
                                    pNode->FirstChild()->SetValue(CALIB_SENSOR_LINAECTRL_BACKLIGHT_MODE_WEIGHT);
                                else if (mCalibDb->aec.LinearAeCtrl.BackLightConf.Mode == AEC_BACKLIT_MODE_DARKROI_METHOD)
                                    pNode->FirstChild()->SetValue(CALIB_SENSOR_LINAECTRL_BACKLIGHT_MODE_DARKROI);
                                else {
                                    pNode->FirstChild()->SetValue("INVALID");
                                    redirectOut << "(XML Write)invalid LINAECTRL BackLitMode (" << mCalibDb->aec.LinearAeCtrl.BackLightConf.Mode << ")" << std::endl;
                                }
                            }

                        }
                        else if (secsubTagname == CALIB_SENSOR_LINAECTRL_BACKLIGHT_WEIGHT_METHOD
                                 && (secsubtag.isType(XmlTag::TAG_TYPE_STRUCT))
                                 && (secsubtag.Size() > 0)) {
                            const XMLNode* pthdsubchild = psecsubchild->ToElement()->FirstChild();
                            autoTabForward();
                            while (pthdsubchild) {
                                XmlTag thdsubtag = XmlTag(pthdsubchild->ToElement());
                                std::string thdsubTagname(pthdsubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                                redirectOut << "thdsubTagname: " << thdsubTagname << std::endl;
#endif
                                if (thdsubTagname == CALIB_SENSOR_LINAECTRL_BACKLIGHT_LUMALOWTH
                                        && (thdsubtag.Size() > 0)) {
                                    int no = ParseFloatArray(pthdsubchild, &mCalibDb->aec.LinearAeCtrl.BackLightConf.lumaLowTh, thdsubtag.Size());
                                    DCT_ASSERT((no == thdsubtag.Size()));
                                }
                                else if (thdsubTagname == CALIB_SENSOR_LINAECTRL_BACKLIGHT_LUMAHIGHTH
                                         && (thdsubtag.Size() > 0)) {
                                    int no = ParseFloatArray(pthdsubchild, &mCalibDb->aec.LinearAeCtrl.BackLightConf.lumaHighTh, thdsubtag.Size());
                                    DCT_ASSERT((no == thdsubtag.Size()));
                                }
                                else if (thdsubTagname == CALIB_SENSOR_LINAECTRL_BACKLIGHT_LVTH
                                         && (thdsubtag.Size() > 0)) {
                                    int no = ParseFloatArray(pthdsubchild, &mCalibDb->aec.LinearAeCtrl.BackLightConf.LvTh, thdsubtag.Size());
                                    DCT_ASSERT((no == thdsubtag.Size()));
                                }
                                else if (thdsubTagname == CALIB_SENSOR_LINAECTRL_BACKLIGHT_WEIGHTMINTH
                                         && (thdsubtag.Size() > 0)) {
                                    int no = ParseFloatArray(pthdsubchild, &mCalibDb->aec.LinearAeCtrl.BackLightConf.weightMinTh, thdsubtag.Size());
                                    DCT_ASSERT((no == thdsubtag.Size()));
                                }
                                else if (thdsubTagname == CALIB_SENSOR_LINAECTRL_BACKLIGHT_WEIGHTMAXTH
                                         && (thdsubtag.Size() > 0)) {
                                    int no = ParseFloatArray(pthdsubchild, &mCalibDb->aec.LinearAeCtrl.BackLightConf.weightMaxTh, thdsubtag.Size());
                                    DCT_ASSERT((no == thdsubtag.Size()));
                                }
                                else {
                                    redirectOut
                                            << "parse error in LinearAeCtrl BackLight WeightMethod (unknow tag: "
                                            << thdsubTagname
                                            << ")"
                                            << std::endl;
                                }
                                pthdsubchild = pthdsubchild->NextSibling();
                            }
                            autoTabBackward();
                        }
                        else if (secsubTagname == CALIB_SENSOR_LINAECTRL_BACKLIGHT_DARKROI_METHOD
                                 && (secsubtag.isType(XmlTag::TAG_TYPE_STRUCT))
                                 && (secsubtag.Size() > 0)) {
                            const XMLNode* pthdsubchild = psecsubchild->ToElement()->FirstChild();
                            autoTabForward();
#ifdef DEBUG_LOG
                            redirectOut << "secsubTagname: " << secsubTagname << std::endl;
#endif
                            while (pthdsubchild) {
                                XmlTag thdsubtag = XmlTag(pthdsubchild->ToElement());
                                std::string thdsubTagname(pthdsubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                                redirectOut << "thdsubTagname: " << thdsubTagname << std::endl;
#endif
                                if (thdsubTagname == CALIB_SENSOR_LINAECTRL_BACKLIGHT_OEROI_LOWTH
                                        && (thdsubtag.Size() > 0)) {
                                    int no = ParseFloatArray(pthdsubchild, &mCalibDb->aec.LinearAeCtrl.BackLightConf.OEROILowTh, thdsubtag.Size());
                                    DCT_ASSERT((no == thdsubtag.Size()));
                                }
                                else if (thdsubTagname == CALIB_SENSOR_LINAECTRL_BACKLIGHT_LV_HIGHTH
                                         && (thdsubtag.Size() > 0)) {
                                    int no = ParseFloatArray(pthdsubchild, &mCalibDb->aec.LinearAeCtrl.BackLightConf.LvHightTh, thdsubtag.Size());
                                    DCT_ASSERT((no == thdsubtag.Size()));
                                }
                                else if (thdsubTagname == CALIB_SENSOR_LINAECTRL_BACKLIGHT_LV_LOWTH
                                         && (thdsubtag.Size() > 0)) {
                                    int no = ParseFloatArray(pthdsubchild, &mCalibDb->aec.LinearAeCtrl.BackLightConf.LvLowTh, thdsubtag.Size());
                                    DCT_ASSERT((no == thdsubtag.Size()));
                                }
                                else if (thdsubTagname == CALIB_SENSOR_LINAECTRL_BACKLIGHT_LUMADISTTH
                                         && (thdsubtag.Size() > 0)) {
                                    int no = ParseFloatArray(pthdsubchild, &mCalibDb->aec.LinearAeCtrl.BackLightConf.LumaDistTh, thdsubtag.Size());
                                    DCT_ASSERT((no == thdsubtag.Size()));
                                }
                                else if (thdsubTagname == CALIB_SENSOR_LINAECTRL_BACKLIGHT_LOWLIGHTPDFTH
                                         && (thdsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                         && (thdsubtag.Size() > 0)) {
                                    int i = (sizeof(mCalibDb->aec.LinearAeCtrl.BackLightConf.LowLightPdfTh) / sizeof(mCalibDb->aec.LinearAeCtrl.BackLightConf.LowLightPdfTh.fCoeff[0]));
                                    int no = ParseFloatArray(pthdsubchild, mCalibDb->aec.LinearAeCtrl.BackLightConf.LowLightPdfTh.fCoeff, i);
                                    DCT_ASSERT((no == thdsubtag.Size()));
                                }
                                else if (thdsubTagname == CALIB_SENSOR_LINAECTRL_BACKLIGHT_NONOEPDFTH
                                         && (thdsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                         && (thdsubtag.Size() > 0)) {
                                    int i = (sizeof(mCalibDb->aec.LinearAeCtrl.BackLightConf.NonOEPdfTh) / sizeof(mCalibDb->aec.LinearAeCtrl.BackLightConf.NonOEPdfTh.fCoeff[0]));
                                    int no = ParseFloatArray(pthdsubchild, mCalibDb->aec.LinearAeCtrl.BackLightConf.NonOEPdfTh.fCoeff, i);
                                    DCT_ASSERT((no == thdsubtag.Size()));
                                }
                                else if (thdsubTagname == CALIB_SENSOR_LINAECTRL_BACKLIGHT_EXPLEVEL
                                         && (thdsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                         && (thdsubtag.Size() > 0)) {
                                    int i = (sizeof(mCalibDb->aec.LinearAeCtrl.BackLightConf.ExpLevel) / sizeof(mCalibDb->aec.LinearAeCtrl.BackLightConf.ExpLevel.fCoeff[0]));
                                    int no = ParseFloatArray(pthdsubchild, mCalibDb->aec.LinearAeCtrl.BackLightConf.ExpLevel.fCoeff, i);
                                    DCT_ASSERT((no == thdsubtag.Size()));
                                }
                                else if (thdsubTagname == CALIB_SENSOR_LINAECTRL_BACKLIGHT_TARGETLLLUMA
                                         && (thdsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                         && (thdsubtag.Size() > 0)) {
                                    int i = (sizeof(mCalibDb->aec.LinearAeCtrl.BackLightConf.TargetLLLuma) / sizeof(mCalibDb->aec.LinearAeCtrl.BackLightConf.TargetLLLuma.fCoeff[0]));
                                    int no = ParseFloatArray(pthdsubchild, mCalibDb->aec.LinearAeCtrl.BackLightConf.TargetLLLuma.fCoeff, i);
                                    DCT_ASSERT((no == thdsubtag.Size()));
                                }
                                else {
                                    redirectOut
                                            << "parse error in LinearAeCtrl BackLight DarkROI section (unknow thdsubtag: "
                                            << thdsubTagname
                                            << ")"
                                            << std::endl;
                                }
                                pthdsubchild = pthdsubchild->NextSibling();
                            }
                            autoTabBackward();
                        }
                        else {
                            redirectOut
                                    << "parse error in LINAECTRL backlight section (unknow tag: "
                                    << secsubTagname
                                    << ")"
                                    << std::endl;
                        }
                        psecsubchild = psecsubchild->NextSibling();
                    }
                    autoTabBackward();
                }
                else if (subTagname == CALIB_SENSOR_AEC_AEC_INTERVAL_ADJUST_STRATEGY) {
                    const XMLNode* psubsubchild = psubchild->ToElement()->FirstChild();
                    autoTabForward();
                    while (psubsubchild) {
                        XmlTag subTag = XmlTag(psubsubchild->ToElement());
                        std::string subsubTagname(psubsubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                        LOGE("%s(%d): subsubTagname = %s\n", __FUNCTION__, __LINE__, subsubTagname);
#endif
                        if ((subsubTagname == CALIB_SENSOR_AEC_ENABLE)
                                && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                && (subTag.Size() > 0)) {
                            int no = ParseUcharArray(psubsubchild, &mCalibDb->aec.LinearAeCtrl.InterAdjustStrategy.enable, subTag.Size());
                            DCT_ASSERT((no == subTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_DLUMA_HIGH_TH)
                                 && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.LinearAeCtrl.InterAdjustStrategy.dluma_high_th, subTag.Size());
                            DCT_ASSERT((no == subTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_DLUMA_LOW_TH)
                                 && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subTag.Size() > 0)) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.LinearAeCtrl.InterAdjustStrategy.dluma_low_th, subTag.Size());
                            DCT_ASSERT((no == subTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_ADJUST_TRIGGER_FRAME)
                                 && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subTag.Size() > 0)) {
                            int no = ParseUcharArray(psubsubchild, &mCalibDb->aec.LinearAeCtrl.InterAdjustStrategy.trigger_frame, subTag.Size());
                            DCT_ASSERT((no == subTag.Size()));
                        }
                        else {
                            redirectOut
                                    << "parse error in AEC LINAECTRL InterAdjustStrategy (unknow tag: "
                                    << subsubTagname
                                    << ")"
                                    << std::endl;
                        }
                        psubsubchild = psubsubchild->NextSibling();
                    }
                    autoTabBackward();
                }
                else if (subTagname == CALIB_SENSOR_AEC_HIST2HAL) {
                    const XMLNode* psubsubchild = psubchild->ToElement()->FirstChild();
                    autoTabForward();
                    while (psubsubchild) {
                        XmlTag subTag = XmlTag(psubsubchild->ToElement());
                        std::string subsubTagname(psubsubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                        LOGE("%s(%d): subsubTagname = %s\n", __FUNCTION__, __LINE__, subsubTagname);
#endif
                        if ((subsubTagname == CALIB_SENSOR_AEC_ENABLE)
                                && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                && (subTag.Size() > 0)) {
                            int no = ParseUcharArray(psubsubchild, &mCalibDb->aec.LinearAeCtrl.Hist2Hal.enable, subTag.Size());
                            DCT_ASSERT((no == subTag.Size()));
                        }
                        else if ((subsubTagname == CALIB_SENSOR_AEC_LOWHISTBINTH)
                                 && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                 && (subTag.Size() > 0)) {
                            int no = ParseUcharArray(psubsubchild, &mCalibDb->aec.LinearAeCtrl.Hist2Hal.lowHistBinTh, subTag.Size());
                            DCT_ASSERT((no == subTag.Size()));
                        }
                        else {
                            redirectOut
                                    << "parse error in AEC LinearAeCtrl Hist2Hal (unknow tag: "
                                    << subsubTagname
                                    << ")"
                                    << std::endl;
                        }
                        psubsubchild = psubsubchild->NextSibling();
                    }
                    autoTabBackward();
                }
                psubchild = psubchild->NextSibling();
            }
            autoTabBackward();
        }
        else if (tagname == CALIB_SENSOR_AEC_HDRAECTRL) {
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            autoTabForward();
            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                LOGE("%s(%d): subTagname = %s\n", __FUNCTION__, __LINE__, subTagname);
#endif
                if ((subTagname == CALIB_SENSOR_AEC_TOLERANCE)
                        && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (subTag.Size() > 0)) {
                    int no = ParseFloatArray(psubchild, &mCalibDb->aec.HdrAeCtrl.Tolerance, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AEC_EVBIAS)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    int no = ParseFloatArray(psubchild, &mCalibDb->aec.HdrAeCtrl.Evbias, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if ((subTagname == CALIB_SENSOR_AEC_STRATEGYMODE)
                         && (subTag.isType(XmlTag::TAG_TYPE_CHAR))
                         && (subTag.Size() > 0)) {
                    char* value = Toupper(subTag.Value());
                    std::string s_value(value);
#ifdef DEBUG_LOG
                    redirectOut << "value:" << value << std::endl;
                    redirectOut << s_value << std::endl;
#endif
                    if (xmlParseReadWrite == XML_PARSER_READ)
                    {
                        if (s_value == CALIB_SENSOR_AEC_STRATEGYMODE_LOWLIGHT) {
                            mCalibDb->aec.HdrAeCtrl.StrategyMode = RKAIQ_AEC_STRATEGY_MODE_LOWLIGHT_PRIOR;
                        }
                        else if (s_value == CALIB_SENSOR_AEC_STRATEGYMODE_HIGHLIGHT) {
                            mCalibDb->aec.HdrAeCtrl.StrategyMode = RKAIQ_AEC_STRATEGY_MODE_HIGHLIGHT_PRIOR;
                        }
                        else {
                            mCalibDb->aec.HdrAeCtrl.StrategyMode = RKAIQ_AEC_STRATEGY_MODE_AUTO;
                            redirectOut << "invalid AEC HdrAe StrategyMode (" << s_value << ")" << std::endl;
                        }
                    }
                    else if (xmlParseReadWrite == XML_PARSER_WRITE)
                    {
                        XMLNode *pNode = (XMLNode*)psubchild;
                        if (mCalibDb->aec.HdrAeCtrl.StrategyMode == RKAIQ_AEC_STRATEGY_MODE_LOWLIGHT_PRIOR) {
                            pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_STRATEGYMODE_LOWLIGHT);
                        }
                        else if (mCalibDb->aec.HdrAeCtrl.StrategyMode == RKAIQ_AEC_STRATEGY_MODE_HIGHLIGHT_PRIOR) {
                            pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_STRATEGYMODE_HIGHLIGHT);
                        }
                        else {
                            pNode->FirstChild()->SetValue("AUTO");
                            redirectOut << "(XML Write)invalid AEC HdrAe StrategyMode (" << mCalibDb->aec.HdrAeCtrl.StrategyMode << ")" << std::endl;
                        }
                    }

                }
                else if (subTagname == CALIB_SENSOR_AEC_HDRAECTRL_LONGFRMMODE) {
                    const XMLNode* psubsubchild = psubchild->ToElement()->FirstChild();
                    autoTabForward();
                    while (psubsubchild) {
                        XmlTag subsubTag = XmlTag(psubsubchild->ToElement());
                        std::string subsubTagname(psubsubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                        LOGE("%s(%d): subsubTagname = %s\n", __FUNCTION__, __LINE__, subsubTagname);
#endif
                        if ((subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_LONGFRMMODE_MODE)
                                && (subsubTag.isType(XmlTag::TAG_TYPE_CHAR))
                                && (subsubTag.Size() > 0)) {
                            char* value = Toupper(subsubTag.Value());
                            std::string s_value(value);
#ifdef DEBUG_LOG
                            redirectOut << "value:" << value << std::endl;
                            redirectOut << s_value << std::endl;
#endif
                            if (xmlParseReadWrite == XML_PARSER_READ)
                            {
                                if (s_value == CALIB_SENSOR_AEC_HDRAECTRL_LONGFRMMODE_NORMAL) {
                                    mCalibDb->aec.HdrAeCtrl.LongfrmMode = RKAIQ_AEC_HDR_LONGFRMMODE_NORMAL;
                                }
                                else if (s_value == CALIB_SENSOR_AEC_HDRAECTRL_LONGFRMMODE_LONGFRAME) {
                                    mCalibDb->aec.HdrAeCtrl.LongfrmMode = RKAIQ_AEC_HDR_LONGFRMMODE_LONG_FRAME;
                                }
                                else if (s_value == CALIB_SENSOR_AEC_HDRAECTRL_LONGFRMMODE_AUTO_LONGFRAME) {
                                    mCalibDb->aec.HdrAeCtrl.LongfrmMode = RKAIQ_AEC_HDR_LONGFRMMODE_AUTO_LONG_FRAME;
                                }
                                else {
                                    mCalibDb->aec.HdrAeCtrl.LongfrmMode = RKAIQ_AEC_HDR_LONGFRMMODE_NORMAL;
                                    redirectOut << "invalid AEC HdrAe LongFrmMode (" << s_value << ")" << std::endl;
                                }
                            }
                            else if (xmlParseReadWrite == XML_PARSER_WRITE)
                            {
                                XMLNode *pNode = (XMLNode*)psubchild;
                                if (mCalibDb->aec.HdrAeCtrl.LongfrmMode == RKAIQ_AEC_HDR_LONGFRMMODE_NORMAL) {
                                    pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_HDRAECTRL_LONGFRMMODE_NORMAL);
                                }
                                else if (mCalibDb->aec.HdrAeCtrl.LongfrmMode == RKAIQ_AEC_HDR_LONGFRMMODE_LONG_FRAME) {
                                    pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_HDRAECTRL_LONGFRMMODE_LONGFRAME);
                                }
                                else if (mCalibDb->aec.HdrAeCtrl.LongfrmMode == RKAIQ_AEC_HDR_LONGFRMMODE_AUTO_LONG_FRAME) {
                                    pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_HDRAECTRL_LONGFRMMODE_AUTO_LONGFRAME);
                                }
                                else {
                                    pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_HDRAECTRL_LONGFRMMODE_NORMAL);
                                    redirectOut << "(XML Write)invalid AEC HdrAe LongFrmMode (" << mCalibDb->aec.HdrAeCtrl.LongfrmMode << ")" << std::endl;
                                }
                            }
                        }
                        else if (subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_LONGFRMMODE_LFRMMODEEXPTH) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.HdrAeCtrl.LfrmModeExpTh, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if (subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_LONGFRMMODE_SFRMMINLINE) {
                            int no = ParseUshortArray(psubsubchild, &mCalibDb->aec.HdrAeCtrl.SfrmMinLine, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        } else {
                            redirectOut
                                    << "parse error in AEC LongFrmMode (unknow tag: "
                                    << subsubTagname
                                    << ")"
                                    << std::endl;
                        }
                        psubsubchild = psubsubchild->NextSibling();
                    }
                    autoTabBackward();
                }
                else if (subTagname == CALIB_SENSOR_AEC_HDRAECTRL_EXPRATIOCTRL) {
                    const XMLNode* psubsubchild = psubchild->ToElement()->FirstChild();
                    autoTabForward();
                    while (psubsubchild) {
                        XmlTag subsubTag = XmlTag(psubsubchild->ToElement());
                        std::string subsubTagname(psubsubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                        LOGE("%s(%d): subsubTagname = %s\n", __FUNCTION__, __LINE__, subsubTagname);
#endif
                        if ((subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_EXPRATIOTYPE)
                                && (subsubTag.isType(XmlTag::TAG_TYPE_CHAR))
                                && (subsubTag.Size() > 0)) {
                            char* value = Toupper(subsubTag.Value());
                            std::string s_value(value);
#ifdef DEBUG_LOG
                            redirectOut << "value:" << value << std::endl;
                            redirectOut << s_value << std::endl;
#endif
                            if (xmlParseReadWrite == XML_PARSER_READ)
                            {
                                if (s_value == CALIB_SENSOR_AEC_HDRAECTRL_EXPRATIOTYPE_AUTO) {
                                    mCalibDb->aec.HdrAeCtrl.ExpRatioType = RKAIQ_HDRAE_RATIOTYPE_MODE_AUTO;
                                }
                                else if (s_value == CALIB_SENSOR_AEC_HDRAECTRL_EXPRATIOTYPE_FIX) {
                                    mCalibDb->aec.HdrAeCtrl.ExpRatioType = RKAIQ_HDRAE_RATIOTYPE_MODE_FIX;
                                }
                                else {
                                    mCalibDb->aec.HdrAeCtrl.ExpRatioType = RKAIQ_HDRAE_RATIOTYPE_MODE_INVALID;
                                    redirectOut << "invalid AEC HdrAe ExpRatioType (" << s_value << ")" << std::endl;
                                }
                            }
                            else if (xmlParseReadWrite == XML_PARSER_WRITE)
                            {
                                XMLNode *pNode = (XMLNode*)psubsubchild;
                                if (mCalibDb->aec.HdrAeCtrl.ExpRatioType == RKAIQ_HDRAE_RATIOTYPE_MODE_AUTO)
                                    pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_HDRAECTRL_EXPRATIOTYPE_AUTO);
                                else if (mCalibDb->aec.HdrAeCtrl.ExpRatioType == RKAIQ_HDRAE_RATIOTYPE_MODE_FIX)
                                    pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_HDRAECTRL_EXPRATIOTYPE_FIX);
                                else
                                {
                                    pNode->FirstChild()->SetValue(CALIB_SENSOR_AEC_HDRAECTRL_EXPRATIOTYPE_AUTO);
                                    redirectOut << "(XML Write)invalid AEC HdrAe ExpRatioType (" << mCalibDb->aec.HdrAeCtrl.ExpRatioType << ")" << std::endl;
                                }
                            }
                        }
                        else if (subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_RATIOEXPDOT) {
                            int no = ParseFloatArray(psubsubchild, mCalibDb->aec.HdrAeCtrl.RatioExpDot.fCoeff, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if (subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_M2SRATIOFIX) {
                            int no = ParseFloatArray(psubsubchild, mCalibDb->aec.HdrAeCtrl.M2SRatioFix.fCoeff, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if (subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_L2MRATIOFIX) {
                            int no = ParseFloatArray(psubsubchild, mCalibDb->aec.HdrAeCtrl.L2MRatioFix.fCoeff, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if (subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_M2SRATIOMAX) {
                            int no = ParseFloatArray(psubsubchild, mCalibDb->aec.HdrAeCtrl.M2SRatioMax.fCoeff, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if (subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_L2MRATIOMAX) {
                            int no = ParseFloatArray(psubsubchild, mCalibDb->aec.HdrAeCtrl.L2MRatioMax.fCoeff, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else {
                            redirectOut
                                    << "parse error in AEC HdrAeCtrl (unknow tag: "
                                    << subsubTagname
                                    << ")"
                                    << std::endl;
                        }
                        psubsubchild = psubsubchild->NextSibling();
                    }
                    autoTabBackward();
                }
                else if (subTagname == CALIB_SENSOR_AEC_HDRAECTRL_LUMADISTTH) {
                    int no = ParseFloatArray(psubchild, &mCalibDb->aec.HdrAeCtrl.LumaDistTh, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                else if (subTagname == CALIB_SENSOR_AEC_HDRAECTRL_LFRAMECTRL) {
                    const XMLNode* psubsubchild = psubchild->ToElement()->FirstChild();
                    autoTabForward();
                    while (psubsubchild) {
                        XmlTag subsubTag = XmlTag(psubsubchild->ToElement());
                        std::string subsubTagname(psubsubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                        LOGE("%s(%d): subsubTagname = %s\n", __FUNCTION__, __LINE__, subsubTagname);
#endif
                        if (subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_OEROILOWTH) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.HdrAeCtrl.LframeCtrl.OEROILowTh, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if (subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_LVHIGHTH) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.HdrAeCtrl.LframeCtrl.LvHighTh, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if (subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_LVLOWTH) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.HdrAeCtrl.LframeCtrl.LvLowTh, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if (subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_LEXPLEVEL) {
                            int no = ParseFloatArray(psubsubchild, mCalibDb->aec.HdrAeCtrl.LframeCtrl.LExpLevel.fCoeff, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if (subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_LSETPOINT) {
                            int no = ParseFloatArray(psubsubchild, mCalibDb->aec.HdrAeCtrl.LframeCtrl.LSetPoint.fCoeff, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if (subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_TARGETLLLUMA) {
                            int no = ParseFloatArray(psubsubchild, mCalibDb->aec.HdrAeCtrl.LframeCtrl.TargetLLLuma.fCoeff, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if (subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_NONOEPDFTH) {
                            int no = ParseFloatArray(psubsubchild, mCalibDb->aec.HdrAeCtrl.LframeCtrl.NonOEPdfTh.fCoeff, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if (subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_LOWLIGHTPDFTH) {
                            int no = ParseFloatArray(psubsubchild, mCalibDb->aec.HdrAeCtrl.LframeCtrl.LowLightPdfTh.fCoeff, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else {
                            redirectOut
                                    << "parse error in AEC HdrAeCtrl LframeCtrl (unknow tag: "
                                    << subsubTagname
                                    << ")"
                                    << std::endl;
                        }
                        psubsubchild = psubsubchild->NextSibling();
                    }
                    autoTabBackward();
                }
                else if (subTagname == CALIB_SENSOR_AEC_HDRAECTRL_MFRAMECTRL) {
                    const XMLNode* psubsubchild = psubchild->ToElement()->FirstChild();
                    autoTabForward();
                    while (psubsubchild) {
                        XmlTag subsubTag = XmlTag(psubsubchild->ToElement());
                        std::string subsubTagname(psubsubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                        LOGE("%s(%d): subsubTagname = %s\n", __FUNCTION__, __LINE__, subsubTagname);
#endif
                        if (subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_MEXPLEVEL) {
                            int no = ParseFloatArray(psubsubchild, mCalibDb->aec.HdrAeCtrl.MframeCtrl.MExpLevel.fCoeff, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if (subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_MSETPOINT) {
                            int no = ParseFloatArray(psubsubchild, mCalibDb->aec.HdrAeCtrl.MframeCtrl.MSetPoint.fCoeff, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else {
                            redirectOut
                                    << "parse error in AEC HdrAeCtrl MframeCtrl (unknow tag: "
                                    << subsubTagname
                                    << ")"
                                    << std::endl;
                        }
                        psubsubchild = psubsubchild->NextSibling();
                    }
                    autoTabBackward();
                }
                else if (subTagname == CALIB_SENSOR_AEC_HDRAECTRL_SFRAMECTRL) {
                    const XMLNode* psubsubchild = psubchild->ToElement()->FirstChild();
                    autoTabForward();
                    while (psubsubchild) {
                        XmlTag subsubTag = XmlTag(psubsubchild->ToElement());
                        std::string subsubTagname(psubsubchild->ToElement()->Name());
#ifdef DEBUG_LOG
                        LOGE("%s(%d): subsubTagname = %s\n", __FUNCTION__, __LINE__, subsubTagname);
#endif
                        if (subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_SEXPLEVEL) {
                            int no = ParseFloatArray(psubsubchild, mCalibDb->aec.HdrAeCtrl.SframeCtrl.SExpLevel.fCoeff, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if (subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_HLROIEXPANDEN) {
                            uint8_t temp = mCalibDb->aec.HdrAeCtrl.SframeCtrl.HLROIExpandEn;
                            int no = ParseUcharArray(psubsubchild, &temp, subsubTag.Size());
                            mCalibDb->aec.HdrAeCtrl.SframeCtrl.HLROIExpandEn = (temp == 0) ? false : true;
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if (subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_TARGETHLLUMA) {
                            int no = ParseFloatArray(psubsubchild, mCalibDb->aec.HdrAeCtrl.SframeCtrl.TargetHLLuma.fCoeff, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if (subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_SSETPOINT) {
                            int no = ParseFloatArray(psubsubchild, mCalibDb->aec.HdrAeCtrl.SframeCtrl.SSetPoint.fCoeff, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else if (subsubTagname == CALIB_SENSOR_AEC_HDRAECTRL_HLLUMATOLERANCE) {
                            int no = ParseFloatArray(psubsubchild, &mCalibDb->aec.HdrAeCtrl.SframeCtrl.HLLumaTolerance, subsubTag.Size());
                            DCT_ASSERT((no == subsubTag.Size()));
                        }
                        else {
                            redirectOut
                                    << "parse error in AEC HdrAeCtrl SframeCtrl (unknow tag: "
                                    << subsubTagname
                                    << ")"
                                    << std::endl;
                        }
                        psubsubchild = psubsubchild->NextSibling();
                    }
                    autoTabBackward();
                }
                else {
                    redirectOut
                            << "parse error in AEC HdrAeCtrl  (unknow tag: "
                            << subTagname
                            << ")"
                            << std::endl;
                }
                psubchild = psubchild->NextSibling();
            }
            autoTabBackward();
        }
        else {
            LOGE("parse error in AEC section (unknow tag:%s)", tagname.c_str());
            return (false);
        }

        pchild = pchild->NextSibling();
    }

    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();

    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAecLinearAeRoute
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);


    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();

    int nTimeDot = 0;
    int nGainDot = 0;
    int nIspDGainDot = 0;
    int nPIrisDot = 0;

    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string Tagname(pchild->ToElement()->Name());
#ifdef DEBUG_LOG
        redirectOut << "Tagname: " << Tagname << ",index:" << index << std::endl;
#endif

        if ((Tagname == CALIB_SENSOR_AEC_NAME)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            char* value = Toupper(tag.Value());
            ParseString(pchild,
                        mCalibDb->aec.CommCtrl.stAeRoute.LinAeSeperate[index].name,
                        sizeof(mCalibDb->aec.CommCtrl.stAeRoute.LinAeSeperate[index].name));
#ifdef DEBUG_LOG
            redirectOut << "value:" << value << std::endl;
            redirectOut << mCalibDb->aec.CommCtrl.stAeRoute.LinAeSeperate[index].name << std::endl;
#endif
        }
        else if ((Tagname == CALIB_SENSOR_AEC_TIMEDOT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->aec.CommCtrl.stAeRoute.LinAeSeperate[index].TimeDot, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            if(tag.Size() > AEC_ROUTE_MAX_NODES) {
                LOGE("array size: %d out of Maximum range(%d)", tag.Size(), AEC_ROUTE_MAX_NODES);
                return false;
            }
            nTimeDot = no;
        }
        else if ((Tagname == CALIB_SENSOR_AEC_GAINDOT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->aec.CommCtrl.stAeRoute.LinAeSeperate[index].GainDot, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            if(tag.Size() > AEC_ROUTE_MAX_NODES) {
                LOGE("array size: %d out of Maximum range(%d)", tag.Size(), AEC_ROUTE_MAX_NODES);
                return false;
            }
            nGainDot = no;
        }
        else if ((Tagname == CALIB_SENSOR_AEC_ISPDGAINDOT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->aec.CommCtrl.stAeRoute.LinAeSeperate[index].IspgainDot, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            if(tag.Size() > AEC_ROUTE_MAX_NODES) {
                LOGE("array size: %d out of Maximum range(%d)", tag.Size(), AEC_ROUTE_MAX_NODES);
                return false;
            }
            nIspDGainDot = no;
        }
        else if ((Tagname == CALIB_SENSOR_AEC_PIRISDOT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->aec.CommCtrl.stAeRoute.LinAeSeperate[index].PIrisDot, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            if(tag.Size() > AEC_ROUTE_MAX_NODES) {
                LOGE("array size: %d out of Maximum range(%d)", tag.Size(), AEC_ROUTE_MAX_NODES);
                return false;
            }
            nPIrisDot = no;
        }
        else {
            redirectOut
                    << "parse error in LinAe Route (unknow tag: "
                    << Tagname
                    << ")"
                    << std::endl;
        }
        pchild = pchild->NextSibling();
    }
    DCT_ASSERT((nGainDot == nTimeDot));
    DCT_ASSERT((nGainDot == nIspDGainDot));
    mCalibDb->aec.CommCtrl.stAeRoute.LinAeSeperate[index].array_size = nTimeDot;
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAecHdrAeRoute
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);


    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    int nTimeDot = 0;
    int nGainDot = 0;
    int nIspDGainDot = 0;
    int nPIrisDot = 0;

    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string Tagname(pchild->ToElement()->Name());

        if ((Tagname == CALIB_SENSOR_AEC_NAME)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            char* value = Toupper(tag.Value());
            ParseString(pchild,
                        mCalibDb->aec.CommCtrl.stAeRoute.HdrAeSeperate[index].name,
                        sizeof(mCalibDb->aec.CommCtrl.stAeRoute.HdrAeSeperate[index].name));
#ifdef DEBUG_LOG
            redirectOut << "value:" << value << std::endl;
            redirectOut << mCalibDb->aec.CommCtrl.stAeRoute.HdrAeSeperate[index].name << std::endl;
#endif
        }
        else if ((Tagname == CALIB_SENSOR_AEC_LTIMEDOT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->aec.CommCtrl.stAeRoute.HdrAeSeperate[index].HdrTimeDot[2], tag.Size());
            DCT_ASSERT((no == tag.Size()));
            if(tag.Size() > AEC_ROUTE_MAX_NODES) {
                LOGE("array size: %d out of Maximum range(%d)", tag.Size(), AEC_ROUTE_MAX_NODES);
                return false;
            }
            nTimeDot = no;
        }
        else if ((Tagname == CALIB_SENSOR_AEC_MTIMEDOT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->aec.CommCtrl.stAeRoute.HdrAeSeperate[index].HdrTimeDot[1], tag.Size());
            DCT_ASSERT((no == tag.Size()));
            if(tag.Size() > AEC_ROUTE_MAX_NODES) {
                LOGE("array size: %d out of Maximum range(%d)", tag.Size(), AEC_ROUTE_MAX_NODES);
                return false;
            }
            DCT_ASSERT((nTimeDot == no));
        }
        else if ((Tagname == CALIB_SENSOR_AEC_STIMEDOT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->aec.CommCtrl.stAeRoute.HdrAeSeperate[index].HdrTimeDot[0], tag.Size());
            DCT_ASSERT((no == tag.Size()));
            if(tag.Size() > AEC_ROUTE_MAX_NODES) {
                LOGE("array size: %d out of Maximum range(%d)", tag.Size(), AEC_ROUTE_MAX_NODES);
                return false;
            }
            DCT_ASSERT((nTimeDot == no));
        }
        else if ((Tagname == CALIB_SENSOR_AEC_LGAINDOT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->aec.CommCtrl.stAeRoute.HdrAeSeperate[index].HdrGainDot[2], tag.Size());
            DCT_ASSERT((no == tag.Size()));
            if(tag.Size() > AEC_ROUTE_MAX_NODES) {
                LOGE("array size: %d out of Maximum range(%d)", tag.Size(), AEC_ROUTE_MAX_NODES);
                return false;
            }
            nGainDot = no;
        }
        else if ((Tagname == CALIB_SENSOR_AEC_MGAINDOT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->aec.CommCtrl.stAeRoute.HdrAeSeperate[index].HdrGainDot[1], tag.Size());
            DCT_ASSERT((no == tag.Size()));
            if(tag.Size() > AEC_ROUTE_MAX_NODES) {
                LOGE("array size: %d out of Maximum range(%d)", tag.Size(), AEC_ROUTE_MAX_NODES);
                return false;
            }
            DCT_ASSERT((no == nGainDot));
        }
        else if ((Tagname == CALIB_SENSOR_AEC_SGAINDOT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->aec.CommCtrl.stAeRoute.HdrAeSeperate[index].HdrGainDot[0], tag.Size());
            DCT_ASSERT((no == tag.Size()));
            if(tag.Size() > AEC_ROUTE_MAX_NODES) {
                LOGE("array size: %d out of Maximum range(%d)", tag.Size(), AEC_ROUTE_MAX_NODES);
                return false;
            }
            DCT_ASSERT((no == nGainDot));
        }
        else if ((Tagname == CALIB_SENSOR_AEC_LISPDGAINDOT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->aec.CommCtrl.stAeRoute.HdrAeSeperate[index].HdrIspDGainDot[2], tag.Size());
            DCT_ASSERT((no == tag.Size()));
            if(tag.Size() > AEC_ROUTE_MAX_NODES) {
                LOGE("array size: %d out of Maximum range(%d)", tag.Size(), AEC_ROUTE_MAX_NODES);
                return false;
            }
            nIspDGainDot = no;
        }
        else if ((Tagname == CALIB_SENSOR_AEC_MISPDGAINDOT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->aec.CommCtrl.stAeRoute.HdrAeSeperate[index].HdrIspDGainDot[1], tag.Size());
            DCT_ASSERT((no == tag.Size()));
            if(tag.Size() > AEC_ROUTE_MAX_NODES) {
                LOGE("array size: %d out of Maximum range(%d)", tag.Size(), AEC_ROUTE_MAX_NODES);
                return false;
            }
            DCT_ASSERT((nIspDGainDot == no));
        }
        else if ((Tagname == CALIB_SENSOR_AEC_SISPDGAINDOT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->aec.CommCtrl.stAeRoute.HdrAeSeperate[index].HdrIspDGainDot[0], tag.Size());
            DCT_ASSERT((no == tag.Size()));
            if(tag.Size() > AEC_ROUTE_MAX_NODES) {
                LOGE("array size: %d out of Maximum range(%d)", tag.Size(), AEC_ROUTE_MAX_NODES);
                return false;
            }
            DCT_ASSERT((nIspDGainDot == no));
        }
        else if ((Tagname == CALIB_SENSOR_AEC_PIRISDOT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->aec.CommCtrl.stAeRoute.HdrAeSeperate[index].PIrisDot, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            if(tag.Size() > AEC_ROUTE_MAX_NODES) {
                LOGE("array size: %d out of Maximum range(%d)", tag.Size(), AEC_ROUTE_MAX_NODES);
                return false;
            }
            nPIrisDot = no;
        }
        else {
            redirectOut
                    << "parse error in HdrAe Route (unknow tag: "
                    << Tagname
                    << ")"
                    << std::endl;
        }
        pchild = pchild->NextSibling();
    }
    DCT_ASSERT((nGainDot == nTimeDot));
    DCT_ASSERT((nGainDot == nIspDGainDot));
    mCalibDb->aec.CommCtrl.stAeRoute.HdrAeSeperate[index].array_size = nTimeDot;
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAecLinearAeDynamicPoint
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);


    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();

    int nExpValue = 0;
    int nDysetpoint = 0;
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string Tagname(pchild->ToElement()->Name());
        INFO_PRINT(Tagname);
        if ((Tagname == CALIB_SENSOR_AEC_NAME)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            char* value = Toupper(tag.Value());
            ParseString(pchild,
                        mCalibDb->aec.LinearAeCtrl.DySetpoint[index].name,
                        sizeof(mCalibDb->aec.LinearAeCtrl.DySetpoint[index].name));
#ifdef DEBUG_LOG
            redirectOut << "value:" << value << std::endl;
            redirectOut << mCalibDb->aec.LinearAeCtrl.DySetpoint[index].name << std::endl;
#endif
        }
        else if ((Tagname == CALIB_SENSOR_AEC_EXPLEVEL)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->aec.LinearAeCtrl.DySetpoint[index].ExpValue, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            if(tag.Size() > AEC_SETPOINT_MAX_NODES) {
                LOGE("array size: %d out of Maximum range(%d)", tag.Size(), AEC_SETPOINT_MAX_NODES);
                return false;
            }
            nExpValue = no;
        }
        else if ((Tagname == CALIB_SENSOR_AEC_DYSETPOINT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->aec.LinearAeCtrl.DySetpoint[index].DySetpoint, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            if(tag.Size() > AEC_SETPOINT_MAX_NODES) {
                LOGE("array size: %d out of Maximum range(%d)", tag.Size(), AEC_SETPOINT_MAX_NODES);
                return false;
            }
            nDysetpoint = no;
        }
        else {
            redirectOut
                    << "parse error in DynamicPoint (unknow tag: "
                    << Tagname
                    << ")"
                    << std::endl;
        }
        pchild = pchild->NextSibling();
    }
    DCT_ASSERT((nDysetpoint == nExpValue));
    mCalibDb->aec.LinearAeCtrl.DySetpoint[index].array_size = nDysetpoint;

    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorInfo
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGI("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());
#ifdef DEBUG_LOG
        LOGE("%s(%d): Tagname = %s\n", __FUNCTION__, __LINE__, tagname);
#endif
        if (tagname == CALIB_SENSOR_SENSORINFO_GAINRANGE
                && (tag.Size() > 0)) {
            int i = tag.Size();
            int no = ParseFloatArray(pchild, mCalibDb->sensor.GainRange.pGainRange, i);
            DCT_ASSERT((no == tag.Size()));
            DCT_ASSERT((no <= CALD_AEC_GAIN_RANGE_MAX_LEN));
            DCT_ASSERT(((i % 7) == 0));
            mCalibDb->sensor.GainRange.array_size = i;
        }
        else if (tagname == CALIB_SENSOR_SENSORINFO_TIMEFACTOR) {
            int i = (sizeof(mCalibDb->sensor.TimeFactor) / sizeof(mCalibDb->sensor.TimeFactor[0]));
            int no = ParseFloatArray(pchild, mCalibDb->sensor.TimeFactor, i);
            DCT_ASSERT((no == tag.Size()));
        }
        else if (tagname == CALIB_SENSOR_SENSORINFO_CISTIMEREG_SUMFAC
                 && (tag.Size() > 0)) {
            int i = (sizeof(mCalibDb->sensor.CISTimeRegSumFac) / sizeof(mCalibDb->sensor.CISTimeRegSumFac.fCoeff[0]));
            int no = ParseFloatArray(pchild, mCalibDb->sensor.CISTimeRegSumFac.fCoeff, i);
            DCT_ASSERT((no == tag.Size()));
        }
        else if (tagname == CALIB_SENSOR_SENSORINFO_CISTIMEREG_ODEVITY
                 && (tag.Size() > 0)) {
            int i = (sizeof(mCalibDb->sensor.CISTimeRegOdevity) / sizeof(mCalibDb->sensor.CISTimeRegOdevity.fCoeff[0]));
            int no = ParseFloatArray(pchild, mCalibDb->sensor.CISTimeRegOdevity.fCoeff, i);
            DCT_ASSERT((no == tag.Size()));
        }
        else if (tagname == CALIB_SENSOR_SENSORINFO_CISTIMEREG_UNEQUALEN
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->sensor.CISTimeRegUnEqualEn, 1);
            DCT_ASSERT((no == tag.Size()));
        }
        else if (tagname == CALIB_SENSOR_SENSORINFO_CISTIMEREG_MIN
                 && (tag.Size() > 0)) {
            int no = ParseUshortArray(pchild, &mCalibDb->sensor.CISTimeRegMin, 1);
            DCT_ASSERT((no == tag.Size()));
        }
        else if (tagname == CALIB_SENSOR_SENSORINFO_CISMINFPS
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->sensor.CISMinFps, 1);
            DCT_ASSERT((no == tag.Size()));
        }
        else if (tagname == CALIB_SENSOR_SENSORINFO_CISAGAIN_RANGE
                 && (tag.Size() > 0)) {
            float tempVal[2] = { mCalibDb->sensor.CISAgainRange.Min, mCalibDb->sensor.CISAgainRange.Max };
            int no = ParseFloatArray(pchild, tempVal, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->sensor.CISAgainRange.Min = tempVal[0];
            mCalibDb->sensor.CISAgainRange.Max = tempVal[1];
        }
        else if (tagname == CALIB_SENSOR_SENSORINFO_CISEXTRAAGAIN_RANGE
                 && (tag.Size() > 0)) {
            float tempVal[2] = { mCalibDb->sensor.CISExtraAgainRange.Min, mCalibDb->sensor.CISExtraAgainRange.Max };
            int no = ParseFloatArray(pchild, tempVal, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->sensor.CISExtraAgainRange.Min = tempVal[0];
            mCalibDb->sensor.CISExtraAgainRange.Max = tempVal[1];
        }
        else if (tagname == CALIB_SENSOR_SENSORINFO_CISDGAIN_RANGE
                 && (tag.Size() > 0)) {
            float tempVal[2] = { mCalibDb->sensor.CISDgainRange.Min, mCalibDb->sensor.CISDgainRange.Max };
            int no = ParseFloatArray(pchild, tempVal, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->sensor.CISDgainRange.Min = tempVal[0];
            mCalibDb->sensor.CISDgainRange.Max = tempVal[1];
        }
        else if (tagname == CALIB_SENSOR_SENSORINFO_CISISPDGAIN_RANGE
                 && (tag.Size() > 0)) {
            float tempVal[2] = { mCalibDb->sensor.CISIspDgainRange.Min, mCalibDb->sensor.CISIspDgainRange.Max };
            int no = ParseFloatArray(pchild, tempVal, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->sensor.CISIspDgainRange.Min = tempVal[0];
            mCalibDb->sensor.CISIspDgainRange.Max = tempVal[1];
        }
        else {
            redirectOut
                    << "parse error in SensorInfo section (unknow tag: "
                    << tagname
                    << ")"
                    << std::endl;
        }
        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAhdr
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if (tagname == CALIB_SENSOR_AHDR_MERGE) {
            if (!parseEntrySensorAhdrMerge(pchild->ToElement())) {
                LOGE("parse error in AHDR Merge (%s)", tagname);
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_AHDR_TMO) {
            if (!parseEntrySensorAhdrTmo(pchild->ToElement())) {
                LOGE("parse error in AHDR Tmo (%s)", tagname);
                return (false);
            }
        }
        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAhdrMerge
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string Tagname(pchild->ToElement()->Name());

        if ((Tagname == CALIB_SENSOR_AHDR_ENVLV)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->ahdr.merge.envLevel, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_MERGE_OECURVE_SMOOTH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->ahdr.merge.oeCurve_smooth, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_MERGE_OECURVE_OFFSET)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->ahdr.merge.oeCurve_offset, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_MERGE_MOVECOEF)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->ahdr.merge.moveCoef, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_MERGE_MDCURVELM_SMOOTH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->ahdr.merge.mdCurveLm_smooth, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_MERGE_MDCURVELM_OFFSET)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->ahdr.merge.mdCurveLm_offset, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_MERGE_MDCURVEMS_SMOOTH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->ahdr.merge.mdCurveMs_smooth, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_MERGE_MDCURVEMS_OFFSET)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->ahdr.merge.mdCurveMs_offset, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_MERGE_OECURVE_DAMP)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->ahdr.merge.oeCurve_damp, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_MERGE_MDCURVELM_DAMP)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->ahdr.merge.mdCurveLm_damp, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_MERGE_MDCURVEMS_DAMP)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->ahdr.merge.mdCurveMs_damp, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }

    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAhdrTmo
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if (tagname == CALIB_SENSOR_AHDR_TMO_GLOBALLUMA) {
            if (!parseEntrySensorAhdrTmoGlobalLuma(pchild->ToElement())) {
                LOGE("parse error in AHDR Tmo GlobalLuma (%s)", tagname);
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_AHDR_TMO_DETAILSHIGHLIGHT) {
            if (!parseEntrySensorAhdrTmoDetailsHighLight(pchild->ToElement())) {
                LOGE("parse error in AHDR Tmo DetailsHighLight(%s)", tagname);
                return (false);
            }

        }
        else if (tagname == CALIB_SENSOR_AHDR_TMO_DETAILSLOWLIGHT) {
            if (!parseEntrySensorAhdrTmoDetailsLowLight(pchild->ToElement())) {
                LOGE("parse error in AHDR Tmo DetailsLowLight(%s)", tagname);
                return (false);
            }

        }
        else if (tagname == CALIB_SENSOR_AHDR_TMO_TMOCONTRAST) {
            if (!parseEntrySensorAhdrTmoContrast(pchild->ToElement())) {
                LOGE("parse error in AHDR Tmo TmoContrast(%s)", tagname);
                return (false);
            }

        }
        else if (tagname == CALIB_SENSOR_AHDR_TMO_MORESETTING) {
            if (!parseEntrySensorAhdrTmoMoreSetting(pchild->ToElement())) {
                LOGE("parse error in AHDR Tmo MoreSetting(%s)", tagname);
                return (false);
            }

        }
        pchild = pchild->NextSibling();
    }


    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAhdrTmoGlobalLuma
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string Tagname(pchild->ToElement()->Name());

        if ((Tagname == CALIB_SENSOR_AHDR_TMO_GLOBALLUMAMODE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->ahdr.tmo.luma.GlobalLumaMode, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_ENVLV)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->ahdr.tmo.luma.envLevel, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_TMO_ISO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->ahdr.tmo.luma.ISO, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_TOLERANCE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->ahdr.tmo.luma.Tolerance, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_TMO_GLOBALLUMA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->ahdr.tmo.luma.globalLuma, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }

    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAhdrTmoDetailsHighLight
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string Tagname(pchild->ToElement()->Name());

        if ((Tagname == CALIB_SENSOR_AHDR_TMO_DETAILSHIGHLIGHTMODE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->ahdr.tmo.HighLight.DetailsHighLightMode, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_TMO_OEPDF)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->ahdr.tmo.HighLight.OEPdf, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_ENVLV)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->ahdr.tmo.HighLight.EnvLv, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_TOLERANCE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->ahdr.tmo.HighLight.Tolerance, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_TMO_DETAILSHIGHLIGHT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->ahdr.tmo.HighLight.detailsHighLight, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }

    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAhdrTmoDetailsLowLight
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string Tagname(pchild->ToElement()->Name());



        if ((Tagname == CALIB_SENSOR_AHDR_TMO_DETAILSLOWLIGHTMODE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->ahdr.tmo.LowLight.DetailsLowLightMode, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_TMO_FOCUSLUMA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->ahdr.tmo.LowLight.FocusLuma, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_TMO_DARKPDF)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->ahdr.tmo.LowLight.DarkPdf, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_TMO_ISO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->ahdr.tmo.LowLight.ISO, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_TOLERANCE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->ahdr.tmo.LowLight.Tolerance, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_TMO_DETAILSLOWLIGHT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->ahdr.tmo.LowLight.detailsLowLight, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }

    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAhdrTmoContrast
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string Tagname(pchild->ToElement()->Name());

        if ((Tagname == CALIB_SENSOR_AHDR_TMO_TMOCONTRASTMODE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->ahdr.tmo.Contrast.TmoContrastMode, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_TMO_DYNAMICRANGE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->ahdr.tmo.Contrast.DynamicRange, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_ENVLV)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->ahdr.tmo.Contrast.EnvLv, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_TOLERANCE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->ahdr.tmo.Contrast.Tolerance, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_TMO_TMOCONTRAST)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->ahdr.tmo.Contrast.TmoContrast, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }

    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAhdrTmoMoreSetting
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string Tagname(pchild->ToElement()->Name());

        if ((Tagname == CALIB_SENSOR_AHDR_TMO_CLIPGAP0)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->ahdr.tmo.MoreSetting.clipgap0, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_TMO_CLIPGAP1)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->ahdr.tmo.MoreSetting.clipgap1, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_TMO_CLIPRATIO0)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->ahdr.tmo.MoreSetting.clipratio0, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_TMO_CLIPRATIO1)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->ahdr.tmo.MoreSetting.clipratio1, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_AHDR_TMO_DAMP)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->ahdr.tmo.MoreSetting.damp, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }

    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}


bool RkAiqCalibParser::parseEntrySensorBlc
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if (tagname == CALIB_SENSOR_BLC_ENABLE) {
            int no = ParseIntArray(pchild, &mCalibDb->blc.enable, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if (tagname == CALIB_SENSOR_BLC_ISO) {
            int no = ParseFloatArray(pchild, mCalibDb->blc.iso, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if (tagname == CALIB_SENSOR_BLC_BLACK_LEVEL) {
            int no = ParseFloatArray(pchild, (float *)mCalibDb->blc.level, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorLut3d
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if (tagname == CALIB_SENSOR_LUT3D_ENABLE) {
            unsigned char enable = mCalibDb->lut3d.enable;
            int no = ParseUcharArray(pchild, &enable, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->lut3d.enable = enable;
        }
        else if (tagname == CALIB_SENSOR_LUT3D_R_LUT) {
            int no = ParseUshortArray(pchild, mCalibDb->lut3d.look_up_table_r, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if (tagname == CALIB_SENSOR_LUT3D_G_LUT) {
            int no = ParseUshortArray(pchild, mCalibDb->lut3d.look_up_table_g, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if (tagname == CALIB_SENSOR_LUT3D_B_LUT) {
            int no = ParseUshortArray(pchild, mCalibDb->lut3d.look_up_table_b, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorDpcc
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD( "%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_DPCC_ENABLE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseIntArray(pchild, &mCalibDb->dpcc.enable, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_VERSION)
                   && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                   && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->dpcc.version,  sizeof(mCalibDb->dpcc.version));
        } else if ((tagname == CALIB_SENSOR_DPCC_ISO)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->dpcc.iso, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_STAGE1_ENABLE
                    && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                    && (tag.Size() > 0)) ) {
            int no = ParseUcharArray(pchild, mCalibDb->dpcc.stage1_Enable, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_GRAYSCALE_MODE)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, mCalibDb->dpcc.grayscale_mode, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_RK_OUT_SEL)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, mCalibDb->dpcc.rk_out_sel, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_DPCC_OUT_SEL)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, mCalibDb->dpcc.dpcc_out_sel, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_STAGE1_RB_3X3)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, mCalibDb->dpcc.stage1_rb_3x3, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_STAGE1_G_3X3)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, mCalibDb->dpcc.stage1_g_3x3, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_STAGE1_INC_RB_CENTER)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, mCalibDb->dpcc.stage1_inc_rb_center, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_STAGE1_INC_G_CENTER)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, mCalibDb->dpcc.stage1_inc_g_center, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_STAGE1_USE_FIX_SET)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, mCalibDb->dpcc.stage1_use_fix_set, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_STAGE1_USE_SET3)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, mCalibDb->dpcc.stage1_use_set3, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_STAGE1_USE_SET2)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, mCalibDb->dpcc.stage1_use_set2, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }  else if ((tagname == CALIB_SENSOR_DPCC_STAGE1_USE_SET1)
                    && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                    && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, mCalibDb->dpcc.stage1_use_set1, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_SET_CELL)
                   && (tag.isType(XmlTag::TAG_TYPE_CELL))
                   && (tag.Size() > 0)) {
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorDpccSetCell)) {
                LOGE("parse error in dpcc set cell (%s)", tagname);
                return (false);
            }
        } else if ((tagname == CALIB_SENSOR_DPCC_PDAF)
                   && (tag.isType(XmlTag::TAG_TYPE_CELL))
                   && (tag.Size() > 0)) {
            if (!parseEntrySensorDpccPdaf(pchild->ToElement())) {
                return (false);
            }
        } else if (tagname == CALIB_SENSOR_DPCC_SENSOR) {
            if (!parseEntrySensorDpccSensor(pchild->ToElement())) {
                LOGE("parse error in DPCC Sensor Dpcc (%s)", tagname);
                return (false);
            }
        }
        pchild = pchild->NextSibling();
    }
    LOGD( "%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorDpccSetCell
(
    const XMLElement*   pelement,
    void*               param
) {
    LOGD( "%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_DPCC_RED_BLUE_ENABLE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            unsigned char enable = mCalibDb->lut3d.enable;
            int no = ParseUcharArray(pchild, (uint8_t *)mCalibDb->dpcc.set[index].red_blue_enable, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->lut3d.enable = enable;
        } else if ((tagname == CALIB_SENSOR_DPCC_GREEN_ENABLE)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, (uint8_t *)mCalibDb->dpcc.set[index].green_enable, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_LINE_THRESH)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, (uint8_t *)mCalibDb->dpcc.set[index].line_thresh, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_LINE_MAD_FAC)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, (uint8_t *)mCalibDb->dpcc.set[index].line_mad_fac, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_PG_FAC)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, (uint8_t *)mCalibDb->dpcc.set[index].pg_fac, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_RND_THRESH)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, (uint8_t *)mCalibDb->dpcc.set[index].rnd_thresh, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_RG_FAC)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, (uint8_t *)mCalibDb->dpcc.set[index].rg_fac, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_RG_LIM)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, (uint8_t *)mCalibDb->dpcc.set[index].rg_lim, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_RND_OFFS)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, (uint8_t *)mCalibDb->dpcc.set[index].rnd_offs, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }

        pchild = pchild->NextSibling();
    }
    LOGD( "%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorDpccPdaf
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD( "%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_DPCC_PDAF_ENABLE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->dpcc.pdaf.en, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_PDAF_POINT_ENABLE)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, mCalibDb->dpcc.pdaf.point_en, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_PDAF_OFFSETX)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUshortArray(pchild, &mCalibDb->dpcc.pdaf.offsetx, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_PDAF_OFFSETY)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUshortArray(pchild, &mCalibDb->dpcc.pdaf.offsety, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_PDAF_WRAPX)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->dpcc.pdaf.wrapx, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_PDAF_WRAPY)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->dpcc.pdaf.wrapy, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_PDAF_WRAPX_NUM)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUshortArray(pchild, &mCalibDb->dpcc.pdaf.wrapx_num, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_PDAF_WRAPY_NUM)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUshortArray(pchild, &mCalibDb->dpcc.pdaf.wrapy_num, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_PDAF_POINT_X)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, mCalibDb->dpcc.pdaf.point_x, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_PDAF_POINT_Y)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, mCalibDb->dpcc.pdaf.point_y, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DPCC_PDAF_POINT_FORWARD_MED)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->dpcc.pdaf.forward_med, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }

    }

    LOGD( "%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorDpccSensor
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string Tagname(pchild->ToElement()->Name());

        if ((Tagname == CALIB_SENSOR_DPCC_SENSOR_AUTO_ENABLE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dpcc.sensor_dpcc.en, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_DPCC_SENSOR_MAX_LEVEL)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dpcc.sensor_dpcc.max_level, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_DPCC_ISO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->dpcc.sensor_dpcc.iso, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_DPCC_SENSOR_LEVEL_SINGLE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->dpcc.sensor_dpcc.level_single, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((Tagname == CALIB_SENSOR_DPCC_SENSOR_LEVEL_MULTIPLE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->dpcc.sensor_dpcc.level_multiple, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }

    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorBayerNrSetting
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_BAYERNR_SETTING_NAME)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            ParseString(pchild,  mCalibDb->bayerNr.setting[index].name, sizeof(mCalibDb->bayerNr.setting[index].name));
        } else if ((tagname == CALIB_SENSOR_BAYERNR_SETTING_MODE)
                   && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                   && (tag.Size() > 0)) {
            ParseString(pchild,  mCalibDb->bayerNr.setting[index].mode, sizeof(mCalibDb->bayerNr.setting[index].mode));
        } else if ((tagname == CALIB_SENSOR_BAYERNR_ISO)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->bayerNr.setting[index].iso, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_BAYERNR_FILTPARA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->bayerNr.setting[index].filtPara, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_BAYERNR_LURATIO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, (float*)mCalibDb->bayerNr.setting[index].luRatio, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_BAYERNR_FIXW)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, (float*)mCalibDb->bayerNr.setting[index].fixW, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_BAYERNR_LULEVEL)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->bayerNr.setting[index].luLevel, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_BAYERNR_LULEVELVAL)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->bayerNr.setting[index].luLevelVal, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_BAYERNR_LAMDA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->bayerNr.setting[index].lamda, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_BAYERNR_GAUSS_EN)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->bayerNr.setting[index].gauss_en, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_BAYERNR_RGAINOFF)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->bayerNr.setting[index].RGainOff, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_BAYERNR_RGAINFILP)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->bayerNr.setting[index].RGainFilp, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_BAYERNR_BGAINOFF)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->bayerNr.setting[index].BGainOff, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_BAYERNR_BGAINFILP)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->bayerNr.setting[index].BGainFilp, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_BAYERNR_EDGESOFTNESS)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->bayerNr.setting[index].edgeSoftness, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_BAYERNR_GAUSSWEIGHT0)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->bayerNr.setting[index].gaussWeight0, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_BAYERNR_GAUSSWEIGHT1)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->bayerNr.setting[index].gaussWeight1, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_BAYERNR_BILEDGEFILTER)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->bayerNr.setting[index].bilEdgeFilter, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_BAYERNR_BILFILTERSTRENG)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->bayerNr.setting[index].bilFilterStreng, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_BAYERNR_BILEDGESOFTRATIO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->bayerNr.setting[index].bilEdgeSoftRatio, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_BAYERNR_BILREGWGT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->bayerNr.setting[index].bilRegWgt, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_BAYERNR_BILEDGESOFT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->bayerNr.setting[index].bilEdgeSoft, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}


bool RkAiqCalibParser::parseEntrySensorBayerNr
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_BAYERNR_ENABLE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseIntArray(pchild, &mCalibDb->bayerNr.enable, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_BAYERNR_VERSION)
                   && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                   && (tag.Size() > 0)) {
            ParseString(pchild,  mCalibDb->bayerNr.version, sizeof(mCalibDb->bayerNr.version));
        } else if (tagname == CALIB_SENSOR_BAYERNR_SETTING) {
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorBayerNrSetting)) {
                LOGE("parse error in BayerNR setting (%s)\n", tagname);
                return (false);
            }
        }

        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorLsc
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    mCalibDb->lsc.tableAllNum = 0;
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if (tagname == CALIB_SENSOR_LSC_ENABLE) {
            uint8_t enable = mCalibDb->lsc.enable;
            int no = ParseUcharArray(pchild, &enable, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->lsc.enable = bool(enable);
            //mCalibDb->lsc.damp_enable = damp_enable;
        }
        else if (tagname == CALIB_SENSOR_LSC_DAMP_ENABLE) {
            uint8_t damp_enable = mCalibDb->lsc.damp_enable;
            int no = ParseUcharArray(pchild, &damp_enable, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->lsc.damp_enable = bool(damp_enable);
            //mCalibDb->lsc.damp_enable = damp_enable;
        }
        else if (tagname == CALIB_SENSOR_LSC_ALSCCOF) {
            if (!parseEntrySensorLscAlscCof(pchild->ToElement())) {
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_LSC_TABLEALL) {
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorLscTableAll)) {
                LOGE("parse error in LSC tableAll (%s)", tagname);
                return (false);
            }
        }
        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}
bool RkAiqCalibParser::parseEntrySensorLscAlscCof
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    memset(mCalibDb->lsc.aLscCof.illuNum,0,sizeof(mCalibDb->lsc.aLscCof.illuNum));
    mCalibDb->lsc.aLscCof.lscResNum = 0;

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if (tagname == CALIB_SENSOR_LSC_ALSCCOF_IllALL) {
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorLscAlscCofIllAll)) {
                LOGE("parse error in LSC aLscCof illAll (%s)", tagname);
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_LSC_ALSCCOF_RESALL) {

            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorLscAlscCofResAll)) {
                LOGE("parse error in LSC aLscCof resolutionAll(%s)", tagname);
                return (false);
            }
        }

        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}
bool RkAiqCalibParser::parseEntrySensorLscAlscCofResAll
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);


    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        const char* value = tag.Value();
        std::string tagname(pchild->ToElement()->Name());
        if ((tagname == CALIB_SENSOR_LSC_ALSCCOF_NAME)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            ParseString(pchild,
                        mCalibDb->lsc.aLscCof.lscResName[index],
                        sizeof(mCalibDb->lsc.aLscCof.lscResName[index]));//check
        }
        pchild = pchild->NextSibling();
    }
    mCalibDb->lsc.aLscCof.lscResNum++;
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorLscAlscCofIllAll
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);


    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    int no1 = 0;
    int no2 = 0;
    int usedForCase = 0;
    index = mCalibDb->lsc.aLscCof.illuNum[usedForCase];

    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        const char* value = tag.Value();
        std::string tagname(pchild->ToElement()->Name());

       if ((tagname == CALIB_SENSOR_LSC_ALSCCOF_CASE)
                       && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                       && (tag.Size() > 0)) {
                  int no = ParseIntArray(pchild, &usedForCase, tag.Size());
                  index = mCalibDb->lsc.aLscCof.illuNum[usedForCase];
                  DCT_ASSERT((no == tag.Size()));
       }
       else if ((tagname == CALIB_SENSOR_LSC_ALSCCOF_NAME)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            ParseString(pchild,
                        mCalibDb->lsc.aLscCof.illAll[usedForCase][index].illuName,
                        sizeof(mCalibDb->lsc.aLscCof.illAll[usedForCase][index].illuName));
        }
        else if ((tagname == CALIB_SENSOR_LSC_ALSCCOF_WBGAIN)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->lsc.aLscCof.illAll[usedForCase][index].awbGain, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_LSC_ALSCCOF_TABLEUSED)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            char* lsc_profiles = Toupper(value);
            int no = ParseLscProfileArray(pchild, mCalibDb->lsc.aLscCof.illAll[usedForCase][index].tableUsed, LSC_PROFILES_NUM_MAX);
            DCT_ASSERT((no <= LSC_PROFILES_NUM_MAX));
            mCalibDb->lsc.aLscCof.illAll[usedForCase][index].tableUsedNO = no;
        }
        else if ((tagname == CALIB_SENSOR_LSC_ALSCCOF_GAINS)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            no1 = ParseFloatArray(pchild, mCalibDb->lsc.aLscCof.illAll[usedForCase][index].vignettingCurve.pSensorGain, tag.Size());
            mCalibDb->lsc.aLscCof.illAll[usedForCase][index].vignettingCurve.arraySize = no1;
            DCT_ASSERT((no1 == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_LSC_ALSCCOF_VIG)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            no2 = ParseFloatArray(pchild, mCalibDb->lsc.aLscCof.illAll[usedForCase][index].vignettingCurve.pVignetting, tag.Size());
            mCalibDb->lsc.aLscCof.illAll[usedForCase][index].vignettingCurve.arraySize = no2;
            DCT_ASSERT((no2 == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }

    DCT_ASSERT(no1 == no2);
    mCalibDb->lsc.aLscCof.illuNum[usedForCase]++;
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}
static int AwbLscGradientCheck
(
    CalibDb_LscTableProfile_t* pLscProfile
) {
    uint32_t i;

    LOGI("%s: (enter)\n", __FUNCTION__);

    if (NULL == pLscProfile) {
        return (1);
    }

    if (0 < pLscProfile->LscYo) {
        for (i = 0; i < (sizeof(pLscProfile->LscYGradTbl) / sizeof(pLscProfile->LscYGradTbl[0])); ++i) {
            if (0 < pLscProfile->LscYSizeTbl[i]) {
                pLscProfile->LscYGradTbl[i] = (uint16_t)((double)(1UL << pLscProfile->LscYo) / pLscProfile->LscYSizeTbl[i] + 0.5);
            }
            else {
                return (2);
            }
        }
    }
    else {
        memset(pLscProfile->LscYGradTbl, 0, sizeof(pLscProfile->LscYGradTbl));
    }

    if (0 < pLscProfile->LscXo) {
        for (i = 0; i < (sizeof(pLscProfile->LscXGradTbl) / sizeof(pLscProfile->LscXGradTbl[0])); ++i) {
            if (0 < pLscProfile->LscXSizeTbl[i]) {
                pLscProfile->LscXGradTbl[i] = (uint16_t)((double)(1UL << pLscProfile->LscXo) / pLscProfile->LscXSizeTbl[i] + 0.5);
            }
            else {
                return (2);
            }
        }
    }
    else {
        memset(pLscProfile->LscXGradTbl, 0, sizeof(pLscProfile->LscXGradTbl));
    }

    LOGI("%s: (exit)\n", __FUNCTION__);

    return (0);
}

bool RkAiqCalibParser::parseEntrySensorLscTableAll
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);


    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        const char* value = tag.Value();
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_LSC_TABLEALL_NAME)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            ParseString(pchild,
                        mCalibDb->lsc.tableAll[index].name,
                        sizeof(mCalibDb->lsc.tableAll[index].name));
        }
        else if ((tagname == CALIB_SENSOR_LSC_TABLEALL_RESOLUTION)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            ParseString(pchild,
                        mCalibDb->lsc.tableAll[index].resolution,
                        sizeof(mCalibDb->lsc.tableAll[index].resolution));

        }
        else if ((tagname == CALIB_SENSOR_LSC_TABLEALL_ILLUMINATION)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            ParseString(pchild,
                        mCalibDb->lsc.tableAll[index].illumination,
                        sizeof(mCalibDb->lsc.tableAll[index].illumination));

        }
        else if ((tagname == CALIB_SENSOR_LSC_TABLEALL_LSC_SECTORS)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUshortArray(pchild, &mCalibDb->lsc.tableAll[index].LscSectors, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_LSC_TABLEALL_LSC_NO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUshortArray(pchild, &mCalibDb->lsc.tableAll[index].LscNo, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_LSC_TABLEALL_LSC_XO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUshortArray(pchild, &mCalibDb->lsc.tableAll[index].LscXo, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_LSC_TABLEALL_LSC_YO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUshortArray(pchild, &mCalibDb->lsc.tableAll[index].LscYo, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_LSC_TABLEALL_LSC_SECTOR_SIZE_X)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUshortArray(pchild, mCalibDb->lsc.tableAll[index].LscXSizeTbl, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_LSC_TABLEALL_LSC_SECTOR_SIZE_Y)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUshortArray(pchild, mCalibDb->lsc.tableAll[index].LscYSizeTbl, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_LSC_TABLEALL_LSC_VIGNETTING)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->lsc.tableAll[index].vignetting, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_LSC_TABLEALL_LSC_SAMPLES_RED)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUshortArray(pchild, mCalibDb->lsc.tableAll[index].LscMatrix[0].uCoeff, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_LSC_TABLEALL_LSC_SAMPLES_GREENR)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUshortArray(pchild, mCalibDb->lsc.tableAll[index].LscMatrix[1].uCoeff, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_LSC_TABLEALL_LSC_SAMPLES_GREENB)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUshortArray(pchild, mCalibDb->lsc.tableAll[index].LscMatrix[2].uCoeff, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_LSC_TABLEALL_LSC_SAMPLES_BLUE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUshortArray(pchild, mCalibDb->lsc.tableAll[index].LscMatrix[3].uCoeff, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        pchild = pchild->NextSibling();
    }
    int ret = AwbLscGradientCheck(&mCalibDb->lsc.tableAll[index]);
    DCT_ASSERT(ret == 0);
    mCalibDb->lsc.tableAllNum++;
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorRKDM
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_RKDM_ENABLE)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->dm.debayer_en, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_RKDM_DEBAYER_FILTER1)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseCharArray(pchild, mCalibDb->dm.debayer_filter1, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_RKDM_DEBAYER_FILTER2)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseCharArray(pchild, mCalibDb->dm.debayer_filter2, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_RKDM_DEBAYER_GAIN_OFFSET)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->dm.debayer_gain_offset, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_RKDM_ISO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseIntArray(pchild, mCalibDb->dm.ISO, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_RKDM_SHARP_STRENGTH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, mCalibDb->dm.sharp_strength, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_RKDM_DEBAYER_HF_OFFSET)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUshortArray(pchild, mCalibDb->dm.debayer_hf_offset, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_RKDM_DEBAYER_OFFSET)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->dm.debayer_offset, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_RKDM_DEBAYER_CLIP_EN)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->dm.debayer_clip_en, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_RKDM_DEBAYER_FILTER_G_EN)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->dm.debayer_filter_g_en, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_RKDM_DEBAYER_FILTER_C_EN)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->dm.debayer_filter_c_en, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_RKDM_DEBAYER_THED0)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->dm.debayer_thed0, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_RKDM_DEBAYER_THED1)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->dm.debayer_thed1, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_RKDM_DEBAYER_DIST_SCALE)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->dm.debayer_dist_scale, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_RKDM_DEBAYER_CNR_STRENGTH)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->dm.debayer_cnr_strength, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }
        else if ((tagname == CALIB_SENSOR_RKDM_DEBAYER_SHIFT_NUM)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->dm.debayer_shift_num, tag.Size());
            DCT_ASSERT((no == tag.Size()));

        }

        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorCCM
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    mCalibDb->ccm.matrixAllNum = 0;
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_CCM_ENABLE)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            uint8_t uValue = mCalibDb->ccm.enable;
            int no = ParseUcharArray(pchild, &uValue, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->ccm.enable = (bool)uValue;
        }
        else if ((tagname == CALIB_SENSOR_CCM_DAMP_ENABLE)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            uint8_t uValue = mCalibDb->ccm.damp_enable;
            int no = ParseUcharArray(pchild, &uValue, tag.Size());
            DCT_ASSERT((no == tag.Size()));
            mCalibDb->ccm.damp_enable = (bool)uValue;
        }
        else if (tagname == CALIB_SENSOR_CCM_LUMA_CCM) {
            INFO_PRINT(tagname);
            const XMLNode* psubchild = pchild->ToElement()->FirstChild();
            autoTabForward();
            while (psubchild) {
                XmlTag subTag = XmlTag(psubchild->ToElement());
                std::string subTagname(psubchild->ToElement()->Name());
                if ((subTagname == CALIB_SENSOR_CCM_LUMA_CCM_RGB2Y_PARA)
                        && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (subTag.Size() > 0)) {
                    INFO_PRINT(tagname);
                    int no = ParseFloatArray(psubchild, mCalibDb->ccm.luma_ccm.rgb2y_para, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));

                }
                else if ((subTagname == CALIB_SENSOR_CCM_LUMA_CCM_LOW_BOUND_POS_BIT)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    INFO_PRINT(tagname);
                    int no = ParseFloatArray(psubchild, &mCalibDb->ccm.luma_ccm.low_bound_pos_bit, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));

                }
                else if ((subTagname == CALIB_SENSOR_CCM_LUMA_CCM_Y_ALPHA_CURVE)
                         && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                         && (subTag.Size() > 0)) {
                    INFO_PRINT(tagname);
                    int no = ParseFloatArray(psubchild, mCalibDb->ccm.luma_ccm.y_alpha_curve, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));

                }
                else if (subTagname == CALIB_SENSOR_CCM_LUMA_CCM_GAIN_ALPHASCALE_CURVE) {
                    INFO_PRINT(tagname);
                    const XMLNode* psubsubchild = psubchild->ToElement()->FirstChild();
                    autoTabForward();
                    int no1 = 0, no2 = 0;
                    while (psubsubchild) {
                        XmlTag subsubTag = XmlTag(psubsubchild->ToElement());
                        std::string subsubTagname(psubsubchild->ToElement()->Name());

                        if (subsubTagname == CALIB_SENSOR_CCM_LUMA_CCM_GAIN) {
                            no1 = ParseFloatArray(psubsubchild, mCalibDb->ccm.luma_ccm.alpha_gain, subsubTag.Size());
                            DCT_ASSERT((no1 == subsubTag.Size()));
                        }
                        else if (subsubTagname == CALIB_SENSOR_CCM_LUMA_CCM_SCALE) {
                            no2 = ParseFloatArray(psubsubchild, mCalibDb->ccm.luma_ccm.alpha_scale, subsubTag.Size());
                            DCT_ASSERT((no2 == subsubTag.Size()));
                        }
                        psubsubchild = psubsubchild->NextSibling();
                    }
                    autoTabBackward();
                    DCT_ASSERT(no1 == no2);
                    mCalibDb->ccm.luma_ccm.gain_scale_cure_size = no1;
                }
                psubchild = psubchild->NextSibling();
            }
            autoTabBackward();
        }
        else if (tagname == CALIB_SENSOR_CCM_ACCMCOF) {
            if (!parseEntrySensorCcmAccmCof(pchild->ToElement())) {
                LOGE("parse error in CCM aCCmCof (%s)\n", tagname);
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_CCM_MATRIXALL) {
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorCcmMatrixAll)) {
                LOGE("parse error in CCM matrixall (%s)\n", tagname);
                return (false);
            }
        }

        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorCcmAccmCof
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    mCalibDb->ccm.aCcmCof.illuNum = 0;

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if (tagname == CALIB_SENSOR_CCM_ACCMCOF_IllALL) {
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorCcmAccmCofIllAll)) {
                LOGE("parse error in LSC aCcmCof illAll (%s)", tagname);
                return (false);
            }
        }
        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}


bool RkAiqCalibParser::parseEntrySensorCcmAccmCofIllAll
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);


    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    int no1 = 0;
    int no2 = 0;
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        const char* value = tag.Value();
        std::string tagname(pchild->ToElement()->Name());
        if ((tagname == CALIB_SENSOR_CCM_ACCMCOF_NAME)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            ParseString(pchild,
                        mCalibDb->ccm.aCcmCof.illAll[index].illuName,
                        sizeof(mCalibDb->ccm.aCcmCof.illAll[index].illuName));
        }
        else if ((tagname == CALIB_SENSOR_CCM_ACCMCOF_WBGAIN)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->ccm.aCcmCof.illAll[index].awbGain, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_CCM_ACCMCOF_MATRIXUSED)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            char* ccm_profiles = Toupper(value);
            int no = ParseLscProfileArray(pchild, mCalibDb->ccm.aCcmCof.illAll[index].matrixUsed, CCM_PROFILES_NUM_MAX);
            DCT_ASSERT((no <= CCM_PROFILES_NUM_MAX));
            mCalibDb->ccm.aCcmCof.illAll[index].matrixUsedNO = no;
        }
        else if ((tagname == CALIB_SENSOR_CCM_ACCMCOF_GAINS)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            no1 = ParseFloatArray(pchild, mCalibDb->ccm.aCcmCof.illAll[index].saturationCurve.pSensorGain, tag.Size());
            mCalibDb->ccm.aCcmCof.illAll[index].saturationCurve.arraySize = no1;
            DCT_ASSERT((no1 == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_CCM_ACCMCOF_SAT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            no2 = ParseFloatArray(pchild, mCalibDb->ccm.aCcmCof.illAll[index].saturationCurve.pSaturation, tag.Size());
            mCalibDb->ccm.aCcmCof.illAll[index].saturationCurve.arraySize = no2;
            DCT_ASSERT((no2 == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }

    DCT_ASSERT(no1 == no2);
    mCalibDb->ccm.aCcmCof.illuNum++;
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}


bool RkAiqCalibParser::parseEntrySensorCcmMatrixAll
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        const char* value = tag.Value();
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_CCM_MATRIXALL_NAME)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            ParseString(pchild,
                        mCalibDb->ccm.matrixAll[index].name,
                        sizeof(mCalibDb->ccm.matrixAll[index].name));
        }
        else if ((tagname == CALIB_SENSOR_CCM_MATRIXALL_ILLUMINATION)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            ParseString(pchild,
                        mCalibDb->ccm.matrixAll[index].illumination,
                        sizeof(mCalibDb->ccm.matrixAll[index].illumination));
        }
        else if ((tagname == CALIB_SENSOR_CCM_MATRIXALL_SAT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->ccm.matrixAll[index].saturation, 1);
            DCT_ASSERT((no == 1));
        }
        else if ((tagname == CALIB_SENSOR_CCM_MATRIXALL_MATRIX)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int i = (sizeof(mCalibDb->ccm.matrixAll[index].CrossTalkCoeff)
                     / sizeof(mCalibDb->ccm.matrixAll[index].CrossTalkCoeff.fCoeff[0]));
            int no = ParseFloatArray(pchild, mCalibDb->ccm.matrixAll[index].CrossTalkCoeff.fCoeff, i);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_CCM_MATRIXALL_OFFSET)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0))
        {
            int i = (sizeof(mCalibDb->ccm.matrixAll[index].CrossTalkOffset)
                     / sizeof(mCalibDb->ccm.matrixAll[index].CrossTalkOffset.fCoeff[0]));
            int no = ParseFloatArray(pchild, mCalibDb->ccm.matrixAll[index].CrossTalkOffset.fCoeff, i);
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }
    mCalibDb->ccm.matrixAllNum++;
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorUVNRSetting
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_UVNR_SETTING_NAME)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->uvnr.setting[index].name, sizeof(mCalibDb->uvnr.setting[index].name));
        } else if ((tagname == CALIB_SENSOR_UVNR_SETTING_MODE)
                   && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                   && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->uvnr.setting[index].mode, sizeof(mCalibDb->uvnr.setting[index].mode));
        } else if ((tagname == CALIB_SENSOR_UVNR_ISO)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].ISO, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP0_UVGRAD_RATIO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step0_uvgrad_ratio, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP0_UVGRAD_OFFSET)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step0_uvgrad_offset, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP1_NONMED1)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step1_nonMed1, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP1_NONBF1)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step1_nonBf1, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP1_DOWNSAMPLE_W)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step1_downSample_w, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP1_DOWNSAMPLE_H)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step1_downSample_h, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP1_DOWNSAMPLE_MEANSIZE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step1_downSample_meansize, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP1_MEDIAN_RATIO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step1_median_ratio, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP1_MEDIAN_SIZE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step1_median_size, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP1_MEDIAN_IIR)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step1_median_IIR, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP1_BF_SIGMAR)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step1_bf_sigmaR, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP1_BF_UVGAIN)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step1_bf_uvgain, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP1_BF_RATIO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step1_bf_ratio, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP1_BF_SIZE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step1_bf_size, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP1_BF_SIGMAD)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step1_bf_sigmaD, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP1_BF_ISROWIIR)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step1_bf_isRowIIR, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP1_BF_ISYCOPY)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step1_bf_isYcopy, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP2_NONEXT_BLOCK)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step2_nonExt_block, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP2_NONMED)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step2_nonMed, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP2_NONBF)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step2_nonBf, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP2_DOWNSAMPLE_W)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step2_downSample_w, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP2_DOWNSAMPLE_H)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step2_downSample_h, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP2_DOWNSAMPLE_MEANSIZE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step2_downSample_meansize, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP2_MEDIAN_RATIO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step2_median_ratio, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP2_MEDIAN_SIZE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step2_median_size, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP2_MEDIAN_IIR)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step2_median_IIR, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP2_BF_SIGMAR)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step2_bf_sigmaR, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP2_BF_UVGAIN)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step2_bf_uvgain, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP2_BF_RATIO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step2_bf_ratio, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP2_BF_SIZE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step2_bf_size, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP2_BF_SIGMAD)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step2_bf_sigmaD, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP2_BF_ISROWIIR)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step2_bf_isRowIIR, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP2_BF_ISYCOPY)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step2_bf_isYcopy, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP3_NONBF3)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step3_nonBf3, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP3_BF_SIGMAR)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step3_bf_sigmaR, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP3_BF_UVGAIN)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step3_bf_uvgain, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP3_BF_RATIO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step3_bf_ratio, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP3_BF_SIZE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step3_bf_size, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP3_BF_SIGMAD)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step3_bf_sigmaD, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP3_BF_ISROWIIR)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step3_bf_isRowIIR, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_STEP3_BF_ISYCOPY)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].step3_bf_isYcopy, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_KERNEL_3X3)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].kernel_3x3, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_KERNEL_5X5)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].kernel_5x5, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_KERNEL_9X9)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].kernel_9x9, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_KERNEL_9X9_NUM)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->uvnr.setting[index].kernel_9x9_num, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_SIGMA_ADJ_LUMA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].sigma_adj_luma, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_SIGMA_ADJ_RATIO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].sigma_adj_ratio, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_THRESHOLD_ADJ_LUMA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].threshold_adj_luma, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_UVNR_THRESHOLD_ADJ_THRE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->uvnr.setting[index].threshold_adj_thre, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }

        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorUVNR
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_UVNR_ENABLE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseIntArray(pchild, &mCalibDb->uvnr.enable, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_UVNR_VERSION)
                   && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                   && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->uvnr.version, sizeof(mCalibDb->uvnr.version));
        } else if (tagname == CALIB_SENSOR_UVNR_SETTING) {
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorUVNRSetting)) {
                LOGE("parse error in BayerNR setting (%s)\n", tagname);
                return (false);
            }
        }


        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorGamma
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());
        if ((tagname == CALIB_SENSOR_GAMMA_GAMMA_EN)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->gamma.gamma_en, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GAMMA_GAMMA_OUT_MODE)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->gamma.gamma_out_mode, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GAMMA_GAMMA_OUT_SEGNUM)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->gamma.gamma_out_segnum, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GAMMA_GAMMA_OUT_OFFSET)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->gamma.gamma_out_offset, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GAMMA_CURVE_NORMAL)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->gamma.curve_normal, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GAMMA_CURVE_HDR)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->gamma.curve_hdr, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GAMMA_CURVE_NIGHT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->gamma.curve_night, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GAMMA_CURVE_USER1)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->gamma.curve_user1, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GAMMA_CURVE_USER2)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->gamma.curve_user2, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }

        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorYnr
(
    const XMLElement*   pelement,
    void*                param
)
{
    (void)param;

    LOGD( "%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_YNR_ENABLE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseIntArray(pchild, &mCalibDb->ynr.enable, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_YNR_VERSION)
                   && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                   && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->ynr.version, sizeof(mCalibDb->ynr.version));
        } else if (tagname == CALIB_SENSOR_YNR_SETTING_CELL) {
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorYnrSetting)) {
                LOGE("parse error in YNR setting (%s)", tagname);
                return (false);
            }
        }

        pchild = pchild->NextSibling();
    }
    LOGD( "%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}


bool RkAiqCalibParser::parseEntrySensorYnrSetting
(
    const XMLElement*   pelement,
    void*                param
)
{
    (void)param;

    LOGD( "%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_YNR_SETTING_NAME)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->ynr.setting[index].name, sizeof(mCalibDb->ynr.setting[index].name));
        } else if ((tagname == CALIB_SENSOR_YNR_SETTING_MODE)
                   && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                   && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->ynr.setting[index].mode, sizeof(mCalibDb->ynr.setting[index].mode));
        } else if (tagname == CALIB_SENSOR_YNR_ISO_CELL) {
            if (!parseEntryCell2(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorYnrISO, (void*)&mCalibDb->ynr.setting[index])) {
                LOGE("parse error in YNR iso (%s)", tagname);
                return (false);
            }
        }

        pchild = pchild->NextSibling();
    }
    LOGD( "%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorYnrISO
(
    const XMLElement*   pelement,
    void*                param,
    int                  index
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    CalibDb_YNR_Setting_t *pSetting = (CalibDb_YNR_Setting_t *)param;

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        const char* value = tag.Value();
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_YNR_ISO)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->ynr_iso[index].iso, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_SIGMA_CURVE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseDoubleArray(pchild, pSetting->ynr_iso[index].sigma_curve, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_YNR_LCI)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->ynr_iso[index].ynr_lci, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_YNR_LHCI)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->ynr_iso[index].ynr_lhci, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_YNR_HLCI)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->ynr_iso[index].ynr_hlci, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_YNR_HHCI)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->ynr_iso[index].ynr_hhci, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_LO_LUMAPOINT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->ynr_iso[index].lo_lumaPoint, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_LO_LUMARATIO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->ynr_iso[index].lo_lumaRatio, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_LO_DIRECTIONSTRENGTH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->ynr_iso[index].lo_directionStrength, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_LO_BFSCALE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->ynr_iso[index].lo_bfScale, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_IMERGE_RATIO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->ynr_iso[index].imerge_ratio, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_IMERGE_BOUND)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->ynr_iso[index].imerge_bound, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_DENOISE_WEIGHT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->ynr_iso[index].denoise_weight, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_HI_LUMAPOINT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->ynr_iso[index].hi_lumaPoint, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_HI_LUMARATIO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->ynr_iso[index].hi_lumaRatio, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_HI_BFSCALE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->ynr_iso[index].hi_bfScale, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_HWITH_D)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->ynr_iso[index].hwith_d, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_HI_DENOISESTRENGTH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->ynr_iso[index].hi_denoiseStrength, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_HI_DETAILMINADJDNW)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->ynr_iso[index].hi_detailMinAdjDnW, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_HI_DENOISEWEIGHT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->ynr_iso[index].hi_denoiseWeight, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_Y_LUMA_POINT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->ynr_iso[index].y_luma_point, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_HGRAD_Y_LEVEL1)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->ynr_iso[index].hgrad_y_level1, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_HGRAD_Y_LEVEL2)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->ynr_iso[index].hgrad_y_level2, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_HGRAD_Y_LEVEL3)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->ynr_iso[index].hgrad_y_level3, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_HGRAD_Y_LEVEL4)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->ynr_iso[index].hgrad_y_level4, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_YNR_HI_SOFT_THRESH_SCALE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->ynr_iso[index].hi_soft_thresh_scale, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }

    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorGic
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());
        if ((tagname == CALIB_SENSOR_GIC_ENABLE)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->gic.gic_en, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_EDGE_EN)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->gic.edge_en, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_GR_RATION)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->gic.gr_ration, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_NOISE_CUT_EN)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->gic.noise_cut_en, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if (tagname == CALIB_SENSOR_GIC_ISO) {
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorGicISO)) {
                LOGE("parse error in GIC ISO (%s)", tagname);
                return (false);
            }
        }

        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorGicISO
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);


    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        const char* value = tag.Value();
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_GIC_ISO_ISO)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].iso, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_MIN_BUSY_THRE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].min_busy_thre, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_MIN_GRAD_THR1)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].min_grad_thr1, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_MIN_GRAD_THR2)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].min_grad_thr2, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_K_GRAD1)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].k_grad1, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_K_GRAD2)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].k_grad2, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_SMOOTHNESS_GB)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].smoothness_gb, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_SMOOTHNESS_GB_WEAK)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].smoothness_gb_weak, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_GB_THRE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].gb_thre, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_MAXCORV)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].maxCorV, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_MAXCORVBOTH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].maxCorVboth, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_MAXCUTV)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].maxCutV, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_DARK_THRE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].dark_thre, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_DARK_THREHI)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].dark_threHi, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_K_GRAD1_DARK)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].k_grad1_dark, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_K_GRAD2_DARK)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].k_grad2_dark, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_MIN_GRAD_THR_DARK1)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].min_grad_thr_dark1, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_MIN_GRAD_THR_DARK2)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].min_grad_thr_dark2, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_GVALUELIMITLO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].GValueLimitLo, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_GVALUELIMITHI)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].GValueLimitHi, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_TEXTURESTRENGTH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].textureStrength, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_SCALELO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].ScaleLo, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_SCALEHI)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].ScaleHi, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_NOISECURVE_0)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].noiseCurve_0, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_NOISECURVE_1)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].noiseCurve_1, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_GLOBALSTRENGTH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].globalStrength, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_NOISE_COEA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].noise_coea, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_NOISE_COEB)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].noise_coeb, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_GIC_DIFF_CLIP)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->gic.gic_iso[index].diff_clip, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorMFNR
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_MFNR_ENABLE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseIntArray(pchild, &mCalibDb->mfnr.enable, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_VERSION)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->mfnr.version, sizeof(mCalibDb->mfnr.version));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_LOCAL_GAIN_EN)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->mfnr.local_gain_en, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_MODE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->mfnr.mode, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_MAX_LEVEL)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->mfnr.max_level, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_MAX_LEVEL_UV)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->mfnr.max_level_uv, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_BACK_REF_NUM)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->mfnr.back_ref_num, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if (tagname == CALIB_SENSOR_MFNR_AWB_UV_RATIO) {
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorMFNRAwbUvRatio)) {
                LOGE("parse error in MFNR awb_uv_ratio (%s)\n", tagname);
                return (false);
            }
        }
        else if (tagname == CALIB_SENSOR_MFNR_SETTING) {
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorMFNRSetting)) {
                LOGE("parse error in MFNR MFNR_ISO (%s)\n", tagname);
                return (false);
            }
        }

        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorMFNRAwbUvRatio
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);


    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        const char* value = tag.Value();
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_MFNR_AWB_UV_RATIO_NAME)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            ParseString(pchild,
                        mCalibDb->mfnr.uv_ratio[index].illum,
                        sizeof(mCalibDb->mfnr.uv_ratio[index].illum));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_AWB_UV_RATIO_RATIO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->mfnr.uv_ratio[index].ratio, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorMFNRSetting
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_MFNR_SETTING_NAME)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->mfnr.setting[index].name, sizeof(mCalibDb->mfnr.setting[index].name));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_SETTING_MODE)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->mfnr.setting[index].mode, sizeof(mCalibDb->mfnr.setting[index].mode));
        }
        else if (tagname == CALIB_SENSOR_MFNR_ISO) {
            if (!parseEntryCell2(pchild->ToElement(), tag.Size(),
                                 &RkAiqCalibParser::parseEntrySensorMFNRISO,
                                 (void *)&mCalibDb->mfnr.setting[index])) {
                LOGE("parse error in MFNR MFNR_ISO (%s)\n", tagname);
                return (false);
            }
        }

        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);

    return (true);
}

bool RkAiqCalibParser::parseEntrySensorMFNRISO
(
    const XMLElement*   pelement,
    void*                param,
    int                  index
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);


    CalibDb_MFNR_Setting_t * pSetting = (CalibDb_MFNR_Setting_t *)param;
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        const char* value = tag.Value();
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_MFNR_ISO_ISO)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->mfnr_iso[index].iso, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_WEIGHT_LIMIT_Y)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].weight_limit_y, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_WEIGHT_LIMIT_UV)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].weight_limit_uv, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_RATIO_FRQ)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].ratio_frq, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_LUMA_W_IN_CHROMA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].luma_w_in_chroma, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_NOISE_CURVE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseDoubleArray(pchild, pSetting->mfnr_iso[index].noise_curve, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_NOISE_CURVE_X00)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseDoubleArray(pchild, &pSetting->mfnr_iso[index].noise_curve_x00, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_Y_LO_NOISEPROFILE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].y_lo_noiseprofile, tag.Size(), 5);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_Y_HI_NOISEPROFILE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].y_hi_noiseprofile, tag.Size(), 5);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_Y_LO_DENOISEWEIGHT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].y_lo_denoiseweight, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_Y_HI_DENOISEWEIGHT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].y_hi_denoiseweight, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_Y_LO_BFSCALE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].y_lo_bfscale, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_Y_HI_BFSCALE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].y_hi_bfscale, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_Y_LUMANRPOINT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].y_lumanrpoint, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_Y_LUMANRCURVE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].y_lumanrcurve, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_Y_DENOISESTRENGTH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->mfnr_iso[index].y_denoisestrength, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_Y_LO_LVL0_GFDELTA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].y_lo_lvl0_gfdelta, tag.Size(), 5);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_Y_HI_LVL0_GFDELTA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].y_hi_lvl0_gfdelta, tag.Size(), 5);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_Y_LO_LVL1_GFDELTA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].y_lo_lvl1_gfdelta, tag.Size(), 5);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_Y_HI_LVL1_GFDELTA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].y_hi_lvl1_gfdelta, tag.Size(), 5);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_Y_LO_LVL2_GFDELTA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].y_lo_lvl2_gfdelta, tag.Size(), 5);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_Y_HI_LVL2_GFDELTA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].y_hi_lvl2_gfdelta, tag.Size(), 5);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_Y_LO_LVL3_GFDELTA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].y_lo_lvl3_gfdelta, tag.Size(), 5);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_Y_HI_LVL3_GFDELTA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].y_hi_lvl3_gfdelta, tag.Size(), 5);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_UV_LO_NOISEPROFILE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].uv_lo_noiseprofile, tag.Size(), 5);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_UV_HI_NOISEPROFILE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].uv_hi_noiseprofile, tag.Size(), 5);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_UV_LO_DENOISEWEIGHT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].uv_lo_denoiseweight, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_UV_HI_DENOISEWEIGHT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].uv_hi_denoiseweight, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_UV_LO_BFSCALE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].uv_lo_bfscale, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_UV_HI_BFSCALE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].uv_hi_bfscale, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_UV_LUMANRPOINT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].uv_lumanrpoint, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_UV_LUMANRCURVE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].uv_lumanrcurve, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_UV_DENOISESTRENGTH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->mfnr_iso[index].uv_denoisestrength, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_UV_LO_LVL0_GFDELTA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].uv_lo_lvl0_gfdelta, tag.Size(), 5);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_UV_HI_LVL0_GFDELTA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].uv_hi_lvl0_gfdelta, tag.Size(), 5);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_UV_LO_LVL1_GFDELTA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].uv_lo_lvl1_gfdelta, tag.Size(), 5);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_UV_HI_LVL1_GFDELTA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].uv_hi_lvl1_gfdelta, tag.Size(), 5);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_UV_LO_LVL2_GFDELTA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].uv_lo_lvl2_gfdelta, tag.Size(), 5);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_UV_HI_LVL2_GFDELTA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].uv_hi_lvl2_gfdelta, tag.Size(), 5);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_LVL0_GFSIGMA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].lvl0_gfsigma, tag.Size(), 5);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_LVL1_GFSIGMA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].lvl1_gfsigma, tag.Size(), 5);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_LVL2_GFSIGMA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].lvl2_gfsigma, tag.Size(), 5);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_MFNR_ISO_LVL3_GFSIGMA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->mfnr_iso[index].lvl3_gfsigma, tag.Size(), 5);
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorSharp
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_SHARP_ENABLE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseIntArray(pchild, &mCalibDb->sharp.enable, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_SHARP_VERSION)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->sharp.version, sizeof(mCalibDb->sharp.version));
        }
        if ((tagname == CALIB_SENSOR_SHARP_LUMA_POINT)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->sharp.luma_point, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if (tagname == CALIB_SENSOR_SHARP_SETTING) {
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorSharpSetting)) {
                LOGE("parse error in SHARP SHARP_Setting (%s)", tagname);
                return (false);
            }
        }

        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}


bool RkAiqCalibParser::parseEntrySensorSharpSetting
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_SHARP_SETTING_NAME)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->sharp.setting[index].name, sizeof(mCalibDb->sharp.setting[index].name));
        }
        else if ((tagname == CALIB_SENSOR_SHARP_SETTING_MODE)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->sharp.setting[index].mode, sizeof(mCalibDb->sharp.setting[index].mode));
        }
        else if (tagname == CALIB_SENSOR_SHARP_SHARP_ISO) {
            if (!parseEntryCell2(pchild->ToElement(), tag.Size(),
                                 &RkAiqCalibParser::parseEntrySensorSharpISO,
                                 (void *)&mCalibDb->sharp.setting[index])) {
                LOGE("parse error in SHARP SHARP_ISO (%s)", tagname);
                return (false);
            }
        }

        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);

    return (true);
}

bool RkAiqCalibParser::parseEntrySensorSharpISO
(
    const XMLElement*   pelement,
    void*                param,
    int                  index
) {

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    CalibDb_Sharp_Setting_t *pSetting = (CalibDb_Sharp_Setting_t *)param;
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        const char* value = tag.Value();
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_SHARP_SHARP_ISO_ISO)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->sharp_iso[index].iso, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_SHARP_SHARP_ISO_HRATIO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->sharp_iso[index].hratio, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_SHARP_SHARP_ISO_LRATIO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->sharp_iso[index].lratio, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_SHARP_SHARP_ISO_MF_SHARP_RATIO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->sharp_iso[index].mf_sharp_ratio, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_SHARP_SHARP_ISO_HF_SHARP_RATIO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->sharp_iso[index].hf_sharp_ratio, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_SHARP_SHARP_ISO_LUMA_SIGMA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->sharp_iso[index].luma_sigma, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_SHARP_SHARP_ISO_PBF_GAIN)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->sharp_iso[index].pbf_gain, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_SHARP_SHARP_ISO_PBF_RATIO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->sharp_iso[index].pbf_ratio, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_SHARP_SHARP_ISO_PBF_ADD)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->sharp_iso[index].pbf_add, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_SHARP_SHARP_ISO_MF_CLIP_POS)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->sharp_iso[index].mf_clip_pos, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_SHARP_SHARP_ISO_MF_CLIP_NEG)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->sharp_iso[index].mf_clip_neg, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_SHARP_SHARP_ISO_HF_CLIP)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->sharp_iso[index].hf_clip, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_SHARP_SHARP_ISO_MBF_GAIN)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->sharp_iso[index].mbf_gain, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_SHARP_SHARP_ISO_HBF_GAIN)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->sharp_iso[index].hbf_gain, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_SHARP_SHARP_ISO_HBF_RATIO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->sharp_iso[index].hbf_ratio, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_SHARP_SHARP_ISO_MBF_ADD)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->sharp_iso[index].mbf_add, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_SHARP_SHARP_ISO_HBF_ADD)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->sharp_iso[index].hbf_add, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_SHARP_SHARP_ISO_LOCAL_SHARP_STRENGTH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->sharp_iso[index].local_sharp_strength, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorEdgeFilter
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_EDGEFILTER_ENABLE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseIntArray(pchild, &mCalibDb->edgeFilter.enable, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_EDGEFILTER_VERSION)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->edgeFilter.version, sizeof(mCalibDb->edgeFilter.version));
        }
        if ((tagname == CALIB_SENSOR_EDGEFILTER_LUMA_POINT)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->edgeFilter.luma_point, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if (tagname == CALIB_SENSOR_EDGEFILTER_SETTING) {
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorEdgeFilterSetting)) {
                LOGE("parse error in EDGEFILTER setting (%s)", tagname);
                return (false);
            }
        }

        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorEdgeFilterSetting
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);

    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_EDGEFILTER_SETTING_NAME)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->edgeFilter.setting[index].name, sizeof(mCalibDb->edgeFilter.setting[index].name));
        }
        else if ((tagname == CALIB_SENSOR_EDGEFILTER_SETTING_MODE)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            ParseString(pchild, mCalibDb->edgeFilter.setting[index].mode, sizeof(mCalibDb->edgeFilter.setting[index].mode));
        }
        else if (tagname == CALIB_SENSOR_EDGEFILTER_EDGEFILTER_ISO) {
            if (!parseEntryCell2(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorEdgeFilterISO, (void *)&mCalibDb->edgeFilter.setting[index])) {
                LOGE("parse error in EDGEFILTER EDGEFILTER_ISO (%s)", tagname);
                return (false);
            }
        }

        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorEdgeFilterISO
(
    const XMLElement*   pelement,
    void*                param,
    int                 index
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);


    CalibDb_EdgeFilter_Setting_t *pSetting = (CalibDb_EdgeFilter_Setting_t *)param;
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        const char* value = tag.Value();
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_EDGEFILTER_EDGEFILTER_ISO_ISO)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->edgeFilter_iso[index].iso, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_EDGEFILTER_EDGEFILTER_ISO_EDGE_THED)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->edgeFilter_iso[index].edge_thed, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_EDGEFILTER_EDGEFILTER_ISO_SRC_WGT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->edgeFilter_iso[index].src_wgt, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_EDGEFILTER_EDGEFILTER_ISO_ALPHA_ADP_EN)
                 && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &pSetting->edgeFilter_iso[index].alpha_adp_en, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_EDGEFILTER_EDGEFILTER_ISO_LOCAL_ALPHA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->edgeFilter_iso[index].local_alpha, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_EDGEFILTER_EDGEFILTER_ISO_GLOBAL_ALPHA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &pSetting->edgeFilter_iso[index].global_alpha, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_EDGEFILTER_EDGEFILTER_ISO_NOISE_CLIP)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->edgeFilter_iso[index].noise_clip, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_EDGEFILTER_EDGEFILTER_ISO_DOG_CLIP_POS)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->edgeFilter_iso[index].dog_clip_pos, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_EDGEFILTER_EDGEFILTER_ISO_DOG_CLIP_NEG)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->edgeFilter_iso[index].dog_clip_neg, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_EDGEFILTER_EDGEFILTER_ISO_DOG_ALPHA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->edgeFilter_iso[index].dog_alpha, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_EDGEFILTER_EDGEFILTER_ISO_DIRECT_FILTER_COEFF)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->edgeFilter_iso[index].direct_filter_coeff, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_EDGEFILTER_EDGEFILTER_ISO_DOG_KERNEL_ROW0)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->edgeFilter_iso[index].dog_kernel_row0, tag.Size(), 6);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_EDGEFILTER_EDGEFILTER_ISO_DOG_KERNEL_ROW1)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->edgeFilter_iso[index].dog_kernel_row1, tag.Size(), 6);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_EDGEFILTER_EDGEFILTER_ISO_DOG_KERNEL_ROW2)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->edgeFilter_iso[index].dog_kernel_row2, tag.Size(), 6);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_EDGEFILTER_EDGEFILTER_ISO_DOG_KERNEL_ROW3)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->edgeFilter_iso[index].dog_kernel_row3, tag.Size(), 6);
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_EDGEFILTER_EDGEFILTER_ISO_DOG_KERNEL_ROW4)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, pSetting->edgeFilter_iso[index].dog_kernel_row4, tag.Size(), 6);
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorDehaze
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();


    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_DEHAZE_DC_BF_H)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->dehaze.dc_bf_h, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_AIR_BF_H)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->dehaze.air_bf_h, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_GAUS_H)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->dehaze.gaus_h, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_HIST_CONV_T0)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->dehaze.hist_conv_t0, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_HIST_CONV_T1)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->dehaze.hist_conv_t1, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_HIST_CONV_T2)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, mCalibDb->dehaze.hist_conv_t2, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if (tagname == CALIB_SENSOR_DEHAZE_ISO) {
            if (!parseEntryCell(pchild->ToElement(), tag.Size(), &RkAiqCalibParser::parseEntrySensorDehazeISO)) {
                LOGE("parse error in DEHAZE DEHAZE_ISO (%s)", tagname);
                return (false);
            }
        }

        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorDehazeISO
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD("%s(%d): (enter)\n", __FUNCTION__, __LINE__);


    int index = *((int*)param);
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        const char* value = tag.Value();
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_DEHAZE_ISO_ISO)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.iso[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_DEHAZE_EN)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->dehaze.dehaze_iso.dehaze_en[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_DC_EN)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->dehaze.dehaze_iso.dc_en[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_HIST_EN)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->dehaze.dehaze_iso.hist_en[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_HIST_CHANNEL)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->dehaze.dehaze_iso.hist_channel[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_GAIN_EN)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->dehaze.dehaze_iso.gain_en[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_DC_MIN_TH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.dc_min_th[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_DC_MAX_TH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.dc_max_th[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_YHIST_TH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.yhist_th[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_YBLK_TH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.yblk_th[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_DARK_TH)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.dark_th[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_BRIGHT_MIN)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.bright_min[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_BRIGHT_MAX)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.bright_max[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_WT_MAX)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.wt_max[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_AIR_MIN)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.air_min[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_AIR_MAX)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.air_max[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_TMAX_BASE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.tmax_base[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_TMAX_OFF)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.tmax_off[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_TMAX_MAX)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.tmax_max[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_HIST_GRATIO)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.hist_gratio[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_HIST_TH_OFF)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.hist_th_off[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_HIST_K)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.hist_k[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_HIST_MIN)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.hist_min[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_ENHANCE_EN)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->dehaze.dehaze_iso.enhance_en[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_ENHANCE_VALUE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.enhance_value[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_HIST_PARA_EN)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->dehaze.dehaze_iso.hist_para_en[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_HIST_SCALE)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.hist_scale[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_STAB_FNUM)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.stab_fnum[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_SIGMA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.sigma[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_WT_SIGMA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.wt_sigma[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_AIR_SIGMA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.air_sigma[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_TMAX_SIGMA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.tmax_sigma[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_CFG_ALPHA)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.cfg_alpha[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_CFG_WT)
                 && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                 && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.cfg_wt[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_CFG_AIR)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.cfg_air[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_CFG_TMAX)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.cfg_tmax[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_CFG_GRATIO)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.cfg_gratio[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_DC_THED)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.dc_thed[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_DC_WEITCUR)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.dc_weitcur[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_AIR_THED)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.air_thed[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_DEHAZE_ISO_AIR_WEITCUR)
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->dehaze.dehaze_iso.air_weitcur[index], tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }
    LOGD( "%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorAf
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD( "%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_AF_WINDOW)
                && (tag.isType(XmlTag::TAG_TYPE_STRUCT))
                && (tag.Size() > 0)) {
            const XMLNode* psecsubchild = pchild->ToElement()->FirstChild();
            while (psecsubchild) {
                XmlTag secsubtag = XmlTag(psecsubchild->ToElement());
                std::string secsubTagname(psecsubchild->ToElement()->Name());

                if (secsubTagname == CALIB_SENSOR_AF_H_OFFS
                        && (secsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (secsubtag.Size() > 0)) {
                    int no = ParseUshortArray(psecsubchild, &mCalibDb->af.win_h_offs, secsubtag.Size());
                    DCT_ASSERT((no == secsubtag.Size()));
                } else if (secsubTagname == CALIB_SENSOR_AF_V_OFFS
                           && (secsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                           && (secsubtag.Size() > 0)) {
                    int no = ParseUshortArray(psecsubchild, &mCalibDb->af.win_v_offs, secsubtag.Size());
                    DCT_ASSERT((no == secsubtag.Size()));
                } else if (secsubTagname == CALIB_SENSOR_AF_H_SIZE
                           && (secsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                           && (secsubtag.Size() > 0)) {
                    int no = ParseUshortArray(psecsubchild, &mCalibDb->af.win_h_size, secsubtag.Size());
                    DCT_ASSERT((no == secsubtag.Size()));
                } else if (secsubTagname == CALIB_SENSOR_AF_V_SIZE
                           && (secsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                           && (secsubtag.Size() > 0)) {
                    int no = ParseUshortArray(psecsubchild, &mCalibDb->af.win_v_size, secsubtag.Size());
                    DCT_ASSERT((no == secsubtag.Size()));
                }
                psecsubchild = psecsubchild->NextSibling();
            }
        } else if ((tagname == CALIB_SENSOR_AF_FIXED_MODE)
                   && (tag.isType(XmlTag::TAG_TYPE_STRUCT))
                   && (tag.Size() > 0)) {
            const XMLNode* psecsubchild = pchild->ToElement()->FirstChild();
            while (psecsubchild) {
                XmlTag secsubtag = XmlTag(psecsubchild->ToElement());
                std::string secsubTagname(psecsubchild->ToElement()->Name());
                if (secsubTagname == CALIB_SENSOR_AF_DEF_CODE
                        && (secsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (secsubtag.Size() > 0)) {
                    int no = ParseUcharArray(psecsubchild, &mCalibDb->af.fixed_mode.code, secsubtag.Size());
                    DCT_ASSERT((no == secsubtag.Size()));
                }
                psecsubchild = psecsubchild->NextSibling();
            }
        } else if ((tagname == CALIB_SENSOR_AF_MACRO_MODE)
                   && (tag.isType(XmlTag::TAG_TYPE_STRUCT))
                   && (tag.Size() > 0)) {
            const XMLNode* psecsubchild = pchild->ToElement()->FirstChild();
            while (psecsubchild) {
                XmlTag secsubtag = XmlTag(psecsubchild->ToElement());
                std::string secsubTagname(psecsubchild->ToElement()->Name());
                if (secsubTagname == CALIB_SENSOR_AF_DEF_CODE
                        && (secsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (secsubtag.Size() > 0)) {
                    int no = ParseUcharArray(psecsubchild, &mCalibDb->af.macro_mode.code, secsubtag.Size());
                    DCT_ASSERT((no == secsubtag.Size()));
                }
                psecsubchild = psecsubchild->NextSibling();
            }
        } else if ((tagname == CALIB_SENSOR_AF_INFINITY_MODE)
                   && (tag.isType(XmlTag::TAG_TYPE_STRUCT))
                   && (tag.Size() > 0)) {
            const XMLNode* psecsubchild = pchild->ToElement()->FirstChild();
            while (psecsubchild) {
                XmlTag secsubtag = XmlTag(psecsubchild->ToElement());
                std::string secsubTagname(psecsubchild->ToElement()->Name());
                if (secsubTagname == CALIB_SENSOR_AF_DEF_CODE
                        && (secsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (secsubtag.Size() > 0)) {
                    int no = ParseUcharArray(psecsubchild, &mCalibDb->af.infinity_mode.code, secsubtag.Size());
                    DCT_ASSERT((no == secsubtag.Size()));
                }
                psecsubchild = psecsubchild->NextSibling();
            }
        } else if ((tagname == CALIB_SENSOR_AF_CONTRAST_AF)
                   && (tag.isType(XmlTag::TAG_TYPE_STRUCT))
                   && (tag.Size() > 0)) {
            const XMLNode* psecsubchild = pchild->ToElement()->FirstChild();
            while (psecsubchild) {
                XmlTag secsubtag = XmlTag(psecsubchild->ToElement());
                std::string secsubTagname(psecsubchild->ToElement()->Name());
                if (secsubTagname == CALIB_SENSOR_AF_CONTRAST_ENABLE
                        && (secsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (secsubtag.Size() > 0)) {
                    int no = ParseUcharArray(psecsubchild, &mCalibDb->af.contrast_af.enable, secsubtag.Size());
                    DCT_ASSERT((no == secsubtag.Size()));
                } else if (secsubTagname == CALIB_SENSOR_AF_SEARCH_STRATEGY
                           && (secsubtag.isType(XmlTag::TAG_TYPE_CHAR))
                           && (secsubtag.Size() > 0)) {
                    char* value = Toupper(secsubtag.Value());
                    std::string s_value(value);
                    if (s_value == CALIB_SENSOR_AF_SEARCH_STRATEGY_ADAPTIVE) {
                        mCalibDb->af.contrast_af.Afss = CAM_AFM_FSS_ADAPTIVE_RANGE;
                    } else if (s_value == CALIB_SENSOR_AF_SEARCH_STRATEGY_HILLCLIMB) {
                        mCalibDb->af.contrast_af.Afss = CAM_AFM_FSS_HILLCLIMBING;
                    } else if (s_value == CALIB_SENSOR_AF_SEARCH_STRATEGY_FULL) {
                        mCalibDb->af.contrast_af.Afss = CAM_AFM_FSS_FULLRANGE;
                    }
                } else if (secsubTagname == CALIB_SENSOR_AF_FULL_DIR
                           && (secsubtag.isType(XmlTag::TAG_TYPE_CHAR))
                           && (secsubtag.Size() > 0)) {
                    char* value = Toupper(secsubtag.Value());
                    std::string s_value(value);
                    if (s_value == CALIB_SENSOR_AF_DIR_POSITIVE) {
                        mCalibDb->af.contrast_af.FullDir = CAM_AFM_POSITIVE_SEARCH;
                    } else if (s_value == CALIB_SENSOR_AF_DIR_NEGATIVE) {
                        mCalibDb->af.contrast_af.FullDir = CAM_AFM_NEGATIVE_SEARCH;
                    } else if (s_value == CALIB_SENSOR_AF_DIR_ADAPTIVE) {
                        mCalibDb->af.contrast_af.FullDir = CAM_AFM_ADAPTIVE_SEARCH;
                    }
                } else if (secsubTagname == CALIB_SENSOR_AF_FULL_RANGE_TBL
                           && (secsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                           && (secsubtag.Size() > 0)) {
                    int ArraySize     = secsubtag.Size();
                    mCalibDb->af.contrast_af.FullSteps = ArraySize;
                    int no = ParseUshortArray(psecsubchild, mCalibDb->af.contrast_af.FullRangeTbl, ArraySize );
                    DCT_ASSERT((no == secsubtag.Size()));
                } else if (secsubTagname == CALIB_SENSOR_AF_ADAPTIVE_DIR
                           && (secsubtag.isType(XmlTag::TAG_TYPE_CHAR))
                           && (secsubtag.Size() > 0)) {
                    char* value = Toupper(secsubtag.Value());
                    std::string s_value(value);
                    if (s_value == CALIB_SENSOR_AF_DIR_POSITIVE) {
                        mCalibDb->af.contrast_af.AdaptiveDir = CAM_AFM_POSITIVE_SEARCH;
                    } else if (s_value == CALIB_SENSOR_AF_DIR_NEGATIVE) {
                        mCalibDb->af.contrast_af.AdaptiveDir = CAM_AFM_NEGATIVE_SEARCH;
                    } else if (s_value == CALIB_SENSOR_AF_DIR_ADAPTIVE) {
                        mCalibDb->af.contrast_af.AdaptiveDir = CAM_AFM_ADAPTIVE_SEARCH;
                    }
                } else if (secsubTagname == CALIB_SENSOR_AF_ADAPTIVE_RANGE_TBL
                           && (secsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                           && (secsubtag.Size() > 0)) {
                    int ArraySize     = secsubtag.Size();
                    mCalibDb->af.contrast_af.AdaptiveSteps = ArraySize;
                    int no = ParseUshortArray(psecsubchild, mCalibDb->af.contrast_af.AdaptRangeTbl, ArraySize );
                    DCT_ASSERT((no == secsubtag.Size()));
                } else if (secsubTagname == CALIB_SENSOR_AF_TRIG_THERS
                           && (secsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                           && (secsubtag.Size() > 0)) {
                    int no = ParseFloatArray(psecsubchild, &mCalibDb->af.contrast_af.TrigThers, secsubtag.Size());
                    DCT_ASSERT((no == secsubtag.Size()));
                } else if (secsubTagname == CALIB_SENSOR_AF_STABLE_THERS
                           && (secsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                           && (secsubtag.Size() > 0)) {
                    int no = ParseFloatArray(psecsubchild, &mCalibDb->af.contrast_af.StableThers, secsubtag.Size());
                    DCT_ASSERT((no == secsubtag.Size()));
                } else if (secsubTagname == CALIB_SENSOR_AF_STABLE_FRAMES
                           && (secsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                           && (secsubtag.Size() > 0)) {
                    int no = ParseUshortArray(psecsubchild, &mCalibDb->af.contrast_af.StableFrames, secsubtag.Size());
                    DCT_ASSERT((no == secsubtag.Size()));
                } else if (secsubTagname == CALIB_SENSOR_AF_STABLE_TIME
                           && (secsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                           && (secsubtag.Size() > 0)) {
                    int no = ParseUshortArray(psecsubchild, &mCalibDb->af.contrast_af.StableTime, secsubtag.Size());
                    DCT_ASSERT((no == secsubtag.Size()));
                } else if (secsubTagname == CALIB_SENSOR_AF_OUT_FOCUS_VALUE
                           && (secsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                           && (secsubtag.Size() > 0)) {
                    int no = ParseFloatArray(psecsubchild, &mCalibDb->af.contrast_af.OutFocusValue, secsubtag.Size());
                    DCT_ASSERT((no == secsubtag.Size()));
                } else if (secsubTagname == CALIB_SENSOR_AF_OUT_FOCUS_POS
                           && (secsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                           && (secsubtag.Size() > 0)) {
                    int no = ParseUshortArray(psecsubchild, &mCalibDb->af.contrast_af.OutFocusPos, secsubtag.Size());
                    DCT_ASSERT((no == secsubtag.Size()));
                } else if (secsubTagname == CALIB_SENSOR_AF_GAMMA_Y
                           && (secsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                           && (secsubtag.Size() > 0)) {
                    int no = ParseUshortArray(psecsubchild, mCalibDb->af.contrast_af.gammaY, secsubtag.Size());
                    DCT_ASSERT((no == secsubtag.Size()));
                } else if (secsubTagname == CALIB_SENSOR_AF_GAUSS_WEIGHT
                           && (secsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                           && (secsubtag.Size() > 0)) {
                    int no = ParseUcharArray(psecsubchild, mCalibDb->af.contrast_af.gaussWeight, secsubtag.Size());
                    DCT_ASSERT((no == secsubtag.Size()));
                }
                psecsubchild = psecsubchild->NextSibling();
            }
        } else if ((tagname == CALIB_SENSOR_AF_LASER_AF)
                   && (tag.isType(XmlTag::TAG_TYPE_STRUCT))
                   && (tag.Size() > 0)) {
            const XMLNode* psecsubchild = pchild->ToElement()->FirstChild();
            while (psecsubchild) {
                XmlTag secsubtag = XmlTag(psecsubchild->ToElement());
                std::string secsubTagname(psecsubchild->ToElement()->Name());
                if (secsubTagname == CALIB_SENSOR_AF_LASERAF_ENABLE
                        && (secsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (secsubtag.Size() > 0)) {
                    int no = ParseUcharArray(psecsubchild, &mCalibDb->af.laser_af.enable, secsubtag.Size());
                    DCT_ASSERT((no == secsubtag.Size()));
                } else if (secsubTagname == CALIB_SENSOR_AF_LASER_AF_VCMDOT
                           && (secsubtag.isType(XmlTag::TAG_TYPE_CHAR))
                           && (secsubtag.Size() > 0)) {
                    int no = ParseFloatArray(psecsubchild, mCalibDb->af.laser_af.vcmDot, secsubtag.Size());
                    DCT_ASSERT((no == secsubtag.Size()));
                } else if (secsubTagname == CALIB_SENSOR_AF_LASER_AF_DISTANCEDOT
                           && (secsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                           && (secsubtag.Size() > 0)) {
                    int no = ParseFloatArray(psecsubchild, mCalibDb->af.laser_af.distanceDot, secsubtag.Size());
                    DCT_ASSERT((no == secsubtag.Size()));
                }
                psecsubchild = psecsubchild->NextSibling();
            }
        } else if ((tagname == CALIB_SENSOR_AF_PDAF)
                   && (tag.isType(XmlTag::TAG_TYPE_STRUCT))
                   && (tag.Size() > 0)) {
            const XMLNode* psecsubchild = pchild->ToElement()->FirstChild();
            while (psecsubchild) {
                XmlTag secsubtag = XmlTag(psecsubchild->ToElement());
                std::string secsubTagname(psecsubchild->ToElement()->Name());
                if (secsubTagname == CALIB_SENSOR_AF_PDAF_ENABLE
                        && (secsubtag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (secsubtag.Size() > 0)) {
                    int no = ParseUcharArray(psecsubchild, &mCalibDb->af.pdaf.enable, secsubtag.Size());
                    DCT_ASSERT((no == secsubtag.Size()));
                }
                psecsubchild = psecsubchild->NextSibling();
            }
        }
        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorLdch
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD( "%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());
        if ((tagname == CALIB_SENSOR_LDCH_ENABLE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->aldch.ldch_en, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if (tagname == CALIB_SENSOR_LDCH_MESH_FILE) {
            char mesh_filename[256];
            ParseString(pchild, mCalibDb->aldch.meshfile, sizeof(mesh_filename));
        } else if (tagname == CALIB_SENSOR_LDCH_CORRECT_LEVEL
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no =  ParseDoubleArray(pchild, &mCalibDb->aldch.correct_level, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorLumaDetect
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD( "%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());
        if ((tagname == CALIB_SENSOR_LUMA_DETECT_ENABLE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->lumaDetect.luma_detect_en, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if (tagname == CALIB_SENSOR_LUMA_DETECT_THRESHOLD
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no =  ParseFloatArray(pchild, &mCalibDb->lumaDetect.mutation_threshold, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorFec
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD( "%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());

        if ((tagname == CALIB_SENSOR_FEC_ENABLE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->afec.fec_en, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if (tagname == CALIB_SENSOR_FEC_MESH_FILE) {
            char mesh_filename[256];
            ParseString(pchild, mCalibDb->afec.meshfile, sizeof(mesh_filename));
        } else if (tagname == CALIB_SENSOR_FEC_CORRECT_LEVEL
                   && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                   && (tag.Size() > 0)) {
            int no =  ParseDoubleArray(pchild, &mCalibDb->afec.correct_level, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorOrb
(
    const XMLElement*   pelement,
    void*               param
) {
    (void)param;

    LOGD( "%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());
        if ((tagname == CALIB_SENSOR_ORB_ENABLE)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->orb.orb_en, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySensorCpsl
(
    const XMLElement* pelement,
    void* param
) {
    (void)param;

    LOGD( "%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();

    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());
        if ((tagname == CALIB_SENSOR_CPSL_ENABLE)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->cpsl.support_en, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_CPSL_MODE)
                && (tag.isType(XmlTag::TAG_TYPE_INT))
                && (tag.Size() > 0)) {
            int no = ParseIntArray(pchild, &mCalibDb->cpsl.mode, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_CPSL_FORCE_GRAY)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->cpsl.gray, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_CPSL_LGHT_SRC)
                && (tag.isType(XmlTag::TAG_TYPE_INT))
                && (tag.Size() > 0)) {
            int no = ParseIntArray(pchild, &mCalibDb->cpsl.lght_src, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_CPSL_AUTO_ADJUST_SENS)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->cpsl.ajust_sens, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_CPSL_AUTO_ON2OFF_TH)
                && (tag.isType(XmlTag::TAG_TYPE_INT))
                && (tag.Size() > 0)) {
            int no = ParseUintArray(pchild, &mCalibDb->cpsl.on2off_th, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_CPSL_AUTO_OFF2ON_TH)
                && (tag.isType(XmlTag::TAG_TYPE_INT))
                && (tag.Size() > 0)) {
            int no = ParseUintArray(pchild, &mCalibDb->cpsl.off2on_th, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_CPSL_AUTO_SW_INTERVAL)
                && (tag.isType(XmlTag::TAG_TYPE_INT))
                && (tag.Size() > 0)) {
            int no = ParseUintArray(pchild, &mCalibDb->cpsl.sw_interval, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_CPSL_MANUAL_ON)
                && (tag.isType(XmlTag::TAG_TYPE_CHAR))
                && (tag.Size() > 0)) {
            int no = ParseUcharArray(pchild, &mCalibDb->cpsl.cpsl_on, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        } else if ((tagname == CALIB_SENSOR_CPSL_MANUAL_STRENGTH)
                && (tag.isType(XmlTag::TAG_TYPE_DOUBLE))
                && (tag.Size() > 0)) {
            int no = ParseFloatArray(pchild, &mCalibDb->cpsl.strength, tag.Size());
            DCT_ASSERT((no == tag.Size()));
        }
        pchild = pchild->NextSibling();
    }
    LOGD("%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    autoTabBackward();
    return (true);
}

bool RkAiqCalibParser::parseEntrySystem
(
    const XMLElement*   pelement,
    void*                param
) {
    LOGD( "%s(%d): (enter)\n", __FUNCTION__, __LINE__);
    autoTabForward();
    const XMLNode* pchild = pelement->FirstChild();
    while (pchild) {
        XmlTag tag = XmlTag(pchild->ToElement());
        std::string tagname(pchild->ToElement()->Name());
        if (tagname == CALIB_SYSTEM_HDR) {
            const XMLNode* psecsubchild = pchild->ToElement()->FirstChild();
            while (psecsubchild) {
                XmlTag subTag = XmlTag(psecsubchild->ToElement());
                const char* value = subTag.Value();
                std::string subTagname(psecsubchild->ToElement()->Name());
                if ((subTagname == CALIB_SYSTEM_HDR_ENABLE)
                        && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (subTag.Size() > 0)) {
                    INFO_PRINT(tagname);
                    int no = ParseUcharArray(psecsubchild, &mCalibDb->sysContrl.hdr_en, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                } else if (subTagname == CALIB_SYSTEM_HDR_SUPPORT_MODE) {
                    INFO_PRINT(tagname);
                    char hdr_mode[32];
                    ParseString(psecsubchild, hdr_mode, sizeof(hdr_mode));
                    if (0 == strcasecmp(hdr_mode, HDR_MODE_2_FRAME_STR))
                        mCalibDb->sysContrl.hdr_mode = RK_AIQ_ISP_HDR_MODE_2_FRAME_HDR;
                    else if (0 == strcasecmp(hdr_mode, HDR_MODE_2_LINE_STR))
                        mCalibDb->sysContrl.hdr_mode = RK_AIQ_ISP_HDR_MODE_2_LINE_HDR;
                    else if (0 == strcasecmp(hdr_mode, HDR_MODE_3_FRAME_STR))
                        mCalibDb->sysContrl.hdr_mode = RK_AIQ_ISP_HDR_MODE_3_FRAME_HDR;
                    else if (0 == strcasecmp(hdr_mode, HDR_MODE_3_LINE_STR))
                        mCalibDb->sysContrl.hdr_mode = RK_AIQ_ISP_HDR_MODE_3_LINE_HDR;
                } else if (subTagname == CALIB_SYSTEM_HDR_LINE_MODE) {
                    INFO_PRINT(tagname);
                    char line_mode[16];
                    ParseString(psecsubchild, line_mode, sizeof(line_mode));
                    if (0 == strcasecmp(line_mode, HDR_LINE_MODE_DCG_STR))
                        mCalibDb->sysContrl.line_mode = RK_AIQ_SENSOR_HDR_LINE_MODE_DCG;
                    else if (0 == strcasecmp(line_mode, HDR_LINE_MODE_STAGGER_STR))
                        mCalibDb->sysContrl.line_mode = RK_AIQ_SENSOR_HDR_LINE_MODE_STAGGER;
                }
                psecsubchild = psecsubchild->NextSibling();
            }
        } else if(tagname == CALIB_SYSTEM_DCG_SETTING) {
            const XMLNode* psecsubchild = pchild->ToElement()->FirstChild();
            while (psecsubchild) {
                XmlTag subTag = XmlTag(psecsubchild->ToElement());
                const char* value = subTag.Value();
                std::string subTagname(psecsubchild->ToElement()->Name());
                if ((subTagname == CALIB_SYSTEM_DCG_SUPPORT_EN)
                        && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (subTag.Size() > 0)) {
                    uint8_t tmp = mCalibDb->sysContrl.dcg.support_en;
                    int no = ParseUcharArray(psecsubchild, &tmp, subTag.Size());
                    mCalibDb->sysContrl.dcg.support_en = (tmp == 1) ? true : false;
                    DCT_ASSERT((no == subTag.Size()));
                } else if ((subTagname == CALIB_SYSTEM_DCG_OPTYPE)
                           && (subTag.isType(XmlTag::TAG_TYPE_CHAR))
                           && (subTag.Size() > 0)) {
                    char* value = Toupper(subTag.Value());
                    std::string s_value(value);
                    if (xmlParseReadWrite == XML_PARSER_READ)
                    {
                        if (s_value == CALIB_SYSTEM_DCG_OPTYPE_AUTO) {
                            mCalibDb->sysContrl.dcg.dcg_optype = RK_AIQ_OP_MODE_AUTO;
                        }
                        else if (s_value == CALIB_SYSTEM_DCG_OPTYPE_MANUAL) {
                            mCalibDb->sysContrl.dcg.dcg_optype = RK_AIQ_OP_MODE_MANUAL;
                        }
                        else {
                            mCalibDb->sysContrl.dcg.dcg_optype = RK_AIQ_OP_MODE_INVALID;
                            LOGE("%s(%d): invalid dcg OpType = %s\n", __FUNCTION__, __LINE__, s_value);
                        }
                    }
                    else if (xmlParseReadWrite == XML_PARSER_WRITE)
                    {
                        XMLNode *pNode = (XMLNode*)psecsubchild;
                        if (mCalibDb->sysContrl.dcg.dcg_optype == RK_AIQ_OP_MODE_AUTO) {
                            pNode->FirstChild()->SetValue(CALIB_SYSTEM_DCG_OPTYPE_AUTO);
                        }
                        else if (mCalibDb->sysContrl.dcg.dcg_optype == RK_AIQ_OP_MODE_MANUAL) {
                            pNode->FirstChild()->SetValue(CALIB_SYSTEM_DCG_OPTYPE_MANUAL);
                        }
                        else {
                            pNode->FirstChild()->SetValue("Invalid");
                            LOGE("%s(%d): (XML Write)invalid dcg OpType = %d\n", __FUNCTION__, __LINE__, mCalibDb->sysContrl.dcg.dcg_optype);
                        }
                    }
                } else if ((subTagname == CALIB_SYSTEM_DCG_MODE_INIT)
                           && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                           && (subTag.Size() > 0)) {
                    int no = ParseIntArray(psecsubchild, mCalibDb->sysContrl.dcg.dcg_mode.Coeff, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                } else if ((subTagname == CALIB_SYSTEM_DCG_RATIO)
                           && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                           && (subTag.Size() > 0)) {
                    int no = ParseFloatArray(psecsubchild, &mCalibDb->sysContrl.dcg.dcg_ratio, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                } else if ((subTagname == CALIB_SYSTEM_DCG_SYNC_SWITCH)
                           && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                           && (subTag.Size() > 0)) {
                    uint8_t tmp = mCalibDb->sysContrl.dcg.sync_switch;
                    int no = ParseUcharArray(psecsubchild, &tmp, subTag.Size());
                    mCalibDb->sysContrl.dcg.sync_switch = (tmp == 1) ? true : false;
                    DCT_ASSERT((no == subTag.Size()));
                } else if (subTagname == CALIB_SYSTEM_DCG_GAINCTRL) {
                    const XMLNode* pthrdsubchild = psecsubchild->ToElement()->FirstChild();
                    while (pthrdsubchild) {
                        XmlTag secsubTag = XmlTag(pthrdsubchild->ToElement());
                        const char* value = secsubTag.Value();
                        std::string secsubTagname(pthrdsubchild->ToElement()->Name());
                        if ((secsubTagname == CALIB_SYSTEM_HDR_ENABLE)
                                && (secsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                && (secsubTag.Size() > 0)) {
                            uint8_t tmp = mCalibDb->sysContrl.dcg.gainCtrl_en;
                            int no = ParseUcharArray(pthrdsubchild, &tmp, secsubTag.Size());
                            mCalibDb->sysContrl.dcg.gainCtrl_en = (tmp == 1) ? true : false;
                            DCT_ASSERT((no == secsubTag.Size()));
                        } else if (secsubTagname == CALIB_SYSTEM_DCG_LCG2HCG_TH) {
                            int no = ParseFloatArray(pthrdsubchild, &mCalibDb->sysContrl.dcg.lcg2hcg_gain_th, secsubTag.Size());
                            DCT_ASSERT((no == secsubTag.Size()));
                        } else if(secsubTagname == CALIB_SYSTEM_DCG_HCG2LCG_TH) {
                            int no = ParseFloatArray(pthrdsubchild, &mCalibDb->sysContrl.dcg.hcg2lcg_gain_th, secsubTag.Size());
                            DCT_ASSERT((no == secsubTag.Size()));
                        }
                        pthrdsubchild = pthrdsubchild->NextSibling();
                    }
                } else if(subTagname == CALIB_SYSTEM_DCG_ENVCTRL) {
                    const XMLNode* pthrdsubchild = psecsubchild->ToElement()->FirstChild();
                    while (pthrdsubchild) {
                        XmlTag secsubTag = XmlTag(pthrdsubchild->ToElement());
                        const char* value = secsubTag.Value();
                        std::string secsubTagname(pthrdsubchild->ToElement()->Name());
                        if ((secsubTagname == CALIB_SYSTEM_HDR_ENABLE)
                                && (secsubTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                                && (secsubTag.Size() > 0)) {
                            uint8_t tmp = mCalibDb->sysContrl.dcg.envCtrl_en;
                            int no = ParseUcharArray(pthrdsubchild, &tmp, secsubTag.Size());
                            mCalibDb->sysContrl.dcg.envCtrl_en = (tmp == 1) ? true : false;
                            DCT_ASSERT((no == secsubTag.Size()));
                        } else if (secsubTagname == CALIB_SYSTEM_DCG_LCG2HCG_TH) {
                            int no = ParseFloatArray(pthrdsubchild, &mCalibDb->sysContrl.dcg.lcg2hcg_env_th, secsubTag.Size());
                            DCT_ASSERT((no == secsubTag.Size()));
                        } else if(secsubTagname == CALIB_SYSTEM_DCG_HCG2LCG_TH) {
                            int no = ParseFloatArray(pthrdsubchild, &mCalibDb->sysContrl.dcg.hcg2lcg_env_th, secsubTag.Size());
                            DCT_ASSERT((no == secsubTag.Size()));
                        }
                        pthrdsubchild = pthrdsubchild->NextSibling();
                    }
                }
                psecsubchild = psecsubchild->NextSibling();
            }
        } else if(tagname == CALIB_SYSTEM_EXP_DELAY) {
            const XMLNode* psecsubchild = pchild->ToElement()->FirstChild();
            while (psecsubchild) {
                XmlTag subTag = XmlTag(psecsubchild->ToElement());
                const char* value = subTag.Value();
                std::string subTagname(psecsubchild->ToElement()->Name());
                if ((subTagname == CALIB_SYSTEM_EXP_DELAY_TIME)
                        && (subTag.isType(XmlTag::TAG_TYPE_DOUBLE))
                        && (subTag.Size() > 0)) {
                    int no = ParseIntArray(psecsubchild, &mCalibDb->sysContrl.time_delay, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                } else if (subTagname == CALIB_SYSTEM_EXP_DELAY_GAIN) {
                    int no = ParseIntArray(psecsubchild, &mCalibDb->sysContrl.gain_delay, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                } else if (subTagname == CALIB_SYSTEM_EXP_DELAY_DCG) {
                    int no = ParseIntArray(psecsubchild, &mCalibDb->sysContrl.dcg_delay, subTag.Size());
                    DCT_ASSERT((no == subTag.Size()));
                }
                psecsubchild = psecsubchild->NextSibling();
            }
        }
        pchild = pchild->NextSibling();
    }

    autoTabBackward();
    LOGD( "%s(%d): (exit)\n", __FUNCTION__, __LINE__);
    return (true);
}

#undef DCT_ASSERT
};
