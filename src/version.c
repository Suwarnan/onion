/*********************************************************************
 * Copyright (c) 2011 Jan Pomikalek                                  *
 * All rights reserved.                                              *
 *                                                                   *
 * This software is licensed as described in the file COPYING, which *
 * you should have received as part of this distribution.            *
 *********************************************************************/

#include <stdio.h>
#include "version.h"

void print_version(const char* progname) {
    printf("%s: onion v%s\n\n", progname, VERSION);
    printf("Copyright (c) 2011 Jan Pomikalek <jan.pomikalek@gmail.com>\n");
}
