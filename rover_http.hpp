#include "esp_http_server.h"

// functions that handle the various pages. They are implemented later in this file.
esp_err_t index_handler(httpd_req_t *req); // will handle "/", presenting the webpage interface
esp_err_t command_handler(httpd_req_t *req); // will handle "/command?left=X&right=Y"
esp_err_t sensors_handler(httpd_req_t *req); // will handle "/sensors"
esp_err_t capture_handler(httpd_req_t *req); // will handle "/capture.jpg"

// the corresponding URI data
const httpd_uri_t uri_index = {
    .uri      = "/",
    .method   = HTTP_GET,
    .handler  = index_handler,
    .user_ctx = NULL
};

const httpd_uri_t uri_command = {
    .uri      = "/command",
    .method   = HTTP_GET,
    .handler  = command_handler,
    .user_ctx = NULL
};

const httpd_uri_t uri_sensors = {
    .uri      = "/sensors",
    .method   = HTTP_GET,
    .handler  = sensors_handler,
    .user_ctx = NULL
};

const httpd_uri_t uri_capture = {
    .uri      = "/capture.jpg",
    .method   = HTTP_GET,
    .handler  = capture_handler,
    .user_ctx = NULL
};

void rover_http_setup() {
  Serial.println("Starting HTTP server.");
  
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  httpd_handle_t server = NULL;

  while(httpd_start(&server, &config) != ESP_OK) {
    Serial.println("Could not start httpd. Retrying in 1s.");
    httpd_stop(server);
    delay(1000);
  }
  
  httpd_register_uri_handler(server, &uri_index);
  httpd_register_uri_handler(server, &uri_capture);
  httpd_register_uri_handler(server, &uri_command);
  httpd_register_uri_handler(server, &uri_sensors);

  Serial.println("HTTP server OK!");
}

esp_err_t index_handler(httpd_req_t *req) {  httpd_resp_set_type(req, "text/html");
  httpd_resp_send(req,
"<!DOCTYPE html>" \
"<html>" \
"<body>" \
"<h1>Remote control tancc</h1>" \
"<img id='im'>" \
"<br>" \
"<input type='range' min='-1' max='1' step='0.1' defaultValue='0' orient='vertical' id='L' />" \
"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" \
"<input type='range' min='-1' max='1' step='0.1' defaultValue='0' orient='vertical' id='R' />" \
"<script>" \
"function loadAnotherImage() {"\
"  im.src='/capture.jpg?' + Date.now();"\
"}"\

"loadAnotherImage();"\
"im.onload = loadAnotherImage;"\
"L.oninput = _ => fetch('/command?left=' + L.value, {method: 'GET'});" \
"R.oninput = _ => fetch('/command?right=' + R.value, {method: 'GET'});" \
"</script>" \
"</body>" \
"</html>", HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

esp_err_t command_handler(httpd_req_t *req) {
  int buf_len = httpd_req_get_url_query_len(req) + 1;
  if (buf_len > 1) {
      char *buf = (char*)malloc(buf_len);
      if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
          char param[32];

          if (httpd_query_key_value(buf, "left", param, sizeof(param)) == ESP_OK) set_left_motor(atof(param));
          if (httpd_query_key_value(buf, "right", param, sizeof(param)) == ESP_OK) set_right_motor(atof(param));
      }
      free(buf);
  }

  httpd_resp_sendstr(req, "OK!");
  
  return ESP_OK;
}

esp_err_t sensors_handler(httpd_req_t *req) {
  String sensor_info = String("{") +
    "\"ultrasonic\": " + ultrasonic_read_mm() + "," +
    "\"psram_free\": " + ESP.getFreePsram() +
  "}";
  
  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, sensor_info.c_str(), HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

esp_err_t capture_handler(httpd_req_t *req) {
  if(rover_camera_capture() != ESP_OK) {
    Serial.println("Couldn't get image.");
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  
  httpd_resp_set_type(req, "image/jpeg");
  httpd_resp_send(req, (const char*)jpeg_buf, jpeg_len);
  return ESP_OK;
}
