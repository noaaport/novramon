/*
 * $Id: logfile.c 1 2009-12-14 01:44:00Z nieves $
 */
#include "logfile.h"

static FILE *gfp = NULL;

FILE *logfile_fopen(const char *name){

  if(name == NULL)
    gfp = stdout;
  else
    gfp = fopen(name, "a");

  return(gfp);
}

void logfile_fclose(void){

  if(gfp == NULL)
     return;

  if(gfp != stdout)
    fclose(gfp);
  else
    fflush(stdout);

  gfp = NULL;
}
