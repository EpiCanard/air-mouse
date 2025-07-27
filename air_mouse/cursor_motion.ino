#include "Adafruit_TinyUSB.h"
#include <math.h>

// --- Curseur ---
float vx = 0, vy = 0;
float old_vx = 0, old_vy = 0;

// --- Paramètres de mapping ---
const float GAIN = 0.08;     // Gain global
const float EXPONENT = 1.5;  // Courbure non-linéaire
const float DEADZONE = 1.5;  // Zone morte en deg/s
const float ALPHA = 0.2;     // Filtre passe-bas

void setup_cursor_motion(Adafruit_USBD_HID usb_hid) {
  /*
#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  // Manual begin() is required on core without built-in support for TinyUSB such as mbed rp2040
  TinyUSB_Device_Init(0);
#endif


  // Notes: following commented-out functions has no affect on ESP32
  usb_hid.setBootProtocol(HID_ITF_PROTOCOL_MOUSE);
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.setStringDescriptor("TinyUSB Mouse");

  usb_hid.begin();

  // wait until device mounted
  while (!TinyUSBDevice.mounted()) delay(1)
  */

}

void handle_cursor_motion_v2(BLEHidAdafruit ble_hid, LSM6DS3 myIMU) {

  // Move cursor
  // In our case, the XIAO is turn from 90° to the right so we use Z instead of X for horizontal movement
  float gyroX = myIMU.readFloatGyroX(); // pitch
  float gyroY = myIMU.readFloatGyroY() * -1; // yaw
  // float gyroX = myIMU.readFloatAccelX(); // pitch
  // float gyroY = myIMU.readFloatAccelY() * -1; // yaw

  // Yone morte
  if (abs(gyroX) < DEADZONE) gyroX = 0;
  if (abs(gyroY) < DEADZONE) gyroY = 0;

  // Mapping non-linéaire - We use Y axis for horizontal (x) and X axis for vertical (y)
  float raw_vx = GAIN * copysign(pow(abs(gyroY), EXPONENT), gyroY);
  float raw_vy = GAIN * copysign(pow(abs(gyroX), EXPONENT), gyroX);

  // Filtre passe-bas
  vx = ALPHA * raw_vx + (1 - ALPHA) * old_vx;
  vy = ALPHA * raw_vy + (1 - ALPHA) * old_vy;

  old_vx = vx;
  old_vy = vy;

  ble_hid.mouseMove(vx, vy);  // right + down
  ble_hid.mouseButtonRelease();
}

