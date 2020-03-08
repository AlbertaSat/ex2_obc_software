/*
 * Copyright (C) 2015 Collin Cupido, Brendan Bruner, Stefan Damkjar
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * bbruner@ualberta.ca
 */
/**
 * @file eps.h
 * @author Collin Cupido
 * @author Brendan Bruner
 * @author Stefan Damkjar
 * @author Keith Mills
 * @date 2014-12-28
 */

#ifndef TEST_H_
#define TEST_H_

#include <stdint.h>
#include <stdio.h>


/****************************************************************************/
/* Typedefs used by eps														*/
/****************************************************************************/
typedef struct test_t 		test_t;

struct test_t
{
	void (*my_test)( void );
};

static void my_test ( void ) {
	printf("MY TEST IS RUNNING!!!!\n");
	return;
}

#endif /* EPS_H_ */
