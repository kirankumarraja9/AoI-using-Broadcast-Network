#include <ESP8266WiFi.h>
#include <espnow.h>
 
// REPLACE WITH RECEIVER MAC Address
//uint8_t broadcastAddress1[] = {0xC4, 0x5B, 0xBE, 0x48, 0xB2, 0xCA};
uint8_t broadcastAddress1[] = {0x3C, 0x61, 0x05, 0xE9, 0x65, 0xBE};
//uint8_t broadcastAddress2[] = {0xC4, 0x5B, 0xBE, 0x49, 0x56, 0xFA};
 
#define BOARD_ID 2
//#define BOARD_ID 3
 
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
 
// Create a structure to hold the readings from each board
struct_message board1;
struct_message board2;
struct_message board3;
 
// Create an array with all the structures
struct_message boardsStruct[3] = {board1, board2, board3};
 
// Create a struct_message to hold incoming sensor readings
struct_message incomingReadings;
 
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
 
// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.printf("Board ID %u: %u bytes\n", incomingReadings.id, len);
  // Update the structures with the new incoming data
  boardsStruct[incomingReadings.id-1].x = incomingReadings.x;
  //boardsStruct[incomingReadings.id-1].y = incomingReadings.y;
  //boardsStruct[incomingReadings.id-1].z = incomingReadings.z;
  Serial.printf("Incoming Date Time: %d \n", boardsStruct[incomingReadings.id-1].x);
  //Serial.printf("Incoming Temperature: %f \n", boardsStruct[incomingReadings.id-1].y);
  //Serial.printf("z value: %d \n", boardsStruct[incomingReadings.id-1].z);
  Serial.println();
}
 
void setup () 
{
    Serial.begin(9600);
    
    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
 
    // Init ESP-NOW
    if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
    }
 
    // Set ESP-NOW Role
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
 
    // Once ESPNow is successfully Init, we will register for Send CB to
   // get the status of Trasnmitted packet
    esp_now_register_send_cb(OnDataSent);
 
    // Register peer
    esp_now_add_peer(broadcastAddress1, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
    //esp_now_add_peer(broadcastAddress2, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  
    // Register for a callback function that will be called when data is received
    esp_now_register_recv_cb(OnDataRecv);
}
 
 
void loop () 
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the x,y values
    previousMillis = currentMillis;
    
    //Set values to send
    myData.id = BOARD_ID;
    myData.x = boardsStruct[incomingReadings.id-1].x;
    //myData.y = temp.AsFloatDegC();
    Serial.println(incomingReadings.x);
    //DHTReadings.temp = temperature;
    //DHTReadings.hum = humidity;
    esp_now_send(broadcastAddress1, (uint8_t *) &myData, sizeof(myData));
    // Send message via ESP-NOW
    //esp_now_send(broadcastAddress1, (uint8_t *) &myData, sizeof(myData));
    //esp_now_send(broadcastAddress2, (uint8_t *) &myData, sizeof(myData));
    //esp_now_send(broadcastAddress, (uint8_t *) &DHTReadings, sizeof(DHTReadings));
 
    // Print incoming readings
    //printIncomingReadings();  
  }
  //delay(10000); // ten seconds
}
