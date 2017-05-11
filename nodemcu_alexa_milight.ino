/**
  ******************************************************************************
  * file		nodemcu_alexa_milight.ino
  * author		Mr.Y
  * date		2017-3-21
  * brief		Control milight by a nodemcu and a nRF24L01+ using Amazon Alexa.
  ******************************************************************************
**/
#include <OpenMiLight.h>
#include <SPI.h>
#include <RF24.h>
#include <PL1167_nRF24.h>
#include <MiLightRadio.h>
#include <CallbackFunction.h>
#include <Switch.h>
#include <UpnpBroadcastResponder.h>

#define CE_PIN D2
#define CSN_PIN D8

void bedroomLightOn();
void bedroomLightOff();
void livingroomLightOn();
void livingroomLightOff();

const char* ssid = "****";
const char* password = "****";

boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;

Switch *bedroom = NULL;
Switch *livingroom = NULL;

RF24 radio(CE_PIN, CSN_PIN);
PL1167_nRF24 prf(radio);
MiLightRadio mlr(prf);
OpenMiLight MiLight(mlr);

void setup()
{
    Serial.begin(115200);
    MiLight.begin();
    wifiConnected = connectWifi();
    if(wifiConnected){
    upnpBroadcastResponder.beginUdpMulticast();
    
    // Define your switches here. Max 14
    // Format: Alexa invocation name, local port no, on callback, off callback
    bedroom = new Switch("bedroom light", 80, bedroomLightOn, bedroomLightOff);
    livingroom = new Switch("living room light", 81, livingroomLightOn, livingroomLightOff);

    Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*bedroom);
    upnpBroadcastResponder.addDevice(*livingroom);
  }
}

void loop()
{
    if (mlr.available()) {
        uint8_t packet[7];
        size_t packet_length = sizeof(packet);
        mlr.read(packet, packet_length);
        printf("\nReceived packet: ");
        for (int i = 0; i < packet_length; i++) {
            printf("%02X", packet[i]);
        }
    }
	
    if(wifiConnected){
        upnpBroadcastResponder.serverLoop();
        bedroom->serverLoop();
        livingroom->serverLoop();
    }
	
    if (Serial.available())
	{
        uint8_t c = Serial.read();
		switch (c)
		{
            case '1':
                MiLight.set_pair(1);
                break;
            case '2':
                MiLight.set_pair(2);
                break;
            case '3':
                MiLight.set_status(1, true);
                break;
            case '4':
                MiLight.set_status(1, false);
                break;
            case '5':
                MiLight.set_status(2, true);
                break;
            case '6':
                MiLight.set_status(2, false);
                break;
            default:break;
        }
    }
}

void bedroomLightOn() {
    Serial.print("Bedroom light turn on ...");
    MiLight.set_status(1, true);
    MiLight.set_color_default(1);
}

void bedroomLightOff() {
    Serial.print("Bedroom light turn off ...");
    MiLight.set_status(1, false);
}

void livingroomLightOn() {
    Serial.print("Living room light turn on ...");
    MiLight.set_status(2, true);
    MiLight.set_color_default(2);
}

void livingroomLightOff() {
    Serial.print("Living room light turn off ...");
    MiLight.set_status(2, false);
}

boolean connectWifi(){
    boolean state = true;
    int i = 0;
  
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");
    Serial.println("Connecting to WiFi");

    // Wait for connection
    Serial.print("Connecting ...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if (i > 10){
            state = false;
            break;
        }
        i++;
    }
  
    if (state){
        Serial.println("");
        Serial.print("Connected to ");
        Serial.println(ssid);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }
    else {
        Serial.println("");
        Serial.println("Connection failed.");
	}
  
    return state;
}
