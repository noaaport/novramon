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
 * novramon [-L] -l <logfile> [-t <seconds>]
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
#define NOVRAMON_CONNECT_TIMEOUT_MS 5000

static struct {
  int opt_v;	/* print version and exit */
  int opt_L;	/* long display */
  int opt_s;	/* print status just once and exit */
  int opt_n;	/* print the data like [-l] but to stdout */
  int opt_r;    /* resets the device and exit */
  int opt_t;	/* log interval in seconds - uint16 */
  char *opt_l;  /* logfile (logger only) */
  char *opt_p;  /* password for s75+ to use with [-r] */
} g = {0, 0, 0, 0, 0, 0, NULL, NULL};

static ReceiverSearch g_rs;
static ReceiverList g_rl;
static Receiver *g_r = NULL;
static int g_f_quit = 0;

static void init_curses(void);
static void kill_curses(void);
static void parse_args(int argc, char **argv);

static void init_signals(void);
static void signal_handler(int signo);
static void cleanup(void);

/* static void print_receiver(Receiver *r); */
static int specify_password(Receiver *r, char *password);

int main(int argc, char **argv){

  int status = 0;
  NOVRA_ERRORS novra_error;
  struct novra_status_st nvstatus;
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

  init_novra_status(&nvstatus);

  g_rs.discoverLocal(&g_rl);
  if(g_rl.count() == 0)
    errx(1, "No local receivers found.\n");
  else if(g_rl.count() > 1) 
    errx(1, "Only one receiver is supported.\n");

  g_r = g_rl.getReceiver(0);
  nvstatus.device_type = g_r->getParameter(DEVICE_TYPE).asShort();

  if(g_r->connect(NOVRAMON_CONNECT_TIMEOUT_MS, novra_error) == false){
    g_r = NULL;
    errx(1, "Cannot connect to device.");
  }

  /*  print_receiver(g_r); */
  /* check_param(g_r); */

  if(g.opt_p != NULL){
    specify_password(g_r, g.opt_p);
    if(g_r->connected(NOVRAMON_CONNECT_TIMEOUT_MS) == false)
      errx(1, "Cannot connect to device.");
  }

  if((g.opt_l == NULL) && (g.opt_n == 0) && (g.opt_r == 0) &&
      (g.opt_s == 0)){
    init_curses();
  }

  if(g.opt_r){
    if(g_r->reset() == false)
      errx(1, "Could not reset the device.");
  }else if(g.opt_s){
    status = get_status(g_r, &nvstatus);
    if(status != 0){
      log_errx_getstatus(status);
   } else
      print_status(&nvstatus, g.opt_L);
  }else{
    if(g.opt_l != NULL){
      if(daemon(0, 0) == -1)
	err(1, "Cannot exec daemon()");

      set_usesyslog();
    }

    while((g_f_quit == 0) && (status == 0)){
      status = get_status(g_r, &nvstatus);

      if(status != 0){
	log_errx_getstatus(status);
      }else if(g.opt_n){
	log_status(NULL, &nvstatus, g.opt_L, g.opt_t);
      }else{
	if(g.opt_l == NULL)
	  print_statusw(&nvstatus, g.opt_L);
	else{
	  log_status(logfile, &nvstatus, g.opt_L, g.opt_t);
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
  uint16_t u16;
  const char *optstr = "Lvl:np:rst:";
  const char *usage = "novramon [-L] [-l <logfile> [-t <seconds>] | -n | -s |"
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
    case 't':
      if(strto_u16(optarg, &u16) != 0){
	errx(1, "Invalid value of -t option.");
      }
      g.opt_t = u16;
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
  
  if(g_r != NULL){
    g_r->disconnect();
    g_r = NULL;
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

static int specify_password(Receiver *r, char *password){

  if(r->hasParameter(PASSWORD)){
    if(r->login(password) == false)
      return(1);
  }

  return(0);
}

/*
static void print_receiver(Receiver *r){

  int device_type = r->getParameter(DEVICE_TYPE).asShort();

  fprintf(stdout, "%d\n", device_type);
  fprintf(stdout, "%s\n", r->enumToString(DEVICE_TYPE, device_type).c_str());

  fprintf(stdout, "IP address: %s\n",
          r->getParameter(RECEIVER_IP).asString().c_str());
  fprintf(stdout, "MAC: %s\n", 
         r->getParameter( RECEIVER_MAC ).asString().c_str());

  exit(0);
}
*/
