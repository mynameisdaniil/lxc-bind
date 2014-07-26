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

#define PID_SIZE 16

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
