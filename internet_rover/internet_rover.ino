// This file just brings in all the other components, but doesn't implement any
// logic per se.

#include "rover_wifi.hpp"
#include "rover_camera.hpp"
#include "rover_motors.hpp"
#include "rover_ultrasonic.hpp"
#include "rover_http.hpp"

void setup() {
  Serial.begin(115200);
  Serial.println("Rover booting up!");
  
  rover_camera_setup();
  rover_wifi_setup();
  rover_motors_setup();
  rover_ultrasonic_setup();
  rover_http_setup();
}

void loop() {
  if(millis() > motor_stop_time) {
    set_left_motor(0);
    set_right_motor(0);
  }
}
