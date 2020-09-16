//
//#include "testing_CLI.h"
//#include "HL_het.h"
//#include "HL_gio.h"
//#include "board_io_tests.h"
//#include "ina226.h"
//#include "deployablescontrol.h"
//#include "demux_handler.h"
//#include "rtcmk.h"
//#include "tmp421.h"
//#include "tempsense_athena.h"
//#include "testing_CLI.h"
//#include "rtos_example_tasks.h"
//#include "HL_spi.h"
//#include "HL_sci.h"
//#include "HL_sys_core.h"
//#include "HL_system.h"
//#include "stdlib.h"
//#include "FreeRTOS.h"
//#include "os_task.h"
//
/////* Define Task Handles */
//xTaskHandle xTask1Handle;
//
//void CLIhandler(void){
//
//    //NOTE: COM3 serial port used with xds110 debugger aux UART connection baud 9600 stop bits: 2
//    //NOTE: ensure that Athena Rx pin is the one on the stack header. Using the one on the separate 100 mil header will not work due to buffer IC problems
//
//    //TODO: Fully test all things. "somewhat tested" = CLI structure and individual rx/tx functions verified, but not tested together
//    //TODO: implement SDRAM test (once it's working)
//    //TODO: implement filesystem calls and more complex tests
//
//    while(1){
//        sciSend(sciREG4, strlen("\r\n\r\nCurrent command list:\r\n\r\n"), (unsigned char *)"\r\n\r\nCurrent command list:\r\n\r\n");//make sure space and reset interrupts are working
//
//        sciSend(sciREG4, strlen("l - LED blinky FreeRTOS test\r\n"), (unsigned char *)"l - LED blinky FreeRTOS test\r\n");//tested
//        sciSend(sciREG4, strlen("s - SPI master test\r\n"), (unsigned char *)"s - SPI master test\r\n");//somewhat tested
//        sciSend(sciREG4, strlen("u - UART test\r\n"), (unsigned char *)"u - UART test\r\n");//somewhat tested
//        sciSend(sciREG4, strlen("i - I2C master test\r\n"), (unsigned char *)"i - I2C master test\r\n");//somewhat tested
//        sciSend(sciREG4, strlen("g - GPIO test\r\n"), (unsigned char *)"g - GPIO test\r\n");//somewhat tested
//        sciSend(sciREG4, strlen("c - CAN test\r\n"), (unsigned char *)"c - CAN test\r\n");//somewhat tested
//        sciSend(sciREG4, strlen("r - RTC test\r\n"), (unsigned char *)"r - RTC test\r\n");//somewhat tested
//        sciSend(sciREG4, strlen("t - Temperature sensor test\r\n"), (unsigned char *)"t - Temperature sensor test\r\n");//tested
//        sciSend(sciREG4, strlen("b - Burnwire test\r\n"), (unsigned char *)"b - Burnwire test\r\n");//tested
//        sciSend(sciREG4, strlen("v - Deployment verification switch test\r\n"), (unsigned char *)"v - Deployment verification switch test\r\n");//tested
//        sciSend(sciREG4, strlen("p - Solar panel chip select test\r\n"), (unsigned char *)"p - Solar panel chip select test\r\n");//tested
//        sciSend(sciREG4, strlen("m - Solar panel current measurement test\r\n\r\n"), (unsigned char *)"m - Solar panel current measurement test\r\n\r\n");//tested
//
//        sciSend(sciREG4, strlen("Note that some tests may get stuck, and a reset is needed to fix them\r\n"), (unsigned char *)"Note that some tests may get stuck, and a reset is needed to fix them\r\n");
//        sciSend(sciREG4, strlen("Sometimes multiple keypresses are required for command to be registered\r\n\r\n"), (unsigned char *)"Sometimes multiple keypresses are required for command to be registered\r\n\r\n");
//
//        unsigned char command = 0;
//        while(1){
//            if(sciIsRxReady(sciREG4)){
//                command = sciReceiveByte(sciREG4);//sometimes takes a few key presses to get right
//                if (command != 0){
//                    break;
//                }
//            }
//        }
//        sciSend(sciREG4, 1, (unsigned char *)command);//reflect selected char
//        sciSend(sciREG4, strlen("\r\n"), (unsigned char *)"\r\n");
//
//        switch(command){
//            case 'l': {
//                sciSend(sciREG4, strlen("Blinking LED using FreeRTOS\r\n"), (unsigned char *)"Blinking LED using FreeRTOS\r\n");
//                sciSend(sciREG4, strlen("Reset device to exit test\r\n"), (unsigned char *)"Reset device to exit test\r\n");
//
//                /* Create Task 1 */
//                if (xTaskCreate(vBlinkyTask,"BlinkyTask", configMINIMAL_STACK_SIZE, NULL, 1, &xTask1Handle) != pdTRUE)
//                {
//                    /* Task could not be created */
//                    while(1);
//                }
//
//                /* Start Scheduler */
//                vTaskStartScheduler();
//
//                /* Run forever */
//                while(1);
//            }
//            case 's': {
//                unsigned char cmd2;
//                sciSend(sciREG4, strlen("Testing Stack SPI as a master\r\n"), (unsigned char *)"Testing Stack SPI as a master\r\n");
//                sciSend(sciREG4, strlen("Slave testing (SPI3) requires custom program using SPISlaveTxTest() and SPISlaveRxTest()\r\n"), (unsigned char *)"Slave testing (SPI3) requires custom program using SPISlaveTxTest() and SPISlaveRxTest()\r\n");
//                sciSend(sciREG4, strlen("Press t to test tx, or r to test rx\r\n"), (unsigned char *)"Press t to test tx, or r to test rx\r\n");
//                sciSend(sciREG4, strlen("Press space to exit test\r\n"), (unsigned char *)"Press space to exit test\r\n");
//                while(1){
//                    cmd2 = sciReceiveByte(sciREG4);
//                    if(cmd2 == 't'){
//                        unsigned char cmd3;
//                        sciSend(sciREG4, strlen("Press 1 or 2 to enable SPI1 or SPI2 tx, respectively\r\n"), (unsigned char *)"Press 1 or 2 to enable SPI1 or SPI2 tx, respectively\r\n");
//                        while(1){
//                            cmd3 = sciReceiveByte(sciREG4);
//                            if(cmd3 == '1'){
//                                SPIMasterTxTest(spiREG4);
//                                sciSend(sciREG4, strlen("1 kB sent over SPI1\r\n"), (unsigned char *)"1 kB sent over SPI1\r\n");
//                                break;
//                            }
//                            else if(cmd3 == '2'){
//                                SPIMasterTxTest(spiREG3);
//                                sciSend(sciREG4, strlen("1 kB sent over SPI2\r\n"), (unsigned char *)"1 kB sent over SPI2\r\n");
//                                break;
//                            }
//                            else if(cmd3 == ' '){
//                                break;
//                            }
//                        }
//                        int i;
//                        for(i=0;i<100000000;i++);
//                        break;
//                    }
//                    if(cmd2 == 'r'){
//                        uint8_t recvbytes = 0;
//                        char recvbytesstr[4] = {0};
//                        unsigned char cmd3;
//                        sciSend(sciREG4, strlen("Press 1 or 2 to enable SPI1 or SPI2 rx, respectively\r\n"), (unsigned char *)"Press 1 or 2 to enable SPI1 or SPI2 rx, respectively\r\n");
//                        sciSend(sciREG4, strlen("Make sure to enable rx here before data is sent from board transmitting data\r\n"), (unsigned char *)"Make sure to enable rx here before data is sent from board transmitting data\r\n");
//                        while(1){
//                            cmd3 = sciReceiveByte(sciREG4);
//                            if(cmd3 == '1'){
//                                sciSend(sciREG4, strlen("Waiting for data on SPI1..."), (unsigned char *)"Waiting for data on SPI1...");
//                                recvbytes = 2*(SPIMasterRxTest(spiREG4));
//                                ltoa(recvbytes, recvbytesstr);
//                                sciSend(sciREG4, strlen("Bytes expected = 1000"), (unsigned char *)"Bytes expected = 1000");
//                                sciSend(sciREG4, strlen("Bytes received over SPI1 = "), (unsigned char *)"Bytes received over SPI1 = ");
//                                sciSend(sciREG4, 4, (unsigned char *)recvbytesstr);
//                                sciSend(sciREG4, strlen("\r\n"), (unsigned char *)"\r\n");
//                                break;
//                            }
//                            else if(cmd3 == '2'){
//                                sciSend(sciREG4, strlen("Waiting for data on SPI2..."), (unsigned char *)"Waiting for data on SPI2...");
//                                recvbytes = 2*(SPIMasterRxTest(spiREG3));
//                                ltoa(recvbytes, recvbytesstr);
//                                sciSend(sciREG4, strlen("Bytes expected = 1000"), (unsigned char *)"Bytes expected = 1000");
//                                sciSend(sciREG4, strlen("Bytes received over SPI2 = "), (unsigned char *)"Bytes received over SPI2 = ");
//                                sciSend(sciREG4, 4, (unsigned char *)recvbytesstr);
//                                sciSend(sciREG4, strlen("\r\n"), (unsigned char *)"\r\n");
//                                break;
//                            }
//                            else if(cmd3 == ' '){
//                                break;
//                            }
//                        }
//                        int i;
//                        for(i=0;i<100000000;i++);
//                        break;
//                    }
//                    else if(cmd2 == ' '){
//                        break;
//                    }
//                }
//            break;
//            }
//            case 'u': {
//                unsigned char cmd2;
//                sciSend(sciREG4, strlen("Testing Stack UART\r\n"), (unsigned char *)"Testing Stack UART\r\n");
//                sciSend(sciREG4, strlen("Press t to test tx, or r to test rx\r\n"), (unsigned char *)"Press t to test tx, or r to test rx\r\n");
//                sciSend(sciREG4, strlen("Press space to exit test\r\n"), (unsigned char *)"Press space to exit test\r\n");
//                while(1){
//                    cmd2 = sciReceiveByte(sciREG4);
//                    if(cmd2 == 't'){
//                        unsigned char cmd3;
//                        sciSend(sciREG4, strlen("Press 2, 3, or 4 to enable UART2, UART3, or UART4 tx, respectively\r\n"), (unsigned char *)"Press 2, 3, or 4 to enable UART2, UART3, or UART4 tx, respectively\r\n");
//                        sciSend(sciREG4, strlen("NOTE: UART1 cannot be tested since it is used for serial terminal\r\n"), (unsigned char *)"NOTE: UART1 cannot be tested since it is used for serial terminal\r\n");
//                        while(1){
//                            cmd3 = sciReceiveByte(sciREG4);
//                            if(cmd3 == '2'){
//                                UARTTxTest(sciREG2);
//                                sciSend(sciREG4, strlen("1 kB sent over UART2\r\n"), (unsigned char *)"1 kB sent over UART2\r\n");
//                                break;
//                            }
//                            else if(cmd3 == '3'){
//                                UARTTxTest(sciREG1);
//                                sciSend(sciREG4, strlen("1 kB sent over UART3\r\n"), (unsigned char *)"1 kB sent over UART3\r\n");
//                                break;
//                            }
//                            else if(cmd3 == '4'){
//                                UARTTxTest(sciREG3);
//                                sciSend(sciREG4, strlen("1 kB sent over UART4\r\n"), (unsigned char *)"1 kB sent over UART4\r\n");
//                                break;
//                            }
//                            else if(cmd3 == ' '){
//                                break;
//                            }
//                        }
//                        int i;
//                        for(i=0;i<100000000;i++);
//                        break;
//                    }
//                    if(cmd2 == 'r'){
//                        uint8_t recvbytes = 0;
//                        char recvbytesstr[4] = {0};
//                        unsigned char cmd3;
//                        sciSend(sciREG4, strlen("Press 2, 3, or 4 to enable UART2, UART3, or UART4 rx, respectively\r\n"), (unsigned char *)"Press 2, 3, or 4 to enable UART2, UART3, or UART4 rx, respectively\r\n");
//                        sciSend(sciREG4, strlen("NOTE: UART1 cannot be tested since it is used for serial terminal\r\n"), (unsigned char *)"NOTE: UART1 cannot be tested since it is used for serial terminal\r\n");
//                        sciSend(sciREG4, strlen("Make sure to enable rx here before data is sent from board transmitting data\r\n"), (unsigned char *)"Make sure to enable rx here before data is sent from board transmitting data\r\n");
//                        while(1){
//                            cmd3 = sciReceiveByte(sciREG4);
//                            if(cmd3 == '2'){
//                                sciSend(sciREG4, strlen("Waiting for data on UART2..."), (unsigned char *)"Waiting for data on UART2...");
//                                recvbytes = UARTRxTest(sciREG2);
//                                ltoa(recvbytes, recvbytesstr);
//                                sciSend(sciREG4, strlen("Bytes expected = 1000"), (unsigned char *)"Bytes expected = 1000");
//                                sciSend(sciREG4, strlen("Bytes received over UART2 = "), (unsigned char *)"Bytes received over UART2 = ");
//                                sciSend(sciREG4, 4, (unsigned char *)recvbytesstr);
//                                sciSend(sciREG4, strlen("\r\n"), (unsigned char *)"\r\n");
//                                break;
//                            }
//                            else if(cmd3 == '3'){
//                                sciSend(sciREG4, strlen("Waiting for data on UART3..."), (unsigned char *)"Waiting for data on UART3...");
//                                recvbytes = UARTRxTest(sciREG1);
//                                ltoa(recvbytes, recvbytesstr);
//                                sciSend(sciREG4, strlen("Bytes expected = 1000"), (unsigned char *)"Bytes expected = 1000");
//                                sciSend(sciREG4, strlen("Bytes received over UART3 = "), (unsigned char *)"Bytes received over UART3 = ");
//                                sciSend(sciREG4, 4, (unsigned char *)recvbytesstr);
//                                sciSend(sciREG4, strlen("\r\n"), (unsigned char *)"\r\n");
//                                break;
//                            }
//                            else if(cmd3 == '4'){
//                                sciSend(sciREG4, strlen("Waiting for data on UART4..."), (unsigned char *)"Waiting for data on UART4...");
//                                recvbytes = UARTRxTest(sciREG3);
//                                ltoa(recvbytes, recvbytesstr);
//                                sciSend(sciREG4, strlen("Bytes expected = 1000"), (unsigned char *)"Bytes expected = 1000");
//                                sciSend(sciREG4, strlen("Bytes received over UART4 = "), (unsigned char *)"Bytes received over UART4 = ");
//                                sciSend(sciREG4, 4, (unsigned char *)recvbytesstr);
//                                sciSend(sciREG4, strlen("\r\n"), (unsigned char *)"\r\n");
//                                break;
//                            }
//                            else if(cmd3 == ' '){
//                                break;
//                            }
//                        }
//                        int i;
//                        for(i=0;i<100000000;i++);
//                        break;
//                    }
//                    else if(cmd2 == ' '){
//                        break;
//                    }
//                }
//                break;
//            }
//            case 'i': {
//                unsigned char cmd2;
//                sciSend(sciREG4, strlen("Testing Stack I2C as a master\r\n"), (unsigned char *)"Testing Stack I2C as a master\r\n");
//                sciSend(sciREG4, strlen("Press t to test tx, or r to test rx\r\n"), (unsigned char *)"Press t to test tx, or r to test rx\r\n");
//                sciSend(sciREG4, strlen("Press space to exit test\r\n"), (unsigned char *)"Press space to exit test\r\n");
//                while(1){
//                    cmd2 = sciReceiveByte(sciREG4);
//                    if(cmd2 == 't'){
//                        sciSend(sciREG4, strlen("Press space to enable tx\r\n"), (unsigned char *)"Press space to enable tx\r\n");
//                        sciSend(sciREG4, strlen("Make sure rx I2C slave 0x57 is ready to receive before enabling tx\r\n"), (unsigned char *)"Make sure rx I2C slave 0x57 is ready to receive before enabling tx\r\n");
//                        while(sciReceiveByte(sciREG4) != ' ');
//                        sciSend(sciREG4, strlen("Starting to send data...\r\n"), (unsigned char *)"Starting to send data...\r\n");
//
//                        I2CMasterTxTest(i2cREG1, 0x57);
//                        sciSend(sciREG4, strlen("1 kB sent to 0x57\r\n"), (unsigned char *)"1 kB sent to 0x57\r\n");
//                        break;
//                    }
//                    if(cmd2 == 'r'){
//                        sciSend(sciREG4, strlen("Press space to enable rx\r\n"), (unsigned char *)"Press space to enable rx\r\n");
//                        sciSend(sciREG4, strlen("Make sure I2C slave 0x57 is ready to send data before enabling rx\r\n"), (unsigned char *)"Make sure I2C slave 0x57 is ready to send data before enabling rx\r\n");
//                        while(sciReceiveByte(sciREG4) != ' ');
//                        sciSend(sciREG4, strlen("Waiting for data...\r\n"), (unsigned char *)"Waiting for data...\r\n");
//
//                        uint8_t recvbytes = 0;
//                        char recvbytesstr[4] = {0};
//                        recvbytes = I2CMasterRxTest(i2cREG1, 0x57);
//                        ltoa(recvbytes, recvbytesstr);
//                        sciSend(sciREG4, strlen("Bytes expected = 1000\r\n"), (unsigned char *)"Bytes expected = 1000\r\n");
//                        sciSend(sciREG4, strlen("Bytes received = "), (unsigned char *)"Bytes received = ");
//                        sciSend(sciREG4, 4, (unsigned char *)recvbytesstr);
//                        sciSend(sciREG4, strlen("\r\n"), (unsigned char *)"\r\n");
//                        int i;
//                        for(i=0;i<100000000;i++);
//                        break;
//                    }
//                    else if(cmd2 == ' '){
//                        break;
//                    }
//
//                }
//                break;
//
//            }
//            case 'g': {
//                unsigned char cmd2 = 0;
//                sciSend(sciREG4, strlen("Press o to test output GPIOs, or i to test input GPIOs\r\n"), (unsigned char *)"Press o to test output GPIOs, or i to test input GPIOs\r\n");
//                sciSend(sciREG4, strlen("Press space to exit test\r\n"), (unsigned char *)"Press space to exit test\r\n");
//                while(1){
//                    cmd2 = sciReceiveByte(sciREG4);
//                    if(cmd2 == 'o'){
//                        sciSend(sciREG4, strlen("Rapidly alternating all stack header GPIOs\r\n"), (unsigned char *)"Rapidly alternating all stack header GPIOs\r\n");
//                        while(1){
//                            GPIOTxTest();
//                            if(sciIsRxReady(sciREG4)){
//                                if(sciReceiveByte(sciREG4) == ' '){//make this an interrupt...
//                                    break;
//                                }
//                            }
//                        }
//                        break;
//                    }
//                    else if(cmd2 == 'i'){
//                        int i;
//                        unsigned char statuschar;
//                        sciSend(sciREG4, strlen("Stack header input GPIO statuses:\r\n"), (unsigned char *)"Stack header input GPIO statuses:\r\n");
//                        sciSend(sciREG4, strlen("GPIO1     GPIO8     GPIO10     GPIO11\r\n"), (unsigned char *)"GPIO1     GPIO8     GPIO10     GPIO11\r\n");
//                        while(1){
//                            ltoa((long)GPIOGetBit(1), &statuschar);
//                            sciSend(sciREG4, 1, &statuschar);
//                            sciSend(sciREG4, strlen("         "), (unsigned char *)"         ");
//                            ltoa((long)GPIOGetBit(8), &statuschar);
//                            sciSend(sciREG4, 1, &statuschar);
//                            sciSend(sciREG4, strlen("         "), (unsigned char *)"         ");
//                            ltoa((long)GPIOGetBit(10), &statuschar);
//                            sciSend(sciREG4, 1, &statuschar);
//                            sciSend(sciREG4, strlen("          "), (unsigned char *)"          ");
//                            ltoa((long)GPIOGetBit(11), &statuschar);
//                            sciSend(sciREG4, 1, &statuschar);
//                            sciSend(sciREG4, strlen("\r\n"), (unsigned char *)"\r\n");
//                            for(i=0;i<30000000;i++);
//                            if(sciIsRxReady(sciREG4)){
//                                if(sciReceiveByte(sciREG4) == ' '){//not sure if this works
//                                    break;
//                                }
//                            }
//                        }
//                        break;
//                    }
//                    else if(cmd2 == ' '){
//                        break;
//                    }
//                }
//                break;
//            }
//            case 'c': {
//                unsigned char cmd2;
//                sciSend(sciREG4, strlen("Testing CAN1\r\n"), (unsigned char *)"Testing CAN1\r\n");
//                sciSend(sciREG4, strlen("Press t to test tx, or r to test rx\r\n"), (unsigned char *)"Press t to test tx, or r to test rx\r\n");
//                sciSend(sciREG4, strlen("Press space to exit test\r\n"), (unsigned char *)"Press space to exit test\r\n");
//                while(1){
//                    cmd2 = sciReceiveByte(sciREG4);
//                    if(cmd2 == 't'){
//                        sciSend(sciREG4, strlen("Press space to enable tx\r\n"), (unsigned char *)"Press space to enable tx\r\n");
//                        sciSend(sciREG4, strlen("Make sure rx CAN node is ready to receive before enabling tx\r\n"), (unsigned char *)"Make sure rx CAN node is ready to receive before enabling tx\r\n");
//                        while(sciReceiveByte(sciREG4) != ' ');
//                        sciSend(sciREG4, strlen("Starting to send data...\r\n"), (unsigned char *)"Starting to send data...\r\n");
//
//                        CANTxTest(canREG1, canMESSAGE_BOX1);
//                        sciSend(sciREG4, strlen("Frames sent = 125\r\n"), (unsigned char *)"Frames sent = 125\r\n");
//                        int i;
//                        for(i=0;i<100000000;i++);
//                        break;
//                    }
//                    if(cmd2 == 'r'){
//                        sciSend(sciREG4, strlen("Press space to enable rx\r\n"), (unsigned char *)"Press space to enable rx\r\n");
//                        sciSend(sciREG4, strlen("Make sure to press space here before data is sent from board transmitting data\r\n"), (unsigned char *)"Make sure to press space here before data is sent from board transmitting data\r\n");
//                        while(sciReceiveByte(sciREG4) != ' ');
//                        sciSend(sciREG4, strlen("Waiting for data...\r\n"), (unsigned char *)"Waiting for data...\r\n");
//
//                        uint8_t recvframes = 0;
//                        char recvframesstr[3] = {0};
//                        recvframes = CANRxTest(canREG1, canMESSAGE_BOX2);
//                        ltoa(recvframes, recvframesstr);
//                        sciSend(sciREG4, strlen("Frames expected = 125\r\n"), (unsigned char *)"Frames expected = 125\r\n");
//                        sciSend(sciREG4, strlen("Frames received = "), (unsigned char *)"Frames received = ");
//                        sciSend(sciREG4, 3, (unsigned char *)recvframesstr);
//                        sciSend(sciREG4, strlen("\r\n"), (unsigned char *)"\r\n");
//                        int i;
//                        for(i=0;i<100000000;i++);
//                        break;
//                    }
//                    else if(cmd2 == ' '){
//                        break;
//                    }
//                }
//                break;
//            }
//            case 'r': {
//                unsigned char cmd2;
//                sciSend(sciREG4, strlen("Press r to reset RTC time, and t to show current time\r\n"), (unsigned char *)"Press r to reset RTC time, and t to show current time\r\n");
//                while(1){
//                    cmd2 = sciReceiveByte(sciREG4);
//                    if (cmd2 == 'r'){
//                        sciSend(sciREG4, strlen("Resetting RTC time...\r\n"), (unsigned char *)"Resetting RTC time...\r\n");
//                        RTCMK_ResetTime(0x64);
//                        sciSend(sciREG4, strlen("RTC time reset\r\n"), (unsigned char *)"RTC time reset\r\n");
//                        break;
//                    }
//                    else if(cmd2 == 't'){
//                        sciSend(sciREG4, strlen("Getting RTC time...\r\n"), (unsigned char *)"Getting RTC time...\r\n");
//                        uint8_t secs = 0;
//                        char secsstr[2];
//                        RTCMK_ReadSeconds(0x64, &secs);
//                        ltoa(secs, secsstr);
//                        sciSend(sciREG4, strlen("RTC time (seconds) = "), (unsigned char *)"RTC time (seconds) = ");
//                        sciSend(sciREG4, 1, (unsigned char *)secsstr);
//                        sciSend(sciREG4, strlen("\r\n"), (unsigned char *)"\r\n");
//
//                        break;
//                    }
//                }
//                break;
//            }
//            case 't': {
//                inittemp_all();
//                sciSend(sciREG4, strlen("Beginning test in a few seconds...\r\n"), (unsigned char *)"Beginning test in a few seconds...\r\n");
//                sciSend(sciREG4, strlen("Press space to exit test\r\n"), (unsigned char *)"Press space to exit test\r\n");
//                int i;
//                sciSend(sciREG4, strlen("Temperature sensor local measured values\r\n"), (unsigned char *)"Temperature sensor local measured values\r\n");
//                sciSend(sciREG4, strlen("(note conversion rate. 265250 = 26.5250 deg C):\r\n\r\n"), (unsigned char *)"(note conversion rate. 265250 = 26.5250 deg C):\r\n\r\n");
//                sciSend(sciREG4, strlen(" 0x1D      0x1E      0x2A      0x4C      0x4F      0x4E\r\n"), (unsigned char *)" 0x1D      0x1E      0x2A      0x4C      0x4F      0x4E\r\n");
//                for(i=0;i<300000000;i++);
//                while(1){
//                    int j;
//                    long measuredtemps[6] = {0};
//                    char tempstr[6];
//                    gettemp_all(measuredtemps);
//                    for(j=0;j<6;j++){
//                        ltoa(measuredtemps[j], tempstr);
//                        sciSend(sciREG4, 6, (unsigned char *)tempstr);
//                        sciSend(sciREG4, strlen("    "), (unsigned char *)"    ");
//                    }
//                    sciSend(sciREG4, strlen("\r\n"), (unsigned char *)"\r\n");
//                    for(i=0;i<10000000;i++);
//                    if(sciIsRxReady(sciREG4)){
//                        if(sciReceiveByte(sciREG4) == ' '){//not sure if this works
//                            break;
//                        }
//                    }
//                }
//                break;
//            }
//
//            case 'b': {
//
//                sciSend(sciREG4, strlen("Press space when ready to test port burnwire\r\n"), (unsigned char *)"Press space when ready to test port burnwire\r\n");
//                while(sciReceiveByte(sciREG4) != ' ');
//
//                sciSend(sciREG4, strlen("Activating port burnwire for 6.5 s\r\n"), (unsigned char *)"Activating port burnwire for 6.5 s\r\n");
//                activate(Port);
//                sciSend(sciREG4, strlen("Press space when ready to test UHF port burnwire\r\n"), (unsigned char *)"Press space when ready to test UHF port burnwire\r\n");
//                while(sciReceiveByte(sciREG4) != ' ');
//
//                sciSend(sciREG4, strlen("Activating UHF port burnwire for 6.5 s\r\n"), (unsigned char *)"Activating UHF port burnwire for 6.5 s\r\n");
//                activate(UHF_P);
//                sciSend(sciREG4, strlen("Press space when ready to test UHF zenith burnwire\r\n"), (unsigned char *)"Press space when ready to test UHF zenith burnwire\r\n");
//                while(sciReceiveByte(sciREG4) != ' ');
//
//                sciSend(sciREG4, strlen("Activating UHF zenith burnwire for 6.5 s\r\n"), (unsigned char *)"Activating UHF zenith burnwire for 6.5 s\r\n");
//                activate(UHF_Z);
//                sciSend(sciREG4, strlen("Press space when ready to test payload burnwire\r\n"), (unsigned char *)"Press space when ready to test payload burnwire\r\n");
//                while(sciReceiveByte(sciREG4) != ' ');
//
//                sciSend(sciREG4, strlen("Activating payload burnwire for 6.5 s\r\n"), (unsigned char *)"Activating payload burnwire for 6.5 s\r\n");
//                activate(Payload);
//                sciSend(sciREG4, strlen("Press space when ready to test starboard burnwire\r\n"), (unsigned char *)"Press space when ready to test starboard burnwire\r\n");
//                while(sciReceiveByte(sciREG4) != ' ');
//
//                sciSend(sciREG4, strlen("Activating starboard burnwire for 6.5 s\r\n"), (unsigned char *)"Activating starboard burnwire for 6.5 s\r\n");
//                activate(Starboard);
//                sciSend(sciREG4, strlen("Press space when ready to test UHF starboard burnwire\r\n"), (unsigned char *)"Press space when ready to test UHF starboard burnwire\r\n");
//                while(sciReceiveByte(sciREG4) != ' ');
//
//                sciSend(sciREG4, strlen("Activating UHF starboard burnwire for 6.5 s\r\n"), (unsigned char *)"Activating UHF starboard burnwire for 6.5 s\r\n");
//                activate(UHF_S);
//                sciSend(sciREG4, strlen("Press space when ready to test DFGM burnwire\r\n"), (unsigned char *)"Press space when ready to test DFGM burnwire\r\n");
//                while(sciReceiveByte(sciREG4) != ' ');
//
//                sciSend(sciREG4, strlen("Activating DFGM burnwire for 6.5 s\r\n"), (unsigned char *)"Activating DFGM burnwire for 6.5 s\r\n");
//                activate(DFGM);
//                sciSend(sciREG4, strlen("Press space when ready to test UHF nadir burnwire\r\n"), (unsigned char *)"Press space when ready to test UHF nadir burnwire\r\n");
//                while(sciReceiveByte(sciREG4) != ' ');
//
//                sciSend(sciREG4, strlen("Activating UHF nadir burnwire for 6.5 s\r\n"), (unsigned char *)"Activating UHF nadir burnwire for 6.5 s\r\n");
//                activate(UHF_N);
//                sciSend(sciREG4, strlen("Test complete\r\n"), (unsigned char *)"Test complete\r\n");
//
//                break;
//            }
//            case 'v': {
//                sciSend(sciREG4, strlen("Beginning test in a few seconds...\r\n"), (unsigned char *)"Beginning test in a few seconds...\r\n");
//                sciSend(sciREG4, strlen("Press space to exit test\r\n"), (unsigned char *)"Press space to exit test\r\n");
//                int i;
//                sciSend(sciREG4, strlen("Switch statuses:\r\n"), (unsigned char *)"Switch statuses:\r\n");
//                sciSend(sciREG4, strlen("Port   UHF_P   UHF_Z   Payload   UHF_S   UHF_N   Starboard   DFGM\r\n"), (unsigned char *)"Port   UHF_P   UHF_Z   Payload   UHF_S   UHF_N   Starboard   DFGM\r\n");
//                for(i=0;i<30000000;i++);
//                int j;
//                unsigned char statuschar;
//                while(1){
//
//                    for(j=0;j<8;j++){
//                        ltoa((long)switchstatus(j), &statuschar);
//                        sciSend(sciREG4, 1, &statuschar);
//                        sciSend(sciREG4, strlen("        "), (unsigned char *)"        ");
//                    }
//                    sciSend(sciREG4, strlen("\r\n"), (unsigned char *)"\r\n");
//                    for(i=0;i<30000000;i++);
//                    if(sciIsRxReady(sciREG4)){
//                        if(sciReceiveByte(sciREG4) == ' '){//not sure if this works
//                            break;
//                        }
//                    }
//                }
//                break;
//            }
//            case 'p': {
//                int i;
//                demux_disable();
//                sciSend(sciREG4, strlen("Press space when ready to test port CS\r\n"), (unsigned char *)"Press space when ready to test port CS\r\n");
//                while(sciReceiveByte(sciREG4) != ' ');
//                sciSend(sciREG4, strlen("Activating port CS for 6.5 s\r\n"), (unsigned char *)"Activating port CS for 6.5 s\r\n");
//
//                demux_select_pin(OUT_Y2);
//                demux_enable();
//                for (i = 0; i < 0x8000000; i++);
//                demux_disable();
//
//                sciSend(sciREG4, strlen("Press space when ready to test port deployable CS\r\n"), (unsigned char *)"Press space when ready to test port deployable CS\r\n");
//                while(sciReceiveByte(sciREG4) != ' ');
//                sciSend(sciREG4, strlen("Activating port deployable CS for 6.5 s\r\n"), (unsigned char *)"Activating port deployable CS for 6.5 s\r\n");
//
//                demux_select_pin(OUT_Y3);
//                demux_enable();
//                for (i = 0; i < 0x8000000; i++);
//                demux_disable();
//
//                sciSend(sciREG4, strlen("Press space when ready to test zenith CS\r\n"), (unsigned char *)"Press space when ready to test zenith CS\r\n");
//                while(sciReceiveByte(sciREG4) != ' ');
//                sciSend(sciREG4, strlen("Activating zenith CS for 6.5 s\r\n"), (unsigned char *)"Activating zenith CS for 6.5 s\r\n");
//
//                demux_select_pin(OUT_Y4);
//                demux_enable();
//                for (i = 0; i < 0x8000000; i++);
//                demux_disable();
//
//                sciSend(sciREG4, strlen("Press space when ready to test starboard deployable CS\r\n"), (unsigned char *)"Press space when ready to test starboard deployable CS\r\n");
//                while(sciReceiveByte(sciREG4) != ' ');
//                sciSend(sciREG4, strlen("Activating starboard deployable CS for 6.5 s\r\n"), (unsigned char *)"Activating starboard deployable CS for 6.5 s\r\n");
//
//                demux_select_pin(OUT_Y5);
//                demux_enable();
//                for (i = 0; i < 0x8000000; i++);
//                demux_disable();
//
//                sciSend(sciREG4, strlen("Press space when ready to test starboard CS\r\n"), (unsigned char *)"Press space when ready to test starboard CS\r\n");
//                while(sciReceiveByte(sciREG4) != ' ');
//                sciSend(sciREG4, strlen("Activating starboard CS for 6.5 s\r\n"), (unsigned char *)"Activating starboard CS for 6.5 s\r\n");
//
//                demux_select_pin(OUT_Y6);
//                demux_enable();
//                for (i = 0; i < 0x8000000; i++);
//                demux_disable();
//
//                sciSend(sciREG4, strlen("Press space when ready to test Nadir CS\r\n"), (unsigned char *)"Press space when ready to test Nadir CS\r\n");
//                while(sciReceiveByte(sciREG4) != ' ');
//                sciSend(sciREG4, strlen("Activating Nadir CS for 6.5 s\r\n"), (unsigned char *)"Activating Nadir CS for 6.5 s\r\n");
//
//                demux_select_pin(OUT_Y7);
//                demux_enable();
//                for (i = 0; i < 0x8000000; i++);
//                demux_disable();
//
//                sciSend(sciREG4, strlen("Test complete\r\n"), (unsigned char *)"Test complete\r\n");
//                break;
//            }
//            case 'm': {
//                int i;
//                sciSend(sciREG4, strlen("Current limit set at 200 mA\r\n"), (unsigned char *)"Current limit set at 200 mA\r\n");
//                sciSend(sciREG4, strlen("If current limit exceeded, press r to turn panel power switch back on. Press space to exit test.\r\n"), (unsigned char *)"If current limit exceeded, press r to turn panel power switch back on. Press space to exit test.\r\n");
//                for(i=0;i<100000000;i++);
//                uint16_t calibrationval = 0b0100111100111111;
//                if(INA226_RegisterSet(i2cREG2, 0x40, INA226_RegCalib, calibrationval)){
//                    while(1);//cannot set calibration register
//                }
//
//                int temp;
//                for (temp = 0; temp < 0x10000; temp++);//temporary fix...
//
//                uint16_t alertlimit = 0b0001111101000000;
//                if(INA226_RegisterSet(i2cREG2, 0x40, INA226_RegAlertLim, alertlimit)){
//                    while(1);
//                }
//
//                for (temp = 0; temp < 0x10000; temp++);
//
//                uint16_t alertsetting = 0b1000000000000000;
//                if(INA226_RegisterSet(i2cREG2, 0x40, INA226_RegMaskEn, alertsetting)){
//                    while(1);
//                }
//
//                for (temp = 0; temp < 0x10000; temp++);
//
//                int16_t v = 0;
//                while(1){
////                    if(INA226_RegisterGet(i2cREG2, 0x40, INA226_RegShuntV, &shuntv)){
////                        while(1);
////                    }
////
////                    for (temp = 0; temp < 0x10000; temp++);
//                    v = 0;
//
//                    int i = 0;
//                    if(INA226_ReadShuntVoltage(i2cREG2, 0x40, &v) == -1){
//                        while(1);
//                    }
//                    if((v>>15) == 1){//checking sign bit
//                        v = -(v & 0b0111111111111111);
//                    }
//                    else{
//                        v = v & 0b0111111111111111;
//                    }
//
//                    for (temp = 0; temp < 0x10000; temp++);
//
//                    char str[5];
//                    v = (25*v)/10;
//                    sciSend(sciREG4, strlen("Shunt V (uV) = "), (unsigned char *)"Shunt V (uV) = ");
//                    ltoa((long)v, str);
//                    sciSend(sciREG4, 5, (unsigned char *)str);
//
//                    i = (v*10)/1000;
//                    sciSend(sciREG4, strlen("   Shunt current (mA) = "), (unsigned char *)"   Shunt current (mA) = ");
//                    ltoa((long)i, str);
//                    sciSend(sciREG4, 3, (unsigned char *)str);
//
//                    if ((gioGetBit(hetPORT2, 0) == 0) | (gioGetBit(hetPORT2, 12) == 0)){//make this an interrupt thing in the future instead of polling
//                        gioSetBit(hetPORT2, 12, 0);
//                        sciSend(sciREG4, strlen("   Switch status = off\r\n"), (unsigned char *)"   Switch status = off\r\n");
//                    }
//                    else{
//                        sciSend(sciREG4, strlen("   Switch status = on\r\n"), (unsigned char *)"   Switch status = on\r\n");
//                    }
//
//                    if(sciIsRxReady(sciREG4)){
//                        if(sciReceiveByte(sciREG4) == 'r'){//not sure if this works
//                            sciSend(sciREG4, strlen("Resetting switch\r\n"), (unsigned char *)"Resetting switch\r\n");
//                            gioSetBit(hetPORT2, 12, 1);
//                        }
//                        if(sciReceiveByte(sciREG4) == ' '){//not sure if this works
//                            break;
//                        }
//                    }
//                    for(temp=0;temp<40000000;temp++);
//                }
//                break;
//            }
//
//            default:{
//                break;
//            }
//        }
//    }
//}
