#include "main.h"
#include "drw.h"
#include "src/modules/sigr1.h"
#include "src/modules/wks.h"
#include <X11/Xft/Xft.h>
#include <X11/Xlib.h>
#include <stdio.h>

static int wait_event(Display *dpy) {
  if (XPending(dpy))
    return 1;
  int fd = ConnectionNumber(dpy);
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(fd, &fds);
  struct timeval tv = {1, 0};
  return select(fd + 1, &fds, NULL, NULL, &tv) > 0;
}

Window create_win(Display *dpy, int win_w, int win_h) {
  int screen = DefaultScreen(dpy);
  Window root = DefaultRootWindow(dpy);
  int mx = 0, my = 0;
  int mw = 0, mh = 0;

  if (XineramaIsActive(dpy)) {
    int n;
    XineramaScreenInfo *info = XineramaQueryScreens(dpy, &n);
    if (!info)
	return 0;

    Window dw;
    int di;
    unsigned int du;
    int cx, cy;
    XQueryPointer(dpy, root, &dw, &dw, &cx, &cy, &di, &di, &du);
    for (int i = 0; i < n; i++) {
      if (cx >= info[i].x_org && cx < info[i].x_org + info[i].width &&
          cy >= info[i].y_org && cy < info[i].y_org + info[i].height) {
        mx = info[i].x_org;
        my = info[i].y_org;
        mw = info[i].width;
        mh = info[i].height;
        break;
      }
    }
    XFree(info);
    (void) mw; (void) mh;
  }

  XSetWindowAttributes attrs = {
      .override_redirect = True,
      .background_pixel = 0x151515,
      .event_mask = ExposureMask | ButtonPressMask | LeaveWindowMask,
  };
  Window win = XCreateWindow(
      dpy, root, mx, my, // top-left of the monitor, no centering offset
      win_w, win_h, 0, CopyFromParent, InputOutput, CopyFromParent,
      CWOverrideRedirect | CWBackPixel | CWEventMask, &attrs);

  Atom net_wm_window_type = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", False);
  Atom net_wm_window_type_dock =
      XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DOCK", False);
  XChangeProperty(dpy, win, net_wm_window_type, XA_ATOM, 32, PropModeReplace,
                  (unsigned char *)&net_wm_window_type_dock, 1);

  unsigned long strut[12] = {0, 0,  my + BAR_HEIGHT, 0, 0, 0, 0,
                             0, mx, mx + win_w,      0, 0};

  Atom net_wm_strut_partial = XInternAtom(dpy, "_NET_WM_STRUT_PARTIAL", False);
  Atom net_wm_strut = XInternAtom(dpy, "_NET_WM_STRUT", False);
  XChangeProperty(dpy, win, net_wm_strut_partial, XA_CARDINAL, 32,
                  PropModeReplace, (unsigned char *)strut, 12);
  XChangeProperty(dpy, win, net_wm_strut, XA_CARDINAL, 32, PropModeReplace,
                  (unsigned char *)strut, 4);

  return win;
}

MInfo move_to_monitor(Display *dpy) {
  MInfo m = {0, 0, DisplayWidth(dpy, DefaultScreen(dpy))};
  Window root = DefaultRootWindow(dpy);
  Window dw;
  int di, cx, cy;
  unsigned int du;
  XQueryPointer(dpy, root, &dw, &dw, &cx, &cy, &di, &di, &du);

  if (!XineramaIsActive(dpy))
    return m;

  int n;
  XineramaScreenInfo *info = XineramaQueryScreens(dpy, &n);
  if (!info)
    return m;

  for (int i = 0; i < n; i++) {
    if (cx >= info[i].x_org && cx < info[i].x_org + info[i].width &&
        cy >= info[i].y_org && cy < info[i].y_org + info[i].height) {
      m.x = info[i].x_org;
      m.y = info[i].y_org;
      m.w = info[i].width;
      break;
    }
  }
  XFree(info);
  return m;
}

