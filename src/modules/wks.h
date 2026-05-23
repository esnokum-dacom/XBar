#ifndef WRK_H
#define WRK_H

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int get_curr_w(Display *dpy);
void get_hour(char *buf, int bufsz, int sec);

#endif
