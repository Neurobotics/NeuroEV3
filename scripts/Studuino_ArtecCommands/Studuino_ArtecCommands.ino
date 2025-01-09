// Установите моторы в порты M1 и M2
// Установите красный LED в порт A0
// Установите зелёный LED в порт A1
// Установите синий LED в порт A2

#include <Servo.h>
#include <Wire.h>
#include <MMA8653.h>
#include <Clock.h>
#include <Studuino.h>

#define M1_A        2       // Управляющая ножка 1 для двигателя 1
#define M1_B        4       // Управляющая ножка 2 для двигателя 1
#define M1_PWM      3       // Управляющая ножка для задания скорости двигателя 1

#define M2_A        7       // Управляющая ножка 1 для двигателя 2
#define M2_B        8       // Управляющая ножка 2 для двигателя 2
#define M2_PWM      5       // Управляющая ножка для задания скорости двигателя 2

#define S1

Studuino board;

char SvCalibrationData[] = { 0, 0, 0, 0, 0, 0, 0, 0};
byte DCCalibrationData[] = { 100, 100};

// Функция задания скорости мотора (отрицательные значения крутят против часовой стрелки)
void speed(int drive, int speed) {
  int portA = drive == 0 ? M1_A : M2_A;
  int portB = drive == 0 ? M1_B : M2_B;
  int portPWM = drive == 0 ? M1_PWM : M2_PWM;   
   
  if (speed < 0) {
    digitalWrite(portA, LOW);
    digitalWrite(portB, HIGH);
  } else {
    digitalWrite(portA, HIGH);
    digitalWrite(portB, LOW);
  }
  
  int absSpeed = abs(speed);
  analogWrite(portPWM, absSpeed);
}

// Функция задания скорости обоих моторов
void control(int speed1, int speed2) {
  speed(0, speed1);
  speed(1, speed2);
}

void setup() {
  // Инициализация DC моторов  
  pinMode(M1_A, OUTPUT);    // Установка управляющих пинов контроллера в режим "вывода данных"
  pinMode(M1_B, OUTPUT);   

  pinMode(M2_A, OUTPUT);    // Установка управляющих пинов контроллера в режим "вывода данных"
  pinMode(M2_B, OUTPUT);   

  // Инициализация LED
  board.InitSensorPort(PORT_A0, PIDLED); // Красный для первого состояния
  board.InitSensorPort(PORT_A1, PIDLED); // Зелёный для второго состояния
  board.InitSensorPort(PORT_A2, PIDLED); // Синий для третьего состояния

  board.InitServomotorPort(PORT_D9);
  board.SetServomotorCalibration(SvCalibrationData);
  board.SetDCMotorCalibration(DCCalibrationData);

  board.Servomotor(PORT_D9, 0);
  
  Serial.begin(115200);
}

long long lastMessage = 0;
bool manipulatorRaised = false;
long long rideUntil = 0;

char CMD_FORWARD = 'f';
char CMD_BACKWARDS = 'b';
char CMD_LEFT = 'l';
char CMD_RIGHT = 'r';
char CMD_RAISE = 'u';
char CMD_DROP = 'd';

String msg = "";
bool msgStarted = false;

void loop() {
  long long ts = millis();
  if (Serial.available() > 0) {
    char c = Serial.read();

    lastMessage = ts;

    if (c == CMD_FORWARD) {
      rideUntil = ts + 1000;
      control(-100, -100);
      msgStarted = false;
    } else if (c == CMD_BACKWARDS) {
      rideUntil = ts + 1000;
      control(100, 100);
      msgStarted = false;
    } else if (c == CMD_LEFT) {
      rideUntil = ts + 1000;
      control(100, -100);
      msgStarted = false;
    } else if (c == CMD_RIGHT) {
      rideUntil = ts + 1000;
      control(-100, 100);
      msgStarted = false;
    } else if (c == CMD_RAISE) {
      board.Servomotor(PORT_D9, 0);
      manipulatorRaised = true;
      msgStarted = false;
    } else if (c == CMD_DROP) {
      board.Servomotor(PORT_D9, 90);
      manipulatorRaised = false;
      msgStarted = false;
    } else if (c == '-') {
      msg += c;
      msgStarted = true;
    } else if (c == '+') {
      msgStarted = true;
    } else if (c == ';') {
      if (msgStarted == true && msg.length() > 0) {    
        int sp = msg.toInt();
        if (sp > 100) sp = 100;
        else if (sp < -100) sp = -100;                
        control(-sp, -sp);
        rideUntil = ts + 1000;
      }
      msg = "";
      msgStarted = false;
    } else if (c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6' || c == '7' || c == '8' || c == '9'){
       msgStarted = true;
       msg += c;
    } else {
      control(0, 0);
      rideUntil = 0; 
    }
  }

  if (rideUntil > 0) {
    if (rideUntil < ts) {
      control(0, 0);
      rideUntil = 0; 
    }
  }
  
  delay(200);

  if ((ts - lastMessage) > 5000) {
    // Если общение с платой прекратилось 5 секунд назад - остановить моторы и выключить LED
    board.LED(PORT_A0, OFF); 
    board.LED(PORT_A1, OFF); 
    board.LED(PORT_A2, OFF); 
    control(0,0);
    lastMessage = ts;
  }
}


