//settings - NodeMCU 1.0 (ESP-12E Module), 115200, 80MHz

#include <Wire.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "config.h"

const char *ssid = mySSID;
const char *password = myPASSWORD;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "uk.pool.ntp.org", 0);
int z;

void clockPrint(int val, int mask, int DST){
  int d = (val/1000)%10;
  int c = (val/100)%10;
  int b = (val/10)%10;
  int a = (val)%10;
  int g = (mask/1000)%10;
  int h = (mask/100)%10;
  int i = (mask/10)%10;
  int j = (mask)%10;
  if(DST){
    c++;
    if((d==0) && (c==10)){
     c=0;
     d=1;
    }    
    if((d==1) && (c==10)){
     c=0;
     d=2;
    }    
    if((d==2) && (c==4)){
     c=0;
     d=0;
    }
  }
  if(g){d= 0xA;}
  if(h){c= 0xA;}
  if(i){b= 0xA;}
  if(j){a= 0xA;}
  byte e = 16*a+b;
  byte l = 16*b+a;
  byte f = 16*c+d;
  byte m = 16*d+c;
  int k = m*16*16+l;
  Serial.println(k, HEX);
  Wire.beginTransmission(0x26);
  Wire.write(e);
  Wire.endTransmission();
  Wire.beginTransmission(0x27);
  Wire.write(f);
  Wire.endTransmission();
}

void setup(){
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  timeClient.begin();
  Wire.begin(D1, D2);
}

void loop() {
  timeClient.update();
  z = 100*timeClient.getHours()+timeClient.getMinutes(); 
  clockPrint(z,0,1);
  delay(1000);
}
