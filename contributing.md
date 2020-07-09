CONTRIBUTION GUIDLINES
*** Preamble ***

All authored content must contain appropriate preamble including copyright and licence information as shown here:
```
/*
 * Copyright (C) 2015  University of Alberta
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
 */
 ````
All source code files must also contain a file name, list of authors (comma delimited), and a date of creation in the following format:
```
/**
 * @file <file_name>.<ext>
 * @author <author_names>
 * @date YYYY-MM-DD
 */
 ```
*** Function Headings ***

Function must be catalogued in their heading according to the following format:

```
/**
 * @brief
 * 		<breif (one line) description of the function>
 * @details
 * 		<more detailed description of the function>
 * @attention
 * 		<if applicable, any comments or concerns regarding the function>
 * @param <argument_1>
 * 		<if applicable, brief (one line) description of one of the function's arguments>
 * @param <argument_2>
 * 		<if applicable, brief (one line) description of one of the function's arguments>
 * @return
 * 		<if applicable, brief description of what is returned by the function>
 */
 ```
*** Prevent Multiple Inclusions ***

All header files should have preprocessor checking to prevent multiple inclusions:
```
#ifndef FILENAME_H
#define FILENAME_H

/* Source code goes here */

#endif /* FILENAME_H_ */
```
The header file should have the minimum required #includes possible. If a file is included because it is required for the implementation of the source file, then that file should be included in the source file, not the header file.

*** Commenting ***

Function prototypes, and implementations should always have a description.
All data structures should have a description explaining what each of their members is used for.
All variables, no matter what their scope is, should have comments explaining their purpose where they are declared.
Comments on source code should emphasize why the code is there. A programmer reading the code should understand what it does and why it is there.

*** Variable Naming ***

The variable or function name begins with a lower-case letter and the start of a subsequent word begins with a capital letter, and abbreviations are all capital and separated by an ‘_’. When variables are declared their names should be aligned with one another. See this example:
```
int     exampleOne;
char    *exampleTwo;
short   exampleThree_SYS;
```
The name of a variable should avoid abbreviations unless the abbreviation is well known or has comments explaining the abbreviation.

All macros should be named using all capital letters, with each word in the name separated by an ‘’. Macros should never start with an ‘’, and should never start, and/or end with a “__” (double underscore). Macros that imitate a function call can be written using the naming convention for functions. For example:

```
#define MAX_PACKETS 6
#define exampleFunction(int arg1,int arg2)
```
*** Coding Standards ***

Source code style is loosely based on "The GNU Coding Standards" (Copyright (C) 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Free Software Foundation, Inc.) (Full text available here: [https://www.gnu.org/prep/standards/standards.html]

All code must have full unit test coverage.

Create a feature branch when you are working on code. Make a new branch for every new feature you will add. Include your name and your feature. For example, make a branch called Collin-mNLP_Driver to develop the mNLP driver. See [https://www.atlassian.com/git/tutorials/comparing-workflows/feature-branch-workflow]

When your branch is ready to be merged into master, request that the code be pulled for review. Expect some back and forth on this. Your branch is yours to manage, but pushing to master requires authorization.

Commit messages should be meaningful. Say what you did and why, include new files, new functions, and any major bugs. One or two sentences is usually enough. If you need more than that, then you need to commit more frequently.

If you are not sure, YOU MUST ASK.
