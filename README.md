# m5fire-camera-viewer
The image is produced by camera on ESP-EYE V2.1 dev board, running espressif/esp-who/examples/single-chip/camera_web_server.

By default, camera_web_server provides two outputs:
1. MJPEG stream, using url as "http://192.168.4.1" or "http://192.168.4.1/stream". MJPEG stream url doesn't work, don't know why currently
2. static JPEG image, using url as "http://192.168.4.1/capture"


Basically the file size of jpeg image provided by ESP-EYE is around 10KB.
Allocating buffer in heap instead of stack, as it is always crash in my M5STACK_FIRE kit.

For M5STACK_FIRE kit, the LCD resolution is 320 x 240 pixels, and 1 unit of the parameter of M5.Lcd.setTextSize function is 10 pixels. In this implementation, the layout design is as below:
1. for information text: (pos.x = 0, pos.y = 0, width = 320, height = 20)
2. for jpeg image: (pox.x = 0, pos.y = 0, width = 320, height = 220)
Be noted that jpeg image drawing is starting from right-bottom corner, so modify the "height" directly will produce the effect wanted.
