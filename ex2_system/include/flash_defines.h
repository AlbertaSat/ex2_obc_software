
#ifndef __FLASH_DEFINES_H__
#define __FLASH_DEFINES_H__

//*****************************************************************************
//
// The frequency (MHz) of the crystal used to clock the microcontroller.
//
// This defines the crystal frequency used by the microcontroller running the
// boot loader.
//
// Depends on: None
// Exclusive of: None
// Requires: None
//
//*****************************************************************************
#define TMS570LC43
#define CRYSTAL_FREQ 16  // MHz
#define SYS_CLK_FREQ 150 // MHz

// The device name is defined in bl_config.h

typedef struct Sectors {
    void *start;
    unsigned int length; // number of 32-bit words
    unsigned int bankNumber;
    unsigned int sectorNumber;
    unsigned int FlashBaseAddress;
} SECTORS;

typedef struct Banks {
    void *start;
    unsigned int length; // number of 32-bit words
    unsigned int bankNumber;
    unsigned int FlashBaseAddress;
    unsigned int numOfSectors;
} BANKS;

#if defined(RM57) || defined(TMS570LC43)
#define NUMBEROFSECTORS 64
static const SECTORS flash_sector[NUMBEROFSECTORS] = {
    (void *)0x00000000, 0x04000, 0, 0,  0xfff87000, (void *)0x00004000, 0x04000, 0, 1,  0xfff87000,
    (void *)0x00008000, 0x04000, 0, 2,  0xfff87000, (void *)0x0000C000, 0x04000, 0, 3,  0xfff87000,
    (void *)0x00010000, 0x04000, 0, 4,  0xfff87000, (void *)0x00014000, 0x04000, 0, 5,  0xfff87000,
    (void *)0x00018000, 0x08000, 0, 6,  0xfff87000, (void *)0x00020000, 0x20000, 0, 7,  0xfff87000,
    (void *)0x00040000, 0x20000, 0, 8,  0xfff87000, (void *)0x00060000, 0x20000, 0, 9,  0xfff87000,
    (void *)0x00080000, 0x40000, 0, 10, 0xfff87000, (void *)0x000C0000, 0x40000, 0, 11, 0xfff87000,
    (void *)0x00100000, 0x40000, 0, 12, 0xfff87000, (void *)0x00140000, 0x40000, 0, 13, 0xfff87000,
    (void *)0x00180000, 0x40000, 0, 14, 0xfff87000, (void *)0x001C0000, 0x40000, 0, 15, 0xfff87000,
    (void *)0x00200000, 0x20000, 1, 0,  0xfff87000, (void *)0x00220000, 0x20000, 1, 1,  0xfff87000,
    (void *)0x00240000, 0x20000, 1, 2,  0xfff87000, (void *)0x00260000, 0x20000, 1, 3,  0xfff87000,
    (void *)0x00280000, 0x20000, 1, 4,  0xfff87000, (void *)0x002A0000, 0x20000, 1, 5,  0xfff87000,
    (void *)0x002C0000, 0x20000, 1, 6,  0xfff87000, (void *)0x002E0000, 0x20000, 1, 7,  0xfff87000,
    (void *)0x00300000, 0x20000, 1, 8,  0xfff87000, (void *)0x00320000, 0x20000, 1, 9,  0xfff87000,
    (void *)0x00340000, 0x20000, 1, 10, 0xfff87000, (void *)0x00360000, 0x20000, 1, 11, 0xfff87000,
    (void *)0x00380000, 0x20000, 1, 12, 0xfff87000, (void *)0x003A0000, 0x20000, 1, 13, 0xfff87000,
    (void *)0x003C0000, 0x20000, 1, 14, 0xfff87000, (void *)0x003E0000, 0x20000, 1, 15, 0xfff87000,

    (void *)0xF0200000, 0x01000, 7, 0,  0xfff87000, (void *)0xF0201000, 0x01000, 7, 1,  0xfff87000,
    (void *)0xF0202000, 0x01000, 7, 2,  0xfff87000, (void *)0xF0203000, 0x01000, 7, 3,  0xfff87000,
    (void *)0xF0204000, 0x01000, 7, 4,  0xfff87000, (void *)0xF0205000, 0x01000, 7, 5,  0xfff87000,
    (void *)0xF0206000, 0x01000, 7, 6,  0xfff87000, (void *)0xF0207000, 0x01000, 7, 7,  0xfff87000,
    (void *)0xF0208000, 0x01000, 7, 8,  0xfff87000, (void *)0xF0209000, 0x01000, 7, 9,  0xfff87000,
    (void *)0xF020A000, 0x01000, 7, 10, 0xfff87000, (void *)0xF020B000, 0x01000, 7, 11, 0xfff87000,
    (void *)0xF020C000, 0x01000, 7, 12, 0xfff87000, (void *)0xF020D000, 0x01000, 7, 13, 0xfff87000,
    (void *)0xF020E000, 0x01000, 7, 14, 0xfff87000, (void *)0xF020F000, 0x01000, 7, 15, 0xfff87000,
    (void *)0xF0210000, 0x01000, 7, 16, 0xfff87000, (void *)0xF0211000, 0x01000, 7, 17, 0xfff87000,
    (void *)0xF0212000, 0x01000, 7, 18, 0xfff87000, (void *)0xF0213000, 0x01000, 7, 19, 0xfff87000,
    (void *)0xF0214000, 0x01000, 7, 20, 0xfff87000, (void *)0xF0215000, 0x01000, 7, 21, 0xfff87000,
    (void *)0xF0216000, 0x01000, 7, 22, 0xfff87000, (void *)0xF0217000, 0x01000, 7, 23, 0xfff87000,
    (void *)0xF0218000, 0x01000, 7, 24, 0xfff87000, (void *)0xF0219000, 0x01000, 7, 25, 0xfff87000,
    (void *)0xF021A000, 0x01000, 7, 26, 0xfff87000, (void *)0xF021B000, 0x01000, 7, 27, 0xfff87000,
    (void *)0xF021C000, 0x01000, 7, 28, 0xfff87000, (void *)0xF021D000, 0x01000, 7, 29, 0xfff87000,
    (void *)0xF021E000, 0x01000, 7, 30, 0xfff87000, (void *)0xF021F000, 0x01000, 7, 31, 0xfff87000,

};

#define NUMBEROFBANKS 3
static const BANKS flash_bank[NUMBEROFBANKS] = {(void *)0x00000000, 0x200000, 0, 0xfff87000, 16,
                                                (void *)0x00200000, 0x400000, 1, 0xfff87000, 16,
                                                (void *)0xF0200000, 0x007D00, 7, 0xfff87000, 32};

#endif /* RM57 */

#endif /* FLASH_DEFINES_H_ */
