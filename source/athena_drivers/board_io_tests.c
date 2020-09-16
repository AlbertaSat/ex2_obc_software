/*
 * board_io_tests.c
 *
 *  Created on Aug 13, 2020
 *
 *      Author: joshdellaz
 */


#include "HL_het.h"
#include "HL_gio.h"
#include "HL_i2c.h"
#include "HL_sci.h"
#include "HL_can.h"
#include "HL_spi.h"
#include "HL_emif.h"



/* Initialize all IO controllers, set the direction for all GPIO pins,
 * and set the default state for enable lines. */
void InitIO(void){
    i2cInit();
    sciInit();
    canInit();
    spiInit();
    gioInit();
    //emif_SDRAMInit(); not using until SDRAM tested
    //hetInit(); don't think this is necessary

    //The following sets the proper direction for all GPIO
    gioSetDirection(hetPORT2, 0xFFFFFFEA);
    gioSetDirection(hetPORT1, 0x9CFF6BEF);
    gioSetDirection(gioPORTA, 0xFFFFFF6F);
    gioSetDirection(gioPORTB, 0xFFFFFFFF);

    gioSetBit(hetPORT2, 12, 1);//solar panel power - enable
    gioSetBit(hetPORT1, 20, 1);//IRIS nCONFIG - disable
    gioSetBit(hetPORT2, 6, 1);//SD card - disable
}


/*  @param[in] bit number 0-13 that specifies the output GPIO to be written to.

*   @param[in] value binary value to write to GPIO
*
*   Writes a value to the specified GPIO pin
*
*   GPIO0 - 2_23
*   GPIO1 - 1_25 - input
*   GPIO2 - 2_21
*   GPIO3 - 1_9
*   GPIO4 - 1_3
*   GPIO5 - 1_7
*   GPIO6 - 2_22
*   GPIO7 - 1_1
*   GPIO8 - 1_24 - input
*   GPIO9 - 1_20
*   GPIO10 - A7 - input
*   GPIO11 - 1_15 - input
*   GPIO12 - B1
*   GPIO13 - B0 */
uint8_t GPIOSetBit(uint8_t bit, uint8_t value){
    switch(bit){
        case 0:
            gioSetBit(hetPORT2, 23, value);
            break;
        case 1:
            while(1);//cannot write to input
            break;
        case 2:
            gioSetBit(hetPORT2, 21, value);
            break;
        case 3:
            gioSetBit(hetPORT1, 9, value);
            break;
        case 4:
            gioSetBit(hetPORT1, 3, value);
            break;
        case 5:
            gioSetBit(hetPORT1, 7, value);
            break;
        case 6:
            gioSetBit(hetPORT2, 22, value);
            break;
        case 7:
            gioSetBit(hetPORT1, 1, value);
            break;
        case 8:
            while(1);//cannot write to input
            break;
        case 9:
            gioSetBit(hetPORT1, 20, value);
            break;
        case 10:
            while(1);//cannot write to input
            break;
        case 11:
            while(1);//cannot write to input
            break;
        case 12:
            gioSetBit(gioPORTB, 1, value);
            break;
        case 13:
            gioSetBit(gioPORTB, 0, value);
            break;
        default:
            return -1;
    }
    return 0;
}

