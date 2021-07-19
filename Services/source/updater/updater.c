#include "updater/updater.h"
#include <FreeRTOS.h>
#include <os_task.h>

#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <main/system.h>
#include "util/service_utilities.h"
#include "services.h"
#include "redposix.h"
#include "bl_eeprom.h"
#include "bl_flash.h"
#include "privileged_functions.h"

//for testing only. do hex dump
//size is the number of bytes we want to print
static void hex_dump(char *stuff, int size){
  uint32_t current_packet_index = 0;
  printf("printing number of bytes: %u\n", size);
    int j = 0;
    for (j = 0; j < size; j += 1) {
      if (stuff[current_packet_index] < 0x10) {
        printf("0");
      }
      printf("%X ", stuff[current_packet_index]);
      current_packet_index += 1;
      if (current_packet_index % 16 == 0) {
        printf("\n");
      }
    }
    printf("\n");
}

// returns the size of the buffer it managed to allocate
uint32_t get_buffer(void **buf) {
    *buf = NULL;
    uint32_t attempts[] = {4096, 2048, 1024, 512, 256, 128, 64, 32, 16, 8};
    int i;
    for (i = 0; i < sizeof(attempts) / sizeof(uint32_t); i++) {
        *buf = pvPortMalloc(attempts[i]);
        if (*buf != NULL) {
            return attempts[i];
        }
    }
    return 0;
}

SAT_returnState updater_app(csp_packet_t *packet) {
    uint8_t ser_subtype = (uint8_t)packet->data[SUBSERVICE_BYTE];
    int32_t fp;
    int8_t status;
    uint8_t *buf;

    if (init_eeprom()) {
        switch (ser_subtype) {
          case FLASH_UPDATE:
    #ifdef GOLDEN_IMAGE
              fp = red_open("VOL0:/application_image.bin", RED_O_RDONLY);
              if (fp == -1) {
                  status = -1; break;
              }
              REDSTAT file_info;
              int res = red_fstat(fp, &file_info);
              if (res == -1) {
                  int err = red_errno;
                  status = -1; break;
              }
              image_info app_info = priv_eeprom_get_app_info();
              if (!BLInternalFlashStartAddrCheck(app_info.addr, (uint32_t)file_info.st_size)){
                  status = -1; break;
              }

              uint8_t oReturnCheck = 0;
              oReturnCheck = priv_Fapi_BlockErase(app_info.addr, (uint32_t)file_info.st_size);
              if (oReturnCheck) {
                  status = -1; break;
              }
              uint32_t flash_destination = app_info.addr;
              uint32_t flash_size = get_buffer(&buf); // returns the size
              int32_t bytes_read;
              bytes_read = red_read(fp, buf, flash_size);
              if (bytes_read < flash_size) {
                  flash_size = bytes_read;
              }
              oReturnCheck = priv_Fapi_BlockProgram(1, flash_destination, (unsigned long)buf, flash_size);
              if (oReturnCheck) {
                  status = -1; break;
              }
              flash_destination += flash_size;

              status = 0;
    #else
              ex2_log("FAILED Attempt to flash from non golden image");
              status = -1;
    #endif
              break;

          case GET_GOLDEN_INFO:

              image_info golden_info = priv_eeprom_get_golden_info();
              status = 0;
              memcpy(&packet->data[OUT_DATA_BYTE], &app_info,sizeof(golden_info));
              set_packet_length(packet, sizeof(int8_t)+sizeof(golden_info) + 1);
              break;

          case GET_APP_INFO:
              image_info application_info = priv_eeprom_get_app_info();
              status = 0;
              memcpy(&packet->data[OUT_DATA_BYTE], &application_info,sizeof(application_info));
              set_packet_length(packet, sizeof(int8_t)+sizeof(application_info) + 1);
              break;

          case SET_APP_ADDRESS:
    #ifdef GOLDEN_IMAGE
              hex_dump(&packet->data, 20);
              int new_address = 0;
              memcpy(&new_address, &packet->data[IN_DATA_BYTE], sizeof(uint32_t));
              image_info addr_info = priv_eeprom_get_app_info();
              addr_info.addr = new_address;
              priv_eeprom_set_app_info(addr_info);
              set_packet_length(packet, sizeof(int8_t) + 1);
              status = 0;
    #else
              status = -1;
              ex2_log("FAILED attempt to set application addr from non golden image");
    #endif
              break;

          case SET_APP_CRC:
    #ifdef GOLDEN_IMAGE
              uint16_t crc = 0;
              memcpy(&crc, &packet->data[IN_DATA_BYTE], sizeof(uint16_t));
              image_info crc_info = priv_eeprom_get_app_info();
              crc_info.crc = crc;
              priv_eeprom_set_app_info(crc_info);
              set_packet_length(packet, sizeof(int8_t) + 1);
              status = 0;
    #else
              status = -1;
              ex2_log("FAILED attempt to set application crc from non golden image");
    #endif
              break;

          case ERASE_APP:
    #ifdef GOLDEN_IMAGE
              image_info erase_info = priv_eeprom_get_app_info();
              erase_info.exists = 0;
              priv_eeprom_set_app_info(erase_info);
              set_packet_length(packet, sizeof(int8_t) + 1);
              status = 0;
    #else
              status = -1;
              ex2_log("FAILED attempt to erase application from non golden image");
    #endif
              break;

          case VERIFY_APPLICATION_IMAGE:
              if (priv_verify_application() != true) {
                  status = -1;
                  break;
              }
              set_packet_length(packet, sizeof(int8_t) + 1);
              status = 0;
              break;

          case VERIFY_GOLDEN_IMAGE:
              if (priv_verify_golden() != true) {
                  status = -1;
                  break;
              }
              set_packet_length(packet, sizeof(int8_t) + 1);
              status = 0;
              break;

          default:
            ex2_log("No such subservice\n");
            shutdown_eeprom();
            return SATR_PKT_ILLEGAL_SUBSERVICE;
        }
    } else {
        status = -1;
    }
    if (status == -1) {
        set_packet_length(packet, sizeof(int8_t) + 1);
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
