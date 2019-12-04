/*
 WiFiEsp example: WebClient

 This sketch connects to google website using an ESP8266 module to
 perform a simple web search.

 For more details see: http://yaab-arduino.blogspot.com/p/wifiesp-example-client.html
*/

#include "WiFiEsp.h"

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#endif

const int buffer_size = 512;
char ssid[] = "LoveE2";            // your network SSID (name)
char pass[] = "gary0206";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char server[] = "arduino.cc";

// Initialize the Ethernet client object
WiFiEspClient client;

void setup()
{
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial1.begin(9600);
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }
  
  // you're connected now, so print out the data
  Serial.println("You're connected to the network");
  printWifiStatus();

  //String re1 = http_get("httpbin.org", "/get", 80);
  //Serial.println(re1);
  String re = http_post("httpbin.org", "/post", "{\"JSON_key\": 22}");
  //Serial.println(re);

}

void loop()
{
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if the server's disconnected, stop the client
  if (!client.connected()) {
    Serial.println();
    Serial.println("Disconnecting from server...");
    client.stop();

    // do nothing forevermore
    while (true);
  }
}


void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
String http_post(String server, String api, const String &content){
  client.stop();
  int value = 2.5;  // an arbitrary value for testing
  String d = "{\"JSON_key\": " + String(value) + "}";
  if (client.connect("httpbin.org", 80)) {
    Serial.println("Connected to server");
    client.println("POST /post HTTP/1.1");
    client.println("Host: httpbin.org");
    client.println("Accept: */*");
    //client.println("Content-Length: " + d.length());
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println();
    //client.println(d);
  }
  
  //char buf[buffer_size] = "";
  int cot = 0;
   while (client.available()) {
    char c = client.read();
    Serial.write(c);
    //buf[cot++] = c;
  }
  
  if (!client.connected()) {
    Serial.println();
    Serial.println("Disconnecting from server...");
    client.stop();
  }
 // int idx = 0;
 // while(buf[idx++] != '{');
  //idx--;
  
 // Serial.print(buf+idx);
  //return String(buf+idx);
}
String http_get(String server, String api,int port){
  if (client.connect(server.c_str(), 80)) {
    Serial.println("Connected to server");
    // Make a HTTP request
    client.println("GET " + api + " HTTP/1.1");
    client.println("Host: " + server);
    client.println("Connection: close");
    client.println();
  }
  char buf[buffer_size] = "";
  int cot = 0;
   while (client.available() && cot < buffer_size) {
    char c = client.read();
    Serial.write(c);
    buf[cot++] = c;
  }
  
  if (!client.connected()) {
    Serial.println();
    Serial.println("Disconnecting from server...");
    client.stop();
  }
  int idx = 0;
  while(buf[idx++] != '{');
  idx--;
  
  Serial.print(buf+idx);
  return String(buf+idx);
}
