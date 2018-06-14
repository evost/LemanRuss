#define switchPinA    11
#define switchPinB    12
#define pinAX         A0
#define pinAY         A1
#define pinBX         A2
#define pinBY         A3
#define ledPin        13
#define bounce        256
#define forward_s     'W'
#define back_s        'S'
#define right_s       'D'
#define left_s        'A'
#define forward2_s    'I'
#define back2_s       'K'
#define right2_s      'L'
#define left2_s       'J'
#define servos_to_def '.'
#define breaking_rc   ' '
#define forward_rc    'w'
#define back_rc       's'
#define right_rc      'd'
#define left_rc       'a'
#define forward2_rc   'i'
#define back2_rc      'k'
#define right2_rc     'l'
#define left2_rc      'j'
#define cam_to_def    ','
int calibrateAX = 512;
int calibrateAY = 512;
int calibrateBX = 512;
int calibrateBY = 512;
int ax = calibrateAX;
int ay = calibrateAY;
int bx = calibrateBX;
int by = calibrateBY;
bool servo = true;
bool fast = false;
char cur_cmd = breaking_rc;
char next_cmd = cur_cmd;
void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(switchPinA, INPUT);
  digitalWrite(switchPinA, HIGH);
  pinMode(switchPinB, INPUT);
  digitalWrite(switchPinB, HIGH);
  calibrateAX = analogRead(pinAX);
  calibrateAY = analogRead(pinAY);
  calibrateBX = analogRead(pinBX);
  calibrateBY = analogRead(pinBY);
  Serial.begin(9600);
  while (!Serial);
  digitalWrite(ledPin, HIGH);
}
void loop() {
  if (digitalRead(switchPinB) == LOW) {
    servo = !servo;
    fast = false;
    while (digitalRead(switchPinB) == LOW);
  }
  ax = analogRead(pinAX);
  ay = analogRead(pinAY);
  bx = analogRead(pinBX);
  by = analogRead(pinBY);
  if (by < calibrateBY - bounce)
    Serial.write(back2_s);
  else if (by > calibrateBY + bounce)
    Serial.write(forward2_s);
  if (bx < calibrateBX - bounce)
    Serial.write(left2_s);
  else if (bx > calibrateBX + bounce)
    Serial.write(right2_s);
  if (servo) {
    if (ay < calibrateAY - bounce)
      Serial.write(back_s);
    else if (ay > calibrateAY + bounce)
      Serial.write(forward_s);
    if (ax < calibrateAX - bounce)
      Serial.write(left_s);
    else if (ax > calibrateAX + bounce)
      Serial.write(right_s);
    if (digitalRead(switchPinA) == LOW) {
      fast = !fast;
      while (digitalRead(switchPinA) == LOW);
    }
  } else {
    if (ay < calibrateAY - bounce)
      next_cmd = back_rc;
    else if (ay > calibrateAY + bounce)
      next_cmd = forward_rc;
    else if (ax < calibrateAX - bounce)
      next_cmd = left_rc;
    else if (ax > calibrateAX + bounce)
      next_cmd = right_rc;
    else
      next_cmd = breaking_rc;
    if (next_cmd != cur_cmd || next_cmd == breaking_rc) {
      cur_cmd = next_cmd;
      Serial.write(cur_cmd);
    }
    if (digitalRead(switchPinA) == LOW) {
      Serial.write(servos_to_def);
      Serial.write(cam_to_def);
      while (digitalRead(switchPinA) == LOW);
    }
  }
  if (servo)
    if (fast)
      delay(8);
    else
      delay(32);
  else
    delay(16);
}
