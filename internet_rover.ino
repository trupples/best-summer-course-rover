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
}
