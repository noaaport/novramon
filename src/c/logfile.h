/*
 * $Id$
 */
#ifndef NOVRAMON_LOGFILE_H
#define NOVRAMON_LOGFILE_H

#include <stdio.h>

__BEGIN_DECLS

FILE *logfile_fopen(const char *name);
void logfile_fclose(void);

__END_DECLS

#endif
