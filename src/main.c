#include <stdio.h>
#include "capture.h"

#include <time.h>
#include <math.h>
#include <signal.h>
#include <stdlib.h>

typedef struct pos_t {
	unsigned int x, y;
} pos_t;

void log_events(Display *, FILE *);
double distance(pos_t *, pos_t *);
void ctrl_c(int);

volatile unsigned char comp;

int main(int argc, char *argv[])
{
	Display *display = NULL;
	XDeviceInfo *devices = NULL;
	FILE *output = stdout;
	int idx;
	unsigned long start, end;
	unsigned int num_capture = 0;
	int *capture_list = NULL;

	comp = 0;
	/* Capture ctrl+c */
	signal(SIGINT, ctrl_c);

	display = XOpenDisplay(NULL);
	if(display == NULL) {
		fprintf(stderr, "Could not connect to X server\n");
		return -1;
	}

	if(argc == 1) {
		list_devices(display, devices);
		XCloseDisplay(display);
		return 0;
	}

	/* Display list of available devices */
	for(idx=1;idx<argc;++idx) {
		if(!strncmp("list", argv[idx], 4)) {
			list_devices(display, devices);
			XCloseDisplay(display);
			return 0;
		}
		else if(!strncmp("-o", argv[idx], 2)) {
			output = fopen(argv[++idx], "w");
			if(output == NULL) {
				fprintf(stderr, "Could not open file (%s)\n", argv[idx]);
				return -1;
			}
		}
		else {
			/* Push to list of devices to capture */
			if(capture_list == NULL) {
				capture_list = (int *)calloc(1, sizeof(int));
			}
			else {
				capture_list = (int *)realloc(capture_list, (sizeof(int) * (num_capture+1)));
			}
			*(capture_list + num_capture++) = atoi(argv[idx]);
		}
	}

	/* Connect to multiple device listed in *capture_list */
	capture(display, devices, num_capture, capture_list);

	start = time(NULL);
	log_events(display, output);
	end = time(NULL);

	fprintf(output, "Overall time: %ld seconds\n", (end - start));

	if(output != stdout) {
		fclose(output);
	}

	/*
	  Clean up
	 */
	free(capture_list);
	XCloseDisplay(display);
	XFreeDeviceList(devices);
	return 0;
}

/*
  Read event list and display events desired
 */
void log_events(Display *display, FILE *output)
{
	XEvent event;
	unsigned char keyboard_event = 0;
	pos_t mouse = {0, 0};
	pos_t mouse_down = {0, 0};

	printf("Capturing user input\n");
	fprintf(output, "time, event, extra\n");
	fflush(output);

	while(!comp) {
		XNextEvent(display, &event);

		if(event.type == motion_type) {
			XDeviceMotionEvent *m = (XDeviceMotionEvent *)&event;
			mouse.x = m->axis_data[0];
			mouse.y = m->axis_data[1];
		}
		else if(event.type == button_press_type) {
			double dis = distance(&mouse_down, &mouse);
			keyboard_event = 0;
			if(dis > 2.0F) {
				fprintf(output, "%ld, move, %lf\n", time(NULL), dis);
				mouse_down.x = mouse.x;
				mouse_down.y = mouse.y;
				fflush(output);
			}
		}
		else if(event.type == button_release_type) {
			/*
			  XDeviceButtonEvent *b = (XDeviceButtonEvent *)&event;
			*/
			double dis = distance(&mouse_down, &mouse);
			fprintf(output, "%ld, mouse, \n", time(NULL));
			keyboard_event = 0;
			if(dis > 2.0F) {
				fprintf(output, "%ld, drag, %lf\n", time(NULL), distance(&mouse_down, &mouse));
				mouse_down.x = mouse.x;
				mouse_down.y = mouse.y;
			}
			fflush(output);
		}
		else if((event.type == key_press_type)
		        ||
		        (event.type == key_release_type)) {
			/*
			  XDeviceKeyEvent *k = (XDeviceKeyEvent *)&event;
			*/
			if(!keyboard_event) {
				fprintf(output, "%ld, keyboard, \n", time(NULL));
				fflush(output);
				keyboard_event = 1;
			}
		}
		/*
		  Ignore other event types
		*/
	}
	printf("/Capturing user input\n");
	return;
}

/*
  Distance between two points
 */
double distance(pos_t *a, pos_t *b)
{
	return sqrt(pow(abs(a->x - b->x), 2) + pow(abs(a->y - b->y), 2));
}

/*
  Capture ctrl+c to gracefully close program down
 */
void ctrl_c(int signal)
{
	UNUSED(signal);
	comp = 1;
	return;
}
