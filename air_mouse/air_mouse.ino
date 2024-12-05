/*********************************************************************
 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 Copyright (c) 2019 Ha Thach for Adafruit Industries
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

#include "Adafruit_TinyUSB.h"
#include "LSM6DS3.h"
#include "Wire.h"
#include "air_mouse.h"

//Create a instance of class LSM6DS3
LSM6DS3 myIMU(I2C_MODE, 0x6A);  //I2C device address 0x6A

const int pin = 9;
duck_mode_t mode = MOTION;


// HID report descriptor using TinyUSB's template
// Single Report (no ID) descriptor
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_MOUSE()
};

// USB HID object. For ESP32 these values cannot be changed after this declaration
// desc report, desc len, protocol, interval, use out endpoint
Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_MOUSE, 2, false);

// the setup function runs once when you press reset or power the board
void setup() {
  setup_cursor_motion(usb_hid);

  Serial.begin(9600);

  // Set up button, pullup opposite to active state
  pinMode(pin, INPUT_PULLUP);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  myIMU.settings.accelSampleRate = 416;
  myIMU.settings.accelRange = 2;
  myIMU.begin();
  setup_touch_tap(myIMU);
  setup_gesture();
}

void loop() {
  // poll gpio once each 10 ms
  delay(15);

  button_tap_t click = handle_tap_dance(pin);

  if (click == DOUBLE_CLICK) {
    Serial.println("DOUBLE");
    mode = (mode == MOTION) ? GESTURE : MOTION;
    if (mode == MOTION) {
      digitalWrite(LED_BLUE, LOW);
      digitalWrite(LED_RED, HIGH);
    } else {
      digitalWrite(LED_BLUE, HIGH);
      digitalWrite(LED_RED, LOW);
    }
  }
  if (click == SINGLE_CLICK) {
    Serial.println("SINGLE");
  }

  if (mode == MOTION) {

    touch_tap_t touch_tap = handle_touch_tap(myIMU);

    if (touch_tap == SINGLE_TAP) {
      Serial.println("SINGLE");
      usb_hid.mouseButtonPress(0, MOUSE_BUTTON_LEFT);
      usb_hid.mouseButtonRelease(0);
    }
    if (touch_tap == DOUBLE_TAP) {
      Serial.println("DOUBLE");
      usb_hid.mouseButtonPress(0, MOUSE_BUTTON_RIGHT);
      usb_hid.mouseButtonRelease(0);
    }

    bool btn_pressed = !digitalRead(pin);
    if (btn_pressed)
      handle_cursor_motion(usb_hid, myIMU);
  }

  if (mode == GESTURE) {
    const char* motion = handle_gesture(myIMU);
    if (motion == "left") {
      usb_hid.mouseScroll(0, 5, 0);
    }
    if (motion == "right") {
      usb_hid.mouseScroll(0, -5, 0);
    }
    Serial.println(motion);
  }
}
