/*
 * tests.c
 *
 *  Created on: Jun 17, 2019
 *      Author: joshd
 */

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "os_task.h"
#include "tests.h"

#include <redfs.h>
#include <redposix.h>
#include <redfse.h>
#include <redconf.h>
#include <redvolume.h>
#include <redtests.h>

void foo(void *p)
{
    //remove compiler warning about unused parameters
    (void)p;

    //can use either VOL0 or VOL1
    int fd = red_open("VOL0:/newFile", RED_O_RDWR | RED_O_CREAT);
    char buff[20];

    if (fd < 0)
    {
        switch (red_errno)
        {
        case RED_EINVAL:
            fprintf(stderr, "ulOpenMode is invalid; or pszPath is NULL ; or the volume containing the path is not mounted; or RED_O_CREAT is included in ulOpenMode , and the path ends with dot or dot-dot.\n");
            break;
        case RED_EEXIST:
            fprintf(stderr, "file already exists\n");
            break;
        }
        REDERROR();
    }

    int iErr = red_write(fd, "Some stuff here", 20);
    if (iErr < 0)
    {
        fprintf(stderr, "Write failed with err %d\n", (int)red_errno);
        exit(red_errno);
    }

    iErr = red_lseek(fd, 0, RED_SEEK_SET);
    if (iErr < 0)
    {
        fprintf(stderr, "Seek file failed %d\n", (int)red_errno);
        exit(red_errno);
    }

    iErr = red_read(fd, buff, 20);
    if (iErr < 0)
    {
        fprintf(stderr, "Write failed with err %d\n", (int)red_errno);
        exit(red_errno);
    }
    printf("%s\n", buff);
}

void vStartFSWriteTask(UBaseType_t uxPriority)
{

    xTaskCreate(foo, "foo", configMINIMAL_STACK_SIZE, NULL, uxPriority, (TaskHandle_t *)NULL);
}
