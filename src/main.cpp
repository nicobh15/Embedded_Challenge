// #include <mbed.h>
// #include <defines.h>
// //this file has all the functions for interacting
// //with the screen
// #include "drivers/LCD_DISCO_F429ZI.h"
// #include "drivers/hal/stm32fxxx_hal.h"
// #define BACKGROUND 1
// #define FOREGROUND 0
// #define GRAPH_PADDING 5


// LCD_DISCO_F429ZI lcd;
// //buffer for holding displayed text strings
// char display_buf[2][60];
// uint32_t graph_width=lcd.GetXSize()-2*GRAPH_PADDING;
// uint32_t graph_height=graph_width;

// //sets the background layer 
// //to be visible, transparent, and
// //resets its colors to all black
// void setup_background_layer(){
//   lcd.SelectLayer(BACKGROUND);
//   lcd.Clear(LCD_COLOR_BLACK);
//   lcd.SetBackColor(LCD_COLOR_BLACK);
//   lcd.SetTextColor(LCD_COLOR_GREEN);
//   lcd.SetLayerVisible(BACKGROUND,ENABLE);
//   lcd.SetTransparency(BACKGROUND,0x7Fu);
// }

// //resets the foreground layer to
// //all black
// void setup_foreground_layer(){
//     lcd.SelectLayer(FOREGROUND);
//     lcd.Clear(LCD_COLOR_BLACK);
//     lcd.SetBackColor(LCD_COLOR_BLACK);
//     lcd.SetTextColor(LCD_COLOR_LIGHTGREEN);
// }

// //draws a rectangle with horizontal tick marks
// //on the background layer. The spacing between tick
// //marks in pixels is taken as a parameter
// void draw_graph_window(uint32_t horiz_tick_spacing){
//   lcd.SelectLayer(BACKGROUND);
  
//   lcd.DrawRect(GRAPH_PADDING,GRAPH_PADDING,graph_width,graph_width);
//   //draw the x-axis tick marks
//   for (int32_t i = 0 ; i < graph_width;i+=horiz_tick_spacing){
//     lcd.DrawVLine(GRAPH_PADDING+i,graph_height,GRAPH_PADDING);
//   }
// }

// //maps inputY in the range minVal to maxVal, to a y-axis value pixel in the range
// //minPixelY to MaxPixelY
// uint16_t mapPixelY(float inputY,float minVal, float maxVal, int32_t minPixelY, int32_t maxPixelY){
//   const float mapped_pixel_y=(float)maxPixelY-(inputY)/(maxVal-minVal)*((float)maxPixelY-(float)minPixelY);
//   return mapped_pixel_y;
// }

// //DON'T COMMENT OUT THE ABOVE CODE


// ///*//START OF EXAMPLE 1---------------------------------------------
// int main() {
//   setup_background_layer();

//   setup_foreground_layer();

//   //creates c-strings in the display buffers, in preparation
//   //for displaying them on the screen
//   snprintf(display_buf[0],60,"width: %d pixels",lcd.GetXSize());
//   snprintf(display_buf[1],60,"height: %d pixels",lcd.GetYSize());
//   lcd.SelectLayer(FOREGROUND);
//   //display the buffered string on the screen
//   lcd.DisplayStringAt(0, LINE(16), (uint8_t *)display_buf[1], LEFT_MODE);
//   lcd.DisplayStringAt(0, LINE(17), (uint8_t *)display_buf[0], LEFT_MODE);
  
//   //draw the graph window on the background layer
//   // with x-axis tick marks every 10 pixels
//   draw_graph_window(10);


//   lcd.SelectLayer(FOREGROUND); 
//   while(1){
//     for(int j=0;j<235;j++){
      
