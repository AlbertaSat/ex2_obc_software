/*
 * test_adcs_handler.c
 *
 *  Created on: Jan 7, 2022
 *      Author: Ron Unrau
 */

#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

#include "adcs_types.h"
#include "adcs_handler.h"

static uint8_t values[256];

Describe(adcs);
BeforeEach(adcs) {
    for (int i=0; i<sizeof(values); i++)
        values[i] = i;
}
AfterEach(adcs) {}

ADCS_returnState send_uart_telecommand(uint8_t *command, uint32_t length) {
    return mock(command, length);
}

ADCS_returnState request_uart_telemetry(uint8_t TM_ID, uint8_t *telemetry, uint32_t length) {
    return mock(TM_ID, telemetry, length);
}

void receive_uart_packet(uint8_t *hole_map, uint8_t *image_bytes) {
    mock(hole_map, image_bytes);
}

Ensure(adcs, file_download_get_buffer) {
    expect(request_uart_telemetry,
           will_return(ADCS_OK),
           when(length, is_equal_to(22)),
           will_set_contents_of_parameter(telemetry, values, 22));

    uint16_t packet_id = 0;
    uint8_t file[20];
    ADCS_returnState rc = ADCS_get_file_download_buffer(&packet_id, file);

    assert_that(rc, is_equal_to(ADCS_OK));
    assert_that(packet_id, is_equal_to(values[0] | ((int) values[1] << 8)));
    assert_that(file, is_equal_to_contents_of(&values[2], 20));
}

Ensure(adcs, file_upload_packet) {
    struct {
        uint8_t cmd;
        uint8_t id[2];
        char data[20];
    } packet = { FILE_UPLOAD_PACKET_ID,
                 { 1, 0 },
                 { 0, 1, 2, 3, 4, 5, 6, 7, 8 , 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 }};
    
    expect(send_uart_telecommand,
           will_return(IS_STUBBED_A),
           when(length, is_equal_to(sizeof(packet))),
           when(command, is_equal_to_contents_of(&packet, sizeof(packet))));

    ADCS_returnState rc = ADCS_file_upload_packet(1, packet.data);
    assert_that(rc, is_equal_to(IS_STUBBED_A));
 }

static TestSuite *file_tests() {
    TestSuite *suite = create_test_suite();

    add_test_with_context(suite, adcs, file_download_get_buffer);
    add_test_with_context(suite, adcs, file_upload_packet);

    return suite;
}

int test_adcs_handler() {
    TestSuite *suite = create_test_suite();

    add_suite(suite, file_tests());
    
    return run_test_suite(suite, create_text_reporter());
}

