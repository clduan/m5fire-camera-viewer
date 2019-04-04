# m5fire-camera-viewer
Connected to ESP-EYE V2.1 dev board, running esp-who/examples/single-chip/camera_web_server.

Basically the file size of jpeg image provided by ESP-EYE is around 10KB.
Allocating buffer in heap instead of stack, as it is always crash in my M5STACK-FIRE kit.
