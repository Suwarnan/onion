/*********************************************************************
 * Copyright (c) 2011-2015 Jan Pomikalek                             *
 * All rights reserved.                                              *
 *                                                                   *
 * This software is licensed as described in the file COPYING, which *
 * you should have received as part of this distribution.            *
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "version.h"

int svnrev_to_int(const char* svnrev) {
    // find the index of the first digit
    int i = 0;
    while (svnrev[i] < '0' ||  svnrev[i] > '9')
        i++;
    // convert to int
    return (int)strtol(svnrev+i, (char**)NULL, 10);
}

void print_version(const char* progname) {
    printf("%s: onion ", progname);
    if (strcmp(VERSION, "svn") == 0)
        printf("rev%i", svnrev_to_int(REVISION));
    else
        printf("v%s", VERSION);
    printf("\n\n");
    printf("Copyright (c) 2011-2015 Jan Pomikalek <jan.pomikalek@gmail.com>\n");
}
