/*
  Copyright © 2014 Daniil Sobol

  This file is part of lxc-bind.

  lxc-bind is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  lxc-bind is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with lxc-bind. If not, see <http://www.gnu.org/licenses/>.
*/

#include "main.h"
#include "version.h"

struct arg_lit  *demon, *version, *help;
struct arg_file *pid, *config;
struct arg_str  *lxcpath;
struct arg_end  *end;

char full_pid[FILENAME_MAX];
char full_config[FILENAME_MAX];

int daemonized = 0;

int main(int argc, char **argv) {
  void *argtable[] = {
    lxcpath = arg_str0("l"  , "lxcpath" , "~/.local/share/lxc"          , "LXC path"),
    pid     = arg_file0("p" , "pid"     , "./lxc-bind.pid"              , "PID file location"),
    config  = arg_file0("c" , "config"  , "./lxc-bind.conf"             , "Config file location"),
    demon   = arg_lit0("d"  , "daemon"  , "Run as daemon")              ,
    version = arg_lit0("v"  , "version" , "Show version and exit")      ,
    help    = arg_lit0("h"  , "help"    , "Show this message and exit") ,
    end     = arg_end(10)
  };

  char cwd[FILENAME_MAX];
  if (!getcwd(cwd, sizeof(cwd)))
    die("Can't get current working directory, errno=%d", errno);

  lxcpath->sval[0]    = "~/.local/share/lxc/";
  pid->filename[0]    = "./lxc-bind.pid";
  config->filename[0] = "./lxc-bind.conf";

  int nerr = arg_parse(argc, argv, argtable);
  if (nerr > 0) {
    arg_print_errors(stdout, end, "You are doing it wrong");
    arg_print_syntax(stdout, argtable, "\n");
    arg_print_glossary(stdout, argtable, " %-35s %s\n");
    exit(EXIT_FAILURE);
  }

  sprintf(full_pid, "%s/%s", cwd, pid->filename[0]);
  sprintf(full_config, "%s/%s", cwd, config->filename[0]);

  printf("pid file: %s\n", pid->filename[0]);
  printf("config file: %s\n", config->filename[0]);
  printf("lxc-path: %s\n", lxcpath->sval[0]);
  /* printf("hello there! %s\n", cwd); */

  if (help->count > 0) {
    arg_print_glossary(stdout, argtable, " %-35s %s\n");
    exit(EXIT_SUCCESS);
  }

  if (version->count > 0) {
    printf("Version: %s\n", VERSION);
    exit(EXIT_SUCCESS);
  }

  if (demon->count > 0)
    daemonize();

  if (atexit(cleanup) != 0)
    die("Can't set exit handler. WTF, is it really could happen?!", errno);

  setlogmask(LOG_UPTO (LOG_DEBUG));
  openlog("lxc-bind", LOG_PID|LOG_CONS|LOG_NDELAY, LOG_DAEMON);
  syslog(LOG_DEBUG, "Daemon sussceefully initialized");

  sleep(10);

  syslog(LOG_DEBUG, "My work is done");

  return 0;
}

void die(const char *message, int code) {
  if (daemonized)
    syslog(LOG_EMERG, "ERROR: %s; errno: %d", message, code);
  else
    printf("ERROR: %s; errno: %d\n", message, code);
  exit(EXIT_FAILURE);
}

void cleanup() {
  syslog(LOG_INFO, "Hasta la vista, baby");
  if (unlink(full_pid) == -1)
    die("Can't unlink PID file", errno);
  closelog();
}

void daemonize () {
  //check for PID file
  int pid_fd = open(full_pid, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
  char pid_buf[PID_SIZE] = "";
  if (pid_fd == -1)
    die("Can't open PID file", errno);
  int len = read(pid_fd, pid_buf, PID_SIZE);
  if (len == -1)
    die("Can't read PID file", errno);
  if (len > 0) {
    pid_buf[len] = 0;
    if (kill(atoi(pid_buf), 0) == -1)
      die("Seems that the last time daemon exit unexpectedly. Remove PID file and then try to launch me once more", errno);
    else
      die("Daemon is already started", 0);
  }

  pid_t pid = fork();
  if (pid == -1)
    die("Failed to fork while daemonising", errno);
  else if (pid != 0)
    exit(EXIT_SUCCESS);
  memset(&pid_buf, 0, PID_SIZE);
  sprintf(pid_buf, "%d", getpid());
  if (write(pid_fd, pid_buf, strlen(pid_buf)) == -1)
    die("Can't write PID file", errno);
  close(pid_fd);

  if (setsid() == -1)
    die("Failed to become a session leader while daemonising", errno);

  signal(SIGHUP,  SIG_IGN);
  signal(SIGPIPE, SIG_IGN);
  signal(SIGALRM, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);

  if (chdir("/") == -1)
    die("Failed to change working directory while daemonising", errno);

  umask(0);

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
  if (open("/dev/null", O_RDONLY) == -1)
    die("Failed to reopen stdin while daemonising", errno);
  if (open("/dev/null", O_WRONLY) == -1)
    die("Failed to reopen stdout while daemonising", errno);
  if (open("/dev/null", O_RDWR) == -1)
    die("Failed to reopen stderr while daemonising", errno);
  daemonized = 1;
}

