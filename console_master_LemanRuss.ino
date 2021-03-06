#define pinX          A0
#define pinY          A1
#define keyA          2
#define keyB          3
#define keyC          4
#define keyD          5
#define keyE          6
#define keyF          7
#define keyK          8
#define led           13
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
#define changmod_rc   '<'
#define cam_to_def    ','
int calibrate_X = 512;
int calibrate_Y = 512;
int x = calibrate_X;
int y = calibrate_Y;
bool manual = true;
bool servo = false;
bool fast = false;
bool is_break = false;
void setup() {
  pinMode(led, OUTPUT);
  for (byte i = keyA; i <= keyK; i++)
    pinMode(i, INPUT);
  calibrate_X = analogRead(pinX);
  calibrate_Y = analogRead(pinY);
  Serial.begin(9600);
  while (!Serial);
  while (true) {
    Serial.write(forward_rc);
    digitalWrite(led, HIGH);
    delay(256);
    digitalWrite(led, LOW);
    delay(256);
    if (Serial.available()) {
      while (Serial.read() != breaking_rc);
      digitalWrite(led, HIGH);
      Serial.write(back_rc);
      break;
    }
  }
}
void loop() {
  if (digitalRead(keyE) == LOW) {
    if (servo)
      fast = !fast;
    else {
      Serial.write(changmod_rc);
      manual = !manual;
      if (manual)
        digitalWrite(led, HIGH);
      else
        digitalWrite(led, LOW);
    }
    while (digitalRead(keyE) == LOW);
  }
  if (digitalRead(keyF) == LOW) {
    servo = !servo;
    fast = false;
    while (digitalRead(keyF) == LOW);
  }
  if (digitalRead(keyK) == LOW) {
    Serial.write(servo ? servos_to_def : cam_to_def);
    while (digitalRead(keyK) == LOW);
  }
  y = analogRead(pinY);
  if (servo) {
    if (y < calibrate_Y - bounce)
      Serial.write(back2_s);
    else if (y > calibrate_Y + bounce)
      Serial.write(forward2_s);
  } else if (manual) {
    if (digitalRead(keyA) != LOW && digitalRead(keyB) != LOW &&
        digitalRead(keyC) != LOW && digitalRead(keyD) != LOW) {
      if (!is_break) {
        Serial.write(breaking_rc);
        is_break = true;
      }
    } else {
      Serial.write(128 + y / 8);
      is_break = false;
    }
  }
  x = analogRead(pinX);
  if (x < calibrate_X - bounce)
    Serial.write(servo ? left2_s : left2_rc);
  else if (x > calibrate_X + bounce)
    Serial.write(servo ? right2_s : right2_rc);
  if (digitalRead(keyA) == LOW)
    Serial.write(servo ? forward_s : forward_rc);
  else if (digitalRead(keyC) == LOW)
    Serial.write(servo ? back_s : back_rc);
  if (digitalRead(keyB) == LOW)
    Serial.write(servo ? left_s : left_rc);
  else if (digitalRead(keyD) == LOW)
    Serial.write(servo ? right_s : right_rc);
  if (servo)
    if (fast)
      delay(8);
    else
      delay(32);
  else
    delay(16);
}
