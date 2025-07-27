#include "LSM6DS3.h"

void setup_touch_tap(LSM6DS3 myIMU) {
  
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_TAP_CFG1, 0x88);     // 1000 1110 - 8E (88)
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_TAP_THS_6D, 0x89);   // 1000 1100 - 8C (= 12 * FS_XL / 2⁵) - FS_XL = 2, 2⁵ = 32 => 0.75
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_INT_DUR2, 0x77);     // 0111 1111 - 7F
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_WAKE_UP_THS, 0x80);  // 1000 0000
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_MD1_CFG, 0x08);      // 0000 1000
  

/*
  // Setup robust single + double tap sur X,Y,Z
  //myIMU.writeRegister(LSM6DS3_ACC_GYRO_TAP_CFG1, 0x0E);     // Tap X,Y,Z
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_TAP_CFG1, 0x88);     // Tap Z only
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_TAP_THS_6D, 0x03);   // Seuil ≈ modéré
  //myIMU.writeRegister(LSM6DS3_ACC_GYRO_TAP_THS_6D, 0x89);   // Seuil ≈ sensible
  //myIMU.writeRegister(LSM6DS3_ACC_GYRO_INT_DUR2, 0x7F);     // DUR=7, QUIET=1, SHOCK=3
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_INT_DUR2, 0x6F);     // DUR=3, QUIET=1, SHOCK=3
  //myIMU.writeRegister(LSM6DS3_ACC_GYRO_INT_DUR2, 0x77);     // DUR=7, QUIET=1, SHOCK=3
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_WAKE_UP_THS, 0x80);  // Active single/double
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_MD1_CFG, 0x40);      // INT1_TAP
  //myIMU.writeRegister(LSM6DS3_ACC_GYRO_MD1_CFG, 0x08);      // INT1_TAP
*/
}

touch_tap_t handle_touch_tap(LSM6DS3 myIMU) {
  uint8_t temp;

  myIMU.readRegister(&temp, LSM6DS3_ACC_GYRO_TAP_SRC);

  if ((temp & LSM6DS3_ACC_GYRO_TAP_EV_STATUS_DETECTED) != 0) {
    if ((temp & LSM6DS3_ACC_GYRO_DOUBLE_TAP_EV_STATUS_DETECTED) != 0)
      return DOUBLE_TAP;
    if ((temp & LSM6DS3_ACC_GYRO_SINGLE_TAP_EV_STATUS_DETECTED) != 0)
      return SINGLE_TAP;
  }

  return NO_TAP;
}
