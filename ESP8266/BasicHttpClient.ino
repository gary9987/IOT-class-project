/**
   BasicHTTPClient.ino

    Created on: 24.05.2015

*/

#include <Arduino.h>
//#include <Adafruit_ADS1015.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <SoftwareSerial.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>

ESP8266WiFiMulti WiFiMulti;

SoftwareSerial bt(D5, D6);

//Adafruit_ADS1115 ads(0x48);

void setup()
{

  Serial.begin(115200);
  bt.begin(9600);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("LoveE2", "gary0206");

  while (WiFiMulti.run() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println("Connected to wifi");
  Serial.print("  Status: ");
  Serial.println(WiFi.status()); // some parameters from the network
  Serial.print("      IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("  Subnet: ");
  Serial.println(WiFi.subnetMask());
  Serial.print(" Gateway: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("    SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("  Signal: ");
  Serial.println(WiFi.RSSI());
  Serial.print("Networks: ");
  Serial.println(WiFi.scanNetworks());

  Serial.println("Getting single-ended readings from AIN0..3");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");

  //ads.begin();
}

void loop()
{
  // wait for WiFi connection
  Serial.println("test");
  /*
int16_t adc0, adc1, adc2, adc3;
float voltage0, voltage1, voltage2, voltage3;
 
adc0 = ads.readADC_SingleEnded(0);
adc1 = ads.readADC_SingleEnded(1);
adc2 = ads.readADC_SingleEnded(2);
adc3 = ads.readADC_SingleEnded(3);
voltage0 = adc0 * 0.0001875;
voltage1 = adc1 * 0.0001875;
voltage2 = adc2 * 0.0001875;
voltage3 = adc3 * 0.0001875;
Serial.print("AIN0: ");
Serial.println(adc0);
Serial.print("AIN1: ");
Serial.println(adc1);
Serial.print("AIN2: ");
Serial.println(adc2);
Serial.print("AIN3: ");
Serial.println(adc3);
Serial.println(" ");
Serial.print("VIN0: ");
Serial.println(v0);
Serial.print("VIN1: ");
Serial.println(v1);
Serial.print("VIN2: ");
Serial.println(v2);
Serial.print("VIN3: ");
Serial.println(v3);
Serial.println(" ");
 */
  delay(1000);
  http_get(WiFiMulti, "http://httpbin.org/get");
  http_post(WiFiMulti, "http://httpbin.org/post", "{}");
}

String http_get(ESP8266WiFiMulti &WiFiMulti, const String &url)
{
  if ((WiFiMulti.run() == WL_CONNECTED))
  {
    WiFiClient client;
    HTTPClient http;
    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, url))
    { // HTTP
      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();
      // httpCode will be negative on error
      if (httpCode > 0)
      {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
        {
          String payload = http.getString();
          Serial.println(payload);
        }
      }
      else
      {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    }
    else
    {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }
  return "";
}
String http_post(ESP8266WiFiMulti &WiFiMulti, const String &url, const String &data)
{
  if ((WiFiMulti.run() == WL_CONNECTED))
  {
    WiFiClient client;
    HTTPClient http;
    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, url))
    { // HTTP
      Serial.print("[HTTP] POST...\n");
      // start connection and send HTTP header
      int httpCode = http.POST(data);
      // httpCode will be negative on error
      if (httpCode > 0)
      {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
        {
          String payload = http.getString();
          Serial.println(payload);
        }
      }
      else
      {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    }
    else
    {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }
  return "";
}
