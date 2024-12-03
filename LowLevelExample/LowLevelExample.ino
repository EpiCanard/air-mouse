/*****************************************************************************/
//  LowLevelExample.ino
//  Hardware:      Grove - 6-Axis Accelerometer&Gyroscope
//	Arduino IDE:   Arduino-1.65
//	Author:	       Lambor
//	Date: 	       Oct,2015
//	Version:       v1.0
//
//  Modified by:
//  Data:
//  Description:
//
//	by www.seeedstudio.com
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
/*******************************************************************************/

#include "LSM6DS3.h"
#include "Wire.h"
#include "SPI.h"

uint16_t errorsAndWarnings = 0;

//Create instance of LSM6DS3Core
LSM6DS3Core myIMU(I2C_MODE, 0x6A);  //I2C device address 0x6A

void setup() {
  //Init Serial port
  Serial.begin(9600);
  while (!Serial)
    ;

    

  uint8_t dataToWrite = 0;  //Temporary variable
  //Call .beginCore() to configure the IMU
  if (myIMU.beginCore() != 0) {
    Serial.print("\nDevice Error.\n");
  } else {
    Serial.print("\nDevice OK.\n");
  }
  //Setup the accelerometer******************************
  /*
    dataToWrite = 0; //Start Fresh!
    dataToWrite |= LSM6DS3_ACC_GYRO_TIMER_EN_ENABLED;
    dataToWrite |= LSM6DS3_ACC_GYRO_TAP_X_EN_ENABLED; 
    dataToWrite |= LSM6DS3_ACC_GYRO_TAP_Y_EN_ENABLED;
    dataToWrite |= LSM6DS3_ACC_GYRO_TAP_Z_EN_ENABLED;
    */
  // 30ms fréquencek
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, 0x60);    // 0110 0000 - enable accelerometer - ODR[4] - FS[2] - 00
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_TAP_CFG1, 0x88);    // 1000 1110 - 8E
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_TAP_THS_6D, 0x89);  // 1000 1100 -  (= 12 * FS_XL / 2⁵) 8C - FS_XL = 2, 2⁵ = 32 => 0.75
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_INT_DUR2, 0x77);    // 0111 1111 - 7F
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_WAKE_UP_THS, 0x80); // 1000 0000
  myIMU.writeRegister(LSM6DS3_ACC_GYRO_MD1_CFG, 0x08);     // 0000 1000

// For interrupt generation, after the first tap recognition there must not be any slope data overthreshold during the
// Quiet time window. Furthermore, since latched mode is disabled, the interrupt is kept high for the duration of the
// Quiet window. The QUIET field of the INT_DUR2 register is set to 11b, therefore the Quiet time is 28.8 ms (= 3 *
// 4 / ODR_XL).
// For the maximum time between two consecutive detected taps, the DUR field of the INT_DUR2 register is set to
// 0111b, therefore the Duration time is 538.5 ms (= 7 * 32 / ODR_XL).

}

void loop() {
  uint8_t temp;

  myIMU.readRegister(&temp, LSM6DS3_ACC_GYRO_TAP_SRC);
  if (temp != 0 && (temp & 0x40) != 0) {
    Serial.print(temp, BIN);
    Serial.print(" - ");
    Serial.print((temp & 0x40) != 0 ? "Tap - " : "");
    Serial.print((temp & 0x20) != 0 ? "SingleTap - " : "");
    Serial.print((temp & 0x10) != 0 ? "DoubleTap - " : "");
    Serial.print((temp & 0x08) == 0 ? "Positive - " : "Negative - ");
    Serial.print((temp & 0x04) != 0 ? "X - " : "");
    Serial.print((temp & 0x02) != 0 ? "Y - " : "");
    Serial.print((temp & 0x01) != 0 ? "Z - " : "");
    Serial.println();
  }


  delay(15);
}
