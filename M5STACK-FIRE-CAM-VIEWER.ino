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

void setup(void) {
  M5.begin();

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
  
  M5.Lcd.setCursor(0, 0);

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
      
      // draw image
      M5.Lcd.drawJpg(buf, len);
      free(buf);
      buf = NULL;
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}
