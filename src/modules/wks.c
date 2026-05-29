#include "wks.h"

int get_curr_w(Display *dpy) {
  Atom net_current_desktop;
  Atom actual_type;
  int actual_format;
  unsigned long nitems;
  unsigned long bytes_after;
  unsigned char *prop = NULL;
  int desktop = -1;

  Window root = DefaultRootWindow(dpy);

  net_current_desktop = XInternAtom(dpy, "_NET_CURRENT_DESKTOP", False);
  if (net_current_desktop == None) {
    fprintf(stderr, "Error: _NET_CURRENT_DESKTOP atom not found.\n");
    return -1;
  }
  if (XGetWindowProperty(dpy, root, net_current_desktop, 0L, 1L, False,
                         XA_CARDINAL, &actual_type, &actual_format, &nitems,
                         &bytes_after, &prop) != Success) {
    fprintf(stderr, "Error: Could not read _NET_CURRENT_DESKTOP.\n");
    return -1;
  }

  if (nitems > 0) {
    desktop = (int)(*(unsigned long *)prop);
  }

  XFree(prop);
  return desktop;
}

void get_hour(char *buf, int bufsz, int sec) {
  int s = sec;

  time_t now = time(NULL);
  struct tm *tm_struct = localtime(&now);
  int hour = tm_struct->tm_hour;
  int minutes = tm_struct->tm_min;
  int seconds = tm_struct->tm_sec;

  if (s == 0)
    snprintf(buf, bufsz, "%02d:%02d", hour, minutes);
  else if (s == 1)
    snprintf(buf, bufsz, "%02d:%02d:%02d", hour, minutes, seconds);
}

void get_battery(char *buf, int bufsz) {
  int cap = -1;
  char raw[128] = "N/A";

  FILE *f = fopen("/sys/class/power_supply/BAT0/capacity", "r");
  if (f) {
    fscanf(f, "%d", &cap);
    fclose(f);
  }

  f = fopen("/sys/class/power_supply/BAT0/status", "r");
  if (f) {
    fscanf(f, "%31s", raw);
    fclose(f);
  }

// const char *state;
// if (strcmp(raw, "Charging") == 0)
//   state = "Charging";
// else if (strcmp(raw, "Discharging") == 0)
//   state = "Discharging";
// else if (strcmp(raw, "Full") == 0)
//   state = "Full";
// else
//   state = raw;

    const char *icon;
    if (cap >= 90)
	icon = " ";
    else if (cap >= 60)
	icon = " ";
    else if (cap >= 30)
	icon = " ";
    else if (cap == 10)
	icon = " ";
    else if (cap == 0)
	icon = " ";


  if (cap >= 0)
    snprintf(buf, bufsz, "%s %d%%", icon, cap);
  else
    snprintf(buf, bufsz, "N/A");
}

void get_temp(char *buf, int bufsz) {
    FILE *f = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (!f) { snprintf(buf, bufsz, "N/A"); return; }
    int raw;
    fscanf(f, "%d", &raw);
    fclose(f);
    snprintf(buf, bufsz, "󰖐 %d°C", raw / 1000);
}
