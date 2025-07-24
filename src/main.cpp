#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include "fauxmoESP.h"

// Rename the credentials.sample.h file to credentials.h and
// edit it according to your router configuration
#include "credentials.h"

fauxmoESP fauxmo;

// -----------------------------------------------------------------------------

#define SERIAL_BAUDRATE 115200

#define A_ON D0
#define A_OFF D1
#define B_ON D2
#define B_OFF D3
#define C_ON D4
#define C_OFF D5
#define D_ON D6
#define D_OFF D7

#define ID_A "Steckdose A"
#define ID_B "Steckdose B"
#define ID_C "Steckdose C"
#define ID_D "Steckdose D"

boolean setA = false;
boolean setB = false;
boolean setC = false;
boolean setD = false;
boolean setState = false;

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------

void wifiSetup()
{

    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);

    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

void setup()
{

    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    Serial.println();

    // LEDs
    pinMode(A_ON, OUTPUT);
    pinMode(A_OFF, OUTPUT);
    pinMode(B_ON, OUTPUT);
    pinMode(B_OFF, OUTPUT);
    pinMode(C_ON, OUTPUT);
    pinMode(C_OFF, OUTPUT);
    pinMode(D_ON, OUTPUT);
    pinMode(D_OFF, OUTPUT);
    digitalWrite(A_ON, HIGH);
    digitalWrite(A_OFF, HIGH);
    digitalWrite(B_ON, HIGH);
    digitalWrite(B_OFF, HIGH);
    digitalWrite(C_ON, HIGH);
    digitalWrite(C_OFF, HIGH);
    digitalWrite(D_ON, HIGH);
    digitalWrite(D_OFF, HIGH);

    // Wifi
    wifiSetup();

    // By default, fauxmoESP creates it's own webserver on the defined port
    // The TCP port must be 80 for gen3 devices (default is 1901)
    // This has to be done before the call to enable()
    fauxmo.createServer(true); // not needed, this is the default value
    fauxmo.setPort(80);        // This is required for gen3 devices

    // You have to call enable(true) once you have a WiFi connection
    // You can enable or disable the library at any moment
    // Disabling it will prevent the devices from being discovered and switched
    fauxmo.enable(true);

    // You can use different ways to invoke alexa to modify the devices state:
    // "Alexa, turn yellow lamp on"
    // "Alexa, turn on yellow lamp
    // "Alexa, set yellow lamp to fifty" (50 means 50% of brightness, note, this example does not use this functionality)

    // Add virtual devices
    fauxmo.addDevice(ID_A);
    fauxmo.addDevice(ID_B);
    fauxmo.addDevice(ID_C);
    fauxmo.addDevice(ID_D);

    fauxmo.onSetState([](unsigned char device_id, const char *device_name, bool state, unsigned char value)
                      {
                          // Callback when a command from Alexa is received.
                          // You can use device_id or device_name to choose the element to perform an action onto (relay, LED,...)
                          // State is a boolean (ON/OFF) and value a number from 0 to 255 (if you say "set kitchen light to 50%" you will receive a 128 here).
                          // Just remember not to delay too much here, this is a callback, exit as soon as possible.
                          // If you have to do something more involved here set a flag and process it in your main loop.

                          Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);

                          // Checking for device_id is simpler if you are certain about the order they are loaded and it does not change.
                          // Otherwise comparing the device_name is safer.

                          if (strcmp(device_name, ID_A) == 0)
                          {
                              setA = true;
                              setState = state;
                          }
                          else if (strcmp(device_name, ID_B) == 0)
                          {
                              setB = true;
                              setState = state;
                          }
                          else if (strcmp(device_name, ID_C) == 0)
                          {
                              setC = true;
                              setState = state;
                          }
                          else if (strcmp(device_name, ID_D) == 0)
                          {
                              setD = true;
                              setState = state;
                          } });
}

void loop()
{

    // fauxmoESP uses an async TCP server but a sync UDP server
    // Therefore, we have to manually poll for UDP packets
    fauxmo.handle();

    // This is a sample code to output free heap every 5 seconds
    // This is a cheap way to detect memory leaks
    static unsigned long last = millis();
    if (millis() - last > 5000)
    {
        last = millis();
        Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
    }

    // If your device state is changed by any other means (MQTT, physical button,...)
    // you can instruct the library to report the new state to Alexa on next request:
    // fauxmo.setState(ID_YELLOW, true, 255);

    if (setA)
    {
        for (int i = 0; i < 2; i++)
        {
            digitalWrite(A_ON, setState ? LOW : HIGH);
            digitalWrite(A_OFF, setState ? HIGH : LOW);
            delay(50);
            digitalWrite(A_ON, HIGH);
            digitalWrite(A_OFF, HIGH);
            delay(50);
        }
        setA = false;
    }
    else if (setB)
    {
        for (int i = 0; i < 2; i++)
        {
            digitalWrite(B_ON, setState ? LOW : HIGH);
            digitalWrite(B_OFF, setState ? HIGH : LOW);
            delay(50);
            digitalWrite(B_ON, HIGH);
            digitalWrite(B_OFF, HIGH);
            delay(50);
        }
        setB = false;
    }
    else if (setC)
    {
        for (int i = 0; i < 2; i++)
        {
            digitalWrite(C_ON, setState ? LOW : HIGH);
            digitalWrite(C_OFF, setState ? HIGH : LOW);
            delay(50);
            digitalWrite(C_ON, HIGH);
            digitalWrite(C_OFF, HIGH);
            delay(50);
        }
        setC = false;
    }
    else if (setD)
    {
        for (int i = 0; i < 2; i++)
        {
            digitalWrite(D_ON, setState ? LOW : HIGH);
            digitalWrite(D_OFF, setState ? HIGH : LOW);
            delay(50);
            digitalWrite(D_ON, HIGH);
            digitalWrite(D_OFF, HIGH);
            delay(50);
        }
        setD = false;
    }
}