/*  @param[in] bit number 0-13 that specifies the GPIO to be read.

*   @param[in] value binary value read from GPIO
*
*   Writes a value to the specified GPIO pin
*
*   GPIO0 - 2_23
*   GPIO1 - 1_25 - input
*   GPIO2 - 2_21
*   GPIO3 - 1_9
*   GPIO4 - 1_3
*   GPIO5 - 1_7
*   GPIO6 - 2_22
*   GPIO7 - 1_1
*   GPIO8 - 1_24 - input
*   GPIO9 - 1_20
*   GPIO10 - A7 - input
*   GPIO11 - 1_15 - input
*   GPIO12 - B1
*   GPIO13 - B0 */
int GPIOGetBit(uint8_t bit){

    switch(bit){
        case 0:
            return gioGetBit(hetPORT2, 23);
        case 1:
            return gioGetBit(hetPORT1, 25);
        case 2:
            return gioGetBit(hetPORT2, 21);
        case 3:
            return gioGetBit(hetPORT1, 9);
        case 4:
            return gioGetBit(hetPORT1, 3);
        case 5:
            return gioGetBit(hetPORT1, 7);
        case 6:
            return gioGetBit(hetPORT2, 22);
        case 7:
            return gioGetBit(hetPORT1, 1);
        case 8:
            return gioGetBit(hetPORT1, 24);
        case 9:
            return gioGetBit(hetPORT1, 20);
        case 10:
            return gioGetBit(gioPORTA, 7);
        case 11:
            return gioGetBit(hetPORT1, 15);
        case 12:
            return gioGetBit(gioPORTB, 1);
        case 13:
            return gioGetBit(gioPORTB, 0);
        default:
            return -1;
    }
}


/* Rapidly alternates all output GPIOs on stack header on and off once
 */
void GPIOTxTest(void){
    int i;
    for(i=0;i<=13;i++){
        if((i == 1) || (i == 8) || (i == 10) || (i == 11)){//make sure inputs are not toggled
            //do nothing
        }
        else{
            GPIOSetBit(i, 1);
        }
    }
    for(i=0;i<=13;i++){
        if((i == 1) || (i == 8) || (i == 10) || (i == 11)){//make sure inputs are not toggled
            //do nothing
        }
        else{
            GPIOSetBit(i, 0);
        }
    }
}



/*  Sends 1000 bytes of pattern 00110101 over UART
 *  Make sure that another board is ready to receive data before running this.
 *
 *  @param[in] regset SCI/UART register to test
 */
void UARTTxTest(sciBASE_t * regset){

    int i;
    for(i = 0; i<1000; i++){
        sciSendByte(regset, 0b00110101);
    }

}

/*  Receives 1000 bytes of pattern 00110101 over UART
 *  Make sure that this is running before the master sends data.
 *
 *  @param[in] regset SCI/UART register to test
 *  @return number of bytes successfully read to be 00110101
 */
uint8_t UARTRxTest(sciBASE_t * regset){

    uint8_t data = 0;
    int i = 0;
    uint8_t pings = 0;

    while(i<1000){
        if(sciIsRxReady(regset) != 0){
            data = 0;
            data = sciReceiveByte(regset);
            if(data == 0b00110101){
                pings++;
            }
            i++;
        }
    }
    return pings;
}

void SPIMasterTxTest(spiBASE_t * regset){

    spiDAT1_t dataconfig1_t;

    dataconfig1_t.CS_HOLD = FALSE;
    dataconfig1_t.WDEL    = FALSE;//not sure
    dataconfig1_t.DFSEL   = SPI_FMT_0;
    dataconfig1_t.CSNR    = 0x00;//no CS lines

    uint16_t data = 0b0011010100110101;
    int i;
    for(i = 0; i<500; i++){
        spiTransmitData(regset, &dataconfig1_t, 1, &data);
    }

}

uint8_t SPIMasterRxTest(spiBASE_t * regset){

    spiDAT1_t dataconfig1_t;

    dataconfig1_t.CS_HOLD = FALSE;
    dataconfig1_t.WDEL    = FALSE;
    dataconfig1_t.DFSEL   = SPI_FMT_0;
    dataconfig1_t.CSNR    = 0x00;

    uint16_t data;
    int i = 0;
    while(i<500){
        data = 0;
        if(GPIOGetBit(8) == 0){//change to interrupt
            spiReceiveData(regset, &dataconfig1_t, 1, &data);
            if(data == 0b0011010100110101){
                i++;
            }
        }
    }
    return i;
}

