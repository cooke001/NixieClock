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
NTPClient timeClient(ntpUDP, "uk.pool.ntp.org", 0);
int z,dst,r;
String HTMLpage = "";

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
  HTMLpage += "<head><title>Nixie Clock</title></head><p><a href=\"DSTON\"><button>SUMMER</button></a>&nbsp;<a href=\"DSTOFF\"><button>WINTER</button></a></p>";
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  Serial.println("WiFi connected");
  MDNS.begin("nixie");
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
  while(0){
    clockPrint(z,0,dst);
    z = random(0,9999);
    delay(100);
  }
  if(r==600){
    r=0;
    timeClient.update();
    Clock.setHour(timeClient.getHours());
    Clock.setMinute(timeClient.getMinutes());
    Clock.setSecond(timeClient.getSeconds());
  }
  DateTime now = RTC.now();
  z = 100*now.hour()+now.minute();
  Webserver.handleClient();
  clockPrint(z,0,dst);
  delay(1000);
  r++;
}
