
/**
   BasicHTTPClient.ino

    Created on: 24.05.2015

*/

#include <Arduino.h>
#include <Adafruit_ADS1015.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <SoftwareSerial.h>
// For DS18B20
#include <DallasTemperature.h>
#include <OneWire.h>
// End
#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>

ESP8266WiFiMulti WiFiMulti;

#define DQ_pin D4
OneWire oneWire(DQ_pin);
DallasTemperature WTsensors(&oneWire);

//SoftwareSerial bt(D5, D6);

Adafruit_ADS1115 ads(0x48);


#define TDS_APIN 0
#define WaterLevel_APIN 1
#define VREF 3.0 // analog reference voltage(Volt) of the ADC
#define SCOUNT 30 // sum of sample point
int analogBuffer[SCOUNT]; // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0,copyIndex = 0;
float averageVoltage = 0,tdsValue = 0,temperature = 25;


int getWaterLevel()
{
    return ads.readADC_SingleEnded(WaterLevel_APIN)*0.0001875*1023/3;
}

int getTds()
{
    while(1)
    {
        static unsigned long analogSampleTimepoint = millis();
        if(millis()-analogSampleTimepoint > 40U) //every 40 milliseconds,read the analog value from the ADC
        {
            analogSampleTimepoint = millis();
            analogBuffer[analogBufferIndex] = ads.readADC_SingleEnded(TDS_APIN)*0.0001875*1023/3; //read the analog value and store into the buffer
            analogBufferIndex++;
            if(analogBufferIndex == SCOUNT)
                analogBufferIndex = 0;
        }
        static unsigned long printTimepoint = millis();
        if(millis()-printTimepoint > 800U)
        {
            printTimepoint = millis();
            for(copyIndex=0; copyIndex<SCOUNT; copyIndex++)
                analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
            averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
            float compensationCoefficient=1.0+0.02*(temperature-25.0); //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
            float compensationVolatge=averageVoltage/compensationCoefficient; //temperature compensation
            tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value
            return tdsValue;
        }
    }
}


void setup()
{

  Serial.begin(115200);
  //bt.begin(9600);
  Serial.setDebugOutput(true);

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

  // FOR ADS 
  ads.begin();
  
}

void loop()
{
  // wait for WiFi connection
  Serial.println("test");
  
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
  Serial.println(voltage0);
  Serial.print("VIN1: ");
  Serial.println(voltage1);
  Serial.print("VIN2: ");
  Serial.println(voltage2);
  Serial.print("VIN3: ");
  Serial.println(voltage3);
  Serial.println(" ");


  Serial.println(getTds());
  Serial.println(getWaterLevel());
  Serial.println(analogRead(A0) );
  WTsensors.requestTemperatures();
  Serial.println(WTsensors.getTempCByIndex(0));
  delay(1000);
  //http_get(WiFiMulti, "http://httpbin.org/get");
  //http_post(WiFiMulti, "http://httpbin.org/post", "{}");
}

int getMedianNum(int bArray[], int iFilterLen)
{
    int bTab[iFilterLen];
    for (byte i = 0; i<iFilterLen; i++)
        bTab[i] = bArray[i];
    int i, j, bTemp;
    for (j = 0; j < iFilterLen - 1; j++)
    {
        for (i = 0; i < iFilterLen - j - 1; i++)
        {
            if (bTab[i] > bTab[i + 1])
            {
                bTemp = bTab[i];
                bTab[i] = bTab[i + 1];
                bTab[i + 1] = bTemp;
            }
        }
    }
    if ((iFilterLen & 1) > 0)
        bTemp = bTab[(iFilterLen - 1) / 2];
    else
        bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
    return bTemp;
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
