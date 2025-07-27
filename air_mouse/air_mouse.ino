/*********************************************************************
 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 Copyright (c) 2019 Ha Thach for Adafruit Industries
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

#include "LSM6DS3.h"
#include "Wire.h"
#include "air_mouse.h"
#include <bluefruit.h>

BLEDis ble_dis;
BLEHidAdafruit ble_hid;

//Create a instance of class LSM6DS3
LSM6DS3 myIMU(I2C_MODE, 0x6A);  //I2C device address 0x6A

const int left_pin = 10;
const int right_pin = 8;
duck_mode_t mode = OFF;
bool left_pressed = false;
bool right_pressed = false;



// USB HID object. For ESP32 these values cannot be changed after this declaration
// desc report, desc len, protocol, interval, use out endpoint
// Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_MOUSE, 2, false);

// the setup function runs once when you press reset or power the board
void setup() {
  //setup_cursor_motion(usb_hid);

  Serial.begin(115200);

  // Set up button, pullup opposite to active state
  pinMode(left_pin, INPUT_PULLUP);
  pinMode(right_pin, INPUT_PULLUP);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  myIMU.settings.accelSampleRate = 416;
  myIMU.settings.accelRange = 2;
  myIMU.begin();
  setup_bluetooth();
  setup_touch_tap(myIMU);
  setup_gesture();
}



void setup_bluetooth() {
  Bluefruit.begin();
  // HID Device can have a min connection interval of 9*1.25 = 11.25 ms
  Bluefruit.Periph.setConnInterval(9, 16);  // min = 9*1.25=11.25 ms, max = 16*1.25=20ms
  Bluefruit.setTxPower(4);                  // Check bluefruit.h for supported values

  // Configure and Start Device Information Service
  ble_dis.setManufacturer("Adafruit Industries");
  ble_dis.setModel("Bluefruit Feather 52");
  ble_dis.begin();
  Bluefruit.setName("DuckMouse");

  // BLE HID
  ble_hid.begin();


  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_MOUSE);

  // Include BLE HID service
  Bluefruit.Advertising.addService(ble_hid);
  Bluefruit.ScanResponse.addName();

  // There is enough room for 'Name' in the advertising packet
  Bluefruit.Advertising.addName();
  Bluefruit.Scanner.setInterval(160, 160);

  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   *
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);  // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);    // number of seconds in fast mode
  Bluefruit.Advertising.start(0);              // 0 = Don't stop advertising after n seconds
}


void loop() {
  // poll gpio once each 10 ms
  delay(10);
  if (!Bluefruit.connected()) {
    return;
  }


  //button_tap_t click = handle_tap_dance(pin);

  /*

  */
  touch_tap_t touch_tap = handle_touch_tap(myIMU);

  if (touch_tap == DOUBLE_TAP) {
    Serial.println("DOUBLE TAP");
    if (mode == OFF) {
      mode = MOTION;
      digitalWrite(LED_BLUE, LOW);
    } else {
      if (mode == MOTION) {
        mode = OFF;
        digitalWrite(LED_BLUE, HIGH);
      }
    }
  }
  if (mode == MOTION) {



    uint32_t left_read_pin = digitalRead(left_pin);
    if (left_read_pin == LOW && !left_pressed) {
      ble_hid.mouseButtonPress(MOUSE_BUTTON_LEFT);
      left_pressed = true;
    }
    if (left_read_pin == HIGH) {
      ble_hid.mouseButtonRelease();
      left_pressed = false;
    }
    uint32_t right_read_pin = digitalRead(right_pin);
    if (right_read_pin == LOW && !right_pressed) {
      ble_hid.mouseButtonPress(MOUSE_BUTTON_RIGHT);
      right_pressed = true;
    }
    if (right_read_pin == HIGH) {
      ble_hid.mouseButtonRelease();
      right_pressed = false;
    }
    /*
    if (click == SINGLE_CLICK) {
      Serial.println("SINGLE");
      ble_hid.mouseButtonPress(MOUSE_BUTTON_LEFT);
      ble_hid.mouseButtonRelease();
    }
    if (click == DOUBLE_CLICK) {
      Serial.println("DOUBLE CLICK");
      ble_hid.mouseButtonPress(MOUSE_BUTTON_RIGHT);
      ble_hid.mouseButtonRelease();
    }
    */

    handle_cursor_motion_v2(ble_hid, myIMU);
  }

  if (mode == GESTURE) {
    const char* motion = handle_gesture(myIMU);
    if (motion == "left") {
      ble_hid.mouseScroll(5, 0);
      ble_hid.mouseButtonRelease();
    }
    if (motion == "right") {
      ble_hid.mouseScroll(-5, 0);
      ble_hid.mouseButtonRelease();
    }
    Serial.println(motion);
  }
}
