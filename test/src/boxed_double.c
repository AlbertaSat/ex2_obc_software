#include <cgreen/boxed_double.h>
#include <stdint.h>
#include <stdlib.h>

/* NOTE: while returning BoxedDouble* here seems logical, it forces casts all over the place */
intptr_t box_double(double value) {
    BoxedDouble *box = (BoxedDouble *) pvPortMalloc(sizeof(BoxedDouble));
    box->value = value;
    return (intptr_t)box;
}

double unbox_double(intptr_t box) {
    double value = as_double(box);
    vPortFree((BoxedDouble *)box);
    return value;
}

double as_double(intptr_t box) {
    return ((BoxedDouble *)box)->value;
}
