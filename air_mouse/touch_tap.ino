#include "LSM6DS3.h"

void setup_touch_tap(LSM6DS3 myIMU) {
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_TAP_CFG1, 0x88);     // 1000 1110 - 8E (88)
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_TAP_THS_6D, 0x89);   // 1000 1100 - 8C (= 12 * FS_XL / 2⁵) - FS_XL = 2, 2⁵ = 32 => 0.75
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_INT_DUR2, 0x77);     // 0111 1111 - 7F
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_WAKE_UP_THS, 0x80);  // 1000 0000
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_MD1_CFG, 0x08);      // 0000 1000
}

touch_tap_t handle_touch_tap(LSM6DS3 myIMU) {
  uint8_t temp;

  myIMU.readRegister(&temp, LSM6DS3_ACC_GYRO_TAP_SRC);
  
  if ((temp & LSM6DS3_ACC_GYRO_DOUBLE_TAP_EV_STATUS_DETECTED) != 0)
    return DOUBLE_TAP;
  if ((temp & LSM6DS3_ACC_GYRO_SINGLE_TAP_EV_STATUS_DETECTED) != 0)
    return SINGLE_TAP;
  return NO_TAP;
}