## ex2 command handling demo
This repo will include hardware agnostic/generic implementations of the ECSS services

## To run locally
* Clone [SatelliteSim](https://github.com/AlbertaSat/SatelliteSim/) into the same root directory as this repository (just to keep things simple) and follow the instructions in the SatelliteSim repo for building CSP

* For now, this still references the [upsat services](https://gitlab.com/librespacefoundation/upsat/upsat-ecss-services) library for some header files, so clone that too

* Compile this code to a static lib (.a) file, the command should be similar to the following,
```
gcc *.c Platform/demo/*.c Platform/demo/hal/*.c Services/*.c -c -I . -I Platform/demo -I Platform/hal -I Services/ -I ../upsat-ecss-services/services/ -I ../SatelliteSim/Source/include/ -I ../SatelliteSim/Project/ -I ../SatelliteSim/libcsp/include/ -I ../SatelliteSim/Source/portable/GCC/POSIX/ -I ../SatelliteSim/libcsp/build/include/ -lpthread -std=c99 -lrt && ar -rsc client_server.a *.o
```
* Link to this static library file in the SatelliteSim's Makefile (again, as described in the readme at that repo), and run
```make clean && make all```
Design Overview and Justification

Ex-Alta 2 will be designed with a service oriented architecture because the approach is easy to reason about for end users, and will encourage a high standard of modularity, and overall elegance. Moreover, the European Space Agency specifies a standard set of services to be implemented, ECSS-E-70-41A, giving the team a clear roadmap; it is expected that adopting such a standard will set a solid foundation for future generations of AlbertaSat members to develop excellent software by building off Ex-Alta 2 code.

Service oriented architecture (SOA) puts forward that each component in a system will offer a set of services which are implemented by application components - some loosely coupled (independent) system components - and are accessible through a network (CSP). A service logically represents a business activity that the hardware offers, a service is also self contained, and will consist of different sub-services; finally, services are black boxes for the end users - that is to say that the user need not know the inner workings of the service.

Perhaps the most crucial feature of the service oriented design it it’s self-contained nature. This means that the logic of a service can be heavily reused, and developed separate from other services.

Cubesat Space Protocol

The chosen network stack, ‘Cubesat Space Protocol’ or CSP provides some key features that will be used in the implementation of a service architecture; namely multi-hop routing which takes care of getting a command packet to the specified service provider using a static routing table (meaning all possible connections are specified upon initialization) as well as support for 32 different service ports. The routing, and other functionality like CRC, and encryption are done in CSP using data in the CSP packet header, which contains the source, destination, and port information.

Figure 1: CSP header





Service Interaction With Hardware

First off, note that this section should be taken with a grain of salt: each system has different needs -- all systems may not comply with this scheme, but consider the merits of this design before deviating.

Figure 2: design diagram

In this figure, a number of crucial things are illustrated. First, note that the layers at the top of the diagram represent greater levels of ‘abstraction’ from the hardware & firmware. We like abstraction because we want to not have to worry about the hardware interfaces when programming applications for the user (i.e. satellite operators).

The blue boxes at the top represent the applications that make up the provided services. For instance, the payload control application takes care of requests made to the payload control services, or similarly, user requests to the housekeeping will be carried out by the housekeeping application (don’t overthink these terms). These applications interact with the ‘On Board Software Data Pool (OBSW DP)’, which will be some data structure that is accessed by the applications, and updated by the hardware equipment handlers. This design makes it easy to separate the work to be done in terms of hardware interactions below the ‘OBSW DP’ line, and user interactions above the ‘OBSW DP’ line.

The equipment handlers simply represent the hardware abstraction layer (HAL), and are responsible for implementing functions to get, set, and otherwise interact with hardware devices VIA their respective I/O protocols on the ‘line drivers’.

The RTOS is shown with the line drivers, but will be present throughout the stack. The ™ and TC decoders represent the handling of command data in and out of the satellite; this will largely be handled by CSP. And finally, the bootloader is shown adjacent to the line drivers; since this is a low level feature of the stack that will be needed to update code later in the mission.

Implementation
In this section, we’ll take a preliminary look at what implementing services will entail in terms of code.
Project Structure
We will aim to keep a coherent file structure that reflects the design, and avoids rewriting code where possible.

All services will be contained in a general ‘services’ repository that will contain generic implementations of all the services offered by any system, as well as platform specific code that will: a) instantiate the service handlers needed by each specific system, and b) provide interfaces to the hardware drivers that will be kept elsewhere in subsystem-specific repositories. The services repository will look something like:

 
Figure 3: file structure


Command Handling/Packet Routing
Command routing will mostly be handled by CSP. In order to get CSP to do it’s job all you must do is initialize CSP with whatever hardware interface is required, and initialize with the appropriate server ID. An example from the AlbertaSat ‘ex2_command_handling_demo’ repo is given:

Step 1 is to start service handlers: this includes instantiating the command queues for each service, as well as the  FreeRTOS tasks that wait on them. This example shows the instantiation of a generic ‘test_app’:

