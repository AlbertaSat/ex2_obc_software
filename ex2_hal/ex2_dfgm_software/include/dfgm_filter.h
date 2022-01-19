#ifndef DFGM_FILTER_H
#define DFGM_FILTER_H

struct SECOND {
    char datetime[19];
    double X[100];
    double Y[100];
    double Z[100];
    char flag;
    double Xfilt;
    double Yfilt;
    double Zfilt;
};

#endif /* DFGM_FILTER_H_ */
