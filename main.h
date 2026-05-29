#ifndef MAIN
#define MAIN

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xft/Xft.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xinerama.h>
#include <X11/extensions/Xrender.h>
#include <X11/keysym.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

/* structs */

typedef struct {
  int x, y, w;
} MInfo;

/* defines */
#define FONT "JetBrainsMono Nerd Font:size=12"
#define BAR_HEIGHT 22

#define STROINT 1
#define STRACT "[~]"

/* functions */
static int wait_event(Display *dpy);
Window create_win(Display *dpy, int win_w, int win_h);
static int default_rt(Display *dpy, Window win, int *win_w, int win_h);
MInfo move_to_monitor(Display *dpy);

#endif
