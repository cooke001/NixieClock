//settings - NodeMCU 1.0 (ESP-12E Module), 115200, 80MHz
#include <DS3231.h>
DS3231 Clock;
#include <Wire.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include "config.h"
RTClib RTC;

ESP8266WebServer Webserver(80);
const char *ssid = mySSID;
const char *password = myPASSWORD;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 0);
int z;
int dst = 0;
int r;
String HTMLpage = "";

void clockPrint(int val, int mask, int DST){
  int f = (val/100000)%10;
  int e = (val/10000)%10;
  int d = (val/1000)%10;
  int c = (val/100)%10;
  int b = (val/10)%10;
  int a = (val)%10;
  if(DST){
    e++;
    if((f==0) && (e==10)){
     e=0;
     f=1;
    }    
    if((f==1) && (e==10)){
     e=0;
     f=2;
    }    
    if((f==2) && (e==4)){
     e=0;
     f=0;
    }
  }
  Serial.print(f);
  Serial.print(e);
  Serial.print(d);
  Serial.print(c);
  Serial.print(b);
  Serial.println(a);
  f=(10-f)%10;
  e=(10-e)%10;
  d=(10-d)%10;
  c=(10-c)%10;
  b=(10-b)%10;
  a=(10-a)%10;
  byte y = 16*e+f;
  byte x = 16*c+d;
  byte m = 16*a+b;
  Wire.beginTransmission(0x38);
  Wire.write(y);
  Wire.endTransmission();
  Wire.beginTransmission(0x39);
  Wire.write(x);
  Wire.endTransmission();  
  Wire.beginTransmission(0x3A);
  Wire.write(m);
  Wire.endTransmission();  

}

void setup(){
  HTMLpage += "<head><title>Nixie Clock</title></head><p><a href=\"DSTON\"><button>SUMMER</button></a>&nbsp;<a href=\"DSTOFF\"><button>WINTER</button></a></p>";
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  Serial.println("WiFi connected");
  MDNS.begin("nixie2");
  timeClient.begin();
  Wire.begin(D1, D2);
  Webserver.on("/", [](){
    Webserver.send(200, "text/html", HTMLpage);
  });
  Webserver.on("/DSTON", [](){
    Webserver.send(200, "text/html", HTMLpage+"<p>GET THE SUNCREAM AND PIMMS OUT</p>");
    dst = 1;
  });
  Webserver.on("/DSTOFF", [](){
    Webserver.send(200, "text/html", HTMLpage+"<p>HOPE YOU FREEZE</p>");
    dst = 0;
  });
  Webserver.begin();
  Serial.println("HTTP Webserver started");
  Clock.setClockMode(false);
  Serial.println("clockStarted");
  timeClient.update();
  r=600;
}

void loop() {
  if(r==600){
    r=0;
    timeClient.update();
    Clock.setHour(timeClient.getHours());
    Clock.setMinute(timeClient.getMinutes());
    Clock.setSecond(timeClient.getSeconds());
    Serial.print(timeClient.getHours());
    Serial.print(":");
    Serial.print(timeClient.getMinutes());
    Serial.print(":");
    Serial.println(timeClient.getSeconds());
  }
  DateTime now = RTC.now();
  z = 10000*now.hour()+100*now.minute()+now.second();
  
  //Serial.println(z);
  
  Webserver.handleClient();
  clockPrint(z,0,dst);
  delay(1000);
  r++;
}
