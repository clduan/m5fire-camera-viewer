/*  
 ESP-EYE dev board + M5Fire stack
 For ESP-EYE dev board, average image size is around 10KB
 */

#include <M5Stack.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char * ssid     = "ESP32-Camera";
const char * password = "Hello123$";
const char * url      = "http://192.168.4.1/capture";
//const char * url      = "http://192.168.4.1/stream";

// take down battery value
int prev_battery = 0;

// read battery level
int8_t getBatteryLevel() {
  Wire.beginTransmission(0x75);
  Wire.write(0x78);
  if (Wire.endTransmission(false) == 0
   && Wire.requestFrom(0x75, 1)) {
    switch (Wire.read() & 0xF0) {
    case 0xE0: return 25;
    case 0xC0: return 50;
    case 0x80: return 75;
    case 0x00: return 100;
    default: return 0;
    }
  }
  
  return -1;
}

// print battery level
void printBatteryLevel(int level) {
  M5.Lcd.setRotation(1);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.fillRect(0, 0, 320, 20, BLACK); // 20 pixels = 2 text size
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("Battery : ");
  M5.Lcd.print(level);
  M5.Lcd.print("%");
}

void setup(void) {
  M5.begin();
  
  // for battery level reading
  Wire.begin();
  prev_battery = getBatteryLevel();
  printBatteryLevel(prev_battery);
  
  WiFi.begin(ssid, password);
  delay(1000);
  while (WL_CONNECTED != WiFi.status()) {
    Serial.println("connecting to Wi-Fi ...");
    delay(1000);
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {

  while (WL_CONNECTED != WiFi.status()) {
    Serial.println("connection lost, waiting for Wi-Fi re-connection ...");
    delay(1000);
  }

  // print battery level, and only re-print when battery level changes
  int curr_battery = getBatteryLevel();
  if (curr_battery != prev_battery) {
    printBatteryLevel(curr_battery);
    // update prev_battery
    prev_battery = curr_battery;
  }

  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  
  // configure server and url
  http.begin(url);
  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    
    if (httpCode == HTTP_CODE_OK) {
      // get lenght of document (is -1 when Server sends no Content-Length header)
      int len = http.getSize();
      Serial.printf("image size = %dB\n", len);
      
      // create buffer for read
      uint8_t * buf = (uint8_t *)malloc(len * sizeof(uint8_t));
      uint8_t * ptr = buf;
      
      // get tcp stream
      WiFiClient * stream = http.getStreamPtr();
      
      // read all data from server
      int left = len;
      while (http.connected() && (left > 0 || len == -1)) {
        // get available data size
        size_t size = stream->available();
        if (size) {
          // confirm buffer is large enough for available data
          int s = ((size > len) ? len : size);
          int c = stream->readBytes(ptr, s);
          ptr += c;
          Serial.printf("read bytes = %dB\n", c);
          
          if (left > 0) { left -= c; }
        }
      }
      
      Serial.println("[HTTP] connection closed or file end.");
      Serial.println();

      M5.Lcd.setRotation(5);
      // draw image
      //M5.Lcd.drawJpg(buf, len);
      M5.Lcd.drawJpg(buf, len, 0, 0, 320, 220);
      
      free(buf);
      buf = NULL;
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}
