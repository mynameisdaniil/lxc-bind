#include "main.h"
#include "version.h"

struct arg_lit  *demon, *version, *help;
struct arg_file *pid, *config;
struct arg_str  *lxcpath;
struct arg_end  *end;

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

  char full_pid[FILENAME_MAX];
  sprintf(full_pid, "%s/%s", cwd, pid->filename[0]);

  char full_config[FILENAME_MAX];
  sprintf(full_config, "%s/%s", cwd, config->filename[0]);

  lxcpath->sval[0]    = "~/.local/share/lxc/";
  pid->filename[0]    = full_pid;
  config->filename[0] = full_config;

  int nerr = arg_parse(argc, argv, argtable);
  if (nerr > 0) {
    arg_print_errors(stdout, end, "You are doing it wrong");
    arg_print_syntax(stdout, argtable, "\n");
    arg_print_glossary(stdout, argtable, " %-35s %s\n");
    exit(EXIT_FAILURE);
  }

  /* printf("pid file: %s\n", pid->filename[0]); */
  /* printf("config file: %s\n", config->filename[0]); */
  /* printf("lxc-path: %s\n", lxcpath->sval[0]); */
  /* printf("hello there! %s\n", cwd); */

  if (help->count > 0) {
    arg_print_glossary(stdout, argtable, " %-35s %s\n");
    exit(EXIT_SUCCESS);
  }

  if (version->count > 0) {
    printf("Version: %s\n", VERSION);
  }

  if (demon->count > 0)
    daemonize();

  if (atexit(cleanup) != 0)
    die("Can't set exit handler. WTF, is it really could happen?!", errno);

  setlogmask(LOG_UPTO (LOG_DEBUG));
  openlog("lxc-bind", LOG_PID|LOG_CONS|LOG_NDELAY, LOG_DAEMON);
  syslog(LOG_DEBUG, "Daemon sussceefully initialized");

  return 0;
}

void die(const char *message, int code) {
  printf("ERROR: %s; errno: %d", message, code);
  syslog(LOG_EMERG, "ERROR: %s; errno: %d", message, code);
  exit(EXIT_FAILURE);
}

void cleanup() {
  syslog(LOG_INFO, "Hasta la vista, baby");
  closelog();
}

void daemonize () {
    pid_t pid = fork();
    if (pid == -1)
      die("Failed to fork while daemonising", errno);
    else if (pid != 0)
      exit(EXIT_SUCCESS);

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
}

