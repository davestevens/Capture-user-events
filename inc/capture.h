#ifndef __GUARD_CAPTURE_H
#define __GUARD_CAPTURE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/extensions/XInput.h>
#include <string.h>

void capture(Display *, int, char **);
void list_devices(Display *);
XDeviceInfo *get_device_info(Display *, int);

#define INVALID_EVENT_TYPE -1

int motion_type;
int button_press_type;
int button_release_type;
int key_press_type;
int key_release_type;

XDeviceInfo *devices;
int num_devices;

#endif

