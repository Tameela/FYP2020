/*
Basic_I2C.ino
Brian R Taylor
brian.taylor@bolderflight.com

Copyright (c) 2017 Bolder Flight Systems

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, 
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or 
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "MPU9250.h"

// an MPU9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68
MPU9250 IMU(Wire,0x68);
int status;
float value;
float offset1;

//Kalman filtering variables 
double est1,est2; //Variables to store the filtered data 
double last_est1 = 0;
double last_est2 = 0;
double process_noise = 1;
double sensor_noise = 2;    // Change the value of sensor noise to get different KF performance

void setup() {
  // serial to display data
  Serial.begin(115200);
  while(!Serial) {}

  // start communication with IMU 
  status = IMU.begin();
  if (status < 0) {
    Serial.println("IMU initialization unsuccessful");
    Serial.println("Check IMU wiring or try cycling power");
    Serial.print("Status: ");
    Serial.println(status);
    while(1) {}
  }
  // calibrating accelerometer
    Serial.println("Starting Accelerometer Calibration");
    IMU.calibrateAccel();
    Serial.println("Switch");
    delay(5000);
    IMU.calibrateAccel();
    Serial.println("Switch");
    delay(5000);
    IMU.calibrateAccel();
    Serial.println("Switch");
    delay(5000);
    IMU.calibrateAccel();
    Serial.println("Switch");
    delay(5000);
    IMU.calibrateAccel();
    Serial.println("Switch");
    delay(5000);
    IMU.calibrateAccel();
    Serial.println("Done");
    
    value = IMU.getAccelBiasX_mss();
    Serial.print("Accel bias x:");
    Serial.println(value); 
    value = IMU.getAccelScaleFactorX();
    Serial.print("Accel scale x:");
    Serial.println(value); 
    value = IMU.getAccelBiasY_mss();
    Serial.print("Accel bias y:");
    Serial.println(value); 
    value = IMU.getAccelScaleFactorY();
    Serial.print("Accel scale y:");
    Serial.println(value); 
    value = IMU.getAccelBiasZ_mss();
    Serial.print("Accel bias z:");
    Serial.println(value); 
    value = IMU.getAccelScaleFactorZ();
    Serial.print("Accel factor z:");
    Serial.println(value);    

    offset1 = -9.8;
    Serial.print("offset: ");
    Serial.println(offset1); 
    Serial.println("Done");
    delay(5000);
}

void loop() {
  float az;
  // read the sensor
  IMU.readSensor();
  // display the data
  Serial.print(IMU.getAccelX_mss(),6);
  Serial.println("\t");
  Serial.print("X acceleration:");
  value = IMU.getAccelX_mss();
  est1 = Kalman(value, last_est1);  //Filtered estimated measurement
  value = est1; //Use the filtered data 
  last_est1 = est1;  // store estimate as previous measurement for next cycle
  Serial.print(value,6);
  Serial.println("\t");
  
  Serial.print(IMU.getAccelY_mss(),6);
  Serial.println("\t");
  Serial.print("Y acceleration:");
  value = IMU.getAccelY_mss();
  est2 = Kalman(value, last_est1);  //Filtered estimated measurement
  value = est2; //Use the filtered data 
  last_est2 = est2;  // store estimate as previous measurement for next cycle
  Serial.print(value,6);
  Serial.println("\t");
  az=IMU.getAccelZ_mss();
  if(az>0){
    Serial.print(az+offset1);
    Serial.println("\t");
  }
  else{
  Serial.print(az-offset1);
  Serial.println("\t");
  }
  /*
  Serial.print(IMU.getGyroX_rads(),6);
  Serial.print("\t");
  Serial.print(IMU.getGyroY_rads(),6);
  Serial.print("\t");
  Serial.print(IMU.getGyroZ_rads(),6);
  Serial.print("\t");
  Serial.print(IMU.getMagX_uT(),6);
  Serial.print("\t");
  Serial.print(IMU.getMagY_uT(),6);
  Serial.print("\t");
  Serial.print(IMU.getMagZ_uT(),6);
  Serial.print("\t");
  Serial.println(IMU.getTemperature_C(),6);*/
  delay(1000);
}

//Kalman filter
double Kalman(double rawdata, double prev_est){   
  double a_priori_est, a_post_est, a_priori_var, a_post_var, kalman_gain;

  a_priori_est = prev_est;  
  a_priori_var = process_noise; 

  kalman_gain = a_priori_var/(a_priori_var+sensor_noise);
  a_post_est = a_priori_est + kalman_gain*(rawdata-a_priori_est);
  a_post_var = (1- kalman_gain)*a_priori_var;
  return a_post_est;
}
