/*
 * FILTER100to1
 *
 * Take the 100 Hz data presented on stdin
 * Apply a 161 point anti-alias filter
 * Write the 1 Hz downsampled data to stdout
 *
 * The filter coefficients provide a 0.25 Hz cut-off
 * for 100 Hz sampled data.
 *
 * To apply the 161 point filter we need to operate
 * on twoseconds of data at a time.
 * The data buffer is filled and the filter applied.
 * The data is read in one second at a time - an array
 * of pointers keeps track of the order of the data
 * within the buffer for the apply_filter function.
 *
 *
 */

#include "dfgm_filter.h"
#include "FreeRTOS.h"
#include "HL_sci.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system.h"

#include "stdio.h"
#include <redconf.h>
#include <redfs.h>
#include <redfse.h>
#include <redposix.h>
#include <redtests.h>
#include <redvolume.h>

#include "printf.h"

#define scilinREG PRINTF_SCI

double dummy;
double dumcomp;

double filter[81] = {
    0.014293879,   0.014285543,    0.014260564,    0.014219019,
    0.014161035,   0.014086794,    0.013996516,    0.013890488,
    0.013769029,   0.013632505,    0.013481341,    0.013315983,
    0.013136936,   0.012944728,    0.012739926,    0.012523144,
    0.012295013,   0.012056194,    0.011807376,    0.011549255,
    0.011282578,   0.011008069,    0.010726496,    0.010438624,
    0.010145215,   0.0098470494,   0.0095449078,   0.0092395498,
    0.0089317473,  0.0086222581,   0.0083118245,   0.0080011814,
    0.0076910376,  0.0073820935,   0.0070750111,   0.0067704498,
    0.0064690227,  0.0061713282,   0.0058779319,   0.0055893637,
    0.0053061257,  0.0050286865,   0.0047574770,   0.0044928941,
    0.0042353003,  0.0039850195,   0.0037423387,   0.0035075136,
    0.0032807556,  0.0030622446,   0.0028521275,   0.0026505100,
    0.0024574685,  0.0022730422,   0.0020972437,   0.0019300507,
    0.0017714123,  0.0016212496,   0.0014794567,   0.0013459044,
    0.0012204390,  0.0011028848,   0.00099304689,  0.00089071244,
    0.00079565205, 0.00070762285,  0.00062636817,  0.00055162174,
    0.00048310744, 0.00042054299,  0.00036364044,  0.00031210752,
    0.00026565061, 0.00022397480,  0.00018678687,  0.00015379552,
    0.00012471308, 9.9256833e-005, 7.7149990e-005, 5.8123173e-005,
    4.1914571e-005

};

struct SECOND buffer[2];
struct SECOND *sptr[2];
struct SECOND *g_ptr;

//// Unused function
//int main_filter(int argc, char **argv) {
//  int i;
//  dummy = 99999.999; // Invalid data value
//  dumcomp = 99999.0; // Value to test against
//
//  /* Initialize the pointers to the data buffer */
//
//  for (i = 0; i < 2; i++) {
//    sptr[i] = &buffer[i];
//  }
//  sciSend(scilinREG, 5, (uint8 *)"yep1\n");
//  /* Fill the buffer with the first 2 seconds */
//
//  for (i = 0; i < 2; i++) {
//    read_second(sptr[i]);
//  }
//  sciSend(scilinREG, 5, (uint8 *)"yep2\n");
//
//  /* Deal with the first 6 samples which can't be filtered */
//
//  //  for (i=0; i<6; i++){
//  print_mean(sptr[0]);
//  //  }
//
//  sciSend(scilinREG, 5, (uint8 *)"yep3\n");
//
//  // Actual filtering occurs here
//  while (1) {
//    apply_filter();
//    print_result(sptr[1]);
//    //      print_mean(sptr[1]);
//    /* Shift the pointers by 1 */
//
//    shift_sptr();
//
//    /* read in the next full second of data */
//
//    read_second(sptr[1]);
//  }
//
//  /* Deal with the last samples which can't be filtered */
//
//  //  print_raw(sptr[1]);
//  return 0;
//}