main.c: 
```
int main(int argc, char **argv) {
  TC_TM_app_id my_address = DEMO_APP_ID; // this value indicates the CSP
     address of this network node (demo) and is defined in services.h
  if (start_service_handlers() != SATR_OK) {
    printf("COULD NOT START TELECOMMAND HANDLER");
    return -1;
  }
...
...
}
```
Code Snippet 1: call ‘start_service_handlers’

demo.c:
```
static void test_app(void *parameters) {
  csp_packet_t packet;
  for (;;) {
    if (xQueueReceive(service_queues.test_app_queue, &packet,
                      NORMAL_TICKS_TO_WAIT) == pdPASS) {
      test_service_app(&packet); // pass packet to the application that     
                                 handles this service
    }
  }
}

SAT_returnState start_service_handlers() {
  /**
   * Create the queues & tasks for each service implemented by this module
   */
  if (!(service_queues.test_app_queue =
            xQueueCreate((unsigned portBASE_TYPE)NORMAL_QUEUE_LEN,
                         (unsigned portBASE_TYPE)NORMAL_QUEUE_SIZE))) {
    printf("FAILED TO CREATE VERIFICATION APP QUEUE");
    return SATR_ERROR;
  };
  xTaskCreate((TaskFunction_t)test_app, "test app", 2048, NULL,
              NORMAL_SERVICE_PRIO, NULL);
  return SATR_OK;
}
```
Code Snippet 2: example start_service_handlers implementation 

Step 2 is to initialize CSP and register the interfaces (zmq in this case) and start CSP:

main.c 
```
int main(int argc, char **argv) {
  ...
  ...
  /* Init CSP with address and default settings */
  csp_conf_t csp_conf;
  csp_conf_get_defaults(&csp_conf);
  csp_conf.address = my_address;
  int error = csp_init(&csp_conf);
  if (error != CSP_ERR_NONE) {
    printf("csp_init() failed, error: %d", error);
    return -1;
  }
  printf("Running at %d\n", my_address);
  /* Set default route and start router & server */
  csp_route_start_task(500, 0);

#ifdef USE_LOCALHOST
  init_zmq_interface();
#else
// implement other interfaces
#endif
  ...
}

SAT_returnState init_zmq_interface() {
  csp_iface_t *default_iface = NULL;
  int error =
      csp_zmqhub_init(csp_get_address(), "localhost", 0, &default_iface);
  if (error != CSP_ERR_NONE) {
    printf("failed to add ZMQ interface [%s], error: %d", "localhost", error);
    return SATR_ERROR;
  }
  csp_rtable_set(CSP_DEFAULT_ROUTE, 0, default_iface, CSP_NO_VIA_ADDRESS);
  return SATR_OK;
}
```
Code Snippet 3: start CSP for the ‘demo’ server

Step 3 is to start the server task which will put incoming commands into the appropriate command queues based on their port number, which is used to indicate the target service for that incoming command:

main.c:
```
int main(int argc, char **argv) {
  ...
  ...
  xTaskCreate((TaskFunction_t)server_loop, "SERVER THREAD", 2048, NULL, 1,
              NULL);

  vTaskStartScheduler();

  for (;;) {
  }

  return 0;
}

void server_loop(void *parameters) {
  csp_socket_t *sock;
  csp_conn_t *conn;
  csp_packet_t *packet;

  /* Create socket and listen for incoming connections */
  sock = csp_socket(CSP_SO_NONE);
  csp_bind(sock, CSP_ANY);
  csp_listen(sock, 5);
  portBASE_TYPE err;

  /* Super loop */
  for (;;) {
    /* Process incoming packet */
    if ((conn = csp_accept(sock, 10000)) == NULL) {
      /* timeout */
      continue;
    }
    while ((packet = csp_read(conn, 50)) != NULL) {
      switch (csp_conn_dport(conn)) {
        case TC_TEST_SERVICE:
          err = xQueueSendToBack(service_queues.test_app_queue, packet,
                                 NORMAL_TICKS_TO_WAIT);
          if (err != pdPASS) {
            printf("FAILED TO QUEUE MESSAGE");
          }
          csp_buffer_free(packet);
          break;

        default:
          csp_service_handler(conn, packet);
          break;
      }
    }
    csp_close(conn);
  }
}
```
Code Snippet 4: CSP server

In summary, for each service a packet will:
Arrive on the CSP socket as shown in Code Snippet 4,
Be put into an RTOS queue based on the port number,
Be passed to a service application by the woken task as seen in Code Snippet 2

Finally, the application will determine the subservice task from the data packet. The subservice will be indicated by the first data byte in the data section of the CSP packet as shown:

test_service.c:
```
SAT_returnState test_service_app(csp_packet_t *pkt) {
  uint8_t ser_subtype = (uint8_t)pkt->data[0];
  switch (ser_subtype) {
    case TEST_SUBSERVICE_1: // ← these types are defined in services.h
      // handle the request
    case TEST_SUBSERVICE_2:
      // handle the request
  ...
  ...
  }
}
```
Code Snippet 5: generic application implementation

From here, as developers, we must write the service application code to handle the requests, and send responses as required.


