#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <argtable2.h>
#include <lxc/lxccontainer.h>
#include <libiptc/libiptc.h>

struct arg_lit  *demon, *version, *help;
struct arg_file *pid, *config;
struct arg_str  *lxcpath;
struct arg_end  *end;

int main(int argc, char **argv) {
  void *argtable[] = {
    demon   = arg_lit0("d"  , "demon"   , "Run as daemon")              ,
    version = arg_lit0("v"  , "version" , "Show version and exit")      ,
    help    = arg_lit0("h"  , "help"    , "Show this message and exit") ,
    lxcpath = arg_str0("l"  , "lxcpath" , "path"                        , "LXC path")             ,
    pid     = arg_file0("p" , "pid"     , "./lxc-bind.pid"              , "PID file location")    ,
    config  = arg_file0("c" , "config"  , "./lxc-bind.conf"             , "Config file location") ,
    end     = arg_end(10)
  };

  char cwd[FILENAME_MAX];
  if (!getcwd(cwd, sizeof(cwd)))
    exit(1);

  lxcpath->sval[0]    = "~/.local/share/lxc/";
  pid->filename[0]    = "lxc-bind.pid";
  config->filename[0] = "lxc-bind.conf";

  char full_pid[FILENAME_MAX];
  sprintf(full_pid, "%s/%s", cwd, pid->filename[0]);

  char full_config[FILENAME_MAX];
  sprintf(full_config, "%s/%s", cwd, config->filename[0]);

  int nerr = arg_parse(argc, argv, argtable);
  if (nerr > 0) {
    arg_print_errors(stdout, end, "lxc-bind");
    arg_print_syntax(stdout, argtable, "\n");
    arg_print_glossary(stdout, argtable, " %-30s %s\n");
    exit(1);
  }

  printf("pid file: %s\n", full_pid);
  printf("hello there! %s\n", cwd);
  exit(0);
}
