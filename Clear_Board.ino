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
#include<HardwareSerial.h>
#include<DFRobotDFPlayerMini.h>
DFRobotDFPlayerMini amthanh;
#define RXD2 16
#define TXD2 17
HardwareSerial mySerial(2);
Adafruit_INA219 ina219;
Servo edf; 
const int edf_pin=33;
const int in[]={26,27,14,12};
const int ena=13;
const int enb= 25;
int i=0;
int speedright;
int speedleft;
int speedx;
int speed;
int coi = 4;
 int z=0;
 int oldtime=0;
int edf_speed;
float emptyVoltage=14.8
;
float fullVoltage=16.8                          ;
float batteryPercent;
int lasttime=0;
// RemoteXY connection settings 
#define REMOTEXY_WIFI_SSID "everylau"
#define REMOTEXY_WIFI_PASSWORD "12345678"
#define REMOTEXY_SERVER_PORT 6377

#include <RemoteXY.h>

// RemoteXY GUI configuration  
#pragma pack(push, 1)  
uint8_t RemoteXY_CONF[] =   // 79 bytes
  { 255,4,0,4,0,72,0,19,0,0,0,0,24,1,106,200,2,1,0,4,
  0,5,35,5,42,42,0,164,26,31,73,71,151,14,32,4,128,9,164,36,
  0,0,0,0,0,0,200,66,0,0,0,0,4,17,77,16,73,64,163,36,
  2,55,95,33,13,0,2,26,31,31,79,78,0,79,70,70,0,0,0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  int8_t doc; // from -100 to 100
  int8_t ngang; // from -100 to 100
  int8_t edf_control; // from 0 to 100
  uint8_t auto_mode; // =1 if switch ON and =0 if OFF

    // output variables
  float pin; // from 0 to 100

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
  // Wire.begin(21, 22);
  RemoteXY_Init (); 
 
  for(int i=0;i<4;i++){pinMode(in[i],OUTPUT);}
  ledcAttach(ena,1000,8);
  ledcAttach(enb,1000,8);
  pinMode(coi,OUTPUT);
  edf.attach(edf_pin,1000,2000);
  edf.write(0);
  delay(1000);
    

  if (!ina219.begin()) {
    Serial.println("Không tìm thấy INA219!");
    z=0;
  }

  Serial.println("INA219 đã sẵn sàng!");
  // TODO you setup code
  z=1;
   mySerial.begin(9600, SERIAL_8N1, RXD2, TXD2); 

   Serial.println("Khởi động DFPlayer Mini...");
    
    if (!amthanh.begin(mySerial)) {
        Serial.println("Không tìm thấy DFPlayer Mini. Kiểm tra kết nối!");
        while (true);  // Dừng chương trình nếu không tìm thấy module
    }

    Serial.println("DFPlayer Mini đã sẵn sàng!");
    amthanh.volume(30);  // Cài đặt âm lượng (0 - 30)
    delay(3000);
    amthanh.play(5);

}

void loop() 
{ 
  RemoteXY_Handler ();
  
  if(z==1){
  float busVoltage = ina219.getBusVoltage_V();
  Serial.print("Điện áp Bus: ");
  Serial.print(busVoltage);
  Serial.print("  ");
   batteryPercent = ((busVoltage - emptyVoltage) / (fullVoltage - emptyVoltage)) * 100.0;
    batteryPercent = constrain(batteryPercent, 0, 100);
    Serial.print(batteryPercent);
     Serial.print("      ");
    if(millis()-oldtime>1000){
      RemoteXY.pin=batteryPercent;
       
      oldtime=millis();
    }
    if(batteryPercent<30){
      if((millis()-lasttime>5500)&&RemoteXY.auto_mode==1)
      {
        amthanh.play(6);
        lasttime=millis();

      }
    } 
    }
  else{
    Serial.println("Vô hiệu hóa đo pin");
  }
// if(RemoteXY.edf_mode){
//    edf_speed=map(RemoteXY.edf_control,0,100,0,180);
//     edf.write(edf_speed);
//     Serial.print(edf_speed);
//      Serial.print("      ");
// }else {edf.write(0);}
    edf_speed=map(RemoteXY.edf_control,0,100,0,180);
    edf.write(edf_speed);
    Serial.print(edf_speed);
     Serial.print("      ");
  speed=map(RemoteXY.doc,-100,100,-255,255);
  Serial.print("    ");
  Serial.print(RemoteXY.doc);
  Serial.print("    ");
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
// void auto_mode(){
//  if(cb==0)
//  {
//   lui();
//   RemoteXY_delay(1000);
//  }
 

// }
