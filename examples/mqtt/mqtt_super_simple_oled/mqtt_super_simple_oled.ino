#define MQTT_DEBUG_MODE
#define WIFI_DEBUG_MODE

#include <WiFiConnector.h>
#include <MqttConnector.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#include <Ticker.h>


/*===== DISPLAY =====*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <ESP_Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

String celsius = "--.--";

MqttConnector *mqtt;
WiFiConnector *wifi;

Ticker flipper;

// MQTT_HOSTT
#define MQTT_HOST "192.168.15.148"
#define MQTT_PORT 1883

#include "init_wifi.h"
#include "_publish.h"
#include "_receive.h"
#include "init_mqtt.h"


const  unsigned char   PROGMEM bitmaptt [] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0x04, 0x20, 0x00,
  0x00, 0x04, 0x20, 0x00, 0x00, 0x04, 0x20, 0x00, 0x00, 0x04, 0x20, 0x00, 0x00, 0x04, 0x20, 0x00,
  0x00, 0x04, 0x20, 0x00, 0x00, 0x04, 0x20, 0x00, 0x00, 0x04, 0x20, 0x00, 0x00, 0x04, 0x20, 0x00,
  0x00, 0x04, 0x20, 0x00, 0x00, 0x05, 0xA0, 0x00, 0x00, 0x05, 0xA0, 0x00, 0x00, 0x05, 0xA0, 0x00,
  0x00, 0x05, 0xA0, 0x00, 0x00, 0x05, 0xA0, 0x00, 0x00, 0x05, 0xA0, 0x00, 0x00, 0x05, 0xA0, 0x00,
  0x00, 0x05, 0xA0, 0x00, 0x00, 0x05, 0xA0, 0x00, 0x00, 0x05, 0xA0, 0x00, 0x00, 0x0D, 0xB0, 0x00,
  0x00, 0x1B, 0xD0, 0x00, 0x00, 0x17, 0xE8, 0x00, 0x00, 0x17, 0xE8, 0x00, 0x00, 0x17, 0xE8, 0x00,
  0x00, 0x17, 0xE8, 0x00, 0x00, 0x1B, 0xD8, 0x00, 0x00, 0x0C, 0x30, 0x00, 0x00, 0x07, 0xE0, 0x00


};
const unsigned char PROGMEM water [] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x03, 0xC0, 0x00,
  0x00, 0x07, 0xE0, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x1F, 0xF8, 0x00, 0x00, 0x1F, 0xF8, 0x00,
  0x00, 0x3B, 0xFC, 0x00, 0x00, 0x7B, 0xFE, 0x00, 0x00, 0x73, 0xFE, 0x00, 0x00, 0xE7, 0xFF, 0x00,
  0x00, 0xE7, 0xFF, 0x00, 0x01, 0xC7, 0xFF, 0x80, 0x01, 0xCF, 0xFF, 0x80, 0x03, 0x8F, 0xFF, 0xC0,
  0x03, 0x8F, 0xFF, 0xC0, 0x03, 0x0F, 0xFF, 0xC0, 0x07, 0x0F, 0xFF, 0xE0, 0x07, 0x1F, 0xFF, 0xE0,
  0x07, 0x1F, 0xFF, 0xE0, 0x07, 0x1F, 0xFF, 0xE0, 0x07, 0x1F, 0xFF, 0xE0, 0x07, 0x1F, 0xFF, 0xE0,
  0x03, 0x8F, 0xFF, 0xC0, 0x03, 0x8F, 0xFF, 0xC0, 0x01, 0xCF, 0xFF, 0x80, 0x01, 0xEF, 0xFF, 0x80,
  0x00, 0xFF, 0xFF, 0x00, 0x00, 0x7F, 0xFE, 0x00, 0x00, 0x1F, 0xF8, 0x00, 0x00, 0x03, 0xC0, 0x00
};
const unsigned char PROGMEM C [] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x03, 0xF8, 0x1E,
  0x07, 0xFC, 0x3F, 0x0E, 0x0C, 0x33, 0x1C, 0x04, 0x33, 0x18, 0x00, 0x33, 0x18, 0x00, 0x3F, 0x18,
  0x00, 0x1E, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x1C, 0x04, 0x00, 0x0C, 0x04,
  0x00, 0x0F, 0x0C, 0x00, 0x07, 0xF8, 0x00, 0x01, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const unsigned char PROGMEM Temperature_water_p [] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x03, 0xC0, 0x00,
  0x00, 0x03, 0xC0, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x03, 0xFF, 0x80, 0x00, 0x03, 0xFF, 0x00,
  0x00, 0x03, 0xC0, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x03, 0xFF, 0x80,
  0x00, 0x03, 0xFF, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x03, 0xC0, 0x00,
  0x00, 0x03, 0xFF, 0x80, 0x00, 0x03, 0xFF, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x03, 0xC0, 0x00,
  0x00, 0x03, 0xC0, 0x00, 0xFE, 0x07, 0xE0, 0x7F, 0xFF, 0xC7, 0xE3, 0xFF, 0x83, 0xC7, 0xE3, 0xC1,
  0x00, 0x03, 0xC0, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x3C, 0x3C, 0x1C,
  0x3F, 0xFF, 0xFF, 0xFC, 0x07, 0xE7, 0xE7, 0xE0, 0x03, 0x81, 0x81, 0xC0, 0x00, 0x00, 0x00, 0x00

};

void init_hardware()
{
  Serial.begin(9600);
  delay(10);
  Serial.println();
  Serial.println("BEGIN");

  display.begin(SSD1306_SWITCHCAPVCC, 0x78 >> 1);
  display.println("begin");
  display.display();
}

void setup()
{
  init_hardware();
  init_wifi();
  init_mqtt();
  flipper.attach(0.5, [&]() { print_display(String(celsius)); });
}

void loop()
{
  mqtt->loop(wifi);
}

void print_display(String text) {
  display.clearDisplay();
  display.drawBitmap(0, 16,  Temperature_water_p, 32, 32, 1);
  display.drawBitmap(105, 20,  C, 24, 24, 1);
  display.setTextSize(2); 
  display.setTextColor(WHITE); 
  display.setCursor(40, 24);
  display.print(text.c_str());
  display.display();
}

void print_text(String text) {
  display.clearDisplay();
  display.setTextSize(1); 
  display.setTextColor(WHITE);   
  display.setCursor(0,0);
  display.print(text.c_str());
  display.display();  
}