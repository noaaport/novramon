/*
 * Copyright (c) 2005 Jose F. Nieves <nieves@ltp.upr.clu.edu>
 *
 * See LICENSE
 *
 * $Id$
 */
#ifndef NOVRAMON_ERR_H
#define NOVRAMON_ERR_H

#include <sys/cdefs.h>
#include <err.h>

__BEGIN_DECLS
void set_progname(const char *ident);
void set_usesyslog(void);
void log_msg(int priority, const char *fmt, ...);
void log_info(const char *fmt, ...);
void log_warnx(const char *fmt, ...);
void log_errx(int e, const char *fmt, ...);
void log_err(int e, const char *fmt, ...);
void log_err_open(const char *fname);
void log_err_write(const char *fname);
void log_err_read(const char *fname);
void log_errn_open(const char *fname);
void log_errn_write(const char *fname);
void log_errn_read(const char *fname);
void log_errx_getstatus(int status);
__END_DECLS

#endif
