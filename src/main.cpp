#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <imumaths.h>
#include "mbed.h"
#include <math.h> 

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (200)

// Serial pc(USBTX, USBRX);
I2C i2c(PF_0, PF_1);
Adafruit_BNO055 bno = Adafruit_BNO055(-1, BNO055_ADDRESS_A, &i2c);

Timer t;
Ticker t2;
float oldRead;
float currY;
int alertCounter;
int printStuff = 0;
void loop();

void processor(){
  if(abs(oldRead - currY) < 0.8)
  {
    alertCounter++;
    if(alertCounter == 10)
    {
      printStuff = 1;
      alertCounter = 0;
    }
  }
  else
  {
    alertCounter = 0;
    printStuff = 0;
  }
  oldRead = currY;
}

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
int main()
{
  oldRead =0;
  currY = 0;
  alertCounter = 0;
  // pc.baud(9600);
  t2.attach(&processor, 1s);
  printf("Orientation Sensor Raw Data Test\r\n");
  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    printf("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!\r\n");
    while(1);
  }
  else
    printf("BNO055 was detected!\r\n");

  thread_sleep_for(250);

  /* Display the current temperature */
  int8_t temp = bno.getTemp();
  printf("Current Temperature: %d C\r\n", temp);
  bno.setExtCrystalUse(true);

  printf("Calibration status values: 0=uncalibrated, 3=fully calibrated\r\n");
  
  while(true)
      loop();
}
/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
void loop()
{

  // Possible vector values can be:
  // - VECTOR_ACCELEROMETER - m/s^2
  // - VECTOR_MAGNETOMETER  - uT
  // - VECTOR_GYROSCOPE     - rad/s
  // - VECTOR_EULER         - degrees
  // - VECTOR_LINEARACCEL   - m/s^2
  // - VECTOR_GRAVITY       - m/s^2
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  currY = euler.y();
  // printf("Counter: %d\n", alertCounter);
  // printf("oldRead: %d\n", oldRead);
  printf("Sensor: %f\n", currY);
  if(printStuff == 1)
  {
    printf("##############  ALERT  ##################\n");
    printf("\tTotal Count of alertCounter = %d\n", alertCounter);
    printf("\tcurrY observed: %f\n", currY);
    printf("\tOldRead observed: %f\n", oldRead);
    printf("\n");
    printStuff =0;
  }

  // currY = euler.y();
  /* Display the floating point data */
  // printf("X: %f Y: %f Z: %f\t\t", euler.x(), euler.y(), euler.z());

  /*
  // Quaternion data
  imu::Quaternion quat = bno.getQuat();
  Serial.print("qW: ");
  Serial.print(quat.w(), 4);
  Serial.print(" qX: ");
  Serial.print(quat.y(), 4);
  Serial.print(" qY: ");
  Serial.print(quat.x(), 4);
  Serial.print(" qZ: ");
  Serial.print(quat.z(), 4);
  Serial.print("\t\t");
  */

  /* Display calibration status for each sensor. */
  uint8_t system, gyro, accel, mag = 0;
  // bno.getCalibration(&system, &gyro, &accel, &mag);
  // printf("CALIBRATION: Sys=%d, Gyro=%d, Accel=%d, Mag=%d\r\n", (int)(system), (int)(gyro), (int)(accel), (int)(mag));
  thread_sleep_for(BNO055_SAMPLERATE_DELAY_MS);
}