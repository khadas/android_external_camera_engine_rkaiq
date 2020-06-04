
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include "RkAiqCalibDb.h"

using namespace RkCam;
#define CALIBDB_PATH_LEN      128

static void getFilePathName(char* iqFile, char *location)
{
    char dir[CALIBDB_PATH_LEN];
    char name[CALIBDB_PATH_LEN];
    char *srcfile = strdup(iqFile);
    char *pstart = strrchr(srcfile,'/');
    char *pend = strrchr(srcfile,'.');
    *pend = '\0';
    strcpy(name, pstart+1);
    sprintf(location, "%s.bin", srcfile);
    free(srcfile);
    printf("calibdb file is %s",location);
}

static bool removeCalibBinIfExist(char* iqFile){
    char path[CALIBDB_PATH_LEN];

    getFilePathName(iqFile, path);
    if (0==access(path, F_OK))
        remove(path);
}

int main(int argc, char **argv)
{
    char config_file[CALIBDB_PATH_LEN];
    strcpy(config_file, argv[1]);
    removeCalibBinIfExist(config_file);
    RkAiqCalibDb::createCalibDb(config_file);
    RkAiqCalibDb::releaseCalibDb();
    return 0;
}
