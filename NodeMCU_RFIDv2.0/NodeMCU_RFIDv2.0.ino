//*******************************libraries********************************
//RFID-----------------------------
#include <SPI.h>
#include <MFRC522.h>
//NodeMCU--------------------------
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
//************************************************************************
/*Defining RFID MF-RC522 Module pins */
#define SS_PIN  D8  //D8
#define RST_PIN D3  //D3
//************************************************************************
MFRC522 mfrc522(SS_PIN, RST_PIN); // Creating instance of MFRC522.
//************************************************************************
/* Setting the desired Wifi credentials. */
const char *ssid = "netis_54A575";
const char *password = "goswamihiya202";
const char* device_token  = "c5b408e62be0db2e"; //copy the device token from the device section
//************************************************************************
String URL = "http://192.168.1.15/rfidattendance/getdata.php"; //computer IP or the server domain
String getData, Link;
String OldCardID = "";
unsigned long previousMillis = 0;
//************************************************************************
void setup() {
  delay(1000);
  Serial.begin(115200);
  SPI.begin();  // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  //---------------------------------------------
  connectToWiFi();
}
//************************************************************************
void loop() {
  //checking Wi-Fi connection
  if(!WiFi.isConnected()){
    connectToWiFi();    //Retry to connect to Wi-Fi
  }
  //---------------------------------------------
  if (millis() - previousMillis >= 15000) {
    previousMillis = millis();
    OldCardID="";
  }
  delay(50);
  //---------------------------------------------
  //looking for new card
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;//if no card present start loop again
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
    }
  String CardID ="";
  for (byte p = 0; p < mfrc522.uid.size; p++) {
    CardID += mfrc522.uid.uidByte[p];
  }
  //----------x-----------------
  if( CardID == OldCardID ){
    return;
  }
  else{
    OldCardID = CardID;
  }
  //------------x-----------------
//  Serial.println(CardID);
  SendCardID(CardID);
  delay(1000);
}
/************sending the Card UID to the website*************/
void SendCardID( String Card_uid ){
  Serial.println("Sending the Card ID");
  if(WiFi.isConnected()){
    HTTPClient http;    //Declaring object of class HTTPClient
    //GET Data
    getData = "?card_uid=" + String(Card_uid) + "&device_token=" + String(device_token);
    //GET methode
    Link = URL + getData;
    http.begin(Link); //initiate HTTP request   //Specify content-type header
    
    int httpCode = http.GET();   //Send the request
    String payload = http.getString();    //Get the response payload

//    Serial.println(Link);   //Print HTTP return code
    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(Card_uid);     //Print Card ID
    Serial.println(payload);    //Print request response payload

    if (httpCode == 200) {
      if (payload.substring(0, 5) == "login") {
        String user_name = payload.substring(5);
    //  Serial.println(user_name);

      }
      else if (payload.substring(0, 6) == "logout") {
        String user_name = payload.substring(6);
    //  Serial.println(user_name);
        
      }
      else if (payload == "succesful") {

      }
      else if (payload == "available") {

      }
      delay(100);
      http.end();  //Close connection
    }
  }
}
/********************connect to the WiFi******************/
void connectToWiFi(){
    WiFi.mode(WIFI_OFF);        //Prevents reconnection issue 
    delay(1000);
    WiFi.mode(WIFI_STA);
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("Connected");
  
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());  //IP address assigned to your ESP
    
    delay(1000);
}
//============================END CODE=================================
