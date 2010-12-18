/*
 * $Id: logfile.h 1 2009-12-14 01:44:00Z nieves $
 */
#ifndef NOVRAMON_LOGFILE_H
#define NOVRAMON_LOGFILE_H

#include <stdio.h>

__BEGIN_DECLS

FILE *logfile_fopen(const char *name);
void logfile_fclose(void);

__END_DECLS

#endif
