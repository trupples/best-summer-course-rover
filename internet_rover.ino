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
  
  rover_wifi_setup();
  rover_camera_setup();
  rover_motors_setup();
  rover_ultrasonic_setup();
  rover_http_setup();
}

void loop() {
  // all the logic is controlled within the HTTP server. That runs in parallel
  // with loop(). If we need any more logic independent of the server, we would
  // place that here, but currently there is nothing to be done besides what the
  // server already does. That might include stuff like PID controllers for the
  // motors, ping/heartbeat packets, etc.


  move_linear(1);
  delay(100);
  move_linear(0.8);

  while(ultrasonic_read_mm() > 2000) {
    delay(10);
  }

  move_linear(-1);
  delay(500);
  move_linear(0);
  delay(100);

  do {
    set_left_motor(1);
    set_right_motor(0);
    delay(250);
    set_left_motor(0);
    set_right_motor(0);
    delay(250);
    set_left_motor(0);
    set_right_motor(-1);
    delay(250);
    set_left_motor(0);
    set_right_motor(0);
    delay(250);
  } while(ultrasonic_read_mm() < 3000);

  // repeat...
}
