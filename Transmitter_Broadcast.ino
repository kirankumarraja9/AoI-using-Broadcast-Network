// this code is the transmitter to the Age of Information using the ESP8266 Wifi Board

#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>
RtcDS3231<TwoWire> Rtc(Wire);
/* for normal hardware wire use above */
 
// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress1[] = {0xE8, 0xDB, 0x84, 0xDC, 0xF8, 0xA4};//E8:DB:84:DC:F8:A4
//uint8_t broadcastAddress2[] = {0xC4, 0x5B, 0xBE, 0x49, 0x56, 0xFA};
 
#define BOARD_ID 1
 
// Updates DHT readings every 10 seconds
const long interval = 1000; 
unsigned long previousMillis = 0;    // will store last time DHT was updated 
 
// Variable to store if sending data was successful
String success;
 
//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    int id; 
    int x;
    float y;   
    int k;
} struct_message;
 
//Create a struct_message called myData to hold x&y
struct_message myData;
 
//Create a struct_message called myData to hold x&y
struct_message Data;
 
// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  char macStr[18];
  Serial.print("Packet to:");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
         mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
    Serial.println();
  }
  else{
    Serial.println("Delivery fail");
    Serial.println();
  }
}
 
int k = 1;
int n =0;
 
void setup () 
{
    Serial.begin(9600);
    
     // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
 
    k = 1;
    n = 0;
    // Init ESP-NOW
    if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
    }
 
    // Set ESP-NOW Role
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
 
    // Once ESPNow is successfully Init, we will register for Send CB to
   // get the status of Trasnmitted packet
    esp_now_register_send_cb(OnDataSent);
 
    // Register peer
    esp_now_add_peer(broadcastAddress1, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
    //esp_now_add_peer(broadcastAddress2, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
 
    //Serial.print("compiled: ");
    //Serial.print(__DATE__);
    //Serial.println(__TIME__);
 
    //--------RTC SETUP ------------
    // if you are using ESP-01 then uncomment the line below to reset the pins to
    // the available pins for SDA, SCL
    // Wire.begin(0, 2); // due to limited pins, use pin 0 and 2 for SDA, SCL
    
    Rtc.Begin();
 
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    //printDateTime(compiled);
    Serial.println();
 
    if (!Rtc.IsDateTimeValid()) 
    {
        if (Rtc.LastError() != 0)
        {
            // we have a communications error
            // see https://www.arduino.cc/en/Reference/WireEndTransmission for 
            // what the number means
            Serial.print("RTC communications error = ");
            Serial.println(Rtc.LastError());
        }
        else
        {
            // Common Causes:
            //    1) first time you ran and the device wasn't running yet
            //    2) the battery on the device is low or even missing
 
            Serial.println("RTC lost confidence in the DateTime!");
 
            // following line sets the RTC to the date & time this sketch was compiled
            // it will also reset the valid flag internally unless the Rtc device is
            // having an issue
 
            Rtc.SetDateTime(compiled);
        }
    }
 
    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }
 
    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        //Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        //Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled) 
    {
        //Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }
 
    // never assume the Rtc was last configured by you, so
    // just clear them to your needed state
    Rtc.Enable32kHzPin(false);
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 
}
 
#define countof(a) (sizeof(a) / sizeof(a[0]))
 
 void printDateTime(const RtcDateTime& dt)
 {
    char datestring[20];
    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
    //Serial.print(dt.Month());
 }
 
 
void loop () 
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the x,y values
    previousMillis = currentMillis;
    if (!Rtc.IsDateTimeValid()) 
    {
        if (Rtc.LastError() != 0)
        {
            // we have a communications error
            // see https://www.arduino.cc/en/Reference/WireEndTransmission for 
            // what the number means
            Serial.print("RTC communications error = ");
            Serial.println(Rtc.LastError());
        }
        else
        {
            // Common Causes:
            //    1) the battery on the device is low or even missing and the power line was disconnected
            Serial.println("RTC lost confidence in the DateTime!");
        }
    }
 
     
    RtcDateTime now = Rtc.GetDateTime();
    //Serial.print(now);
    //Serial.print(",");
    Serial.print("the date and time are");
    printDateTime(now);
    Serial.print(",");
    //Serial.println();
 
   RtcTemperature temp = Rtc.GetTemperature();
   //temp.Print(Serial);
   // you may also get the temperature as a float and print it
   //t = temp.AsFloatDegC();
   //Serial.print(temp.AsFloatDegC());
   //Serial.print("C");
   //Serial.print(",");
    
    //Set values to send
    myData.id = BOARD_ID;
    myData.x = now;
    //myData.k = k;
    Data.k = k;
    //myData.y = temp.AsFloatDegC();
    
    if (now == 690813750){
      esp_now_send(broadcastAddress1, (uint8_t *) &myData, sizeof(myData));
      Serial.println(now);
      Serial.print("data sent");
    }
    else if (now == 690813800){
      esp_now_send(broadcastAddress1, (uint8_t *) &myData, sizeof(myData));
    }
    else if (now == 690639301){
      esp_now_send(broadcastAddress1, (uint8_t *) &myData, sizeof(myData));
    }
   else if (now == 690639401){
      esp_now_send(broadcastAddress1, (uint8_t *) &myData, sizeof(myData));
    }
    
   else if (now == 690639501){
      esp_now_send(broadcastAddress1, (uint8_t *) &myData, sizeof(myData));
    }
    else{
      esp_now_send(0, 0, 0);
      Serial.println(now);
      Serial.print("Data not sent");
    } 
    
    
 
    // Send message via ESP-NOW
    //esp_now_send(broadcastAddress1, (uint8_t *) &myData, sizeof(myData));
    //esp_now_send(broadcastAddress2, (uint8_t *) &myData, sizeof(myData));
    //esp_now_send(broadcastAddress, (uint8_t *) &DHTReadings, sizeof(DHTReadings));
    Serial.println();
    // Print incoming readings
    //printIncomingReadings();  
  }
  //delay(10000); // ten seconds
}
