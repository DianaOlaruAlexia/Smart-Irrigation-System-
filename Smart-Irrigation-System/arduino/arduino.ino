void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, LOW);
}

void loop() {

  int senzor1 = 42;
  int senzor2 = 55;
 // int average = (senzor1 + senzor2) / 2;
  int average = 20;
  Serial.println(average);
  delay(1000);

  if(Serial.available()){
    String command = Serial.readStringUntil('\n');
    command.trim();
    if(command == "ON"){
      digitalWrite(LED_BUILTIN, HIGH);
    } else if(command == "OFF"){
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}
