#include <stdio.h>
#include "capture.h"

#include <time.h>
#include <math.h>

typedef struct pos_t {
	unsigned int x, y;
} pos_t;

void log_events(Display *);
double distance(pos_t, pos_t);
unsigned int abs_diff(int, int);

int main(int argc, char *argv[])
{
	Display *display;
	int idx;

	devices = NULL;
	num_devices = 0;

	display = XOpenDisplay(NULL);
	if(display == NULL) {
		fprintf(stderr, "Could not connect to X server\n");
		return -1;
	}

	if(argc == 1) {
		list_devices(display);
		XCloseDisplay(display);
		return 0;
	}

	/* Display list of available devices */
	for(idx=1;idx<argc;++idx) {
		if(!strncmp("list", argv[idx], 4)) {
			list_devices(display);
			XCloseDisplay(display);
			return 0;
		}
	}

	/* Connect to multiple device */
	capture(display, argc-1, &argv[1]);
	log_events(display);

	XCloseDisplay(display);
	XFreeDeviceList(devices);
	return 0;
}

void log_events(Display *display)
{
	XEvent event;
	pos_t pos, cur = {0, 0};
	unsigned char mouse_release = 0;
	unsigned char keyboard_event = 0;

	printf("Capturing user input\n");
	fflush(stdout);

	while(1) {
		XNextEvent(display, &event);

		if(event.type == motion_type) {
			XDeviceMotionEvent *m = (XDeviceMotionEvent *)&event;
			pos.x = m->axis_data[0];
			pos.y = m->axis_data[1];
			if(mouse_release) {
				double dis = distance(cur, pos);
				/* Hide some of the output
				   Ignore any movements less than 2 pixels
				*/
				if(dis > 2.0F) {
					printf("(%ld) Mouse event: %lf\n", time(NULL), dis);
					fflush(stdout);
					cur.x = pos.x;
					cur.y = pos.y;
				}
				mouse_release = 0;
			}
		}
		else if(event.type == button_press_type) {
			/* Work out mouse travel */
			printf("(%ld) Mouse event: %lf\n", time(NULL), distance(cur, pos));
			fflush(stdout);
			cur.x = pos.x;
			cur.y = pos.y;
			keyboard_event = 0;
		}
		else if(event.type == button_release_type) {
			mouse_release = 1;
			keyboard_event = 0;
			/*XDeviceButtonEvent *b = (XDeviceButtonEvent *)&event;*/
		}
		else if((event.type == key_press_type)
		        ||
		        (event.type == key_release_type)) {
			/*XDeviceKeyEvent *k = (XDeviceKeyEvent *)&event;*/
			if(!keyboard_event) {
				printf("(%ld) Keyboard event\n", time(NULL));
				fflush(stdout);
				keyboard_event = 1;
			}
		}
		else {
			/*printf("Unknown\n");*/
		}
	}
	return;
}

double distance(pos_t a, pos_t b)
{
	return sqrt(pow(abs_diff(a.x, b.x), 2) + pow(abs_diff(a.y, b.y), 2));
}

unsigned int abs_diff(int a, int b)
{
	return (a > b) ? (a - b) : (b - a);
}
