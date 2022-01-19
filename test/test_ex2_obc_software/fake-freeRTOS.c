#include <stdint.h>
#include <stdlib.h>

void *pvPortMalloc(size_t len) {
    return malloc(len);
}

void vPortFree(void *pv) {
    free(pv);
}
