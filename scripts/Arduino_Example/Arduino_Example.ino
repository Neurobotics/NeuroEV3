String msg = "";
bool msgStarted = false;

void setup() {
  Serial.begin(115200);
}

void loop() {
  if (Serial.available() > 0) {
    char c = Serial.read();

    if (c == 'f') {
      // ДВИГАТЬСЯ ВПЕРЕД
    } else if (c == 'b') {
      // ДВИГАТЬСЯ НАЗАД
    } else if (c == 'l') {
      // ДВИГАТЬСЯ ВЛЕВО
    } else if (c == 'r') {
      // ДВИГАТЬСЯ ВПРАВО
    } else if (c == ';') { // ОКОНЧАНИЕ ПОСЫЛКИ СО СКОРОСТЬЮ  
      if (msg.length() > 0) {    
        int speed = msg.toInt();                
        // ДВИГАТЬСЯ СО СКОРОСТЬЮ "speed"
      }
      msg = "";
    } else if (с == '-' || c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6' || c == '7' || c == '8' || c == '9'){
      msg += c;
      // ДОБАВЛЕНИЕ СИМВОЛА К ТЕКСТОВОЙ ПОСЫЛКЕ СКОРОСТИ
    }
  }
  delay(200);
}