void SPISlaveTxTest(spiBASE_t * regset){

    //RUN THIS ON A DEV BOARD TO SEND DATA TO ATHENA
    //MAKE SURE TO SET THE BOARD TO BE A SLAVE

    spiDAT1_t dataconfig1_t;

    dataconfig1_t.CS_HOLD = FALSE;
    dataconfig1_t.WDEL    = FALSE;//not sure
    dataconfig1_t.DFSEL   = SPI_FMT_0;
    dataconfig1_t.CSNR    = 0x00;//no CS lines

    uint16_t data = 0b0011010100110101;
    int i;
    for(i = 0; i<500; i++){
        spiTransmitData(regset, &dataconfig1_t, 1, &data);
    }

}

void SPISlaveRxTest(spiBASE_t * regset, uint8 spinumber){

    //Make this an interrupt thing when the CS line is enabled

    spiDAT1_t dataconfig1_t;

    dataconfig1_t.CS_HOLD = FALSE;
    dataconfig1_t.WDEL    = FALSE;//not sure
    dataconfig1_t.DFSEL   = SPI_FMT_0;
    dataconfig1_t.CSNR    = 0x00;//no CS lines

    uint16_t data;
    int i = 0;
    while(i < 500){
        data = 0;
        spiReceiveData(regset, &dataconfig1_t, 1, &data);
        if(data == 0b0011010100110101){
            i++;
        }

    }

}

/*  Sends 1000 bytes of byte 00110101 as an I2C master.
 *  Make sure that another board is ready to receive data before running this.
 *
 *  @param[in] regset I2C register to test
 *  @param[in] addr I2C slave address to send data to
 */
void I2CMasterTxTest(i2cBASE_t * regset, uint8_t addr){

    i2cSetSlaveAdd(regset, addr);

    i2cSetDirection(regset, I2C_TRANSMITTER);
    i2cSetCount(regset, 1000);
    i2cSetMode(regset, I2C_MASTER);
    i2cSetStop(regset);
    i2cSetStart(regset);
    int i;
    for(i = 0; i<1000; i++){
        i2cSendByte(regset, 0b00110101);
    }


    /* Wait until Bus Busy is cleared */
    while(i2cIsBusBusy(regset) == true);

    /* Wait until Stop is detected */
    while(i2cIsStopDetected(regset) == 0);

    /* Clear the Stop condition */
    i2cClearSCD(regset);

}

/*  Receives 1000 bytes of byte 00110101 as an I2C master.
 *  Make sure that another board is ready to send data before running this.
 *
 *  @param[in] regset I2C register to test
 *  @param[in] addr I2C slave address to receive data from
 *  @return number of successfully read bytes
 */
uint8_t I2CMasterRxTest(i2cBASE_t * regset, uint8_t addr){

    uint8_t data = 0;

    i2cSetSlaveAdd(regset, addr);
    /* Set direction to receiver */
    i2cSetDirection(regset, I2C_RECEIVER);
    i2cSetCount(regset, 1000);
    /* Set mode as Master */
    i2cSetMode(regset, I2C_MASTER);
    i2cSetStop(regset);
    /* Transmit Start Condition */
    i2cSetStart(regset);

    int i = 0;
    int pings = 0;
    while(i<1000){
        if(i2cIsRxReady(regset) != 0){
            data = 0;
            data = i2cReceiveByte(regset);
            if(data == 0b00110101){
                pings++;
            }
            i++;
        }
    }

    /* Wait until Bus Busy is cleared */
    while(i2cIsBusBusy(regset) == true);

    /* Wait until Stop is detected */
    while(i2cIsStopDetected(regset) == 0);

    /* Clear the Stop condition */
    i2cClearSCD(regset);

    return pings;

}

/*  Receives 1000 bytes of byte 00110101 as an I2C slave.
 *  Make sure that this is running before the master sends data.
 *
 *  @param[in] regset I2C register to test
 *  @param[in] addr sets slave (self) address to this value
 *  @return number of successfully read bytes
 */
