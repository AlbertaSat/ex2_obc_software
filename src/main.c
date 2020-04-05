#include "FreeRTOS.h"
#include <stdio.h>

#include <telecommand/telecommand_prototype_manager.h>
#include <string.h>
#include "telecommand/script_daemon.h"

#define COUNT_KEY "count"
static telecommand_counter_t count_command;
#define ARG_EXP_STRING "derp_string"
#define SCRIPT_STRING "COMMAND( \"count two\", \"derp_string\" );"; // valid count command

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
	telecommand_prototype_manager_t*	prototype_manager;
	telecommand_counter_t							count_command;
	driver_toolkit_t* 								driver_toolkit;
	script_daemon_t										daemon;
	int									count;
	_Bool								err;

	initialize_driver_toolkit(driver_toolkit); // This will instantiate hardware interfaces, in this case, using the mock ground_station

	prototype_manager = get_telecommand_prototype_manager( ); // this is used to register a new telecommand

	initialize_command_counter( &count_command, &count, driver_toolkit ); // this overrides virtual functions with the command

	err = prototype_manager->register_prototype( prototype_manager, "count two", (telecommand_t*) &count_command );
	if( !err ) {
		printf( "failed to register command prototype" );
		return -1;
	}
	/* Pretend to be ground station, and write telecommand. */

	driver_toolkit->gs.write( &gs, (uint8_t*) "COMMAND( \"count two\", \"derp_string\" );", strlen( script ), TELECOMMAND_PORT, BLOCK_FOREVER );
  printf("about to init daemon");
	/* Initialize daemon, it should start up, see the script and run it. */
	err = initialize_script_daemon( &daemon, driver_toolkit->gs );
	if( err == false )
	{
		/* Error creating daemon. */
		((telecommand_t*) &count_command)->destroy( (telecommand_t*) &count_command );
	}

	/* Give daemon CPU time. */
	task_delay( 300 );
	/* Assert the daemon ran. */
	// ASSERT( "Daemon did not execute command", count == 1 );

	((telecommand_t*) &count_command)->destroy( (telecommand_t*) &count_command );
	daemon.destroy( &daemon );
}

int main() {
	task_t TEST_TELECOMMAND;
	printf("in main!");
	create_task(	test_telecommand,
								"test_telecommand",
								500,
								NULL,
								1,
								&TEST_TELECOMMAND );

	vTaskStartScheduler();
	return 0;
}
