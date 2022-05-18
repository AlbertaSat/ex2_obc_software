
/*
 * malloc.h
 *
 *  Created on: Mar. 10, 2022
 *      Author: robert
 */

#ifndef INCLUDE_EX2_OS_MALLOC_H_
#define INCLUDE_EX2_OS_MALLOC_H_

#define malloc(size) pvPortMalloc(size)
#define free(ptr) vPortFree(ptr)

#endif /* INCLUDE_EX2_OS_MALLOC_H_ */
