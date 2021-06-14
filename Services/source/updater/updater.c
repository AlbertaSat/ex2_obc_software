#include "updater/updater.h"
#include "bl_eeprom.h"
#include "bl_flash.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <main/system.h>
#include "util/service_utilities.h"
#include "services.h"
#include "redposix.h"
#include "bl_eeprom.h"
#include "privileged_functions.h"

// returns the size of the buffer it managed to allocate
uint32_t get_buffer(uint8_t *buf) {
    buf = NULL;
    uint32_t attempts[] = {2048, 1024, 512, 256, 128, 64, 32, 16, 8};
    int i;
    for (i = 0; i < sizeof(attempts) / sizeof(uint32_t); i++) {
        buf = (uint8_t *)pvPortMalloc(attempts[i]);
        if (buf != NULL) {
            return attempts[i];
        }
    }
    return 0;
}

SAT_returnState updater_app(csp_packet_t *packet) {
    uint8_t ser_subtype = (uint8_t)packet->data[SUBSERVICE_BYTE];
    int32_t fp;
    int8_t status;
    uint8_t *buf = NULL;
    image_info app_info;

    switch (ser_subtype) {
      case FLASH_UPDATE:
          fp = red_open("application_image.bin", RED_O_RDONLY);
          if (fp == -1) {
              status = -1; break;
          }
          REDSTAT file_info;
          if (!red_fstat(fp, &file_info)) {
              status = -1; break;
          }
          if (!init_eeprom()) {
              status = -1; break;
          }
          app_info = eeprom_get_app_info();
          if (!BLInternalFlashStartAddrCheck(app_info.addr, (uint32_t)file_info.st_size)){
              status = -1; break;
          }
          uint8_t oReturnCheck = 0;
          oReturnCheck = Fapi_BlockErase(app_info.addr, (uint32_t)file_info.st_size);
          if (oReturnCheck) {
              status = -1; break;
          }
          uint32_t flash_destination = app_info.addr;
          uint32_t flash_size = get_buffer(buf); // returns the size
          uint32_t bytes_read;
          while((bytes_read = red_read(fp, buf, flash_size)) > 0) {
              if (bytes_read < flash_size) {
                  flash_size = bytes_read;
              }
              oReturnCheck = Fapi_BlockProgram(1, flash_destination, (unsigned long)buf, flash_size);
              if (oReturnCheck) {
                  status = -1; break;
              }
              flash_destination += flash_size;
          }
          status = 0;
          break;

      case GET_GOLDEN_INFO:
          if (!init_eeprom()) {
              status = -1; break;
          }
          app_info = priv_eeprom_get_golden_info();
          status = 0;
          memcpy(&packet->data[OUT_DATA_BYTE], &app_info,sizeof(app_info));
          set_packet_length(packet, sizeof(int8_t)+sizeof(app_info) + 1);
          break;

      case GET_APP_INFO:
          if (!init_eeprom()) {
              status = -1; break;
          }
          app_info = priv_eeprom_get_app_info();
          status = 0;
          memcpy(&packet->data[OUT_DATA_BYTE], &app_info,sizeof(app_info));
          set_packet_length(packet, sizeof(int8_t)+sizeof(app_info) + 1);
          break;


      case SET_GOLDEN_INFO:
          memcpy(&packet->data[IN_DATA_BYTE], &app_info, sizeof(app_info));
          priv_eeprom_set_golden_info(app_info);
          set_packet_length(packet, sizeof(int8_t) + 1);
          status = 0;
          break;

      case SET_APP_INFO:
          memcpy(&packet->data[IN_DATA_BYTE], &app_info, sizeof(app_info));
          priv_eeprom_set_app_info(app_info);
          set_packet_length(packet, sizeof(int8_t) + 1);
          status = 0;
          break;

      default:
        ex2_log("No such subservice\n");
        shutdown_eeprom();
        return SATR_PKT_ILLEGAL_SUBSERVICE;
    }
    shutdown_eeprom();
    memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
    return SATR_OK;
}

void updater_service(void *param) {
    csp_socket_t *sock;
    sock = csp_socket(CSP_SO_RDPREQ); // require RDP connection
    csp_bind(sock, TC_UPDATER_SERVICE);
    csp_listen(sock, SERVICE_BACKLOG_LEN);

    for(;;) {
        csp_conn_t *conn;
        csp_packet_t *packet;
        if ((conn = csp_accept(sock, CSP_MAX_TIMEOUT)) == NULL) {
          /* timeout */
          continue;
        }
        while ((packet = csp_read(conn, 50)) != NULL) {
          if (updater_app(packet) != SATR_OK) {
            // something went wrong, this shouldn't happen
            csp_buffer_free(packet);
          } else {
              if (!csp_send(conn, packet, 50)) {
                  csp_buffer_free(packet);
              }
          }
        }
        csp_close(conn);
    }
}


SAT_returnState start_updater_service(void) {
    if (xTaskCreate((TaskFunction_t)updater_service,
                    "updater_service", 300, NULL, NORMAL_SERVICE_PRIO,
                    NULL) != pdPASS) {
      ex2_log("FAILED TO CREATE TASK updater_service\n");
      return SATR_ERROR;
    }
    return SATR_OK;
}
