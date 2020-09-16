
#include "rtos_example_tasks.h"

#include "HL_het.h"
#include "HL_gio.h"

#include <redfs.h>
#include <redposix.h>
#include <redfse.h>
#include <redconf.h>
#include <redvolume.h>
#include <redtests.h>

#include "FreeRTOS.h"
#include "os_task.h"
#include "integer.h"
#include "sd_io.h"
#include "stdio.h"

///* Task1 */
//void vBlinkyTask(void *pvParameters)
//{
//    for(;;)
//    {
//        /* Taggle HET[1] with timer tick */
//        gioSetBit(hetPORT1, 18, 1);
//        vTaskDelay(100);
//        gioSetBit(hetPORT1, 18, 0);
//        vTaskDelay(100);
//    }
//}
//
//void vTaskFileSys(void *pvParameters)
//{
//
//    int32_t iErr;
//    const char *pszVolume0 = gaRedVolConf[0].pszPathPrefix;
//
//    iErr = red_init();
//    if (iErr == -1)
//    {
//        fprintf(stderr, "Unexpected error %d from red_init()\r\n", (int)red_errno);
//        exit(red_errno);
//    }
//
//    iErr = red_format(pszVolume0);
//    if (iErr == -1)
//    {
//        fprintf(stderr, "Unexpected error %d from red_format()\r\n", (int)red_errno);
//        exit(red_errno);
//    }
//
//    iErr = red_mount(pszVolume0);
//    if (iErr == -1)
//    {
//        fprintf(stderr, "Unexpected error %d from red_mount()\r\n", (int)red_errno);
//        exit(red_errno);
//    }
//
//    fprintf(stderr, "Mounted\r\n");
//
//    char buf[1024] = "";
//
//    red_getcwd(buf, 1024);
//
//    fprintf(stderr, "CWD = %s\r\n", buf);
//
//    iErr = red_mkdir("home");
//    if (iErr == -1)
//    {
//        fprintf(stderr, "Unexpected error %d from red_mkdir()\r\n", (int)red_errno);
//        exit(red_errno);
//    }
//    iErr = red_chdir("home");
//    if (iErr == -1)
//    {
//        fprintf(stderr, "Unexpected error %d from red_chdir()\r\n", (int)red_errno);
//        exit(red_errno);
//    }
//
//    red_getcwd(buf, 1024);
//
//    fprintf(stderr, "CWD = %s\r\n", buf);
//
//
//    int32_t file1;
//
//    file1 = red_open("damn.txt", RED_O_RDWR | RED_O_CREAT);
//    if (file1 == -1)
//    {
//        fprintf(stderr, "Unexpected error %d from red_open()\r\n", (int)red_errno);
//        exit(red_errno);
//    }
//
//    iErr = red_write(file1, "8 7 6 5 4 3 2 1\r\n", strlen("8 7 6 5 4 3 2 1\r\n"));
//    if (iErr == -1)
//    {
//        fprintf(stderr, "Unexpected error %d from red_write()\r\n", (int)red_errno);
//        exit(red_errno);
//    }
//    else{
//        fprintf(stderr, "%d bytes written\r\n", iErr);
//    }
//
//    iErr = red_close(file1);
//    if (iErr == -1)
//    {
//        fprintf(stderr, "Unexpected error %d from red_close()\r\n", (int)red_errno);
//        exit(red_errno);
//    }
//
//    file1 = red_open("damn.txt", RED_O_RDWR);
//    if (file1 == -1)
//    {
//        fprintf(stderr, "Unexpected error %d from red_open()\r\n", (int)red_errno);
//        exit(red_errno);
//    }
//
//    char readdata[30] = {0};
//
//    iErr = red_read(file1, readdata, strlen("1 2 3 4 5 6 7 8\r\n"));
//    if (iErr == -1)
//    {
//        fprintf(stderr, "Unexpected error %d from red_read()\r\n", (int)red_errno);
//        exit(red_errno);
//    }
//    else{
//        fprintf(stderr, "%d B read\r\n", iErr);
//    }
//
//    iErr = red_close(file1);
//    if (iErr == -1)
//    {
//        fprintf(stderr, "Unexpected error %d from red_close()\r\n", (int)red_errno);
//        exit(red_errno);
//    }
//}
