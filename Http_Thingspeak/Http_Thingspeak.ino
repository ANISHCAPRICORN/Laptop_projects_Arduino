/*
 ESP8266 --> ThingSpeak Channel via MKR1000 Wi-Fi
 
 This sketch sends the Wi-Fi Signal Strength (RSSI) of an ESP8266 to a ThingSpeak
 channel using the ThingSpeak API (https://www.mathworks.com/help/thingspeak).
 
 Requirements:
 
   * ESP8266 Wi-Fi Device
   * Arduino 1.6.9+ IDE
   * Additional Boards URL: http://arduino.esp8266.com/stable/package_esp8266com_index.json
   * Library: esp8266 by ESP8266 Community
 
 ThingSpeak Setup:
 
   * Sign Up for New User Account - https://thingspeak.com/users/sign_up
   * Create a new Channel by selecting Channels, My Channels, and then New Channel
   * Enable one field
   * Note the Channel ID and Write API Key
    
 Setup Wi-Fi:
  * Enter SSID
  * Enter Password
  
 Tutorial: http://nothans.com/measure-wi-fi-signal-levels-with-the-esp8266-and-thingspeak
   
 Created: Feb 1, 2017 by Hans Scharler (http://nothans.com)
*/

#include <ESP8266WiFi.h>

// Wi-Fi Settings
const char* ssid = "BJ REDDY 1"; // your wireless network name (SSID)
const char* password = "96422312131"; // your Wi-Fi network password
WiFiClient client;
const char* server = "http://blacksheeprunner.000webhostapp.com";
const int postingInterval = 3 * 1000; // post data every 20 seconds

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}
int i=30;
void loop() {
  if (client.connect(server, 80)) {
    Serial.println("Connected to server");
//    WiFi.printDiag(Serial);

//api.thingspeak.com/update?api_key=74QLDQU0SCF4CAOD&field1=0
//blacksheeprunner.000webhostapp.com/agriculture/temp.php?temp=+78



    String body = "temp=70";
           
    client.println("GET /agriculture/temp.php HTTP/1.1");
    client.println("Host: blacksheeprunner.000webhostapp.com");
    client.println("Cache-Control: no-cache");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: " + String(body.length()));
    client.println("\n");
    client.print(body);
    client.println("\n\n\n\n");
    
    Serial.println("[Response:]");
    while (client.connected())
    {
      if (client.available())
      {
        String line = client.readStringUntil('\n');
        Serial.println(line);
      }
    }

    
    
    

  }
  client.stop();

  // wait and then post again
  delay(postingInterval);
  i+=1;
}
