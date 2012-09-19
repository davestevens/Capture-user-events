#Tool to log user interactions with computer.#

Tool to output mouse and keyboard events in X11. Used for logging user interaction with computer.

##Usage##

###Arguments###
`-o <file>`
   - defines output to be written to `<file>`

`list`
   - lists available X11 devices to log

`device_id0, device_id1, ...`
   - list of X11 device ids to log

###Example###
```
./CAPTURE list
```
Runs program and outputs a list of avaiable X11 device ids and names

```
./CAPTURE 10 11 -o output_file.csv
```
Captures device id 10 and 11 and prints output to 'output_file.csv'

##Modify##
Modify log_events() function in src/main.c to define the required output. Currently the output events are specified as:
- Keyboard
  - Any interaction with the keyboard between mouse interaction
  - A single event it generated
- Mouse
  - Any mouse button (press and release)
  - Press and Release counted as single event
- Move
  - Movement in mouse between release and press
  - Calculates distance (in pixels) between last mouse event
- Drag
  - Movement in mouse between press and release
  - Calculates distance (in pixels) between last mouse event

Creates a csv formated output file:
```
timestamp, event, notes
```
*Notes include relative mouse movements between mouse events*

##Compile##
```
Make
```

##Dependencies##
Requires X11, including packages:
- libx11-dev
- libxi-dev
