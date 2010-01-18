/*
 * $Id$
 */

/*
 * Usage -
 *
 * novramon [-L]
 * novramon -v
 * novramon -r [-p <password>]
 * novramon [-L] -s
 * novramon [-L] -l <logfile>
 * novramon [-L] -n
 */
#include <unistd.h>
#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <curses.h>
#include <time.h>
#include <signal.h>
#include <getopt.h>
#include "version.h"
#include "util.h"
#include "err.h"
#include "logfile.h"
#include "status.h"

#define NOVRAMON_IDENT "novramon"
#define NOVRAMON_OPENDEVICE_TIMEOUT_MS 5000

static struct {
  int opt_v;	/* print version and exit */
  int opt_L;	/* long display */
  int opt_s;	/* print status just once and exit */
  int opt_n;	/* print the data like [-l] but to stdout */
  int opt_r;    /* resets the device and exit */
  char *opt_l;  /* logfile (logger only) */
  char *opt_p;  /* password for s75+ to use with [-r] */
} g = {0, 0, 0, 0, 0, NULL, NULL};

static S75_Handle g_s75h = NULL;
static int g_f_quit = 0;

/* static void print_device(struct Novra_DeviceEntry *dev); */
static void init_curses(void);
static void kill_curses(void);
static void parse_args(int argc, char **argv);

static void init_signals(void);
static void signal_handler(int signo);
static void cleanup(void);

int main(int argc, char **argv){

  struct Novra_DeviceList devlist;
  int status = 0;
  struct novra_status_st s75status;
  const char *logfile = NULL;

  set_progname(NOVRAMON_IDENT);	/* init err lib */
  init_signals();
  atexit(cleanup);

  parse_args(argc, argv);

  if(g.opt_v != 0){
    fprintf(stdout, "%s\n", NOVRAMON_DISPLAY_VERSION);
    return(0);
  }

  if(g.opt_l != NULL){
    logfile = g.opt_l;
    if(logfile[0] != '/')
      errx(1, "The logfile must be a full path.");
  }

  status = discoverDevices(&devlist);
  if(status == 0)
    errx(1, "Cannot discoverDevices().");

  /*  print_device(&devlist.Device[0]); */

  s75status.model = devlist.Device[0].DeviceType;

  g_s75h = openDevice(&devlist.Device[0],
		      NOVRAMON_OPENDEVICE_TIMEOUT_MS,
		      status);
  if(g_s75h == NULL)
    errx(1, "Cannot openDevice()");

  if(g.opt_p != NULL)
    specifyPassword(g_s75h, g.opt_p);

  if((g.opt_l == NULL) && (g.opt_n == 0) && (g.opt_r == 0) &&
      (g.opt_s == 0)){
    init_curses();
  }

  if(g.opt_r){
    status = resetS75(g_s75h);
    if(status != S75_OK)
      errx(1, "Could not reset the device.");
  }else if(g.opt_s){
    status = get_status(g_s75h, &s75status);
    if(status != 0){
      log_errx_getstatus(status);
   } else
      print_status(&s75status, g.opt_L);
  }else{
    if(g.opt_l != NULL){
      if(daemon(0, 0) == -1)
	err(1, "Cannot exec daemon()");

      set_usesyslog();
    }

    while((g_f_quit == 0) && (status == 0)){
      status = get_status(g_s75h, &s75status);

      if(status != 0){
	log_errx_getstatus(status);
      }else if(g.opt_n){
	log_status(NULL, &s75status, g.opt_L);
      }else{
	if(g.opt_l == NULL)
	  print_statusw(&s75status, g.opt_L);
	else{
	  log_status(logfile, &s75status, g.opt_L);
	}
      }
    }
  }

  return(0);
}
  
static void init_curses(void){

  initscr(); cbreak(); noecho(); nonl();
  intrflush(stdscr, FALSE); 
  keypad(stdscr, TRUE);
  refresh();

  atexit(kill_curses);
}

static void kill_curses(void){

  (void)endwin();
}

static void parse_args(int argc, char **argv){

  int c;
  int status = 0;
  const char *optstr = "Lvl:np:rs";
  const char *usage = "novramon [-L] [-l <logfile> | -n | -s |"
    " -r [-p <password>] | -v]";
  int conflict_lnrsv = 0;

  while((status == 0) && ((c = getopt(argc, argv, optstr)) != -1) ){
    switch(c){
    case 'L':
      g.opt_L = 1;
      break;
    case 'l':
      g.opt_l = optarg;
      ++conflict_lnrsv;
      break;
    case 'n':
      g.opt_n = 1;
      ++conflict_lnrsv;
      break;
    case 'p':
      g.opt_p = optarg;
      break;
    case 'r':
      g.opt_r = 1;
      ++conflict_lnrsv;
      break;
    case 's':
      g.opt_s = 1;
      ++conflict_lnrsv;
      break;
    case 'v':
      g.opt_v = 1;
      ++conflict_lnrsv;
      break;
    default:
      status = 1;
      errx(1, usage);
      break;
    }
  }

  if(conflict_lnrsv > 1)
    errx(1, usage);
}

static void cleanup(void){

  logfile_fclose();
  
  if(g_s75h != NULL){
    closeS75(g_s75h);
    g_s75h = NULL;
  }
}

static void init_signals(void){

  signal(SIGALRM, signal_handler);
  signal(SIGHUP, signal_handler);
  /* signal(SIGPIPE, signal_handler); */
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);
  signal(SIGQUIT, signal_handler);
  signal(SIGABRT, signal_handler);
}

static void signal_handler(int signo){

  /* log_info("Caught signal %d.", signo); */
  g_f_quit = 1;
}

/*
static void print_device(struct Novra_DeviceEntry *dev){
  
  int i;

  for(i = 0; i <= 3; ++i)
    fprintf(stdout, "%u ", dev->DeviceIP[i]);

  fprintf(stdout, "\n");

  for(i = 0; i <= 5; ++i)
    fprintf(stdout, "%u ", dev->DeviceMAC[i]);

  fprintf(stdout, "\n");

  fprintf(stdout, "%hu\n", dev->StatusPort);
  fprintf(stdout, "%hu\n", dev->DeviceType);

  exit(0);
}
*/
