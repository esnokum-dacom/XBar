#ifndef WRK_H
#define WRK_H

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


int get_curr_w(Display *dpy);
void get_hour(char *buf, int bufsz, int sec);
void get_battery(char *buf, int bufsz);
void get_temp(char *buf, int bufsz);

#endif
