#include "FreeRTOS.h"
#include <stdio.h>

#include <telecommand/telecommand_prototype_manager.h>
#include <string.h>
#include "telecommand/script_daemon.h"
#include "telecommand/commands/registry.h"
#include "systems/ground_station/ground_station.h"

static telecommand_counter_t count_command;

void vAssertCalled(
unsigned long ulLine, const char * const pcFileName
);

void vAssertCalled(
unsigned long ulLine, const char * const pcFileName
)
{
		printf("error line: %lu in file: %s", ulLine, pcFileName);
}

void test_telecommand(void) {
	printf("-here-");
	telecommand_prototype_manager_t*	prototype_manager;
	telecommand_counter_t							count_command;
	driver_toolkit_t* 								driver_toolkit;
	script_daemon_t										daemon;
	int									count;
	_Bool								err;
	ground_station_t		gs;

	/*
	Step 1: initialize driver toolkit - this contains the (mock) ground station, and other hardware in the future
	*/
	printf("\n-- initializing driver toolkit --\n");
	initialize_driver_toolkit(driver_toolkit); // This will instantiate hardware interfaces, in this case, using the mock ground_station

	/*
	Step 2: initialize the counter command. All prototypes for commands are found in the telecommand/command folder. This step fills in the execution functions of the command
	*/
	printf("\n-- initialize counter command --\n");
	initialize_command_counter( &count_command, &count, driver_toolkit ); // this overrides virtual functions with the command

	/*
	Step 3: register the telecommand and it's trigger with the prototype_manager. This is done only once.
					After this step, "COMMAND( count, ... )" will trigger the registered command
	*/
	printf("\n-- registring telecommand --\n");
	prototype_manager = get_telecommand_prototype_manager( ); // this is used to register a new telecommand
	prototype_manager->register_prototype( prototype_manager, "count", (telecommand_t*) &count_command );

	/* Pretend to be ground station, and write telecommand. */
	printf("\n-- writing command to ground station --\n");
	char* script = "COMMAND( \"count\", \"derp_string\" );";
	initialize_mock_up_ground_station_dynamic_sizes( &gs );
	gs.write( &gs, (uint8_t*) script, strlen( script ), TELECOMMAND_PORT, BLOCK_FOREVER );

	/*
	Step 4: Initialize daemon, it should start up, see the script and run it
	*/
	printf("\n-- starting telecommand server --\n");
	if( !initialize_script_daemon( &daemon, &gs ) )
	{
		/* Error creating daemon. */
		printf("could not create daemon");
		((telecommand_t*) &count_command)->destroy( (telecommand_t*) &count_command );
	}

	/* Give daemon CPU time. */
	task_delay( 300 );

	((telecommand_t*) &count_command)->destroy( (telecommand_t*) &count_command );
	daemon.destroy( &daemon );
}

int main() {
	printf("\n -- starting telecommand example -- \n");

	task_t TEST_TELECOMMAND;

	create_task(	test_telecommand,
								"test_telecommand",
								500,
								NULL,
								1,
								&TEST_TELECOMMAND );

	vTaskStartScheduler();

	return 0;
}