//       //loop through each column of the graph, and update the data,
//       //erasing the old points and drawing the new points, one pixel at a time.
//       for(unsigned int i=0;i<graph_width;i++){
//         //the x coordinate of the graph value being updated.
//         //think about it like a vertical line
//         //that sweeps across the graph from left to right,
//         //updating each point in the graph as it travels across.
//         const uint32_t target_x_coord=GRAPH_PADDING+i;
//         //previous iterations function value at the target x coordinate
//         const float last_function_val= sin((i+j-1)*0.1)+1.0;
//         //current iterations function value at the target x coordinate
//         const float current_function_val=sin((i+j)*0.1)+1.0;
//         //y coordinate of the previous function value at the target x coordinate
//         const uint32_t old_pixelY=mapPixelY(last_function_val,0,2,GRAPH_PADDING,GRAPH_PADDING+graph_height);
//         //y coordinate of the current function value at the target x coordinate
//         const uint32_t new_pixelY=mapPixelY(current_function_val,0,2,GRAPH_PADDING,GRAPH_PADDING+graph_height);
//         //remove (set to black) the old pixel for the current x coordinate
//         //from the screen
//         lcd.DrawPixel(target_x_coord,old_pixelY,LCD_COLOR_BLACK);
//         //draw the new pixel for the current x coordinate on the screen
//         lcd.DrawPixel(target_x_coord,new_pixelY,LCD_COLOR_BLUE);
//       }
//       //delay to control update speed
//       thread_sleep_for(10);
//     }
//      for(int i=0;i<230;i++){
//        //remove old pixels after the last iteration
//        uint32_t old_pixel=mapPixelY(sin((i+234)*0.1)+1,-1,1,5,235);
//        lcd.DrawPixel(5+i,old_pixel,LCD_COLOR_BLACK); 
//      }
//   }
// }
// //*///END OF EXAMPLE 1-------------------------------------------


// /*//START OF EXAMPLE 2----------------------------------------
// #define BUFFER_SIZE 230
// uint32_t graph_pixels[BUFFER_SIZE];
// uint32_t next_write_index=0;
// uint32_t function_count=0;
// int main() {
//   setup_background_layer();

//   setup_foreground_layer();

//   //draw the graph window on the background layer
//   // with x-axis tick marks every 10 pixels
//   draw_graph_window(10);
//   //set initial values of the pixel buffer
//   for(unsigned int i=0;i<graph_width;i++){
//     graph_pixels[i]= GRAPH_PADDING+graph_height;
//   }

//   lcd.SelectLayer(FOREGROUND);

//   while(1){
    
//     float next_value=sin((function_count)*0.1)+1.0;
  
//     for(unsigned int i = 0; i<(graph_width-1);i++){
//       //the x coordinate of the graph value being updated.
//       //think about it like a vertical line
//       //that sweeps across the graph from left to right,
//       //updating each point in the graph as it travels across.
//       const uint32_t target_x_coord=GRAPH_PADDING+i;
//       //y coordinate of the previous function value at the target x coordinate
//       const uint32_t old_pixelY=graph_pixels[(i+next_write_index)%BUFFER_SIZE];
//       //y coordinate of the current function value at the target x coordinate
//       const uint32_t new_pixelY=graph_pixels[(i+next_write_index+1)%BUFFER_SIZE];
//       //remove (set to black) the old pixel for the current x coordinate
//       //from the screen
//       lcd.DrawPixel(target_x_coord,old_pixelY,LCD_COLOR_BLACK);
//       //draw the new pixel for the current x coordinate on the screen
//       lcd.DrawPixel(target_x_coord,new_pixelY,LCD_COLOR_BLUE);
//     }
//     //retrieve and erase the right most(last) pixel in the graph
//     const uint32_t last_old_pixelY= graph_pixels[(graph_width-1+next_write_index)%graph_width]; 
//     lcd.DrawPixel(GRAPH_PADDING+graph_width-1,last_old_pixelY,LCD_COLOR_BLACK);
//     //map, draw and store the newest value
//     graph_pixels[next_write_index]=mapPixelY(next_value,0,2,GRAPH_PADDING,GRAPH_PADDING+graph_height);
//     lcd.DrawPixel(GRAPH_PADDING+graph_width-1,graph_pixels[next_write_index],LCD_COLOR_BLUE);
//     next_write_index=(next_write_index+1)%graph_width;
//     function_count++;
//     thread_sleep_for(10);
//   }
// }
// //*///END OF EXAMPLE 2--------------------------------------------------