static int
default_rt(Display *dpy, Window win, int *win_w, int win_h)
{
    int screen = DefaultScreen(dpy);
    Visual *vis = DefaultVisual(dpy, screen);
    Colormap cmap = DefaultColormap(dpy, screen);
    XftFont *font = XftFontOpenName(dpy, screen, FONT);
    ColorScheme col = {0};
    XftScheme scheme = {0};    
    
    XMapRaised(dpy, win);
    
    GC gc = XCreateGC(dpy, win, 0, NULL);
    
    Pixmap buf =
        XCreatePixmap(dpy, win, *win_w, win_h, DefaultDepth(dpy, screen));
    XftDraw *xdraw = XftDrawCreate(dpy, buf, vis, cmap);
    
    XEvent ev = {0};
    int c_w = *win_w, c_x = 0;
    
    load_colors(dpy, &col);
    load_xft_scheme(dpy, vis, cmap, &col, &scheme);
    signal(SIGUSR1, 0);
    
    int running = 1;
    int first = 1;
    
    MInfo m = {0, 0, *win_w};
    
    XftColor title_color;
    
    while (running) {
      if (!first)
        wait_event(dpy);
      first = 0;
    
      if (reload_colors) {
        reload_colors = 0;
        
        free_xft_scheme(dpy, vis, cmap, &scheme);
    
        load_colors(dpy, &col);
        load_xft_scheme(dpy, vis, cmap, &col, &scheme);
      }
    
      m = move_to_monitor(dpy);
    
      if (m.w != c_w || m.x != c_x) {
        XMoveResizeWindow(dpy, win, m.x, m.y, m.w, BAR_HEIGHT); // solo aquí
        unsigned long strut[12] = {0, 0,   m.y + BAR_HEIGHT, 0, 0, 0, 0,
                                   0, m.x, m.x + m.w,        0, 0};
        Atom net_wm_strut_partial =
            XInternAtom(dpy, "_NET_WM_STRUT_PARTIAL", False);
        Atom net_wm_strut = XInternAtom(dpy, "_NET_WM_STRUT", False);
        XChangeProperty(dpy, win, net_wm_strut_partial, XA_CARDINAL, 32,
                        PropModeReplace, (unsigned char *)strut, 12);
        XChangeProperty(dpy, win, net_wm_strut, XA_CARDINAL, 32, PropModeReplace,
                        (unsigned char *)strut, 4);
    
        XftDrawDestroy(xdraw);
        XFreePixmap(dpy, buf);
        buf = XCreatePixmap(dpy, win, m.w, BAR_HEIGHT, DefaultDepth(dpy, screen));
        xdraw = XftDrawCreate(dpy, buf, vis, cmap);
        c_w = m.w;
        c_x = m.x;
        *win_w = m.w;
      }
    
      while (XPending(dpy)) {
        XNextEvent(dpy, &ev);
        //     if (ev.type == LeaveNotify && ev.xcrossing.mode == NotifyNormal &&
        //         ev.xcrossing.detail != NotifyInferior)
        //       running = 0;
      }
      if (!running)
        break;
    
      // int line_h = font->ascent + font->descent + 2;
    
      int text_y = font->ascent - 1;
      int pos_t = 15;
    
      int current = get_curr_w(dpy);
      char t[64];
      char b[64];
      char c[64];
    
      get_hour(t, sizeof(t), 0);
      get_battery(b, sizeof(b));
      get_temp(c, sizeof(c));
    
      XSetForeground(dpy, gc, col.background);
      XFillRectangle(dpy, buf, gc, 0, 0, m.w, win_h);
    
      int l_ls = (current >= 5) ? current + 1 : 5;
    
      for (int i = 0; i < l_ls; i++) {
        char label[16];
        if (current - 1 == i)
          if (STROINT)
            snprintf(label, sizeof(label), STRACT);
          else
            snprintf(label, sizeof(label), "[%d]", i + 1);
        else
          snprintf(label, sizeof(label), "[%d]", i + 1);
    
        XftColor *w_color = (current - 1 == i) ? &scheme.active : &scheme.foreground;
    
        int ac_rw = (current - 1 == i) ? 30 : 0;
        int ac_rh = (current - 1 == i) ? BAR_HEIGHT : 0;
    
        XSetForeground(dpy, gc, col.colors[1]);
        XFillRectangle(dpy, buf, gc, pos_t * (i * 2), 0, ac_rw, ac_rh);
        draw_text(dpy, xdraw, font, &scheme.title, pos_t * (i * 2), text_y, label, 0);
      }
      draw_text(dpy, xdraw, font, &scheme.title, (m.w / 2.3), text_y, b, 3);
      draw_text(dpy, xdraw, font, &scheme.title, (m.w / 2), text_y, t, 3);
      draw_text(dpy, xdraw, font, &scheme.title, (m.w / 1.7), text_y, c, 1);
    
      XCopyArea(dpy, buf, win, gc, 0, 0, m.w, win_h, 0, 0);
      XMapWindow(dpy, win);
      XFlush(dpy);
    }
    
    free_xft_scheme(dpy, vis, cmap, &scheme);
    XftDrawDestroy(xdraw);
    XFreePixmap(dpy, buf);
    XftFontClose(dpy, font);
    XFreeGC(dpy, gc);
    return 0;
}

int main(void) {
  Display *dpy = XOpenDisplay(NULL);
  int screen = DefaultScreen(dpy);

  if (!dpy) {
    fprintf(stderr, "Could not open display\n");
    return 1;
  }

  int mw = DisplayWidth(dpy, screen);
  int win_w = mw, win_h = BAR_HEIGHT;
  Window win = create_win(dpy, mw, win_h);

  int ret;
  ret = default_rt(dpy, win, &win_w, win_h);

  XDestroyWindow(dpy, win);
  XCloseDisplay(dpy);
  return ret;
}
