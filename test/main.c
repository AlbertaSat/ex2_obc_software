#include "logger/test_logger.h"
#include "leop/test_leop.h"
//#include "test_adcs_handler.h"

int main() {
    int status = 0;
    status += test_logger();
    status += test_leop();
//    status += test_adcs_handler();
//    status += test_leop();
    return status;
}