void apply_filter(void) {

  /*  The 41 point filter is applied to every other data point */

  double Xfilt, Yfilt, Zfilt;
  int i, negsamp, possamp, negsec, possec;
  int Xinvalid, Yinvalid, Zinvalid;
  //  sptr[6]->Xfilt = sptr[0]->X[0];

  //  Does invalid data appear in this filter width?
  Xinvalid = 0;
  Yinvalid = 0;
  Zinvalid = 0;

  /*  "DC" component centred on the 0 time sample */
  Xfilt = sptr[1]->X[0] * filter[0];
  Yfilt = sptr[1]->Y[0] * filter[0];
  Zfilt = sptr[1]->Z[0] * filter[0];

  negsamp = 99; // sample indices
  possamp = 1;

  negsec = 5; // second indices
  possec = 6;

  for (i = 1; i < 81; i++) {

    // Flag invalid data
    //  if ((sptr[negsec]->X[negsamp] > dumcomp) || (sptr[possec]->X[possamp] >
    //  dumcomp)) Xinvalid=1; if ((sptr[negsec]->Y[negsamp] > dumcomp) ||
    //  (sptr[possec]->Y[possamp] > dumcomp)) Yinvalid=1; if
    //  ((sptr[negsec]->Z[negsamp] > dumcomp) || (sptr[possec]->Z[possamp] >
    //  dumcomp)) Zinvalid=1;

    // Apply filter to data
    Xfilt += (sptr[0]->X[negsamp] + sptr[1]->X[possamp]) * filter[i];
    Yfilt += (sptr[0]->Y[negsamp] + sptr[1]->Y[possamp]) * filter[i];
    Zfilt += (sptr[0]->Z[negsamp] + sptr[1]->Z[possamp]) * filter[i];

    // printf("sptr[%d]->X[%d] + sptr[%d]->X[%d]\n",
    // negsec,negsamp,possec,possamp);

    negsamp -= 1;
    possamp += 1;

    //      if (negsamp < 0) {
    //          negsamp=6;
    //          negsec--;
    //      }
    //      if (possamp > 7) {
    //          possamp=0;
    //          possec++;
    //      }
  }

  // If data is invalid then assign dummy value
  //  if (Xinvalid) Xfilt=dummy;
  //  if (Yinvalid) Yfilt=dummy;
  //  if (Zinvalid) Zfilt=dummy;
  //  if (Xinvalid || Yinvalid || Zinvalid) sptr[6]->flag='x';

  sptr[1]->Xfilt = Xfilt;
  sptr[1]->Yfilt = Yfilt;
  sptr[1]->Zfilt = Zfilt;
}

//// Unused function
//void read_second(struct SECOND *ptr) {
//  g_ptr = ptr;
//  sciSend(scilinREG, 5, (uint8 *)"scanf");
//}

//// Unused function
//void print_result(struct SECOND *ptr) {
//  char outBuff[50];
//  sprintf(outBuff, "%s %12.3lf%12.3lf%12.3lf %c\n", ptr->datetime, ptr->Xfilt,
//          ptr->Yfilt, ptr->Zfilt, ptr->flag);
//  sciSend(scilinREG, 50, (unsigned char *)&outBuff);
//  free(outBuff);
//}

//// Unused function
//void print_raw(struct SECOND *ptr) {
//  char outBuff[50];
//  sprintf(outBuff, "%s %12.3lf %12.3lf %12.3lf %c\n", ptr->datetime, ptr->X[0],
//          ptr->Y[0], ptr->Z[0], ptr->flag);
//  sciSend(scilinREG, 50, (unsigned char *)&outBuff);
//  free(outBuff);
//}

//// Unused function
//void print_mean(struct SECOND *ptr) {
//  double Xmean, Ymean, Zmean;
//  int sample;
//  int Xinvalid, Yinvalid, Zinvalid;
//
//  //  Does invalid data appear in this second?
//  Xinvalid = 0;
//  Yinvalid = 0;
//  Zinvalid = 0;
//  //
//  Xmean = 0.0;
//  Ymean = 0.0;
//  Zmean = 0.0;
//
//  for (sample = 0; sample < 100; sample++) {
//
//    //      if (ptr->X[sample] > dumcomp) Xinvalid=1;
//    //      if (ptr->Y[sample] > dumcomp) Yinvalid=1;
//    //      if (ptr->Z[sample] > dumcomp) Zinvalid=1;
//
//    Xmean += ptr->X[sample];
//    Ymean += ptr->Y[sample];
//    Zmean += ptr->Z[sample];
//  }
//
//  if (Xinvalid)
//    Xmean = dummy;
//  else
//    Xmean = Xmean / 100.0;
//  if (Yinvalid)
//    Ymean = dummy;
//  else
//    Ymean = Ymean / 100.0;
//  if (Zinvalid)
//    Zmean = dummy;
//  else
//    Zmean = Zmean / 100.0;
//
//  if (Xinvalid || Yinvalid || Zinvalid)
//    ptr->flag = 'x';
//
//  sciSend(scilinREG, 5, (uint8 *)"yepx\n");
//
//  sciSend(scilinREG, 8, (uint8 *)&Xmean);
//  sciSend(scilinREG, 1, (uint8 *)" ");
//  sciSend(scilinREG, 8, (uint8 *)&Ymean);
//  sciSend(scilinREG, 1, (uint8 *)" ");
//  sciSend(scilinREG, 8, (uint8 *)&Zmean);
//  sciSend(scilinREG, 1, (uint8 *)"\n");
//
//  //    sprintf(outBuff, "%s %12.3lf %12.3lf %12.3lf %c\n", ptr->datetime,
//  //    Xmean, Ymean, Zmean, ptr->flag);
//}

