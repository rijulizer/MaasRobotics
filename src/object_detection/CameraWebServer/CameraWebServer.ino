#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include "ESP32Servo.h"
#include <HardwareSerial.h>

// define the board
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
#define SERVO_PIN 12
// remap the open pins
#define TX_PIN 14  // Pin for TX (connect to RX of the microcontroller)
#define RX_PIN 15  // Pin for RX (optional, for bidirectional communication)
HardwareSerial MCSerial(2); // Use Serial1 for UART communication

#include "camera_pins.h"
// define id,password
const char *ssid = "ESP_AP";
const char *password = "12345678";
// define ip, gateway and subnet
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
// setup camera server
void startCameraServer();
void setupLedFlash(int pin);
// setup http server at port 82
WebServer server(82);
// Create the servo object
Servo servo_cam;


void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  // start a serial UART to communicate with microcontroller
  MCSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

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
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

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

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  s->set_vflip(s, 1);
#endif

// Setup LED FLash if LED pin is defined in camera_pins.h
#if defined(LED_GPIO_NUM)
  setupLedFlash(LED_GPIO_NUM);
#endif
  // Accesspoint mode
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  // webserver related code
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.on("/test_get", handle_TestGet);
  server.on("/test_post", handle_TestPost);
  server.on("/control_servo", servo_control);
  // start camera server
  startCameraServer();
  // start http server
  server.begin();

  Serial.print("Camera + HTTP server started! Use 'http://");
  // Serial.print(WiFi.localIP());
  Serial.print(local_ip);
  Serial.println("' to connect");
  // attach servo pin
  servo_cam.attach(SERVO_PIN);
  servo_cam.write(60);
  Serial.print("Servo connected to pin: ");
  Serial.print(SERVO_PIN);

}
void handle_OnConnect() {
  Serial.println("Client connected to the server.");
  server.send(200, "text/plain", "Welcome to the ESP32-CAM!");
}

void handle_NotFound() {
  Serial.println("404: Not Found. A client tried accessing a non-existent route.");
  server.send(404, "text/plain", "404: Not Found");
}

void handle_TestGet() {
  Serial.println("GET request received on /test_get");
  String received_data = "GET request successful! Here is your response.";
  // receive data via UART
  if (MCSerial.available() > 0){
    received_data = MCSerial.readString();
    Serial.print(received_data);
  }
  server.send(200, "text/plain", received_data);
}

void handle_TestPost() {
  Serial.println("POST request received on /test_post");
  
  // Read the POST body data
  String body = server.arg("plain");
  Serial.println("POST Body:");
  Serial.println(body);
  
  // Send the data via UART
  MCSerial.println(String(body));
  Serial.println("Data sent to the microcontroller via UART.");
  // Send response
  server.send(200, "text/plain", "POST request received and processed. Data:\n" + body);
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
  server.send(200);
}
void loop() {
  // Do nothing. Everything is done in another task by the web server
  server.handleClient();
  delay(10000);
}
