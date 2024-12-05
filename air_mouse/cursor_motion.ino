#include "Adafruit_TinyUSB.h"
#include <math.h>

void setup_cursor_motion(Adafruit_USBD_HID usb_hid) {
#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  // Manual begin() is required on core without built-in support for TinyUSB such as mbed rp2040
  TinyUSB_Device_Init(0);
#endif


  // Notes: following commented-out functions has no affect on ESP32
  // usb_hid.setBootProtocol(HID_ITF_PROTOCOL_MOUSE);
  // usb_hid.setPollInterval(2);
  // usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  // usb_hid.setStringDescriptor("TinyUSB Mouse");

  usb_hid.begin();

  // wait until device mounted
  while (!TinyUSBDevice.mounted()) delay(1);
  
}

void handle_cursor_motion(Adafruit_USBD_HID usb_hid, LSM6DS3 myIMU) {

  // Remote wakeup
  if (TinyUSBDevice.suspended()) {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    TinyUSBDevice.remoteWakeup();
  }

  if (!usb_hid.ready())
    return;

  // Move cursor
  float x = myIMU.readFloatAccelX() * 10;
  float y = myIMU.readFloatAccelY() * 10;

  usb_hid.mouseMove(0, x * abs(x), y * abs(y));  // right + down
}