void shift_sptr(void) {
  int i;

  //  for (i=0; i<; i++){

  sptr[0] = sptr[1];

  //  }

  //  sptr[11] = sptr[0];
}

void save_second(struct SECOND *second, char * filename) {
    int32_t iErr;

    // open or create file
    int32_t dataFile;
    dataFile = red_open(filename, RED_O_WRONLY | RED_O_CREAT | RED_O_APPEND);
    if (iErr == -1) {
        printf("Unexpected error %d from red_open() in save_second()\r\n", (int)red_errno);
        exit(red_errno);
    }

    // build string to save from data in second
    char dataSample[100];
    sprintf(dataSample, " %d %d %d\n", second->Xfilt, second->Yfilt, second->Zfilt);

    // save 1 Hz sample
    iErr = red_write(dataFile, dataSample, strlen(dataSample));
    if (iErr == -1) {
        printf("Unexpected error %d from red_write() in save_second()\r\n", (int)red_errno);
        exit(red_errno);
    }

    // close file
    iErr = red_close(dataFile);
    if (iErr == -1) {
        printf("Unexpected error %d from red_close() in save_second()\r\n", (int)red_errno);
        exit(red_errno);
    }
}

void convert_100Hz_to_1Hz(char * filename100Hz, char * filename1Hz) {
    /*------------------- Read the file into a string ------------------*/
    // Assumes filesystem is already initialized, formatted, and mounted
    int32_t iErr;

    // open file
    int32_t dataFile;
    dataFile = red_open(filename100Hz, RED_O_RDONLY);
    if (iErr == -1) {
        printf("Unexpected error %d from red_open() filter\r\n", (int)red_errno);
        exit(red_errno);
    }

    // read data to string
    char * data100Hz = (char *)pvPortMalloc(100000*sizeof(char));
    iErr = red_read(dataFile, data100Hz, 100000);
    if (iErr == -1) {
       printf("Unexpected error %d from red_read() filter\r\n", (int)red_errno);
       exit(red_errno);
    }

    // close file
    iErr = red_close(dataFile);
    if (iErr == -1) {
        printf("Unexpected error %d from red_close() filter\r\n", (int)red_errno);
        exit(red_errno);
    }

    /*------------------- Parse & convert packet to SECOND, then filter & save new 1 Hz sample ------------------*/
    // Note that filtering takes in one packet at a time, but requires 2 packets to apply the filter

    // get the first value in the file via a token
    // all sequential values can be read by using NULL as the string name in strtok()
    char * value = strtok(data100Hz, " ");
    int firstValueOfPacketFlag = 1;

    // a SECOND struct contains both the 100 X-Y-Z samples and the 1 filtered X-Y-Z sample
    // sptr probably stands for "second pointer"
    sptr[0] = &buffer[0];
    sptr[1] = &buffer[1];

    // There must be 2 packets of data before filtering can start
    int firstPacketFlag = 1;
    int count = 0;

    // repeat until there are no more packets left to read in the file
    while(value != NULL) {
        /*---------------------- Parse & convert one packet, then store into a second ----------------------*/
        // 3 #s from file = 1 X-Y-Z sample, 100 X-Y-Z samples = 1 packet, 300 #s from file = 1 packet
        for (int sample = 0; sample < 100; sample++) {
            // For distinguishing what value the packet starts with
            if (firstValueOfPacketFlag) {
                sptr[1]->X[sample] = strtod(value, NULL);
                firstValueOfPacketFlag = 0;
                count += 1;
            } else {
                value = strtok(NULL, " ");
                sptr[1]->X[sample] = strtod(value, NULL);
                count += 1;
            }

            value = strtok(NULL, " ");
            sptr[1]->Y[sample] = strtod(value, NULL);
            count += 1;

            value = strtok(NULL, " ");
            sptr[1]->Z[sample] = strtod(value, NULL);
            count += 1;
        }

        // strtok returns NULL if there are no more tokens that can be processed (i.e. EOF is reached)
        value = strtok(NULL, " ");
        firstValueOfPacketFlag = 1;

        /*---------------------- Apply filter and save filtered sample to a file ----------------------*/
        if (firstPacketFlag) {
            // Ensures there are 2 packets in the buffer before filtering
            shift_sptr();
            firstPacketFlag = 0;
        } else {
            apply_filter();
            save_second(sptr[1], filename1Hz);
            printf("second saved");
            shift_sptr();
        }
    }

    // Free memory
    vPortFree(data100Hz);
}
