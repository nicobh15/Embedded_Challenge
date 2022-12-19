/* 
* ECE-GY 6483 : Embedded Challenge for Fall 2022.
* Participants: Nico Harris-de La Tour - nh2321
              : Savinay Shukla         - ss16924
              : Aneek Roy              - ar8002
              : Karan Joglekar         - kaj9196

  SENSOR USED : ADAFRUIT BNO055
*/

// Include modules
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include "drivers/LCD_DISCO_F429ZI.h"
#include <imumaths.h>
#include "mbed.h"
#include <math.h>

//Screen Definitions
#define BACKGROUND 1
#define FOREGROUND 0
#define GRAPH_PADDING 5

//Algorithm buffer and threshold, values tuned to a specific individual, may require additional tuning for others
#define MAX_BUFFER 7
#define THRESHOLD 0.65

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (200)


// Display
LCD_DISCO_F429ZI lcd;
char display_buf[5][60];

// I2C(SDA, SCL) maps to Pins B_11 and B_10 
// BNO055_ADDRESS_A = 0x28 as defined in datasheet
I2C i2c(PB_11, PB_10);
Adafruit_BNO055 bno = Adafruit_BNO055(-1, BNO055_ADDRESS_A, &i2c);

// Board LED'S maps to Pins G_13 and G_14
DigitalOut LED_BOARD_1(PG_13);
DigitalOut LED_BOARD_2(PG_14);

// On board button
DigitalIn button(PA_0);


Ticker tick_interrupt;

//Variable declarations
float oldReadTotal;
float oldReadX;
float oldReadY;
float oldReadZ;
float currX;
float currY;
float currZ;
uint8_t buff;
uint8_t alertCounter;
uint8_t trigger_flag;

void loop();

/*
    Alarm function: Displays a warning message on screen, turns screen red, blinks on board LED's 
*/
void alert()
{
  bool read_button = 0;

  //Set board screen red 
  lcd.Clear(LCD_COLOR_RED);
  lcd.SelectLayer(FOREGROUND);
  lcd.SetBackColor(LCD_COLOR_RED);
  snprintf(display_buf[3], 60, "WARNING!!!");

  // Display warning message on multiple lines on screen
  for (int display_index = 2; display_index <= 17; display_index++)
  {
    lcd.DisplayStringAt(0, LINE(display_index), (uint8_t *)display_buf[3], CENTER_MODE);
  }
  //Starting the algorithm after reset
  snprintf(display_buf[3], 60, "HOLD BUTTON TO RESET!");
  lcd.DisplayStringAt(0, LINE(18), (uint8_t *)display_buf[3], CENTER_MODE);

  while (!read_button)
  {
    printf("Alert! Hold Button to reset!\n");
    read_button = button.read();
    //LED Flashing for alert function
    LED_BOARD_1 = 1;
    thread_sleep_for(100);
    LED_BOARD_1 = 0;
    LED_BOARD_2 = 1;
    thread_sleep_for(100);
    LED_BOARD_1 = 1;
    thread_sleep_for(250);
    LED_BOARD_1 = 0;
    LED_BOARD_2 = 0;
  }
  //Reset LCD
  lcd.Clear(LCD_COLOR_WHITE);
  lcd.SelectLayer(FOREGROUND);
  lcd.SetBackColor(LCD_COLOR_WHITE);
}


