#define MPU6500  //  Used instead of #define MPU9250 to #undef AK8963_SECONDARY
#define INTERRUPT_PIN 35   // Choose WOM interrupt pin

#include "SparkFunMPU9250-DMP.h"
MPU9250_DMP imu;
bool imuWoke;    // ISR Woke-on-Motion Flag
extern "C" {
#include "util/inv_mpu.h"
#include "util/inv_mpu_dmp_motion_driver.h"
}

void imuISR() {
  // Set Wake-on-Motion Flag only
  imuWoke = true;
}

bool SetupImu()
{

  if (imu.begin() != INV_SUCCESS)
  {
    while (1)
    {
      Serial.println("Unable to communicate with MPU-9250");
      Serial.println("Check connections, and try again.");
      Serial.println();
      delay(5000);
    }
  }

  // Power Down Gyro and Compass
  imu.setSensors(INV_XYZ_ACCEL);
  // The interrupt level can either be active-high or low. Configure as active-low.
  // Options are INT_ACTIVE_LOW or INT_ACTIVE_HIGH
  imu.setIntLevel(INT_ACTIVE_LOW);
  // disable Auxiliary I2C Master I/F
  mpu_set_bypass(0);
  // Enabe Wake On Motion low power mode with a threshold of 40 mg and
  // an accelerometer data rate of 15.63 Hz. 
  // Only accelerometer is enabled in LP mode
  // The interrupt is 50us pulse.
  if (mpu_lp_motion_interrupt(40,0,2) != INV_SUCCESS) {    
    // Failed to initialize MPU-9250, report somehow
    Serial.println(F("IMU set up failed. Please check installed IMU IC."));    
  }
  // Attach Interupt Service Routine to be called on device motion (to check angles for example)
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), imuISR, FALLING);
  //detachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN)); 
  imuWoke = false;
  return true;
}