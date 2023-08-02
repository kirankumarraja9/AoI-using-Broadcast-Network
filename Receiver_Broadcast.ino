#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>
RtcDS3231<TwoWire> Rtc(Wire);
/* for normal hardware wire use above */
 
/*
// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress1[] = {0xE8, 0xDB, 0x84, 0xDC, 0xF5, 0x46};
//uint8_t broadcastAddress2[] = {0xC4, 0x5B, 0xBE, 0x49, 0x56, 0xFA};
 
#define BOARD_ID 2
*/
 
// Updates DHT readings every 10 seconds
const long interval = 1000; 
unsigned long previousMillis = 0;    // will store last time DHT was updated 
 
int Age;
 
//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    int id; 
    int x;
    float y;       
    int k;
} struct_message;
 
int n =0;
 
// Create a structure to hold the readings from each board
struct_message board1;
struct_message board2;
struct_message board3;
 
// Create an array with all the structures
struct_message boardsStruct[3] = {board1, board2, board3};
 
// Create a struct_message to hold incoming sensor readings
struct_message incomingReadings;
 
 
 
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
 
// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) {
  char macStr[18];
  //Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  //Serial.println(macStr);
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  //Serial.printf("Board ID %u: ", incomingReadings.id); 
  //Serial.print(",");   
  //Serial.printf("Board ID %u: %u bytes\n", incomingReadings.id, len);
  // Update the structures with the new incoming data
  boardsStruct[incomingReadings.id-1].x = incomingReadings.x;
  //boardsStruct[incomingReadings.id-1].y = incomingReadings.y;
  //boardsStruct[incomingReadings.id-1].k = incomingReadings.k;
  //Serial.printf("Incoming Date Time: %d \n", boardsStruct[incomingReadings.id-1].x);
  //Serial.print("Tx Date Time in seconds:");
  //Serial.print(boardsStruct[incomingReadings.id-1].x);
  //Serial.print(",");
  //Serial.println();
  //Serial.print("TRANSMISSION Date Time:");
  RtcDateTime now = boardsStruct[incomingReadings.id-1].x;
  //printDateTime(now);
  //Serial.print(",");
  //Serial.println();
  //Serial.printf("Incoming Temperature: %f \n", boardsStruct[incomingReadings.id-1].y);
  //Serial.printf("z value: %d \n", boardsStruct[incomingReadings.id-1].z);
  //Serial.print(boardsStruct[incomingReadings.id-1].y);
  //Serial.print(",");
  //Serial.println();
  //Serial.print(boardsStruct[incomingReadings.id-1].k);
  //Serial.print(",");
}
 
void setup () 
{
    Serial.begin(9600);
    
     // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    
    n = 1;
    // Init ESP-NOW
    if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
    }
 
    // Set ESP-NOW Role
    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  
    // Register for a callback function that will be called when data is received
    esp_now_register_recv_cb(OnDataRecv);
 
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
    //Serial.println();
 
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
 
    RtcDateTime NOW = Rtc.GetDateTime();
    if (NOW < compiled) 
    {
        //Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (NOW > compiled) 
    {
        //Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (NOW == compiled) 
    {
        //Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }
 
  // never assume the Rtc was last configured by you, so
  // just clear them to your needed state
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);
  int Age = NOW - boardsStruct[incomingReadings.id-1].x;  
}
 
#define countof(b) (sizeof(b) / sizeof(b[0]))
 
 void PrintDateTime(const RtcDateTime& DT)
 {
    char Datestring[20];
    snprintf_P(Datestring, 
            countof(Datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            DT.Month(),
            DT.Day(),
            DT.Year(),
            DT.Hour(),
            DT.Minute(),
            DT.Second() );
    Serial.print(Datestring);
    //Serial.print(dt.Month());
 }
 
 
void loop () 
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the x,y values
    previousMillis = currentMillis;
    //Serial.printf("currentMillis:%d",currentMillis);
    //Serial.print(", ");
    /*
    RtcDateTime now = boardsStruct[incomingReadings.id-1].x;
    printDateTime(now);
    Serial.println();
    Serial.print("Incoming Temperature:");
    Serial.print(incomingReadings.y);  
    Serial.println();
    */
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
  
    RtcDateTime NOW = Rtc.GetDateTime();
    //Serial.print("Rx Date Time in Seconds:");
    Serial.print(NOW);
    Serial.print(",");    
    //Serial.println();
    //Serial.print("Receiving Date Time:");
    PrintDateTime(NOW);
    Serial.print(",");
    int Age = NOW - boardsStruct[incomingReadings.id-1].x;
    Serial.print(boardsStruct[incomingReadings.id-1].x);
    Serial.print(",");
    //Serial.println();
    //Serial.print("Age of Information:");   
    Serial.print(Age);
    Serial.print(",");
    //Serial.print("secs");    
    //Serial.println();  
    //k++;
    //Serial.print(k);
    //Serial.print(",");
    Serial.println();   
  }
  //delay(10000); // ten seconds
}
