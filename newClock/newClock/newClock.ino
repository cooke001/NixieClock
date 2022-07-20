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
  int n = (10-(val/100000)%10)%10;
  int o = (10-(val/10000)%10)%10;
  int d = (10-(val/1000)%10)%10;
  int c = (10-(val/100)%10)%10;
  int b = (10-(val/10)%10)%10;
  int a = (10-(val)%10)%10;
  byte e = 16*a+b;
  byte f = 16*c+d;
  byte p = 16*o+n;
  
  byte l = 16*b+a;
  byte m = 16*d+c;
  int k = m*16*16+l;
  Serial.println(k, HEX);
  int g = (mask/1000)%10;
  int h = (mask/100)%10;
  int i = (mask/10)%10;
  int j = (mask)%10;
  
  Wire.beginTransmission(0x3A);
  Wire.write(e);
  Wire.endTransmission();  
  Wire.beginTransmission(0x39);
  Wire.write(f);
  Wire.endTransmission();  
  Wire.beginTransmission(0x38);
  Wire.write(p);
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
  //z = 10000*now.hour()+100*now.minute()+now.second();
  int x = 0;
  while(1){
    z = x;
    x=x+111111;
    
    clockPrint(z,0,dst);
    if(z>=999999){
      x=0;
    }
    delay(1000);
  }
  
  Webserver.handleClient();
  clockPrint(z,0,dst);
  delay(1000);
  r++;
}
