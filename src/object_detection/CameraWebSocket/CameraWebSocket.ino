#include "esp_camera.h"
#include <WiFi.h>
// #include <WebServer.h>
#include <WebSocketsClient.h>
#include "ESP32Servo.h"
#include <HardwareSerial.h>

// define the board
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
#include "camera_pins.h"

// remap the open pins
#define TX_PIN 14  // Pin for TX (connect to RX of the microcontroller)
#define RX_PIN 15  // Pin for RX (optional, for bidirectional communication)
HardwareSerial MCSerial(2); // Use Serial1 for UART communication


// define id,password
const char *ssid = "ESP_AP";
const char *password = "12345678";

// define ip, gateway and subnet
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

// Websocket Server 
// step1: check the ip assigned to the laptop after connecting to the esp AP
// step2: create  websocket server in that host set it (0.0.0.0)
// step3: Client should listen to that address
const char* websocket_server = "192.168.1.2";
const uint16_t websocket_port = 8000;

// Create the servo object
Servo servo_cam;
#define SERVO_PIN 12

// Websocket client
WebSocketsClient webSocket;

// Function prototypes
void takeSnapsSend();
void handleWebSocketMessage(const char* message);


void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;  // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      // Limit the frame size when PSRAM is not available
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // Best option for face detection/recognition
    config.frame_size = FRAMESIZE_240X240;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);        // flip it back
    s->set_brightness(s, 1);   // up the brightness just a bit
    s->set_saturation(s, -2);  // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  if (config.pixel_format == PIXFORMAT_JPEG) {
    s->set_framesize(s, FRAMESIZE_QVGA);
  }

  // Wi-Fi connection -- Accesspoint mode
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  Serial.println("\nWiFi connected...");
  Serial.println(WiFi.softAPIP());

  // start a serial UART to communicate with microcontroller
  MCSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial.println("Second Serial communication started");

  // attach servo pin
  servo_cam.attach(SERVO_PIN);
  servo_cam.write(60);
  Serial.print("Servo connected to pin: ");
  Serial.print(SERVO_PIN);
  
  // Initialize WebSocket
  webSocket.begin(websocket_server, websocket_port, "/");
  webSocket.onEvent([](WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
      case WStype_CONNECTED:
        Serial.println("WebSocket connected");
        break;
      case WStype_TEXT:
        Serial.printf("Received message: %s\n", payload);
        handleWebSocketMessage((const char*)payload);
        break;
      case WStype_DISCONNECTED:
        Serial.println("WebSocket disconnected");
        break;
      default:
        break;
    }
  });
}

void loop() {
  // Do nothing. Everything is done in another task by the web server
  webSocket.loop();
}

void handleWebSocketMessage(const char* message) {
  if (strcmp(message, "servo_control") == 0) {
    servo_control();
  } else if (strcmp(message, "take_snap") == 0) {
    takeSnapsSend();
  } else if (strcmp(message, "test_get") == 0) {
    handle_TestGet();
  } else if (strcmp(message, "test_post") == 0) {
    handle_TestPost();
  } else {
    Serial.println("Unknown message received");
  }

}
void handle_TestGet() {
  Serial.println("GET request received on /test_get");
  String received_data = "GET request successful! Here is your response.";
  // receive data via UART
  if (MCSerial.available() > 0){
    received_data = MCSerial.readString();
    Serial.print(received_data);
  }
  // server.send(200, "text/plain", received_data);
}

void handle_TestPost() {
  Serial.println("POST request received on /test_post");
  
  // Read the POST body data
  // String body = server.arg("plain");
  Serial.println("POST Body:");
  // Serial.println(body);
  
  // Send the data via UART
  // MCSerial.println(String(body));
  Serial.println("Data sent to the microcontroller via UART.");
  // Send response
  // server.send(200, "text/plain", "POST request received and processed. Data:\n" + body);
}
void servo_control(){
  // rotate the motor clockwise
  servo_cam.write(50);
  delay(1000);
  // rotate the motor counter-clockwise
  servo_cam.write(70);
  delay(1000);
   // bring back to center
  servo_cam.write(60);
  delay(1000);
  // server.send(200);
}
// Take a snapshot and send it via WebSocket
void takeSnapsSend() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }
  // Send image as binary data
  webSocket.sendBIN(fb->buf, fb->len);
  Serial.println("Image sent to server...");
  // Return the frame buffer
  esp_camera_fb_return(fb);
}