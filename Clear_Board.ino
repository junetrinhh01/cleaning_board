/*
   -- testserver --
   
   This source code of graphical user interface 
   has been generated automatically by RemoteXY editor.
   To compile this code using RemoteXY library 3.1.13 or later version 
   download by link http://remotexy.com/en/library/
   To connect using RemoteXY mobile app by link http://remotexy.com/en/download/                   
     - for ANDROID 4.15.01 or later version;
     - for iOS 1.12.1 or later version;
    
   This source code is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.    
*/

//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// you can enable debug logging to Serial at 115200
//#define REMOTEXY__DEBUGLOG    

// RemoteXY select connection mode and include library 
#define REMOTEXY_MODE__WIFI_POINT

#include <WiFi.h>
#include<ESP32Servo.h>
#include<Wire.h>
#include<Adafruit_INA219.h>
Adafruit_INA219 ina219;
Servo edf; 
const int edf_pin=33;
const int in[]={27,26,12,14};
const int ena=25;
const int enb= 13;
int i=0;
int speedright;
int speedleft;
int speedx;
int speed;
int coi = 4;
 int z=0;
 int oldtime=0;
const float fullVoltage = 12.6;
const float emptyVoltage = 9.6;
// RemoteXY connection settings 
#define REMOTEXY_WIFI_SSID "RemoteXY"
#define REMOTEXY_WIFI_PASSWORD "12345678"
#define REMOTEXY_SERVER_PORT 6377
#include <RemoteXY.h>
#pragma pack(push, 1)  
uint8_t RemoteXY_CONF[] =   // 126 bytes
  { 255,3,0,12,0,119,0,19,0,0,0,0,24,1,106,200,2,1,0,5,
  0,5,30,4,42,42,0,164,26,31,73,83,168,11,16,4,128,9,164,36,
  0,0,0,0,0,0,200,66,0,0,0,0,4,16,80,9,61,64,163,36,
  71,49,51,47,47,58,0,163,37,99,0,0,0,0,0,0,122,67,0,0,
  72,66,0,0,32,65,0,0,0,64,164,0,71,50,111,44,44,58,0,164,
  1,135,0,0,0,0,0,0,122,67,0,0,72,66,0,0,32,65,0,0,
  0,64,164,0,0,0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  int8_t doc; // from -100 to 100
  int8_t ngang; // from -100 to 100
  int8_t edf_control; // from 0 to 100

    // output variables
  float pin; // from 0 to 100
  float motor_speed; // from 0 to 250
  float motor_speed1; // from 0 to 250

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;   
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////


void setup() 
{
  Serial.begin(9600);
  Wire.begin(21, 22);
  RemoteXY_Init (); 
  edf.attach(edf_pin,1000,2000);
  edf.write(0);
  for(int i=0;i<4;i++){pinMode(in[i],OUTPUT);}
  ledcAttach(ena,1000,8);
  ledcAttach(enb,1000,8);
  pinMode(coi,OUTPUT);
    

  if (!ina219.begin()) {
    Serial.println("Không tìm thấy INA219!");
    z=0;
  }

  Serial.println("INA219 đã sẵn sàng!");
  // TODO you setup code
  z=1;
}

void loop() 
{ 
  RemoteXY_Handler ();
  
  if(z==1){
  float busVoltage = ina219.getBusVoltage_V();
  Serial.print("Điện áp Bus: ");
  Serial.print(busVoltage);
  Serial.print("  ");
   float batteryPercent = ((busVoltage - emptyVoltage) / (fullVoltage - emptyVoltage)) * 100.0;
    batteryPercent = constrain(batteryPercent, 0, 100);
    if(millis()-oldtime>1000){
      RemoteXY.pin=batteryPercent;
      oldtime=millis();
    }
    if(batteryPercent<30){
      digitalWrite(coi,1);
    } 
    else 
    {
      digitalWrite(coi,0);

    }
  }
  else{
    Serial.println("Vô hiệu hóa đo pin");
  }
    int edf_speed=map(RemoteXY.edf_control,0,100,0,180);
    edf.write(edf_speed);
  
  speed=map(RemoteXY.doc,-100,100,-255,255);
  speedx=map(RemoteXY.ngang,-100,100,255,-255);
 /*Serial.print(speed);
 Serial.print("t       ");
 Serial.print(speedx);
 Serial.println("       ");*/
if(speed>15)
{

   control_speed();
   tien();
}
else if(speed<-15)
{ speed=-speed;
    control_speed();
    lui();
  }
else{
  speedleft=0;
  speedright=0;
  tien();
}

Serial.print(speedleft);
   Serial.print("   ");
   Serial.println(speedright);
  // TODO you loop code
  // use the RemoteXY structure for data transfer
  // do not call delay(), use instead RemoteXY_delay() 
 
RemoteXY.motor_speed=map(speedleft,0,255,0,250);
RemoteXY.motor_speed1=map(speedright,0,255,0,250);
RemoteXY_delay(10);
}
void tien()
{
   ledcWrite(ena,speedleft);
   ledcWrite(enb,speedright);
  digitalWrite(in[0],1);
  digitalWrite(in[1],0);
  digitalWrite(in[2],1);
  digitalWrite(in[3],0);
}
void lui()
{ Serial.print("đang lùi ");
Serial.print("      ");
  ledcWrite(ena,speedleft);
   ledcWrite(enb,speedright);
  digitalWrite(in[0],0);
  digitalWrite(in[1],1);
  digitalWrite(in[2],0);
  digitalWrite(in[3],1);
}
void control_speed()
{
  speedright=constrain(speed-speedx,0,255);
   speedleft=constrain(speed+speedx,0,255);
  
}