uint8_t I2CSlaveRxTest(i2cBASE_t * regset, uint8_t addr){

    i2cSetMode(regset, I2C_SLAVE);
    i2cSetOwnAdd(regset, addr);
    i2cSetDirection(regset, I2C_RECEIVER);
    i2cSetCount(regset, 1000);

    uint8_t data = 0;
    int i = 0;
    int pings = 0;

    while(i<1000){
        if(i2cIsRxReady(regset) != 0){
            data = 0;
            data = i2cReceiveByte(regset);
            if(data == 0b00110101){
                pings++;
            }
            i++;
        }
    }

    /* Wait until Bus Busy is cleared */
    while(i2cIsBusBusy(regset) == true);

    /* Wait until Stop is detected */
    while(i2cIsStopDetected(regset) == 0);

    /* Clear the Stop condition */
    i2cClearSCD(regset);

    return pings;

}

/*  Transmits 1000 bytes of byte 00110101 as an I2C master.
 *  Make sure that this is running before the master requests data.
 *
 *  @param[in] regset I2C register to test
 *  @param[in] addr sets slave (self) address to this value
 */
void I2CSlaveTxTest(i2cBASE_t * regset, uint8_t addr){

    i2cSetMode(regset, I2C_SLAVE);
    i2cSetOwnAdd(regset, addr);
    i2cSetDirection(regset, I2C_TRANSMITTER);
    i2cSetCount(regset, 1000);

    int i;
    for(i = 0; i<1000; i++){
        i2cSendByte(regset, 0b00110101);
    }

    /* Wait until Bus Busy is cleared */
    while(i2cIsBusBusy(regset) == true);

    /* Wait until Stop is detected */
    while(i2cIsStopDetected(regset) == 0);

    /* Clear the Stop condition */
    i2cClearSCD(regset);

}

/*  Sends 1000 bytes (125 CAN frames) of 00110101.
 *  Make sure that another board is ready to receive data before running this.
 *
 *  @param[in] regset CAN register to test
 *  @param[in] msgbox Message box to use for Tx. Typically canMESSAGE_BOX1.
 */
void CANTxTest(canBASE_t * regset, uint8_t msgbox){

    int i;

    uint8_t data[8] = {53, 53, 53, 53, 53, 53, 53, 53};

    for(i = 0; i<125; i++){
        while(canIsTxMessagePending(regset, msgbox));
        canTransmit(regset, msgbox, data);
    }
}

/*  Receives 1000 bytes (125 CAN frames) of 00110101.
 *  Make sure to run this before message is sent from another board.
 *
 *  @param[in] regset CAN register to test
 *  @param[in] msgbox Message box to use for Rx. Typically canMESSAGE_BOX2.
 *  @return number of successfully received packets (should be 125)
 */
uint8_t CANRxTest(canBASE_t * regset, uint8_t msgbox){

    uint8_t data[8] = {0};
    int i = 0;
    int j = 0;
    int pings = 0;

    while(i<125){
        while(!canIsRxMessageArrived(regset, msgbox));
        for(j=0;j<8;j++){
            data[j] = 0;
        }
        canGetData(regset, msgbox, data);
        for(j=0;j<8;j++){
            if(data[j] == 53){
                pings++;
            }
        }
        i++;
    }
    return (pings/8);
}

/* Used by SDRAM_Test() */
#pragma SET_CODE_SECTION(".blinky_section")
void blinky()
{
    int i;
    while(1)
    {
        gioSetBit(hetPORT1, 18, 1);
        for(i=0;i<1000000;i++);
        gioSetBit(hetPORT1, 18, 0);
        for(i=0;i<1000000;i++);
    }
}
#pragma SET_CODE_SECTION()

extern uint32 BlinkyLoadStart;
extern uint32 BlinkyLoadEnd;
extern uint32 BlinkySize;
extern uint32 BlinkyStartAddr;
extern uint32 BlinkyEndAddr;


/*  Tests SDRAM by blinking LED using code stored/executed in SDRAM
 *
 *  Currently not working
 */
void SDRAM_Test(){

    int i;
    uint32 size=(uint32)&BlinkySize;

    for(i=0;i<size;i++)
    {
        ((char *)&BlinkyStartAddr)[i] =((char *)&BlinkyLoadStart)[i];
    }

    blinky();

}
