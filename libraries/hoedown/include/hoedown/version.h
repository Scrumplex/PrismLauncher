// SPDX-FileCopyrightText: 2008, Natacha Porté
// SPDX-FileCopyrightText: 2011, Vicent Martí
// SPDX-FileCopyrightText: 2014, Xavier Mendez, Devin Torres and the Hoedown authors
// SPDX-License-Identifier: ISC

/* version.h - holds Hoedown's version */

#ifndef HOEDOWN_VERSION_H
#define HOEDOWN_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif


/*************
 * CONSTANTS *
 *************/

#define HOEDOWN_VERSION "3.0.2"
#define HOEDOWN_VERSION_MAJOR 3
#define HOEDOWN_VERSION_MINOR 0
#define HOEDOWN_VERSION_REVISION 2


/*************
 * FUNCTIONS *
 *************/

/* hoedown_version: retrieve Hoedown's version numbers */
void hoedown_version(int *major, int *minor, int *revision);


#ifdef __cplusplus
}
#endif

#endif /** HOEDOWN_VERSION_H **/
