#include "FreeRTOS.h"
#include <stdio.h>
#include "dependency_injection.h"
#include <telecommand_prototype_manager.h>
#include <parser/interpreter/telecommand_expression.h>
#include <parser/parser.h>
#include <string.h>

#define COUNT_KEY "count"
static telecommand_counter_t count_command;
#define ARG_EXP_STRING "derp_string"
#define COUNT_KEY2 "count two"
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

int main() {
    // Init toolkit and groundstation
    ground_station_t* gs = pvPortMalloc(sizeof(ground_station_t));

    initialize_mock_up_ground_station(gs);
    initialize_mock_up_ground_station_dynamic_sizes(gs);

    driver_toolkit_t* driver_toolkit = pvPortMalloc(sizeof(driver_toolkit_t));
    driver_toolkit->gs = gs;

    parser_t                            parser;
    char const*                         script = SCRIPT_STRING;
    script_expression_t*                expression;
    telecommand_prototype_manager_t*    prototypes;
    char*                               key = COUNT_KEY2;
    int                                 count;
    _Bool                              err;


    /* Register the count command with the prototype manager. */
    prototypes = get_telecommand_prototype_manager( );
    initialize_command_counter( &count_command, &count, driver_toolkit );
    err = prototypes->register_prototype( prototypes, key, (telecommand_t*) &count_command );
    if( !err )
    {
        printf( "failed to register command prototype" );
    }

    initialize_parser( &parser );
    expression = parser_parse_string( &parser, script, strlen( script ) );

    if( expression != NULL )
    {
        printf( "Command has parsed, now we will interpret it\n" );
        expression->interpret( expression );
        printf( "command should have executed\n" );
        expression->destroy( expression );
    }
	return 0;
}