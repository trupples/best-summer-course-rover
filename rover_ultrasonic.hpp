#define ULTRASONIC_MM_PER_US 2.9388 // calibrate if needed!
#define ULTRASONIC_TRIG_PIN 4
#define ULTRASONIC_ECHO_PIN 2

#define ULTRASONIC_RATE 100 // 100ms; recommended at least 60ms

void rover_ultrasonic_setup() {
  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
}

float ultrasonic_read_mm() {
  static unsigned long long last_capture = 0;
  static float last_distance = 0;
  if(millis() - last_capture < ULTRASONIC_RATE) return last_distance;
  
  // send a pulse to the TRIG pin
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);

  // wait for the echo and measure its length
  unsigned long echo_microseconds = pulseIn(ULTRASONIC_ECHO_PIN, HIGH, 10000); // 10ms timeout ~ 3m
  if(echo_microseconds == 0) // no pulse
    echo_microseconds = 10000;

  // convert that time to a distance
  return last_distance = echo_microseconds * ULTRASONIC_MM_PER_US;
}
