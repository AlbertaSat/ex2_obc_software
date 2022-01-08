#include "logger/test_logger.h"
#include "test_adcs_handler.h"

int main() {
    int status = 0;
    status += test_logger();
    status += test_adcs_handler();
    return status;
}
