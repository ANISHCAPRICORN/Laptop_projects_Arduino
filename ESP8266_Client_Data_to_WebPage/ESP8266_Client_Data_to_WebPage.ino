/**
  Author: Anish Kumar
  Date : 19-May-2018
  Brief:
   Data uploading from ESP8266-12E to own Webpage

    pin Connections for Debug Mode
    ------------------------------

    Connect GPIO15 to Gnd and then do Reset... Now ESP will enter into Debug Mode
    During Debug Mode you can see responses from webpage through Serial port with 115200 baudrate

    To exit from Debug mode Just remove GPIO15 from Gnd and then do Reset.

*/
/*Header file include*/
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

/*Macro declaration*/
#define DEBUG_PIN D1 // GPIO15 as Debug Pin

enum MODE {ON, OFF};
int DEBUG_MODE = OFF;
int Number_Of_Repetition = 0;
char *F_OFF = "0";
char *F_ON = "1";
/*WiFi Credentials*/
char *SSID = "BJ REDDY 1";
char *PASSWORD = "96422312131";

String URL = "http://blacksheeprunner.000webhostapp.com/agriculture/temp.php?temp=+";
int Temperature = 0;
String Data = "";

ESP8266WiFiMulti WiFiMulti;
HTTPClient http;
void setup()
{
  pinMode(DEBUG_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  if (!digitalRead(DEBUG_PIN))
  {
    DEBUG_MODE = ON;
  }
  else
  {
    DEBUG_MODE = OFF;
  }
  if ( DEBUG_MODE == ON )
  {
    Serial.begin(115200);
    delay(1000);
    Serial.println("");
    Serial.println(">>>>>  DEBUG MODE ON  <<<<<");
    for (uint8_t t = 4; t > 0; t--)
    {
      Serial.printf(">>[SETUP] WAIT %d...\n", t);
      Serial.flush();
      delay(1000);
    }
  }
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(SSID, PASSWORD);

}/*setup*/

void loop()
{
  String Data_Prev = Data;
  int stat = WiFiMulti.run();   // Checking status of WiFi connection
  Temperature = (analogRead(A0) * 0.48); // Temperature Conversion see link --> http://www.instructables.com/id/ARDUINO-TEMPERATURE-SENSOR-LM35/
  Data = String(Temperature);
  if (( stat == WL_CONNECTED))  // wait for WiFi connection
  {
    if ( DEBUG_MODE == ON )
      Serial.print(">>[HTTP] begin...\n");
    (Data == Data_Prev) ? Number_Of_Repetition += 1 :  Number_Of_Repetition = 0;
    if ( DEBUG_MODE == ON )
      Serial.printf(">>Temperature Value = %d\n\r", Temperature);
    ( Number_Of_Repetition < 3 ) ? updateData() : No_Update();// Avoid from updating Same data
    readFanStatus();
    delay(500); // waiting time for next cycle
  }
  else
  {
    Serial.print(">>WiFi not connected! run returned: ");
    Serial.println(stat);
  }

}/*loop*/

void updateData()
{

  //http://blacksheeprunner.000webhostapp.com/agriculture/temp.php?temp=+30

  http.begin(URL + Data); //HTTP url

  if ( DEBUG_MODE == ON )
    Serial.print(">>[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    if ( DEBUG_MODE == ON )
      Serial.printf(">>[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      if ( DEBUG_MODE == ON )
        Serial.println(payload);
    }
  } else {
    if ( DEBUG_MODE == ON )
      Serial.printf(">>[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();

}/*updateData*/

void No_Update()
{
  if ( DEBUG_MODE == ON )
    Serial.println(">>Same Data Found Update Cancelled");
}/*No_Update*/

void readFanStatus()
{
  if ( DEBUG_MODE == ON )
    Serial.print(">>Reading Fan Status\n");
    
  //http://blacksheeprunner.000webhostapp.com/agriculture/display_fan.php?fan

  http.begin("http://blacksheeprunner.000webhostapp.com/agriculture/display_fan.php?fan"); //HTTP url

  if ( DEBUG_MODE == ON )
    Serial.print(">>[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    if ( DEBUG_MODE == ON )
      Serial.printf(">>[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      if ( DEBUG_MODE == ON )
      {
        Serial.println(payload);
        if (payload == "0")
        {
          Serial.println(">>>>>>FAN OFF <<<<<<");
          digitalWrite(LED_BUILTIN, HIGH);
        }
        else if (payload == "1")
        {
          Serial.println(">>>>>>FAN ON <<<<<<");
          digitalWrite(LED_BUILTIN, LOW);
        }
      }
    }
  } else {
    if ( DEBUG_MODE == ON )
      Serial.printf(">>[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}


