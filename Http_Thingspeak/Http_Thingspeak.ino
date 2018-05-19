
#include <ESP8266WiFi.h>

// Wi-Fi Settings
const char* ssid = ""; // your wireless network name (SSID)
const char* password = ""; // your Wi-Fi network password
WiFiClient client;
// ThingSpeak Settings
const int channelID = 489907;// write channelID key for your ThingSpeak 
String writeAPIKey = ""; // write API key for your ThingSpeak Channel
const char* server = "api.thingspeak.com";
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
    
    // Measure Signal Strength (RSSI) of Wi-Fi connection
    long rssi = WiFi.RSSI();

    // Construct API request body
    String body = "field1=";
           body += String(rssi);
    
    Serial.print("RSSI: ");
    Serial.println(rssi); 

    client.println("POST /update HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("Connection: close");
    client.println("X-THINGSPEAKAPIKEY: " + writeAPIKey);
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(body.length()));
    client.println("");
    client.print(body);
    
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
