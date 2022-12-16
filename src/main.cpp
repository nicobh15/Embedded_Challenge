#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include "drivers/LCD_DISCO_F429ZI.h"
#include <imumaths.h>
#include "mbed.h"
#include <math.h> 
#define BNO055_SAMPLERATE_DELAY_MS (200)
#include "drivers/LCD_DISCO_F429ZI.h"
#define BACKGROUND 1
#define FOREGROUND 0
#define GRAPH_PADDING 5

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (200)

// Display
LCD_DISCO_F429ZI lcd;
char display_buf[5][60];

// I2C(SDA, SCL);
I2C i2c(PB_11, PB_10);
Adafruit_BNO055 bno = Adafruit_BNO055(-1, BNO055_ADDRESS_A, &i2c);

DigitalOut LED_BOARD_1(PG_13);
DigitalOut LED_BOARD_2(PG_14);
DigitalIn button(PA_0); // Configure P1_14 pin as input

Timer t;
Ticker t2;
float oldReadTotal;
float oldReadX;
float oldReadY;
float oldReadZ;
float currX;
float currY;
float currZ;

bool first;
int buff;
int alertCounter;
int printStuff = 0;
void loop();

void alert(){
  bool read_button = 0;
  lcd.Clear(LCD_COLOR_RED);
  lcd.SelectLayer(FOREGROUND);
  lcd.SetBackColor(LCD_COLOR_RED);
  snprintf(display_buf[3],60,"WARNING!!!");

  for(int i=2;i<=17;i++){
  lcd.DisplayStringAt(0, LINE(i), (uint8_t *)display_buf[3], CENTER_MODE);
  }
  snprintf(display_buf[3],60,"HOLD BUTTON TO RESET!");
  lcd.DisplayStringAt(0, LINE(18), (uint8_t *)display_buf[3], CENTER_MODE);

  while(!read_button){
    printf("Alert! Hold Button to reset!\n");
    LED_BOARD_1 = 1;
    thread_sleep_for(100);
    LED_BOARD_1 = 0;
    LED_BOARD_2 = 1;
    thread_sleep_for(100);
    LED_BOARD_1 = 1;
    //

    read_button = button.read();
    thread_sleep_for(250);
    LED_BOARD_1 = 0;
    LED_BOARD_2 = 0;
  }
  lcd.Clear(LCD_COLOR_WHITE);
  lcd.SelectLayer(FOREGROUND);
  lcd.SetBackColor(LCD_COLOR_WHITE);
}

void processor(){
  oldReadTotal = oldReadX+oldReadY+oldReadZ;
  if(!(abs(oldReadTotal - currY-currX-currZ) < 0.55) && first){
      buff++;
    }
  if(abs(oldReadTotal - currY-currX-currZ) < 0.55 || (buff != 0 && buff < 3))
  {
    if(alertCounter == 0){
      first = 1;
    } 
    alertCounter++;

    if(alertCounter == 10)
    {
      printStuff = 1;
      alertCounter = 0;

    }
  }
  else
  {
    first = 0;
    buff = 0;
    alertCounter = 0;
    printStuff = 0;
  }
  oldReadX = currX;
  oldReadY = currY;
  oldReadZ = currZ;
}




int main()
{
  first = 0;
  bool read_button = 0;

  while(!read_button){
    snprintf(display_buf[3],60,"Hold Button to begin");
    lcd.DisplayStringAt(0, LINE(10), (uint8_t *)display_buf[3], CENTER_MODE);
    printf("Press Button to begin.\n");
    read_button = button.read();
    thread_sleep_for(250);
  }
  lcd.Clear(LCD_COLOR_WHITE);
  oldReadTotal = 0;
  oldReadX = 0;
  oldReadY = 0;
  oldReadZ = 0;
  currX = 0;
  currY = 0;
  currZ = 0;
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
  // int8_t temp = bno.getTemp();
  // printf("Current Temperature: %d C\r\n", temp);
  bno.setExtCrystalUse(true);

  // printf("Calibration status values: 0=uncalibrated, 3=fully calibrated\r\n");
  
  while(true){
      loop();
  }
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
  currX = euler.x();
  currY = euler.y();
  currZ = euler.z();
  snprintf(display_buf[0],60,"X: %f", currX);
  snprintf(display_buf[1],60,"Y: %f", currY);
  snprintf(display_buf[2],60,"Z: %f", currZ);
  lcd.DisplayStringAt(0, LINE(14), (uint8_t *)display_buf[0], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(15), (uint8_t *)display_buf[1], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(16), (uint8_t *)display_buf[2], LEFT_MODE);
  // printf("Counter: %d\n", alertCounter);
  // printf("oldRead: %d\n", oldRead);
  // printf("Sensor X: %f\n", currX);
  // printf("Sensor Y: %f\n", currY);
  // printf("Sensor Z: %f\n", currZ);
  printf("\t Diff: %f\n",oldReadTotal-currX-currY-currZ);
  printf("Buff: %d\t Alert Counter: %d\n", buff,alertCounter);
  if(printStuff == 1)
  {
    printf("##############  ALERT  ##################\n");
    printf("\tTotal Count of alertCounter = %d\n", alertCounter);
    printf("\tcurrY observed: %f\n", currY);
    printf("%f\n", oldReadTotal);
    printf("\n");
    printStuff =0;
    t2.detach();
    alert();
    t2.attach(&processor, 1s);
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
  // uint8_t system, gyro, accel, mag = 0;
  // bno.getCalibration(&system, &gyro, &accel, &mag);
  // printf("CALIBRATION: Sys=%d, Gyro=%d, Accel=%d, Mag=%d\r\n", (int)(system), (int)(gyro), (int)(accel), (int)(mag));
  thread_sleep_for(BNO055_SAMPLERATE_DELAY_MS);
}