/* Algorithm logic: readings from each direction are combined into one value.
This value is initialized at 0 and updated at each interrupt.
Interrupt takes place every 500ms.
If the absolute difference between stored value and sum of current readings is below threshold, counter "alertCounter" is initiated.
This counter will tick up until at 7 (MAX BUFFER) readings greater than the threshold are registered.
The buffer of 7 was used to minimize the false positive rate while still maintaining a good success rate on the average time to obtain the reading.
This buffer is only updated on readings larger than the threshold and only if the alert has been tripped at least once.
Once it is confirmed that a non breathing event is not occuring, the buffer, trigger, alert counter, and the flag to track the buffer are reset to 0.
*/
void processor()
{
  oldReadTotal = oldReadX + oldReadY + oldReadZ;    //Fetching old values of X, Y, Z.

  //If the abs diff between oldTotal and new combination of X, Y, Z are below the threshold : 0.65
  // param : THRESHOLD
  if (!(abs(oldReadTotal - currY - currX - currZ) < THRESHOLD) && alertCounter > 0)
  {
    //Increment an outlier buffer count to account for outliers.
    buff++;
  }
  
  //If the prev condtion is true OR our outlier counter is between 0 and 7(MAX BUFFER) not inclusive, then alert counter is incremented
  if (abs(oldReadTotal - currY - currX - currZ) < THRESHOLD || (buff != 0 && buff < MAX_BUFFER))
  {
    alertCounter++;
    if (alertCounter == 20)
    {
      trigger_flag = 1;
      alertCounter = 0;
    }
  }
  else
  {
    buff = 0;
    alertCounter = 0;
    trigger_flag = 0;
  }
  //Update the old readings with the new ones.
  oldReadX = currX;
  oldReadY = currY;
  oldReadZ = currZ;
}

int main()
{
  bool read_button = 0;
  //Initialization requires user input of on board button
  while (!read_button)
  {
    snprintf(display_buf[3], 60, "Hold Button to begin");
    lcd.DisplayStringAt(0, LINE(10), (uint8_t *)display_buf[3], CENTER_MODE);
    printf("Hold Button to begin.\n");
    read_button = button.read();
    thread_sleep_for(250);
  }

  //Variable initialization
  lcd.Clear(LCD_COLOR_WHITE);
  oldReadTotal = 0;
  oldReadX = 0;
  oldReadY = 0;
  oldReadZ = 0;
  currX = 0;
  currY = 0;
  currZ = 0;
  alertCounter = 0;
  trigger_flag = 0;
  
  // Ticker to check difference in acceleration runs every 0.5s
  tick_interrupt.attach(&processor, 500ms);

  //Sensor Initialization
  // printf("Orientation Sensor Raw Data Test\r\n");
  /* Initialise the sensor */
  if (!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    printf("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!\r\n");
    while(1);

  }
  else
    printf("BNO055 was detected!\r\n");

  thread_sleep_for(BNO055_SAMPLERATE_DELAY_MS);
  bno.setExtCrystalUse(true);


  while (true)
  {
    //function to read in sensor data
    loop();
  }
}


// Function to read in sensory data. Read into a vector and stored in global x,y,z variables. X,Y,Z are also displayed on the screen
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

  // Fetching X, Y, and Z data.
  currX = euler.x();
  currY = euler.y();
  currZ = euler.z();

  // Printing X, Y, Z data values on the screen.
  snprintf(display_buf[0], 60, "X: %f", currX);
  snprintf(display_buf[1], 60, "Y: %f", currY);
  snprintf(display_buf[2], 60, "Z: %f", currZ);
  lcd.DisplayStringAt(0, LINE(14), (uint8_t *)display_buf[0], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(15), (uint8_t *)display_buf[1], LEFT_MODE);
  lcd.DisplayStringAt(0, LINE(16), (uint8_t *)display_buf[2], LEFT_MODE);

  // Uncomment for Debugging.
  // printf("\t Diff: %f\n",oldReadTotal-currX-currY-currZ);
  // printf("Buff: %d\t Alert Counter: %d\n", buff,alertCounter);

  // If breathing has stopped for 20 interrupts (10 seconds), enter alarm condition
  if (trigger_flag == 1)
  {
    // Uncomment below code snippets for debugging.
    // printf("##############  ALERT  ##################\n");
    // printf("\tTotal Count of alertCounter = %d\n", alertCounter);
    // printf("\tcurrY observed: %f\n", currY);
    // printf("%f\n", oldReadTotal);
    // printf("\n");
    trigger_flag = 0;

    //Ticker detach to avoid interrupting alarm
    tick_interrupt.detach();
    // Alarm function. Will not halt until user manually interrupts
    alert();
    //Ticker reattach once user has cleared error
    tick_interrupt.attach(&processor, 500ms);
  }

  thread_sleep_for(BNO055_SAMPLERATE_DELAY_MS);
}