#ifndef __AIR_MOUSE_H__
#define __AIR_MOUSE_H__

typedef enum {
  NO_CLICK,
  SINGLE_CLICK,
  DOUBLE_CLICK
} button_tap_t;

typedef enum {
  NO_TAP,
  SINGLE_TAP,
  DOUBLE_TAP
} touch_tap_t;

typedef enum {
  OFF,
  MOTION,
  GESTURE
} duck_mode_t;

// array to map gesture index to a name
const char* GESTURES[] = {
  "left",
  "right",
  "return"
};

#define NUM_GESTURES (sizeof(GESTURES) / sizeof(GESTURES[0]))


#endif