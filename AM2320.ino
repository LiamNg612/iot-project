#include <Wire.h>
#include "AM2320.h"
#include <ESP8266WiFi.h>
AM2320 th;
#define SSID    "ierg"
#define PASS    "12345678"
#define HOST    "api.thingspeak.com" // ThingSpeak IP Address: 184.106.153.149
#define PORT    80
#define RL 47  //The value of resistor RL is 47K
#define m -0.263 //Enter calculated Slope 
#define b 0.42 //Enter calculated intercept
#define Ro 30 //Enter found Ro value
String APIKEY  = "LCVWU3NKZQ0NMLCV";
float temperature, humidity,ppm;
void setup() {
  Serial.begin(115200);
   WiFi.begin(SSID,PASS);
  while(WiFi.status() != WL_CONNECTED) {
   delay(500);
   Serial.print(".");
    Serial.println( "WiFi connected" );
    Serial.println( "IP address: " );
    Serial.println( WiFi.localIP() );
    Serial.println( "" );
    
  }
}
void ThinkSpeak() {
   WiFiClient client;
   if( !client.connect( HOST, PORT ) )
    {
        Serial.println( "connection failed" );
        return;
    }
    else
    {
        String postStr = APIKEY + "&field1=" + String((int)temperature) + \
                                  "&field2=" + String((int)humidity) + 
                                  "&field3=" + String((int)ppm) + "\r\n\r\n";

        client.print( "POST /update HTTP/1.1\n" );
        client.print( "Host: api.thingspeak.com\n" );
        client.print( "Connection: close\n" );
        client.print( "X-THINGSPEAKAPIKEY: "+APIKEY+"\n" );
        client.print( "Content-Type: application/x-www-form-urlencoded\n" );
        client.print( "Content-Length: " );
        client.print( postStr.length() );
        client.print( "\n\n" );
        client.print( postStr );
        
        client.stop();
        delay(15000);
    }
  
}

void loop() {
  humidity    = th.getHumidity();
  temperature = th.getTemperature();
  float VRL; //Voltage drop across the MQ sensor
  float Rs; //Sensor resistance at gas concentration 
  float ratio; //Define variable for ratio
   
  //Serial.println("---------------");
  Serial.print("humidity: ");
  Serial.print(humidity);
  Serial.print("%");
  Serial.print("|| temperature: ");
  Serial.print(temperature);
  Serial.println("*C");
  Serial.println("---------------");
  VRL = analogRead(A0)*(5.0/1023.0); //Measure the voltage drop and convert to 0-5V
  Rs = ((5.0*RL)/VRL)-RL; //Use formula to get Rs value
  ratio = Rs/Ro;  // find ratio Rs/Ro
 
  ppm = pow(10, ((log10(ratio)-b)/m)); //use formula to calculate ppm

   Serial.print("NH3 (ppm) = "); //Display a ammonia in ppm
   Serial.println(ppm);
    ThinkSpeak();
    delay(5000);
}
