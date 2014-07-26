#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <argtable2.h>
#include <sys/stat.h>
#include <lxc/lxccontainer.h>
#include <libiptc/libiptc.h>

typedef struct ConfigEntry {
  const char* src_iface;
  uint16_t    src_port;
  uint32_t    dest_ip;
  uint16_t    dest_port;
} ConfigEntry;

void die(const char *, int);
void daemonize();
void cleanup();
// ConfigEntry()[] parse(const char *);

#endif //MAIN_H
