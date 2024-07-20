#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include <FastLED.h>

const char* ssid = "302-2";// 连接WiFi
const char* password = "ptsfdtz12138";

#define LED_PIN 4        
#define NUM_LEDS 14      
#define BRIGHTNESS 200     
#define LED_TYPE WS2812B  
#define COLOR_ORDER GRB   

CRGB leds[NUM_LEDS];

WebServer server(8000);

void setup() {
  Serial.begin(115200);

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  server.on("/", handleRoot);
  server.on("/setColor", handleSetColor);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  File file = SPIFFS.open("/index.html", "r");
  if (!file) {
    server.send(404, "text/plain", "File not found");
    return;
  }
  String html = file.readString();
  server.send(200, "text/html", html);
  file.close();
}

void handleSetColor() {
  String color = server.arg("color");

  long number = strtol(&color[1], NULL, 16);
  int r = number >> 16;
  int g = number >> 8 & 0xFF;
  int b = number & 0xFF;

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(r, g, b);
  }
  FastLED.show();

  String response = "Color set to: " + color + "<br><a href=\"/\">Back</a>";
  server.send(200, "text/html", response);
}
