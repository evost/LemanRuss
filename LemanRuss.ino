#include <Servo.h>
#define PIRpin        22 //датчик движения
#define buzzer        23 //зуммер
#define dir_left      4 //левый мотор
#define speed_left    5 //левый мотор, PWM
#define speed_right   6 //правый мотор, PWM
#define dir_right     7 //правый мотор
#define min_distance  40 //минимальная дистанция для автоповорота, см
#define trigPin       3 //HC-SR04 Trig's
#define echoPin_left  2 //левый HC-SR04 Echo
#define echoPin_right 8 //правый HC-SR04 Echo
#define turn_delay    512 //продолжительность автоповорота, мс
#define servo1_pin    9 //манипулятор влево вправо
#define servo2_pin    10 //манипулятор вверх вниз
#define servo3_pin    11 //зажим вверх вниз
#define servo4_pin    12 //сжатие расжатие зажима
#define servo5_pin    13 //поворот камеры
#define speed_sound   70 //значемние ШИМ для писка моторов
#define speed_def     255 //скорость по умолчанию
//настройки мнимальных, максимальных и углов серв по-умолчанию
#define servo1_min    0
#define servo1_def    75
#define servo1_max    180
#define servo2_min    45
#define servo2_def    45
#define servo2_max    145
#define servo3_min    0
#define servo3_def    0
#define servo3_max    180
#define servo4_min    60
#define servo4_def    60
#define servo4_max    180
#define servo5_min    0
#define servo5_def    90
#define servo5_max    180
#define servo_angle   1  //шаг поворота сервы за одну команду
//команды должны быть #0..#127
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
//#define DEBUG         true //включение отладки по Serial
unsigned long standby_time = 0; //переменная отсчета времени в спящем режиме
bool manual = true; //ручной режим
unsigned char command = breaking_rc;
unsigned char speed_cur = speed_def;
int servo1_angle = servo1_def;
int servo2_angle = servo2_def;
int servo3_angle = servo3_def;
int servo4_angle = servo4_def;
int servo5_angle = servo5_def;
int dist_left = 0;
int dist_right = 0;
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
void set_speed(unsigned char spd) {
  analogWrite(speed_left, spd);
  analogWrite(speed_right, spd);
}
void forward() {
  digitalWrite(dir_left, HIGH);
  digitalWrite(dir_right, HIGH);
  set_speed(speed_cur);
}
void left() {
  digitalWrite(dir_left, HIGH);
  digitalWrite(dir_right, LOW);
  set_speed(speed_cur);
}
void right() {
  digitalWrite(dir_left, LOW);
  digitalWrite(dir_right, HIGH);
  set_speed(speed_cur);
}
void back() {
  digitalWrite(dir_left, LOW);
  digitalWrite(dir_right, LOW);
  set_speed(speed_cur);
}
void beep(unsigned int sound, unsigned int pause, unsigned int count) {
  for (unsigned  i = 0; i < count; i++) {
    set_speed(speed_sound);
    tone(buzzer, 256, sound);
    delay(sound);
    set_speed(0);
    delay(pause);
  }
}
void dist() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5); //2-5 us
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); //10 us
  digitalWrite(trigPin, LOW);
}
float distance_left() {
  dist();
  return pulseIn(echoPin_left, HIGH, 29100) / 58.2;
}
float distance_right() {
  dist();
  return pulseIn(echoPin_right, HIGH, 29100) / 58.2;
}
void setup() {
  servo1.attach(servo1_pin);
  servo2.attach(servo2_pin);
  servo3.attach(servo3_pin);
  servo4.attach(servo4_pin);
  servo5.attach(servo5_pin);
  servo1.write(servo1_def);
  servo2.write(servo2_def);
  servo3.write(servo3_def);
  servo4.write(servo4_def);
  servo5.write(servo5_def);
  for (int i = 4; i <= 7; i++)
    pinMode(i, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(echoPin_left, INPUT);
  pinMode(echoPin_right, INPUT);
  pinMode(PIRpin, INPUT);
  randomSeed(analogRead(0));
#ifdef DEBUG
  Serial.begin(9600);
#endif
  if (manual) {
    Serial2.begin(9600);
    while (!Serial2);
    while (!Serial2.available());
    while (Serial2.read() != forward_rc);
    Serial2.write(breaking_rc);
    while (Serial2.read() != back_rc);
    beep(256, 128, 3);
    beep(128, 64, 3);
  } else
    beep(512, 256, 2);
}
void loop() {
#ifdef DEBUG
  Serial.print(String(command) + " $ " + String(distance_left()) + " - " + String(distance_right()));
#endif
  if (manual) {
    while (!Serial2.available()) {
      if (millis() - standby_time >= 8192 && digitalRead(PIRpin) == HIGH) {
        servo3.write(45);
        delay(256);
        servo3.write(135);
        standby_time = millis();
      }
    }
    standby_time = millis();
    command = Serial2.read();
    switch (command) {
      case right_s:
        servo1_angle -= servo_angle;
        if (servo1_angle < servo1_min)
          servo1_angle = servo1_min;
        servo1.write(servo1_angle);
        break;
      case left_s:
        servo1_angle += servo_angle;
        if (servo1_angle > servo1_max)
          servo1_angle = servo1_max;
        servo1.write(servo1_angle);
        break;
      case forward_s:
        servo2_angle -= servo_angle;
        if (servo2_angle < servo2_min)
          servo2_angle = servo2_min;
        servo2.write(servo2_angle);
        break;
      case back_s:
        servo2_angle += servo_angle;
        if (servo2_angle > servo2_max)
          servo2_angle = servo2_max;
        servo2.write(servo2_angle);
        break;
      case forward2_s:
        servo3_angle += servo_angle;
        if (servo3_angle > servo3_max)
          servo3_angle = servo3_max;
        servo3.write(servo3_angle);
        break;
      case back2_s:
        servo3_angle -= servo_angle;
        if (servo3_angle < servo3_min)
          servo3_angle = servo3_min;
        servo3.write(servo3_angle);
        break;
      case right2_s:
        servo4_angle += servo_angle * 3;
        if (servo4_angle > servo4_max)
          servo4_angle = servo4_max;
        servo4.write(servo4_angle);
        break;
      case left2_s:
        servo4_angle -= servo_angle * 3;
        if (servo4_angle < servo4_min)
          servo4_angle = servo4_min;
        servo4.write(servo4_angle);
        break;
      case servos_to_def:
        servo1.write(servo1_def);
        servo2.write(servo2_def);
        servo3.write(servo3_def);
        break;
      case forward_rc:
        forward();
        break;
      case back_rc:
        back();
        break;
      case right_rc:
        right();
        break;
      case left_rc:
        left();
        break;
      case breaking_rc:
        set_speed(0);
        break;
      case left2_rc:
        servo5_angle += servo_angle;
        if (servo5_angle > servo5_max)
          servo5_angle = servo5_max;
        servo5.write(servo5_angle);
        break;
      case right2_rc:
        servo5_angle -= servo_angle;
        if (servo5_angle < servo5_min)
          servo5_angle = servo5_min;
        servo5.write(servo5_angle);
        break;
      case cam_to_def:
        servo5.write(servo5_def);
        break;
      case changmod_rc:
        speed_cur = speed_def;
        manual = !manual;
        break;
      default:
        if (command > 127)
          speed_cur = command;
        else {
          set_speed(0);
          tone(buzzer, 256, 512); //error
        }
        break;
    }
  } else {
    while (Serial2.available())
      if (Serial2.read() == changmod_rc)
        manual = !manual;
    dist_right = distance_right();
    dist_left = distance_left();
    if ((dist_left < min_distance && dist_left > 0) || (dist_right < min_distance && dist_right > 0)) {
      if (dist_left > dist_right)
        left();
      else if (dist_left < dist_right)
        right();
      else {
        back();
        delay(turn_delay);
        if (rand() % 2 == 0)
          left();
        else
          right();
      }
      delay(turn_delay);
      set_speed(0);
    } else {
      forward();
      delay(turn_delay);
    }
  }
}
