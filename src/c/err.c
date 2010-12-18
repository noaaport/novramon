/*
 * Copyright (c) 2005 Jose F. Nieves <nieves@ltp.upr.clu.edu>
 *
 * See LICENSE
 *
 * $Id: err.c 1 2009-12-14 01:44:00Z nieves $
 */
#include <assert.h>
#include <unistd.h>
#include <syslog.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "err.h"

/*
 * In FreeBSD we can use the function getprogname() but not in linux
 * (that I know of), so we keep it ourselves from argv[0].
 */
static int f_usesyslog = 0;
static const char *gprogname = NULL;

void set_progname(const char *ident){

  gprogname = ident;
}

void set_usesyslog(void){

  openlog(gprogname, 0, LOG_USER);
  atexit(closelog);
  f_usesyslog = 1;
}

void log_msg(int priority, const char *fmt, ...){

  va_list ap;

  va_start(ap, fmt);

  if(f_usesyslog == 1)
    vsyslog(priority, fmt, ap);
  else{
    fprintf(stderr, "%s: ", gprogname);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
  }

  va_end(ap);
}

void log_info(const char *fmt, ...){

  va_list ap;

  va_start(ap, fmt);

  if(f_usesyslog == 1)
    vsyslog(LOG_INFO, fmt, ap);
  else{
    fprintf(stderr, "%s: ", gprogname);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
  }

  va_end(ap);
}

void log_warnx(const char *fmt, ...){

  va_list ap;

  va_start(ap, fmt);

  if(f_usesyslog == 1)
    vsyslog(LOG_WARNING, fmt, ap);
  else{
    fprintf(stderr, "%s: ", gprogname);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
  }

  va_end(ap);
}

void log_errx(int e, const char *fmt, ...){

  va_list ap;

  va_start(ap, fmt);

  if(f_usesyslog == 1)
    vsyslog(LOG_ERR, fmt, ap);
  else{    
    fprintf(stderr, "%s: ", gprogname);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
  }

  va_end(ap);

  if(e != 0)
    exit(e);
}

void log_err(int e, const char *fmt, ...){

  va_list ap;
  char *s = NULL;
  char *m = ": %m";
  const char *t = NULL;

  va_start(ap, fmt);

  if(f_usesyslog == 1){
    s = malloc(strlen(fmt) + strlen(m) + 1);
    if(s != NULL){
      sprintf(s, "%s%s", fmt, m);
      t = s;
    }else
      t = fmt;

    vsyslog(LOG_ERR, t, ap);
    if(s != NULL)
      free(s);
  }else{
    fprintf(stderr, "%s: ", gprogname);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, ": %s\n", strerror(errno));
  }

  va_end(ap);

  if(e != 0)
    exit(e);
}

void log_err_open(const char *fname){

  log_err(1, "Could not open %s", fname);
}

void log_err_write(const char *fname){

  log_err(1, "Could not write to %s", fname);
}

void log_err_read(const char *fname){

  log_err(1, "Could not read from %s", fname);
}

void log_errn_open(const char *fname){

  log_err(0, "Could not open %s", fname);
}

void log_errn_write(const char *fname){

  log_err(0, "Could not write to %s", fname);
}

void log_errn_read(const char *fname){

  log_err(0, "Could not read from %s", fname);
}

void log_errx_getstatus(int status){

  assert((status == 1) || (status == -1));

  if(status == -1)
    log_errx(0, "Error from get_status: %s", "Novra library getStatus error.");
  else if(status == 1)
    log_errx(0, "Error from get_status: %s", "Unsupported novra device.");
}
