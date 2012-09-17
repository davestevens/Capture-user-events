#include "capture.h"

void list_devices(Display *display)
{
	int idx;
	if(devices == NULL) {
		devices = XListInputDevices(display, &num_devices);
	}

	printf("ID\tNAME\n");
	for(idx=0;idx<num_devices;++idx) {
		printf("%02d\t%s\n", (int)devices[idx].id, (char *)devices[idx].name);
	}
	return;
}

void capture(Display *display, int argc, char *argv[])
{
	int idx;
	int id;
	XEventClass  event_list[5];
	unsigned long screen;
	Window root_win;
	int number = 0;

	motion_type = button_press_type = button_release_type = key_press_type = key_release_type = INVALID_EVENT_TYPE;

	screen = DefaultScreen(display);
	root_win = RootWindow(display, screen);

	for(idx=0;idx<argc;++idx) {
		XDeviceInfo *device_info = NULL;
		XDevice *device = NULL;
		XInputClassInfo *ip = NULL;
		int i;

		id = atoi(argv[idx]);

		/* Find this id with in the device list */
		device_info = get_device_info(display, id);
		if(device_info == NULL) {
			fprintf(stderr, "Could not find device id: %d\n", id);
			return;
		}

		device = XOpenDevice(display, id);
		if(device == NULL) {
			fprintf(stderr, "Could not open device id: %d\n", id);
			return;
		}

		if(device->num_classes > 0) {
			for(ip = device->classes, i=0;
			    i < device_info->num_classes;
			    ++ip, ++i) {
				switch(ip->input_class) {
					case KeyClass:
						key_press_type = ip->event_type_base + _deviceKeyPress;
						event_list[number++] = device->device_id << 8 | key_press_type;

						key_release_type = ip->event_type_base + _deviceKeyRelease;
						event_list[number++] = device->device_id << 8 | key_release_type;
						break;
					case ButtonClass:
						button_press_type = ip->event_type_base + _deviceButtonPress;
						event_list[number++] = device->device_id << 8 | button_press_type;

						button_release_type = ip->event_type_base + _deviceButtonRelease;
						event_list[number++] = device->device_id << 8 | button_release_type;
						break;
					case ValuatorClass:
						motion_type = ip->event_type_base + _deviceMotionNotify;
						event_list[number++] =  device->device_id << 8 | motion_type;
					break;
					default:
						break;
				}
			}
		}
	}

	if(XSelectExtensionEvent(display, root_win, event_list, number)) {
		fprintf(stderr, "Error selecting extended events\n");
		return;
	}

	/*log_events(display);*/
	return;
}

XDeviceInfo *get_device_info(Display *display, int id)
{
	int idx;
	if(devices == NULL) {
		devices = XListInputDevices(display, &num_devices);
	}

	for(idx=0;idx<num_devices;++idx) {
		if((int)devices[idx].id == id) {
			return &(devices[idx]);
		}
	}
	return NULL;
}
