#include "esp_camera.h"
#include "camera_pins.h"

// config
#define IMAGE_RESOLUTION FRAMESIZE_VGA // VGA 640x480; SVGA 800x600
#define CAPTURE_RATE 100 // 100ms ~ 10FPS

byte *jpeg_buf; // this will store the last captured image
size_t jpeg_len; // length in bytes of the last captured image
unsigned long long last_capture; // this will be the output of millis() at the last capture

void rover_camera_setup() {
  // These can be ignored
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
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  config.frame_size = IMAGE_RESOLUTION;
  config.jpeg_quality = 10;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
  }
}

esp_err_t rover_camera_capture() {
  // if we request an image before 100ms passed, just use the last captured image!
  if(millis() - last_capture < CAPTURE_RATE) {
    return ESP_OK;
  }

  // get the framebuffer
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
      Serial.println("Camera capture failed!");
      return ESP_FAIL;
  }

  // copy it to jpeg_buf
  jpeg_len = fb->len;
  Serial.print("Length: "); Serial.println(fb->len);
  free(jpeg_buf);
  jpeg_buf = (byte*) ps_malloc(jpeg_len);
  memcpy(jpeg_buf, fb->buf, jpeg_len);

  // then release the memory
  esp_camera_fb_return(fb);

  return ESP_OK;
}