// /*//START OF EXAMPLE 3-------------------------------------------------
// #define BUFFER_SIZE 230
// //stack used by the draw_thread
// unsigned char draw_thread_stack[4096];
// Thread draw_thread(osPriorityBelowNormal1,4096,draw_thread_stack);
// //circular buffer is used like a queue. The main thread pushes
// //new data into the buffer, and the draw thread pops them out
// //and updates the graph
// CircularBuffer<float, BUFFER_SIZE> new_values;
// //semaphore used to protect the new_values buffer
// Semaphore new_values_semaphore(0,BUFFER_SIZE);


// uint32_t graph_pixels[BUFFER_SIZE];
// uint32_t next_write_index=0;

// void draw_thread_proc(){
//   static float next_value=0.0;
//   setup_background_layer();
//   setup_foreground_layer();
//   draw_graph_window(10);
//   lcd.SelectLayer(FOREGROUND);
//   for(int i=0;i<graph_width;i++){
//     graph_pixels[i]= GRAPH_PADDING+graph_height;
//   }
//   while(1){
//     //wait for main thread to release a semaphore,
//     //to indicate a new sample is ready to be graphed
//     new_values_semaphore.acquire();
//     new_values.pop(next_value);
//     for(int i = 0; i<(graph_width-1);i++){
//         //the x coordinate of the graph value being updated.
//         //think about it like a vertical line
//         //that sweeps across the graph from left to right,
//         //updating each point in the graph as it travels across.
//         const uint32_t target_x_coord=GRAPH_PADDING+i;
//          //y coordinate of the previous function value at the target x coordinate
//          const uint32_t old_pixelY=graph_pixels[(i+next_write_index)%graph_width];
//         //y coordinate of the current function value at the target x coordinate
//         const uint32_t new_pixelY=graph_pixels[(i+next_write_index+1)%graph_width];
//          //remove (set to black) the old pixel for the current x coordinate
//          //from the screen
//          lcd.DrawPixel(target_x_coord,old_pixelY,LCD_COLOR_BLACK);
//         //draw the new pixel for the current x coordinate on the screen
//         lcd.DrawPixel(target_x_coord,new_pixelY,LCD_COLOR_BLUE);
//     }
//     //retrieve and erase the right most(last) pixel in the graph
//     const uint32_t last_old_pixelY= graph_pixels[(graph_width-1+next_write_index)%graph_width]; 
//     lcd.DrawPixel(GRAPH_PADDING+graph_width-1,last_old_pixelY,LCD_COLOR_BLACK);
//     //map, draw and store the newest value
//     graph_pixels[next_write_index]=mapPixelY(next_value,0,2,GRAPH_PADDING,GRAPH_PADDING+graph_height);
//     lcd.DrawPixel(GRAPH_PADDING+graph_width-1,graph_pixels[next_write_index],LCD_COLOR_BLUE);
//     next_write_index=(next_write_index+1)%graph_width;
//   }


// }

// int main(){

//   draw_thread.start(draw_thread_proc);
  
//   thread_sleep_for(1000);
//   uint32_t index=0;
//   while(1){

//     if(!new_values.full()){
//       float current_val=sin((index)*0.1)+1.0;
//       index++;
//       //push the next value into the circular buffer
//       new_values.push(current_val);
//       if(new_values_semaphore.release()!=osOK){
//         MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_APPLICATION,MBED_ERROR_CODE_OUT_OF_MEMORY),"semaphore overflow\r\n");
//       }
//     }
//     else{
//      MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_APPLICATION,MBED_ERROR_CODE_OUT_OF_MEMORY),"circular buffer is full\r\n");
//     }

//     thread_sleep_for(10);
//   }
// }
// //*///END OF EXAMPLE 3----------------------------------------------

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

void loop();
Timer t;
Ticker t1;

void testing(){
  printf("Testing");
}

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
int main()
{
  double prevX;
  double prevY;
  double prevZ;
  // pc.baud(9600);
  t1.attach(&testing, 1.0);
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

  thread_sleep_for(1000);

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
void loop(void)
{
  // Possible vector values can be:
  // - VECTOR_ACCELEROMETER - m/s^2
  // - VECTOR_MAGNETOMETER  - uT
  // - VECTOR_GYROSCOPE     - rad/s
  // - VECTOR_EULER         - degrees
  // - VECTOR_LINEARACCEL   - m/s^2
  // - VECTOR_GRAVITY       - m/s^2
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER); 
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