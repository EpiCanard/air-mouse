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
#include <math.h>
#include "hid_boot_mouse.h"

//Create a instance of class LSM6DS3
LSM6DS3 myIMU(I2C_MODE, 0x6A);  //I2C device address 0x6A

const int pin = 9;


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
#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  // Manual begin() is required on core without built-in support for TinyUSB such as mbed rp2040
  TinyUSB_Device_Init(0);
#endif

  // Set up button, pullup opposite to active state
  pinMode(pin, INPUT_PULLUP);

  // Notes: following commented-out functions has no affect on ESP32
  // usb_hid.setBootProtocol(HID_ITF_PROTOCOL_MOUSE);
  // usb_hid.setPollInterval(2);
  // usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  // usb_hid.setStringDescriptor("TinyUSB Mouse");

  usb_hid.begin();

  Serial.begin(115200);

  // wait until device mounted
  while (!TinyUSBDevice.mounted()) delay(1);

  //Call .begin() to configure the IMUs
  if (myIMU.begin() != 0) {
    Serial.println("Device error");
  } else {
    Serial.println("Device OK!");
  }
}

void loop() {
  // poll gpio once each 10 ms
  delay(10);


  // Whether button is pressed
  //bool btn_pressed = !digitalRead(pin);

  // Remote wakeup
  if (TinyUSBDevice.suspended()) {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    TinyUSBDevice.remoteWakeup();
  }

  if (!usb_hid.ready())
    return;

  uint8_t const report_id = 0;



  char click = handle_tap_dance(pin);
  switch (click) {
    case SINGLE_CLICK:
      usb_hid.mouseButtonPress(report_id, MOUSE_BUTTON_LEFT);
      Serial.println("SINGLE_CLICK");
      break;
    case DOUBLE_CLICK:
      usb_hid.mouseButtonPress(report_id, MOUSE_BUTTON_RIGHT);
      Serial.println("DOUBLE_CLICK");
      //usb_hid.mouseButtonPress(0, MOUSE_BUTTON_RIGHT);
      break;
    case NO_CLICK:
      // Serial.println("NO_CLICK");
      break;
  }

  // Press Button
  //if (btn_pressed)
    //usb_hid.mouseButtonPress(0, MOUSE_BUTTON_LEFT);

  // Move cursor
  float x = myIMU.readFloatAccelX() * 10;
  float y = myIMU.readFloatAccelY() * 10;

  usb_hid.mouseMove(report_id, x * abs(x), y * abs(y));  // right + down
  // Serial.print("X =");
  // Serial.print((int8_t)x * abs(x));
  // Serial.print(" Y =");
  // Serial.println((int8_t)y * abs(y));
}
