/**
  Author: Anish Kumar
  Date : 19-May-2018
  Brief:
   Data uploading from ESP8266-12E to own Webpage

*/
/*Header file include*/
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

/*Mcro Definitions*/
/*Config*/
#define DEBUG               1             //  DEBUG Macro will Enable Error Responses to Terminal
#define RESPONSE            0             //  RESPONSE Macro will Enable Update Status Responses from website to Terminal
/*Sensors*/
#define TEMP_SENSOR         A0            //  Temperature Sensor connected to ADC-0
/*Devices*/
#define FAN                 D0            //  FAN connected to D0 (or) GPIO16       
#define PUMP                D1            //  PUMP connected to D1 (or) GPIO5

/*WiFi Credentials*/
char *SSID = "BJ REDDY 1";
char *PASSWORD = "96422312131";

/*Variable Declarations*/
int Temperature = 0;
int stat;
int Number_Of_Repetition = 0;
char *F_OFF = "0";
char *F_ON = "1";
String Data = "";
String Data_Prev = "";
String URL = "http://blacksheeprunner.000webhostapp.com/agriculture/temp.php?temp=+";


ESP8266WiFiMulti WiFiMulti;
HTTPClient http;

/*Setup Function*/
void setup()
{
  pinMode(FAN, OUTPUT);
  pinMode(PUMP, OUTPUT);
  digitalWrite(FAN, LOW);
  digitalWrite(PUMP, LOW);
  Serial.begin(115200);
  Serial.println("");
  Serial.println(">>>>>  DEBUG MODE  <<<<<");
  for (uint8_t t = 4; t > 0; t--)
  {
    Serial.printf(">>[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(SSID, PASSWORD);

}/*setup*/

/*Continuous Running Loop function*/
void loop()
{
  Data_Prev = Data;                                                               //  Storing Previous data value to Data_Prev
  stat = WiFiMulti.run();                                                         //  Checking status of WiFi connection
  Temperature = (analogRead(TEMP_SENSOR) * 0.48);                                 //  Temperature Conversion see link --> http://www.instructables.com/id/ARDUINO-TEMPERATURE-SENSOR-LM35/
  Data = String(Temperature);                                                     //  Updating Data Variable with Temperature Value
  if (( stat == WL_CONNECTED))                                                    //  wait for WiFi connection
  {
#if RESPONSE
    Serial.print(">>[HTTP] begin...\n");
#endif
    (Data == Data_Prev) ? Number_Of_Repetition += 1 :  Number_Of_Repetition = 0;  //  Counting Number of repeated datas
    Serial.printf("\n\r>>Temperature Value = %d\n\r", Temperature);
    ( Number_Of_Repetition < 2 ) ? updateData() : No_Update();                    //  Avoid from updating Same data after updating 2 times
    readStatus("FAN");                                                            //  Calling readStatus Function for checking FAN Status
    readStatus("PUMP");                                                           //  Calling readStatus Function for checking PUMP Status
    delay(500);                                                                   //  waiting time for next cycle
  }

  else
  {
#if DEBUG
    Serial.print(">>WiFi not connected! run returned: ");
    Serial.println(stat);
#endif
  }


}/*loop*/

/*Temperature Data Upload Function*/
void updateData()
{

  //  http://blacksheeprunner.000webhostapp.com/agriculture/temp.php?temp=+30

  http.begin(URL + Data); //HTTP url
#if RESPONSE
  Serial.print(">>[HTTP] GET...\n");
#endif
  int httpCode = http.GET();                                                      //  start connection and send HTTP header
  if (httpCode > 0)                                                               //  httpCode will be negative on error
  {
    //  HTTP header has been send and Server response header has been handled
#if RESPONSE
    Serial.printf(">>[HTTP] GET... code: %d\n", httpCode);
#endif
    if (httpCode == HTTP_CODE_OK)
    {
      //  file found at server
      String payload = http.getString();
      Serial.println(">>Temperature Value Uploaded Successfully");
#if RESPONSE
      Serial.println(payload);
#endif
    }
  }

  else
  {
#if DEBUG
    Serial.printf(">>[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
#endif
  }


  http.end();

}/*updateData*/

void No_Update()
{
  Serial.println(">>Same Data Found Update Cancelled");
}/*No_Update*/

/*Status Reading Function*/
void readStatus(String dat)
{

  //http://blacksheeprunner.000webhostapp.com/agriculture/display_fan.php?fan
  //http://blacksheeprunner.000webhostapp.com/agriculture/display_pump.php?pump

  if (dat == "FAN")
  {
    Serial.print("\n\r>>Reading Fan Status\n");
    http.begin("http://blacksheeprunner.000webhostapp.com/agriculture/display_fan.php?fan");        //  HTTP url for FAN status check
  }
  else if (dat == "PUMP")
  {
    Serial.print("\n\r>>Reading Pump Status\n");
    http.begin("http://blacksheeprunner.000webhostapp.com/agriculture/display_pump.php?pump");      //  HTTP url for PUMP Status check
  }

#if RESPONSE
  Serial.print(">>[HTTP] GET...\n");
#endif
int httpCode = http.GET();                                                                          //  start connection and send HTTP header

  
  if (httpCode > 0)
  {                                                                                                 //  httpCode will be negative on error
    // HTTP header has been send and Server response header has been handled
#if RESPONSE
    Serial.printf(">>[HTTP] GET... code: %d\n", httpCode);
#endif
    // file found at server
    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();                                                            //  Checking Value from Website
#if RESPONSE
      Serial.println(payload);
#endif
      if (payload == "0")                                                                           
      {
        if (dat == "FAN")
        {
          Serial.println(">>FAN OFF");
          digitalWrite(FAN, LOW);                                                                   //  FAN Switched OFF
        }
        else if (dat == "PUMP")
        {
          Serial.println(">>PUMP OFF");               
          digitalWrite(PUMP, LOW);                                                                  //  PUMP Switched OFF
        }
      }
      else if (payload == "1")
      {
        if (dat == "FAN")
        {
          Serial.println(">>FAN ON");
          digitalWrite(FAN, HIGH);                                                                  //  FAN Switched ON
        }
        else if (dat == "PUMP")
        {
          Serial.println(">>PUMP ON");
          digitalWrite(PUMP, HIGH);                                                                 //  PUMP Switched ON
        }
      }
    }
  }
  else
  {
#if DEBUG
    Serial.printf(">>[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
#endif
  }

  http.end();
}/*readStatus*/
/*EOF*/

