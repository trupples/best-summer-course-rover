// config
#define PWM_MIN 140 // calibrate to be the lowest number for which the wheel spins
#define PWM_MAX 200 // calibrate so the max speed isn't very fast

void set_left_motor(float x) {
  if(x < -1) x = -1;
  if(x > 1) x = 1;
  if(x >= 0.1) {
    ledcWrite(4, x * (PWM_MAX-PWM_MIN) + PWM_MIN);
    ledcWrite(5, 0);
  } else if(x <= -0.1) {
    ledcWrite(4, 0);
    ledcWrite(5, -x * (PWM_MAX-PWM_MIN) + PWM_MIN);
  } else {
    ledcWrite(4, 0);
    ledcWrite(5, 0);
  }
}

void set_right_motor(float x) {
  if(x < -1) x = -1;
  if(x > 1) x = 1;
  if(x >= 0.1) {
    ledcWrite(2, x * (PWM_MAX-PWM_MIN) + PWM_MIN);
    ledcWrite(3, 0);
  } else if(x <= -0.1) {
    ledcWrite(2, 0);
    ledcWrite(3, -x * (PWM_MAX-PWM_MIN) + PWM_MIN);
  } else {
    ledcWrite(2, 0);
    ledcWrite(3, 0);
  }
}

void move_linear(float x) {
  set_left_motor(x);
  set_right_motor(x);
}

void turn(float x) {
  if(x > 0) {
    set_left_motor(x);
    set_right_motor(0);
  } else {
    set_left_motor(0);
    set_right_motor(-x);
  }
}

void rover_motors_setup() {
  ledcSetup(2, 20000, 8); ledcAttachPin(14, 2);
  ledcSetup(3, 20000, 8); ledcAttachPin(15, 3);
  ledcSetup(4, 20000, 8); ledcAttachPin(12, 4);
  ledcSetup(5, 20000, 8); ledcAttachPin(13, 5);
}
