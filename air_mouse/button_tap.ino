
#include "air_mouse.h"

uint8_t tap_count = 0;
uint32_t previousPressMillis = 0;
uint32_t previousReleaseMillis = 0;
uint16_t delay_click = 200;      // Delay between press and release to be consider as a click or long press
uint16_t delay_dbl_click = 200;  // Delay between two click to be considère as double click


button_tap_t handle_tap_dance(uint32_t btn_pin) {
  uint32_t readPin = digitalRead(btn_pin);
  uint32_t currentMillis = millis();
  if (readPin == LOW) {
    Serial.println("LOW");
    if (previousPressMillis == 0)
      previousPressMillis = millis();
  }

  if (readPin == HIGH && previousPressMillis != 0) {
    Serial.println("HIGH");
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
}