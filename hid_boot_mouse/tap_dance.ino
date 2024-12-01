
#include "hid_boot_mouse.h"

uint8_t tap_count = 0;
uint32_t previousPressMillis = 0;
uint32_t previousReleaseMillis = 0;
uint16_t delay_click = 200;       // Delay between press and release to be consider as a click or long press
uint16_t delay_dbl_click = 200;  // Delay between two click to be considère as double click


char handle_tap_dance(uint32_t btn_pin) {
  uint32_t readPin = digitalRead(btn_pin);
    uint32_t currentMillis = millis();
  if (readPin == LOW) {
    if (previousPressMillis == 0)
      previousPressMillis = millis();
    else if (currentMillis - previousPressMillis > delay_click)
      return SINGLE_CLICK;
    return NO_CLICK;
  }

  if (readPin == HIGH && previousPressMillis != 0) {
    if (currentMillis - previousPressMillis <= delay_click) {
      previousPressMillis = 0;
      if (tap_count == 1) {
        tap_count = 0;
        previousReleaseMillis = 0;
        return DOUBLE_CLICK;
      }
      tap_count = 1;
      previousReleaseMillis = currentMillis;
      previousPressMillis = 0;
    } else {
      previousPressMillis = 0;
      tap_count = 0;
      return SINGLE_CLICK;
    }
    return NO_CLICK;
  }
  if (previousReleaseMillis != 0 && currentMillis - previousReleaseMillis >= delay_dbl_click) {
    tap_count = 0;
    previousReleaseMillis = 0;
    return SINGLE_CLICK;
  }
  return NO_CLICK;
  /*
  if (digitalRead(btn_pin) == LOW) {
  if (digitalRead(btn_pin)) {
    uint32_t currentMillis = millis();
    if (currentMillis - previousMillis >= 200 && !digitalRead(btn_pin)) {
      //oneclick

      previousMillis = currentMillis;
      bool firstClick = true;

    } else if (currentMillis - previousMillis >= 200 && digitalRead(btn_pin) && firstclick) {

      //DoubleClick

      previousMillis = currentMillis;
      firstClick = false;

    } else if (currentMillis - previousMillis >= 2000 && digitalRead(btn_pin)) {
      //long click (hold for 2000 ms or 2s)
      previousMillis = currentMillis;
    }

    firstClick = false;
  }
  */
}